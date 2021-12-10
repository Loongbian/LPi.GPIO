#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include "c_gpio.h"
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include <sys/stat.h>

#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)


static volatile uint32_t *gpio_map;

int setup(void)
{
    int mem_fd;
    uint8_t *gpio_mem;
    uint32_t gpio_base;

    /*
    int fd;
    int32_t value, number;

    printf("\nOpening Driver\n");
    fd = open("/dev/ls2k_gpio_device", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file...\n");
        return 0;
    }

    printf("Enter the Value to send\n");
    scanf("%d",&number);
    printf("Writing Value to Driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &number);

    printf("Reading Value from Driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);
    printf("Value is %d\n", value);

    printf("Closing Driver\n");
    close(fd);
*/

/*
    printf("setup()\n");
    // try /dev/gpiomem first - this does not require root privs
    // if ((mem_fd = open("/dev/gpiomem", O_RDWR|O_SYNC)) > 0)
    if ((mem_fd = open("/dev/etx_device", O_RDWR|O_SYNC)) > 0)
    {
        if ((gpio_map = (uint32_t *)mmap(NULL, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, 0)) == MAP_FAILED) {
	    printf("mmap failed\n");    
            return SETUP_MMAP_FAIL;
        } else {
            return SETUP_OK;
        }
    }

    // mmap the GPIO memory registers
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
        return SETUP_DEVMEM_FAIL;

    if ((gpio_mem = malloc(BLOCK_SIZE + (PAGE_SIZE-1))) == NULL)
        return SETUP_MALLOC_FAIL;

    if ((uint32_t)gpio_mem % PAGE_SIZE)
        gpio_mem += PAGE_SIZE - ((uint32_t)gpio_mem % PAGE_SIZE);

    if ((gpio_map = (uint32_t *)mmap( (void *)gpio_mem, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_FIXED, mem_fd, gpio_base)) == MAP_FAILED)
        return SETUP_MMAP_FAIL;

*/

    return SETUP_OK;
}

void setup_gpio(int gpio, int direction)
{

}

void output_gpio(int gpio, int gpiovalue)
{
    int offset, shift;

    int fd;
    int32_t value, number;

    number=gpio;
    printf("\nOpening Driver\n");
    fd = open("/dev/ls2k_gpio_device", O_RDWR);
    if(fd < 0) {
        printf("Cannot open device file...\n");
        return 0;
    }

//    printf("Enter the Value to send\n");
//   scanf("%d",&number);
    printf("Writing Value to Driver\n");
    ioctl(fd, WR_VALUE, (int32_t*) &number);

    printf("Reading Value from Driver\n");
    ioctl(fd, RD_VALUE, (int32_t*) &value);
    printf("Value is %d\n", value);

    printf("Closing Driver\n");
    close(fd);

    printf("output_gpio\n");

}
