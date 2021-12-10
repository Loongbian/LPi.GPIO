#include <linux/module.h>
#include <linux/build-salt.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0x9a8c5fdb, "module_layout" },
	{ 0x55e440e5, "cdev_del" },
	{ 0x608f8683, "device_destroy" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x24d08582, "class_destroy" },
	{ 0x65feb45e, "device_create" },
	{ 0xee77a63f, "__class_create" },
	{ 0x781385d5, "cdev_add" },
	{ 0x9569678f, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0xfb578fc5, "memset" },
	{ 0x5397c01e, "__copy_user" },
	{ 0xc5850110, "printk" },
	{ 0x9514151a, "_mcount" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "521976C3FF73449A96CC5BB");
