#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/fs.h>     
#include <linux/types.h>  
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/root_dev.h>
#include <linux/ctype.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/tty.h>
#include <linux/sched.h>

#include <linux/timekeeping.h>

extern void (*block_fun)(struct task_struct *,struct bio*);
int dummy=1;
struct request_queue *queue,*queue1;
struct gendisk *disk; 
struct request *req;
struct bio *bio;
char buf[90];
struct list_head *req_queue,*list;
struct task_struct *simple_tsk;
dev_t dev;
char RW;
struct bvec_iter bvec;
static int major_num = 0;
unsigned long long sector;
unsigned int size;
unsigned long seq,diff_sec,diff_nsec;
int timer=30;
struct timespec init_time,current_time;
/*unsigned long long int _makedev (unsigned int __major,unsigned int __minor)
{
	return ((__major<<20)|(((1U<<20)-1)&&__minor));
}
*/
/*static void printString(char *string) {
    struct tty_struct *tty;

    tty = get_current_tty();

    if(tty != NULL) {

        (tty->driver->ops->write) (tty, string, strlen(string));
    }

    else
        printk("tty equals to zero");
}
*/
dev_t name_to_dev_t(char *name)
{
	char s[32];
	char *p;
	dev_t res = 0;
	int part;

	if (strncmp(name, "/dev/", 5) != 0) {
		unsigned maj, min;

		if (sscanf(name, "%u:%u", &maj, &min) == 2) {
			res = MKDEV(maj, min);
			if (maj != MAJOR(res) || min != MINOR(res))
				goto fail;
		} else {
			res = new_decode_dev(simple_strtoul(name, &p, 16));
			if (*p)
				goto fail;
		}
		goto done;
	}

	name += 5;
	res = Root_NFS;
	if (strcmp(name, "nfs") == 0)
		goto done;
	res = Root_RAM0;
	if (strcmp(name, "ram") == 0)
		goto done;

	if (strlen(name) > 31)
		goto fail;
	strcpy(s, name);
	for (p = s; *p; p++)
		if (*p == '/')
			*p = '!';
	res = blk_lookup_devt(s, 0);
	if (res)
 	{	
		printk("after lookup\n");
		goto done;
	}
while (p > s && isdigit(p[-1]))
		p--;
	if (p == s || !*p || *p == '0')
		goto fail;

	/* try disk name without <part number> */
	part = simple_strtoul(p, NULL, 10);
	*p = '\0';
	res = blk_lookup_devt(s, part);
	if (res)
		goto done;

	/* try disk name without p<part number> */
	if (p < s + 2 || !isdigit(p[-2]) || p[-1] != 'p')
		goto fail;
	p[-1] = '\0';
	res = blk_lookup_devt(s, part);
	if (res)
		goto done;

fail:
	return 0;
done:
	return res;
}

static void my_block_fun(struct task_struct *task,struct bio* bio)
{
	struct request_queue *req;
	struct request *request;
	int cpu;
	int pid;
	req=bdev_get_queue(bio->bi_bdev);
	getrawmonotonic(&current_time);
	if(req)
	{
	        if(queue==req)
		{
			seq++;
			RW=(bio->bi_rw)?'R':'W';
			sector=(bio->bi_io_vec->bv_offset);
			size=bio_sectors(bio);
			cpu=task_thread_info(task)->cpu;
			diff_sec=current_time.tv_sec-init_time.tv_sec;
			diff_nsec=current_time.tv_nsec-init_time.tv_nsec;	
			printk("%d,%d %d %ld %ld.%ld %d %c %d+%d %s\n",MAJOR(dev),MINOR(dev),cpu,seq,diff_sec,diff_nsec,task->pid,RW,sector,size,task->comm); 
		}
	}
	if((current_time.tv_sec-init_time.tv_sec)>timer)
	{
		block_fun=NULL;
		printk("complete\n");
	}
}

static __init int start_module(void)
{
   	dev=name_to_dev_t("/dev/sda1");
    printk(KERN_INFO "Major %d minor %d\n",MAJOR(dev),MINOR(dev));
 	disk = get_gendisk(dev,&dummy);
 	printk(KERN_INFO "disk name %s\n",disk->disk_name);
	if(!disk)
	return 0;
	queue=disk->queue;
	getrawmonotonic(&init_time);
	block_fun=my_block_fun;
return 0;
}
static __exit void end_module(void)
{
	block_fun=NULL;
	printk(KERN_INFO "exited\n");
}







module_init(start_module);
module_exit(end_module);
MODULE_LICENSE("Dual BSD/GPL");

