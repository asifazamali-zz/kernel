#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/time.h>
static int __init config_init(void)
{
	struct timespec time;
	time=current_kernel_time();
	//clock_gettime(CLOCK_REALTIME,time);
	printk(KERN_INFO "%ld %ld\n ",time.tv_sec,time.tv_nsec);
	return 0;
}

static void __exit config_exit(void)
{
	printk("exited\n");
}

module_init(config_init);
module_exit(config_exit);
MODULE_LICENSE("GPL");

