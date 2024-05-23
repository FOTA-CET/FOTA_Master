#ifndef __GPIO_CONTROL_H
#define __GPIO_CONTROL_H

#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

// Base addr
#define GPIO_BASE 0x3F200000
#define BLOCK_SIZE 4096

// Offset
#define GPFSEL_OFFSET 0x00
#define GPSET_OFFSET 0x1C
#define GPCLR_OFFSET 0x28
#define GPLEV_OFFSET 0x34

class GPIO
{
public:
    GPIO(int pin);
    ~GPIO();
    void setModeOutput();
    void setModeInput();
    void setPinHigh();
    void setPinLow();
    bool readPin();

private:
    int fd;
    int pin;
    void *gpio_map;
    volatile unsigned *gpio;
};

#endif