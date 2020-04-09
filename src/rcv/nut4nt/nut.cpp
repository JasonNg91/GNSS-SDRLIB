#include <thread>
#include "nut.h"

IFx3Device* dev = nullptr;
fx3_dev_debug_info_t info;
NUT2NT_FIFO fifo;
bool run;
std::thread* th;

//Init NUT

int nut_init(void) {
    SDRPRINTF("Init...");

    dev = Fx3DeviceFactory::CreateInstance(CYAPI_DEVICE_TYPE, 2 * 1024 * 1024, 7);


    //path to the frimware
    int ret = dev->init("nut2files/AmungoItsFx3Firmware-nut2.img", 0/*ntcfg.c_str()*/);
    if (ret != FX3_ERR_OK) {
        SDRPRINTF("Problems with hardware or driver type:\n");
        switch (ret) {
        case FX3_ERR_FIRMWARE_FILE_IO_ERROR:
            SDRPRINTF("Firmware file error\n");
            break;
        }
        dev->Release();
        return -1;
    }

    if (dev->init_fpga("nut2files/nuts2_impl1_algo.sea", "nut2files/nuts2_impl1_data.sed") != FX3_ERR_OK) {
        SDRPRINTF("Problems with loading Lattice firmware (dev->init_fpga)\n");
        dev->Release();
        return -1;
    }

    //path to the nt configs
    if (dev->load1065Ctrlfile("nut2files/ConfigSet_all_GPS_L1_patched_ldvs.hex", 48) != FX3_ERR_OK) {
        SDRPRINTF("Problems with loading nt config file (dev->load1065Ctrlfile)\n");
        dev->Release();
        return -1;
    }

    info = dev->getDebugInfoFromBoard(true);
    //SDRPRINTF("Init done!\n");

    Sleep(1000);
    dev->startRead(nullptr);

    // This is temporary workaround for strange bug of 'odd launch'
    Sleep(200);
    dev->stopRead();
    Sleep(200);
    dev->startRead(nullptr);
    Sleep(1000);

    dev->getDebugInfoFromBoard(true);

    double size_mb = 0.0;
    double phy_errs = 0;
    int sleep_ms = 200;
    int iter_cnt = 5;
    double overall_seconds = (sleep_ms * iter_cnt) / 1000.0;
    fx3_dev_debug_info_t info = dev->getDebugInfoFromBoard(true);
    for (int i = 0; i < iter_cnt; i++) {
        Sleep(200);
        info = dev->getDebugInfoFromBoard(true);
        size_mb += info.size_tx_mb_inc;
        phy_errs += info.phy_err_inc;
    }

    int64_t CHIP_SR = (int64_t)((size_mb * 1024.0 * 1024.0) / overall_seconds);

    SDRPRINTF("SAMPLE RATE  is ~%d MHz\n", CHIP_SR / 1000000);
    dev->changeHandler(&fifo);
    return 0;
}

void readloop() {
    SDRPRINTF("Start dump thread\n");
    int lastOfCount = info.overflows;

    while (run) {
        if (fifo.size() > NUT2NT_DATABUFF_SIZE) {
            mlock(hbuffmtx);
            fifo.read_from_NUT4NT_first_channel(&sdrstat.buff[(sdrstat.buffcnt % MEMBUFFLEN) * NUT2NT_DATABUFF_SIZE], NUT2NT_DATABUFF_SIZE);
            unmlock(hbuffmtx);

            mlock(hreadmtx);
            sdrstat.buffcnt++;
            unmlock(hreadmtx);

            if (lastOfCount != info.overflows) {
                lastOfCount = info.overflows;
                SDRPRINTF("OWERFLOW\n");
            }
        }
    }
    SDRPRINTF("Stop dump thread\n");
}

void nut_quit(void) {
    run = false;
    th->join();
    dev->Release();
}

int nut_initconf(void) {
    return 0;
}

int nut_start(void) {
    //th = new std::thread(readloop);
    //dev->startRead(&fifo);
    //run = true;
    int lastOfCount = info.overflows;
    
    while (fifo.size() < NUT2NT_DATABUFF_SIZE);

    mlock(hbuffmtx);
    fifo.read_from_NUT4NT_first_channel(&sdrstat.buff[(sdrstat.buffcnt % MEMBUFFLEN) * NUT2NT_DATABUFF_SIZE], NUT2NT_DATABUFF_SIZE);
    unmlock(hbuffmtx);

    mlock(hreadmtx);
    sdrstat.buffcnt++;
    unmlock(hreadmtx);

    if (lastOfCount != info.overflows) {
        lastOfCount = info.overflows;
        SDRPRINTF("OWERFLOW\n");
    }
    

    return 0;
}

int read_from_NUT4NT(void* dst) {
    return fifo.read_from_NUT4NT_first_channel(dst, NUT2NT_DATABUFF_SIZE);
}

void nut_exp(uint8_t* buf, int n, char* expbuf) {

}

void nut_getbuff(uint64_t buffloc, int n, char* expbuf) {
    uint64_t membuffloc = buffloc % (MEMBUFFLEN * NUT2NT_DATABUFF_SIZE);
    int nout;
    nout = (int)((membuffloc + n) - (MEMBUFFLEN * NUT2NT_DATABUFF_SIZE));

    mlock(hbuffmtx);
    if (nout > 0) {
        memcpy(expbuf, &sdrstat.buff[membuffloc], n - nout);
        memcpy(&expbuf[n - nout], &sdrstat.buff[0], nout);
        //expbuf[i] = (char)((buf[i] - 127.5));
        //rtlsdr_exp(&sdrstat.buff[membuffloc], n - nout, expbuf);
        //rtlsdr_exp(&sdrstat.buff[0], nout, &expbuf[n - nout]);
    }
    else {
        memcpy(expbuf, &sdrstat.buff[membuffloc], n);
        //rtlsdr_exp(&sdrstat.buff[membuffloc], n, expbuf);
    }
    unmlock(hbuffmtx);
    
}

void nut_pushtomembuf(void) {

}