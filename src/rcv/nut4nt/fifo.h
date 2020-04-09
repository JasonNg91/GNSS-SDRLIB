#pragma once

#include "IFx3Device.h"
#include <vector>

class NUT2NT_FIFO : public DeviceDataHandlerIfce {
public:
    NUT2NT_FIFO();
    ~NUT2NT_FIFO();

    size_t size();

    //recieve callback
    void HandleDeviceData(void* data_pointer, unsigned int size_in_bytes);

    //pop data from vector
    //maxSize - max size of the dst buffer
    int read_from_NUT4NT_all_channels(void* dst, size_t maxSize);
    int read_from_NUT4NT_first_channel(void* dst, size_t maxSize);
private:
    std::vector<char> mem;
};

