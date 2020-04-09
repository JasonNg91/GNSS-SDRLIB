#include "fifo.h"
#include "../../../src/sdr.h"

mlock_t m_mutex;
int l1 = 0, l2 = 0;

NUT2NT_FIFO::NUT2NT_FIFO() {
	//mem.reserve(1);
	initmlock(m_mutex);
}

NUT2NT_FIFO::~NUT2NT_FIFO() {
	mem.clear();
}

void NUT2NT_FIFO::HandleDeviceData(void* data_pointer, unsigned int size_in_bytes) {
	l2++;
	unsigned int lastSize = mem.size();
	mlock(m_mutex);
	//mem.resize(mem.size() + size_in_bytes);
	//memcpy((char*)mem.data() + lastSize, data_pointer, size_in_bytes);
	mem.insert(mem.end(), (char*)data_pointer, (char*)data_pointer + size_in_bytes);
	unmlock(m_mutex);
}

int NUT2NT_FIFO::read_from_NUT4NT_all_channels(void* dst, size_t maxSize) {
	if (mem.size() > maxSize) {
		memcpy(dst, mem.data(), maxSize);
		mlock(m_mutex);
		mem.erase(mem.begin(), mem.begin() + maxSize);
		unmlock(m_mutex);
		return maxSize;
	}
	else {
		memcpy(dst, mem.data(), mem.size());
		mlock(m_mutex);
		mem.erase(mem.begin(), mem.begin() + mem.size());
		unmlock(m_mutex);
		return mem.size();
	}

}

int NUT2NT_FIFO::read_from_NUT4NT_first_channel(void* dst, size_t maxSize) {
	l1++;
	char* tmpbuff = new char[maxSize];
	int max = mem.size() > maxSize ? maxSize : mem.size();

	mlock(m_mutex);

	for (int i = 0; i < max; i++) {
		switch (mem[i] & 0b11)
		{
		case 0b00:
			tmpbuff[i] = 1;
			break;
		case 0b01:
			tmpbuff[i] = 3;
			break;
		case 0b10:
			tmpbuff[i] = -1;
			break;
		case 0b11:
			tmpbuff[i] = -3;
			break;
		}
	}

 	mem.erase(mem.begin(), mem.begin() + max);
	unmlock(m_mutex);

	memcpy(dst, tmpbuff, max);
	delete[] tmpbuff;
	return max;
	
}

size_t NUT2NT_FIFO::size() {
	return mem.size();
}