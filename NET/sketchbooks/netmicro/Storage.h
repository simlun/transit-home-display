#ifndef STORAGE_H
#define STORAGE_H

#include <Arduino.h>

class Storage {
    private:
        int address;
        int size_in_bytes;
    public:
        Storage(int, int);

        // The first element represents the sub address of the memory space in
        // steps of 8 bytes, followed by 8 bytes of data.
        void update(byte[]);
};

#endif
