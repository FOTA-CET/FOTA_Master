#include <cerrno>
#include <cstring>

#include "gpioControl.hh"

GPIO::~GPIO()
{
    // munmap(gpio_map, BLOCK_SIZE);
    // close(fd);
}

void GPIO::setModeOutput()
{
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    *(gpio + reg) &= ~(7 << shift);
    *(gpio + reg) |= (1 << shift);
}

void GPIO::setModeInput()
{
    int reg = pin / 10;
    int shift = (pin % 10) * 3;
    *(gpio + reg) &= ~(7 << shift);
}

void GPIO::setPinHigh()
{
    *(gpio + GPSET_OFFSET / 4) = (1 << pin);
}

void GPIO::setPinLow()
{
    *(gpio + GPCLR_OFFSET / 4) = (1 << pin);
}

bool GPIO::readPin()
{
    return (*(gpio + GPLEV_OFFSET / 4) & (1 << pin)) != 0;
}

GPIO::GPIO(int pin) : pin(pin)
{
    if ((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
    {
        std::cerr << "Cap quyen sudo cho file thuc thi" << std::endl;
        exit(EXIT_FAILURE);
    }

    gpio_map = (char *)mmap(
        NULL,
        BLOCK_SIZE,
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        GPIO_BASE);

    if (gpio_map == MAP_FAILED)
    {
        std::cerr << "Lỗi mmap" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }

    gpio = (volatile unsigned *)gpio_map;
}