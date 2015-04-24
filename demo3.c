#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/jiffies.h>
#include<linux/time.h>
#include<linux/trace_clock.h>
static int __init config_init(void)
{
	struct timespec init_time,init_time1,init_time2;
	unsigned long diff;
	struct timespec current_time;
	int flag1=-1,flag2=-1;
	unsigned long long initial_clock,current_clock;
	getrawmonotonic(&init_time1);
	getrawmonotonic(&init_time2);
//	printk("%ld\n",init_time2.tv_nsec-init_time1.tv_nsec);
//	initial_clock=trace_clock_counter();
   getrawmonotonic(&init_time);
   getrawmonotonic(&current_time);
   diff=current_time.tv_sec-init_time.tv_sec;
   //printk("difference %ld\n",diff);
   while(diff<10)
	{
		
		//printk("%ld\n",diff);
		flag2=diff;
		if(flag1!=flag2)
		{
			printk("%d\n",diff);
			flag1=flag2;
		}

		getrawmonotonic(&current_time);
		diff=current_time.tv_sec-init_time.tv_sec;
	//	current_clock=trace_clock_counter();
//		printk("timediff %ld  %ld\n",current_time.tv_sec-init_time.tv_sec,current_time.tv_nsec-init_time.tv_nsec);//,current_time->tv_nsec-init_time->tv_nsec);
	}
	return 0;
}

static void __exit config_exit(void)
{
	printk("exited\n");
}

module_init(config_init);
module_exit(config_exit);
MODULE_LICENSE("GPL");

