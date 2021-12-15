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
#include "gpio_adv_drv.h"


#define PAGE_SIZE  (4*1024)
#define BLOCK_SIZE (4*1024)

#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)

int fd_gpio;
static volatile uint32_t *gpio_map;

int setup(void)
{
    int mem_fd;


    printf("setup()\n");

    fd_gpio = open("/dev/ls2k_gpio_device", O_RDWR);
    if(fd_gpio < 0) {
            printf("Cannot open device file...\n");
            return 0;
    }

    // mmap the GPIO memory registers
    if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
        return SETUP_DEVMEM_FAIL;

    return SETUP_OK;
}

void setup_gpio(int gpio, int direction)
{

    if (direction == INPUT)
    {
	__gpio_in_enable(fd_gpio, gpio, 1);
    } else {   // direction == OUTPUT
	__gpio_out_enable(fd_gpio, gpio, 0);
    
    }

}

void output_gpio(int gpio, int value)
{

   __gpio_out_value(fd_gpio, gpio, value);

}

int input_gpio(int gpio)
{
   int value=gpio;

   __gpio_in_value(fd_gpio, &value);

   return value;
	
}

void cleanup(void)
{
	
        close(fd_gpio);
	printf("cleanup\n");

}
