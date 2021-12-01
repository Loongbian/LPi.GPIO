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
	{ 0x7ff2ba64, "gpiod_unexport" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0x608f8683, "device_destroy" },
	{ 0x42aad7d1, "gpiod_export" },
	{ 0xcf2b3842, "gpiod_direction_output_raw" },
	{ 0xfe990052, "gpio_free" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x65feb45e, "device_create" },
	{ 0x24d08582, "class_destroy" },
	{ 0xee77a63f, "__class_create" },
	{ 0x55e440e5, "cdev_del" },
	{ 0x781385d5, "cdev_add" },
	{ 0x9569678f, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x8f611937, "gpiod_get_raw_value" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x2cfde9a2, "warn_slowpath_fmt" },
	{ 0x24e9e4ee, "gpiod_set_raw_value" },
	{ 0xbcfadf9e, "gpio_to_desc" },
	{ 0xfb578fc5, "memset" },
	{ 0x5397c01e, "__copy_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0xc5850110, "printk" },
	{ 0x9514151a, "_mcount" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "77D7EDFDFD3F21D96A3CC72");
