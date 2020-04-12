#ifndef FX3DEVIFCE
#define FX3DEVIFCE

#include "IFx3Device.h"
#include "fx3deverr.h"
#include "fx3devdebuginfo.h"
#include "fx3commands.h"
//#include "../../../src/sdr.h"
#include <stddef.h>
#include <mutex>
#include <vcclr.h>
#include <msclr/lock.h>

#ifdef WIN32
#include <windows.h>
#include <process.h>

#define mlock_t       HANDLE
#define initmlock(f)  (f=CreateMutex(NULL,FALSE,NULL))
#define mlock(f)      WaitForSingleObject(f,INFINITE)
#define unmlock(f)    ReleaseMutex(f)
#define delmlock(f)   CloseHandle(f)
#define event_t       HANDLE
#define initevent(f)  (f=CreateEvent(NULL,FALSE,FALSE,NULL))
#define setevent(f)   SetEvent(f)
#define waitevent(f,m) WaitForSingleObject(f,INFINITE)
#define delevent(f)   CloseHandle(f)
#define waitthread(f) WaitForSingleObject(f,INFINITE)
#define cratethread(f,func,arg) (f=(thread_t)_beginthread(func,0,arg))
#define THRETVAL      
#else
#define mlock_t       pthread_mutex_t
#define initmlock(f)  pthread_mutex_init(&f,NULL)
#define mlock(f)      pthread_mutex_lock(&f)
#define unmlock(f)    pthread_mutex_unlock(&f)
#define delmlock(f)   pthread_mutex_destroy(&f)
#define event_t       pthread_cond_t
#define initevent(f)  pthread_cond_init(&f,NULL)
#define setevent(f)   pthread_cond_signal(&f)
#define waitevent(f,m) pthread_cond_wait(&f,&m)
#define delevent(f)   pthread_cond_destroy(&f)
#define waitthread(f) pthread_join(f,NULL)
#define cratethread(f,func,arg) pthread_create(&f,NULL,func,arg)
#define THRETVAL      NULL
#endif

class FX3DevIfce : public IFx3Device {
public:

    FX3DevIfce();

    int addRef();
    void Release();

    // Opens device and flash it if neccessary (set firmwareFileName to NULL to disable flashing)
    virtual fx3_dev_err_t init( const char* firmwareFileName,
                                const char* additionalFirmwareFileName ) = 0;
    virtual fx3_dev_err_t init_fpga(const char* algoFileName, const char* dataFileName)
    { return FX3_ERR_CTRL_TX_FAIL; }

    // Starts reading of signal from device and sends data to handler.
    // If handler is NULL, data will be read and skipped
    virtual void startRead( DeviceDataHandlerIfce* handler ) = 0;

    // Stop's reading from device.
    virtual void stopRead() = 0;
    virtual void changeHandler(DeviceDataHandlerIfce* handler);
    virtual fx3_dev_err_t getReceiverRegValue(unsigned char addr, unsigned char& value);
    virtual fx3_dev_err_t putReceiverRegValue(unsigned char addr, unsigned char value);

    virtual void sendAttCommand5bits( uint32_t bits ) = 0;

    virtual fx3_dev_debug_info_t getDebugInfoFromBoard( bool ask_speed_only = false ) = 0;
    virtual void readFwVersion();

    //----------------------- Lattice control ------------------
    virtual fx3_dev_err_t send8bitSPI(uint8_t addr, uint8_t data);
    virtual fx3_dev_err_t read8bitSPI(uint8_t addr, uint8_t* data);
    virtual fx3_dev_err_t sendECP5(uint8_t* buf, long len);
    virtual fx3_dev_err_t recvECP5(uint8_t* buf, long len);
    virtual fx3_dev_err_t resetECP5();
    virtual fx3_dev_err_t switchoffECP5();
    virtual fx3_dev_err_t checkECP5();
    virtual fx3_dev_err_t csonECP5();
    virtual fx3_dev_err_t csoffECP5();
    virtual fx3_dev_err_t setDAC(unsigned int data);
    virtual fx3_dev_err_t device_start();
    virtual fx3_dev_err_t device_stop();
    virtual fx3_dev_err_t device_reset();
    virtual fx3_dev_err_t reset_nt1065();
    virtual fx3_dev_err_t load1065Ctrlfile(const char* fwFileName, int lastaddr);

protected:
    virtual fx3_dev_err_t ctrlToDevice(   uint8_t cmd, uint16_t value = 0, uint16_t index = 0, void* data = nullptr, size_t data_len = 0 ) = 0;
    virtual fx3_dev_err_t ctrlFromDevice( uint8_t cmd, uint16_t value = 0, uint16_t index = 0, void* dest = nullptr, size_t data_len = 0 ) = 0;

    virtual void writeGPIO( uint32_t gpio, uint32_t value );
    virtual void readGPIO( uint32_t gpio, uint32_t* value );

    virtual fx3_dev_err_t resetFx3Chip();
    virtual void pre_init_fx3();
    virtual void init_ntlab_default();


    virtual uint32_t GetNt1065ChipID();
    virtual void readNtReg(uint32_t reg);

    virtual void startGpif();

    virtual ~FX3DevIfce() {}

    FirmwareDescription_t fwDescription;

protected:
    DeviceDataHandlerIfce* data_handler;

private:
    mlock_t m_mutex;
    //gcroot<System::Threading::Mutex^> m_mutex;
    //std::mutex m_mutex;
    int m_ref_count;

};

#endif // FX3DEVIFCE

