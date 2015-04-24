#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/jiffies.h>
#include<linux/time.h>
#include<linux/trace_clock.h>
static int __init config_init(void)
{
	struct timespec init_time;
	getnstimeofday(&init_time);
	struct timespec current_time;
	unsigned long long initial_clock,current_clock;
//	initial_clock=trace_clock_counter();
        do
	{
		getnstimeofday(&current_time);
	//	current_clock=trace_clock_counter();
		printk("timediff %ld   \n",current_time.tv_sec,init_time.tv_sec);//,current_time->tv_nsec-init_time->tv_nsec);
	}while((current_time.tv_sec-init_time.tv_sec)<10);
	return 0;
}

static void __exit config_exit(void)
{
	printk("exited\n");
}

module_init(config_init);
module_exit(config_exit);
MODULE_LICENSE("GPL");

