/***************************************************************************//**
*  \file       test_app.c
*
*  \details    Userspace application to test the Device driver
*
*
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include<sys/ioctl.h>
#include "include/gpio_adv_drv.h"
#include "include/gpio_adv_service.h"
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
#define TEST_VALUE _IOW('a','c',int32_t)

#define IN1 60

void gpio40_callback(int gpio_num)
{
    //printk("--->gpio40 callback.\r\n");
    printf("--->gpio40 callback.\r\n");
}

void gpio41_callback(int gpio_num)
{
    //printk("--->gpio41 callback.\r\n");
    printf("--->gpio41 callback.\r\n");

}

int main()
{

	
        int fd_gpio;
	int i;
        int32_t value, number;
        printf("*********************************\n");
 
        fd_gpio = open("/dev/ls2k_gpio_device", O_RDWR);
        if(fd_gpio < 0) {
                printf("Cannot open device file...\n");
                return 0;
        }
/*
	// enable	
        //__gpio_in_enable(fd_gpio, IN1, 1);
	//printf("gpio in enable\n ");

	int v1 = IN1;
	for (i=0; i<4; i++)
	{
	
        printf("Enter the Value to send\n");
        scanf("%d",&v1);

        __gpio_in_enable(fd_gpio, v1, 1);

	// value
	__gpio_in_value(fd_gpio, &v1);
	printf("gpio in value:%d\n",v1);

	// disable
	__gpio_in_disable(fd_gpio, IN1);

	}

*/
	//-------------------------------------------------------------------------
        // 测试 GPIO OUT, 2/3/54/55
        //-------------------------------------------------------------------------

	#define OUT1    2

/*	
	unsigned out_val = 0;
	u_int64_t out64;


	__gpio_out_enable(fd_gpio, OUT1, 0);
	__gpio_out_value(fd_gpio, OUT1, out_val);
	printf("out gpio[%i]=%i\n", OUT1, out_val);
	
	out_val = out_val == 0 ? 1 : 0;

	__gpio_out_disable(fd_gpio, OUT1);
*/


/*
        printf("Enter the Value to send\n");
        scanf("%d",&number);

        ioctl(fd_gpio, WR_VALUE, (int32_t*) &number); 	
        ioctl(fd_gpio, RD_VALUE, (int32_t*) &value);
        printf("Number is %d, Value is %d\n",number, value);
 
 
        ioctl(fd_gpio, TEST_VALUE, 40+41);
*/

#define IN1     40
#define IN2     41
    // 40 in enable wait for edge
    // in gpio_adv_service.c ioctl
    // IN1 40
    // IN_EDGE_RISING
    gpio_in_enable(IN1, 1);
    wait_for_edge(IN1, IN_EDGE_RISING, 5000);

    // 41 in enable 
    gpio_in_enable(IN2, 1);
    add_event_detect(IN2, IN_EDGE_RISING, gpio41_callback, 100);


        printf("Closing Driver\n");
        close(fd_gpio);
}
