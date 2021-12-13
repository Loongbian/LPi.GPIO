/***************************************************************************//**
*  \file       driver.c
*
*  \details    Simple Linux device driver (IOCTL)
*
*
*******************************************************************************/
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 //kmalloc()
#include<linux/uaccess.h>              //copy_to/from_user()
#include <linux/ioctl.h>
#include <loongson-2k.h>
#include <gpio_adv_drv.h>
 
 
#define WR_VALUE _IOW('a','a',int32_t*)
#define RD_VALUE _IOR('a','b',int32_t*)
#define TEST_VALUE _IOW('a','c',int32_t)

int32_t value = 0;

#define GPIO_INT_ENABLE 0x1fe10530
#define INT_POL                 0x1fe11470
#define INT_EDGE                0x1fe11474

#define GPIO_EN 0x1fe10500
#define LS2K_GPIO_EN 0x1fe10500
#define GPIO_O 0x1fe10510
#define GPIO_I  0x1fe10520
#define LS2K_GPIO_O 0x1fe10510

 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev ls2k_gpio_cdev;

#define CBS_MAX         5

#define DBG_GPIO_IN(...)    printk(__VA_ARGS__)
#define DBG_GPIO_OUT(...)   printk(__VA_ARGS__)
#define DBG_GPIO_PWM(...)   printk(__VA_ARGS__)

/*
 * GPIO Definition
 *
 **/
typedef struct
{
    int             index;                  // gpio number

    int             work_mode;              // 工作模式

//    rtems_id        __timer;                    // IN: use to bounce; PWM: pulse
    /**************************************************************************
     * IN 工作模式相关参数
     *
     */
    int             in_inverse;             // 输入：电平反相

    /**************************************************************************
     * OUT 工作模式相关参数
     */

    int             out_inverse;            // 输出：电平反相 



} gpio_desc_t;

//-------------------------------------------------------------------------------------------------
// Local variables
//-------------------------------------------------------------------------------------------------
static gpio_desc_t gpios[GPIO_COUNT];               // GPIO 描述符


/*
** Function Prototypes
*/
static int      __init ls2k_gpio_driver_init(void);
static void     __exit ls2k_gpio_driver_exit(void);
static int      ls2k_gpio_open(struct inode *inode, struct file *file);
static int      ls2k_gpio_release(struct inode *inode, struct file *file);
static ssize_t  ls2k_gpio_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  ls2k_gpio_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long     ls2k_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

/*
** File operation sturcture
*/
static struct file_operations fops =
{
        .owner          = THIS_MODULE,
        .read           = ls2k_gpio_read,
        .write          = ls2k_gpio_write,
        .open           = ls2k_gpio_open,
        .unlocked_ioctl = ls2k_gpio_ioctl,
        .release        = ls2k_gpio_release,
};

/*
 * led up 
 */
static int led_up(int value)
{
    int gpio=value;
    char offset;
    int error;

    offset=26;
    int irq= 66;

    pr_info("output led up gpio :%d\n",gpio);

    //ls2k_writel(ls2k_readl(GPIO_EN) & ~(1 << 2),  GPIO_EN);    //set edge
    //ls2k_writel(ls2k_readl(GPIO_O) | (1 << 2),  GPIO_O);    //set edge
    pr_info("ls2k_readq(GPIO_EN:%llx):%llx\n",GPIO_EN,ls2k_readq(GPIO_EN));
    ls2k_writeq(ls2k_readq(GPIO_EN) & ~(1 << gpio),  GPIO_EN);    //set edge
    pr_info("ls2k_readq(GPIO_EN:%llx):%llx\n",GPIO_EN,ls2k_readq(GPIO_EN));
    ls2k_writeq(ls2k_readq(GPIO_O) | (1 << gpio),  GPIO_O);    //set edge
    printk(KERN_DEBUG "Hello World\n");
    return 0;

	
}

/*
** This function will be called when we open the Device file
*/
static int ls2k_gpio_open(struct inode *inode, struct file *file)
{
        pr_info("Device File Opened...!!!\n");
        return 0;
}

/*
** This function will be called when we close the Device file
*/
static int ls2k_gpio_release(struct inode *inode, struct file *file)
{
        pr_info("Device File Closed...!!!\n");
        return 0;
}

/*
** This function will be called when we read the Device file
*/
static ssize_t ls2k_gpio_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	printk(KERN_DEBUG "read in ls2k_gpio");
        pr_info("Read Function\n");
        return 0;
}

/*
** This function will be called when we write the Device file
*/
static ssize_t ls2k_gpio_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	int gpio=value;
	
	printk(KERN_DEBUG "write in ls2k_gpio gpio:%d",gpio);

        pr_info("Write function\n");
        return len;
}

/*
 * 返回GPIO的工作模式
 */
static int gpio_get_workmode(int gpio_num, unsigned int *mode)
{
	CHECK_GPIO_NUM(gpio_num);
	//pr_info("in gpio_get_workmode/n");
	//pr_info("gpio_num is %d/n",gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];
        *mode = p_gpio->work_mode;		
	return 0;
}

/*
 * 取消所有GPIO的工作模式
 */
static int gpio_clear_any(int gpio_num)
{
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	pr_info("in gpio_clear_any/n");
	pr_info("GPIO_NONE %x, GPIO_IN %x, GPIO_OUT %x, GPIO_PWM %x/n",GPIO_NONE, GPIO_IN, GPIO_OUT, GPIO_PWM);
	pr_info("p_gpio->work_mode: %x",p_gpio->work_mode);
	switch (p_gpio->work_mode)
	{
		case GPIO_IN:
			// gpio_in_mode_unset(gpio_num);
			break;

		case GPIO_OUT:
			// gpio_out_mode_unset(gpio_num);
			break;
			
		case GPIO_PWM:
			// gpio_pwm_mode_unset(gpio_num);
			break;

		case GPIO_NONE:
			break;

		default:
			return -1;

	};

	return 0;

}

/******************************************************************************
 * GPIO IN 基本操作
 */

static int g_input_count = 0;

/*
 * 设置GPIO为IN模式
 */
static int gpio_in_mode_set(int gpio_num, int inverse)
{
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	//pr_info("gpio_in_mode_set\n");
	if (p_gpio->work_mode != GPIO_NONE)
	{
		DBG_GPIO_IN("gpio[%i] is not empty.\r\n", gpio_num);
	}

	p_gpio->in_inverse = inverse;

	
	//gpio enable
	//gpio_enable(gpio_num, DIR_IN);
	gpio_num=2;	
        pr_info("ls2k_readq(GPIO_EN:%llx):%llx\n",GPIO_EN,ls2k_readq(GPIO_EN));
        ls2k_writeq(ls2k_readq(GPIO_EN) | (1 << gpio_num),  GPIO_EN);    //set edge
        pr_info("ls2k_readq(GPIO_EN:%llx):%llx\n",GPIO_EN,ls2k_readq(GPIO_EN));

	p_gpio->work_mode = GPIO_IN;
	g_input_count += 1;

	DBG_GPIO_IN("set gpio[%i] as IN.\r\n", gpio_num);
	return 0;

}

/*
 * 读取一个GPIO的输入
 */
static int gpio_in_get_value(int gpio_num, unsigned int *in_val)
{
	int value;
	CHECK_GPIO_NUM(gpio_num);
        gpio_desc_t *p_gpio = &gpios[gpio_num];

	pr_info("gpio_in_get_value\n");
	if (p_gpio->work_mode != GPIO_IN)
	{
		DBG_GPIO_IN("gpio[%i] is not at IN.\r\n",gpio_num);
		return -2;
	}

	value  = ls2k_readq(GPIO_I) >> gpio_num & 1;
	pr_info("value is %d",value);
	*in_val=value;
	// *in_val = gpio_read(gpio_num);
	pr_info("gpio in get value %d\n", *in_val);
	if (p_gpio->in_inverse)
		*in_val = (*in_val) ? 0 : 1;

	return 0;
}

/*
 * 读取全部64个GPIO的输入
 */
static int gpio_in_get_all_values(uint64_t *p_val64)
{
	return 0;
}

/*
 * 取消GPIO的IN模式
 */
static int gpio_in_mode_unset(int gpio_num)
{
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	pr_info("gpio_in_mode_unset\n");

	if (p_gpio->work_mode != GPIO_IN)
	{
		DBG_GPIO_IN("gpio[%i] is not at IN.\r\n",gpio_num);
		return -2;
	}

	//gpio_disable(gpio_num);
	p_gpio->work_mode = GPIO_NONE;
	p_gpio->in_inverse = 0;
	g_input_count -= 1;
	DBG_GPIO_IN("unset IN at gpio[%i].\r\n", gpio_num);

	return 0;
}

//-------------------------------------------------------------------------------------------------
// GPIO OUT functions
//-------------------------------------------------------------------------------------------------

/*
 * 设置GPIO为OUT模式
 */
static int gpio_out_mode_set(int gpio_num, int inverse)
{
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	//pr_info("gpio_out_mode_set\n");
	if (p_gpio->work_mode != GPIO_NONE)
	{
		DBG_GPIO_OUT("gpio[%i] is not empty.\r\n", gpio_num);
		return -2;
	}
	
	p_gpio->work_mode = GPIO_OUT;
	p_gpio->out_inverse = inverse;
        ls2k_writeq(ls2k_readq(LS2K_GPIO_EN) & ~(1 << gpio_num),  LS2K_GPIO_EN);    //set edge, 配置GPIO%为输出模式
	//gpio_enable(gpio_num, DIR_OUT);
	//pr_info("mode_set: gpio_num:%d, p_gpio->work_mode:%d, GPIO_OUT:%d\n",gpio_num,p_gpio->work_mode,GPIO_OUT);
	//DBG_GPIO_OUT("set gpio[%i] as OUT.\r\n", gpio_num);

	return 0;
}

/*
 * GPIO 输出一个值
 */
static int gpio_out_set_value(int gpio_num, unsigned int out_val)
{
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	pr_info("gpio_out_set_value\n");
	pr_info("set value: gpio_num:%d, p_gpio->work_mode:%d, GPIO_OUT:%d\n",gpio_num,p_gpio->work_mode,GPIO_OUT);
	if (p_gpio->work_mode != GPIO_OUT)
	{
		DBG_GPIO_OUT("gpio[%i] is not at OUT.\r\n",gpio_num);
		return -2;
	}

	
	pr_info("gpio number: %d\n",gpio_num);
        ls2k_writeq(ls2k_readq(LS2K_GPIO_O) | (1 << gpio_num),  LS2K_GPIO_O);    //set edge

	/*
	 * 处理反向信号
	 */

	return 0;
}

/*
 * GPIO 输出全部值
 */
static int gpio_out_set_all_values(uint64_t *p_val64)
{
	return 0;
}

/*
 * 取消GPIO的输出模式
 */
static int gpio_out_mode_unset(int gpio_num)
{
	pr_info("gpio_out_mode_unset\n");
	CHECK_GPIO_NUM(gpio_num);
	gpio_desc_t *p_gpio = &gpios[gpio_num];

	if (p_gpio->work_mode != GPIO_OUT)
	{
		
		DBG_GPIO_OUT("gpio[%i] is not at OUT.\r\n",gpio_num);
		return -2;
	}

	//gpio_disable(gpio_num);
	p_gpio->out_inverse = 0;
	p_gpio->work_mode = GPIO_NONE;
	DBG_GPIO_OUT("clear OUT at gpio[%i].\r\n", gpio_num);
	
	return 0;
}

/*
** This function will be called when we write IOCTL on the Device file
*/
static long ls2k_gpio_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

	unsigned int gpio, val, *pv32;
	int32_t v_arg;
	int rt  = 1;
	
        switch(cmd) {
	       /*
		*GPIO Common
		*/
		case IOCTL_GPIO_GET_MODE:
			pr_info("arg is %d\n",arg);
			pr_info("pv32 is %d\n",pv32);
			rt = gpio_get_workmode((*pv32), pv32);
			break;
		case IOCTL_GPIO_RESET:
			pr_info("gpio is %d\n", gpio);
			rt = gpio_clear_any(gpio);
			break;
                /*
		 * GPIO 输入模式
		 */
		case IOCTL_IN_ENABLE:
			v_arg=arg;
			UNPACK_16_16(v_arg, gpio, val);
			rt = gpio_in_mode_set(gpio, val);
			break;

		case IOCTL_IN_GET_VAL:
			// copy from user 
                        if( copy_from_user(&v_arg ,(int32_t*) arg, sizeof(v_arg)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
			pv32 = &v_arg;

			// get gpio value
			rt = gpio_in_get_value((*pv32), pv32);

			// copy to user 
			v_arg=*pv32;
                        if( copy_to_user((int32_t*) arg, &v_arg, sizeof(v_arg)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
			break;

		case IOCTL_IN_GET_ALL:
			//IOC_BUFFER_CHECK_RETURN;
			//rt = gpio_in_get_all_values((uint64_t *)ioarg->buffer);
			break;

		case IOCTL_IN_DISABLE:
			gpio = arg;
			rt = gpio_in_mode_unset(gpio);
			break;

		/*
		 * GPIO 输出模式
		 */
		case IOCTL_OUT_ENABLE:
			v_arg=arg;
			UNPACK_16_16(v_arg, gpio, val);
			rt = gpio_out_mode_set(gpio, val);
			break;
		case IOCTL_OUT_SET_VAL:
			v_arg=arg;
			UNPACK_16_16(v_arg, gpio, val);
			rt = gpio_out_set_value(gpio, val);
			break;
		case IOCTL_OUT_SET_ALL:
			//IOC_BUFFER_CHECK_RETURN;
			//rt = gpio_out_set_all_values((uint64_t *)ioarg->buffer);
			break;
		case IOCTL_OUT_DISABLE:
			gpio = arg;
			rt = gpio_out_mode_unset(gpio);
			break;
                case WR_VALUE:
                        if( copy_from_user(&value ,(int32_t*) arg, sizeof(value)) )
                        {
                                pr_err("Data Write : Err!\n");
                        }
                        pr_info("Value = %d\n", value);
			//led_up(value);
                        break;
                case RD_VALUE:
                        if( copy_to_user((int32_t*) arg, &value, sizeof(value)) )
                        {
                                pr_err("Data Read : Err!\n");
                        }
                        break;
		case TEST_VALUE:
                        pr_info("TEST Value = %d\n",arg );
			break;
                default:
                        pr_info("Default\n");
                        break;
        }
        return 0;
}
 
/*
** Module Init function
*/
static int __init ls2k_gpio_driver_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "ls2k_gpio_Dev")) <0){
                pr_err("Cannot allocate major number\n");
                return -1;
        }
        pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
 
        /*Creating cdev structure*/
        cdev_init(&ls2k_gpio_cdev,&fops);
 
        /*Adding character device to the system*/
        if((cdev_add(&ls2k_gpio_cdev,dev,1)) < 0){
            pr_err("Cannot add the device to the system\n");
            goto r_class;
        }
 
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"ls2k_gpio_class")) == NULL){
            pr_err("Cannot create the struct class\n");
            goto r_class;
        }
 
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"ls2k_gpio_device")) == NULL){
            pr_err("Cannot create the Device 1\n");
            goto r_device;
        }
        pr_info("Device Driver Insert...Done!!!\n");
        return 0;
 
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}

/*
** Module exit function
*/
static void __exit ls2k_gpio_driver_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&ls2k_gpio_cdev);
        unregister_chrdev_region(dev, 1);
        pr_info("Device Driver Remove...Done!!!\n");
}
 
module_init(ls2k_gpio_driver_init);
module_exit(ls2k_gpio_driver_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("...");
MODULE_DESCRIPTION("Simple Linux device driver (IOCTL)");
MODULE_VERSION("1.0");
