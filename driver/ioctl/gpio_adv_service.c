#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "include/gpio_adv_drv.h"
#include "include/gpio_adv_service.h"

//-------------------------------------------------------------------------------------------------
// 驱动程序文件句柄
//-------------------------------------------------------------------------------------------------

static int f_gpio_drv = 0;  /* 驱动文件句柄 */

/**
 * GPIO IN
 */

int gpio_in_enable(int gpio_num, int inverse)
{
    // CHECK_FILE_HANDLE();
    printf("gpio_in_enable\n");
    return ioctl(f_gpio_drv, IOCTL_IN_ENABLE, PACK_16_16(gpio_num, inverse));
}


//-------------------------------------------------------------------------------------------------
//
// 扩展接口
//
//-------------------------------------------------------------------------------------------------

/*
 *
 * 功能： 等待一个边沿触发的发生.阻塞等待按键发生，直到超时.
 *
 * 参数： gpio_num       gpio编号
 * 	  trigger_mode   触发模式
 * 	  wait_timeout   等待超时
 *
 * 返回： 0
 *
 *
 *
 */
int wait_for_edge(int gpio_num, int trigger_mode, int timeout)
{
    return 0;
}

/*
 * 功能：设置事件检测
 *
 * 参数：gpio_num   gpio编号
 * 	 edge       边沿触发模式
 * 	 callback   回调函数(gpio_callback_t),NULL将移去全部callback
 * 	 bouncetime 去抖时间(ms)
 *
 * 返回：0 = 成功
 *
 */
int add_event_detect(int gpio_num, int edge, gpio_callback_t cb, int bouncetime)
{


    return 0;
}


