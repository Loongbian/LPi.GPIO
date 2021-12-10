/**
 * GPIO memory device driver
 *
 * Creates a chardev /dev/gpiomem which will provide user access to
 * the BCM2835's GPIO registers when it is mmap()'d.
 * No longer need root for user GPIO access, but without relaxing permissions
 * on /dev/mem.
 *
 * Copyright (c) 2015, Raspberry Pi (Trading) Ltd.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions, and the following disclaimer,
 *    without modification.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The names of the above-listed copyright holders may not be used
 *    to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * ALTERNATIVELY, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2, as published by the Free
 * Software Foundation.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/pagemap.h>
#include <linux/io.h>
#include <asm/pgtable.h>

#define DEVICE_NAME "ls2k-gpiomem"
#define DRIVER_NAME "gpiomem-ls2k"
#define DEVICE_MINOR 0

struct ls2k_gpiomem_instance {
	unsigned long gpio_regs_phys;
	struct device *dev;
};

static struct cdev ls2k_gpiomem_cdev;
static dev_t ls2k_gpiomem_devid;
static struct class *ls2k_gpiomem_class;
static struct device *ls2k_gpiomem_dev;
static struct ls2k_gpiomem_instance *inst;


/****************************************************************************
*
*   GPIO mem chardev file ops
*
***************************************************************************/

static int ls2k_gpiomem_open(struct inode *inode, struct file *file)
{
	int dev = iminor(inode);
	int ret = 0;

       // printk(KERN_DEBUG "INT_EDGE:%x\n", ls2k_readl(INT_EDGE));
	
        printk(KERN_DEBUG "gpiomem_open\n");

	if (dev != DEVICE_MINOR) {
		dev_err(inst->dev, "Unknown minor device: %d", dev);
		ret = -ENXIO;
	}
	return ret;
}

static int ls2k_gpiomem_release(struct inode *inode, struct file *file)
{
	int dev = iminor(inode);
	int ret = 0;

        printk(KERN_DEBUG "gpiomem_release\n");
	if (dev != DEVICE_MINOR) {
		dev_err(inst->dev, "Unknown minor device %d", dev);
		ret = -ENXIO;
	}
	return ret;
}

static const struct vm_operations_struct ls2k_gpiomem_vm_ops = {
#ifdef CONFIG_HAVE_IOREMAP_PROT
	.access = generic_access_phys
#endif
};

static int ls2k_gpiomem_mmap(struct file *file, struct vm_area_struct *vma)
{
	
        printk(KERN_DEBUG "gpiomem_mmap\n");
	/* Ignore what the user says - they're getting the GPIO regs
	   whether they like it or not! */
	unsigned long gpio_page = inst->gpio_regs_phys >> PAGE_SHIFT;

	// error: implicit declaration of function ‘phys_mem_access_prot’; did you mean ‘phys_mem_access_prot_allowed’?	
	vma->vm_page_prot = phys_mem_access_prot(file, gpio_page,
						 PAGE_SIZE,
						 vma->vm_page_prot);

	vma->vm_ops = &ls2k_gpiomem_vm_ops;
	if (remap_pfn_range(vma, vma->vm_start,
			gpio_page,
			PAGE_SIZE,
			vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

static const struct file_operations
ls2k_gpiomem_fops = {
	.owner = THIS_MODULE,
	.open = ls2k_gpiomem_open,
	.release = ls2k_gpiomem_release,
	.mmap = ls2k_gpiomem_mmap,
};


 /****************************************************************************
*
*   Probe and remove functions
*
***************************************************************************/


static int ls2k_gpiomem_probe(struct platform_device *pdev)
{
	int err;
	void *ptr_err;
	struct device *dev = &pdev->dev;
	struct resource *ioresource;

        printk(KERN_DEBUG "gpiomem_probe\n");
	/* Allocate buffers and instance data */

	inst = kzalloc(sizeof(struct ls2k_gpiomem_instance), GFP_KERNEL);

	if (!inst) {
		err = -ENOMEM;
		goto failed_inst_alloc;
	}

	inst->dev = dev;

	ioresource = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (ioresource) {
		inst->gpio_regs_phys = ioresource->start;
	} else {
		dev_err(inst->dev, "failed to get IO resource");
		err = -ENOENT;
		goto failed_get_resource;
	}

	/* Create character device entries */

	err = alloc_chrdev_region(&ls2k_gpiomem_devid,
				  DEVICE_MINOR, 1, DEVICE_NAME);
	if (err != 0) {
		dev_err(inst->dev, "unable to allocate device number");
		goto failed_alloc_chrdev;
	}
	cdev_init(&ls2k_gpiomem_cdev, &ls2k_gpiomem_fops);
	ls2k_gpiomem_cdev.owner = THIS_MODULE;
	err = cdev_add(&ls2k_gpiomem_cdev, ls2k_gpiomem_devid, 1);
	if (err != 0) {
		dev_err(inst->dev, "unable to register device");
		goto failed_cdev_add;
	}

	/* Create sysfs entries */

	ls2k_gpiomem_class = class_create(THIS_MODULE, DEVICE_NAME);
	ptr_err = ls2k_gpiomem_class;
	if (IS_ERR(ptr_err))
		goto failed_class_create;

	ls2k_gpiomem_dev = device_create(ls2k_gpiomem_class, NULL,
					ls2k_gpiomem_devid, NULL,
					"gpiomem");
	ptr_err = ls2k_gpiomem_dev;
	if (IS_ERR(ptr_err))
		goto failed_device_create;

	dev_info(inst->dev, "Initialised: Registers at 0x%08lx",
		inst->gpio_regs_phys);

	return 0;

failed_device_create:
	class_destroy(ls2k_gpiomem_class);
failed_class_create:
	cdev_del(&ls2k_gpiomem_cdev);
	err = PTR_ERR(ptr_err);
failed_cdev_add:
	unregister_chrdev_region(ls2k_gpiomem_devid, 1);
failed_alloc_chrdev:
failed_get_resource:
	kfree(inst);
failed_inst_alloc:
	dev_err(inst->dev, "could not load ls2k_gpiomem");
	return err;
}

static int ls2k_gpiomem_remove(struct platform_device *pdev)
{
	struct device *dev = inst->dev;
	
        printk(KERN_DEBUG "gpiomem_remove\n");
	kfree(inst);
	device_destroy(ls2k_gpiomem_class, ls2k_gpiomem_devid);
	class_destroy(ls2k_gpiomem_class);
	cdev_del(&ls2k_gpiomem_cdev);
	unregister_chrdev_region(ls2k_gpiomem_devid, 1);

	dev_info(dev, "GPIO mem driver removed - OK");
	return 0;
}

 /****************************************************************************
*
*   Register the driver with device tree
*
***************************************************************************/

static const struct of_device_id ls2k_gpiomem_of_match[] = {
	{.compatible = "brcm,ls2k-gpiomem",},
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, ls2k_gpiomem_of_match);

static struct platform_driver ls2k_gpiomem_driver = {
	.probe = ls2k_gpiomem_probe,
	.remove = ls2k_gpiomem_remove,
	.driver = {
		   .name = DRIVER_NAME,
		   .owner = THIS_MODULE,
		   .of_match_table = ls2k_gpiomem_of_match,
		   },
};

module_platform_driver(ls2k_gpiomem_driver);

MODULE_ALIAS("platform:gpiomem-ls2k");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("gpiomem driver for accessing GPIO from userspace");
