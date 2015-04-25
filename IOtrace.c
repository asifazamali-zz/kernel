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
extern void (*block_requeue)(struct request_queue *q,struct request *rq);
extern void (*block_comp)(struct request *rq);
int dummy=1;
struct request_queue *queue;
struct gendisk *disk; 
struct bio *bio;
char *device="/dev/sda1";
dev_t dev;
char RW,F;
unsigned long long sector;
unsigned int size;
unsigned long seq,diff_sec;
unsigned long  diff_nsec;
int timer=10;
struct timespec init_time,current_time;
int cpu;
int pid;
module_param(device,charp,S_IRUGO|S_IWUSR);
module_param(timer,int,0);



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
		goto done;
	}
while (p > s && isdigit(p[-1]))
		p--;
	if (p == s || !*p || *p == '0')
		goto fail;

	part = simple_strtoul(p, NULL, 10);
	*p = '\0';
	res = blk_lookup_devt(s, part);
	if (res)
		goto done;

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
static void my_block_requeue(struct request_queue *q,struct request *rq)
{
	seq++;
        diff_sec=current_time.tv_sec-init_time.tv_sec;
         diff_nsec=current_time.tv_nsec-init_time.tv_nsec;

	 bio=rq->bio;
	if(bio==NULL)
		 printk("%d,%d %d %ld %ld.%ld 'R'    \n",MAJOR(dev),MINOR(dev),rq->cpu,seq,diff_sec,diff_nsec);
	else
	{
	 RW=(bio->bi_rw)?'R':'W';
	 F='R';
         sector=(bio->bi_io_vec->bv_offset);
         size=bio_sectors(bio);
		 printk("%d,%d %d %ld %ld.%ld %c  %c %lld+%d \n",MAJOR(dev),MINOR(dev),rq->cpu,seq,diff_sec,diff_nsec,F,RW,sector,size);
        }

}

static void my_block_comp(struct request *rq)
{
	 seq++; 
        diff_sec=current_time.tv_sec-init_time.tv_sec;
         diff_nsec=current_time.tv_nsec-init_time.tv_nsec;
   	bio=rq->bio;
	if(bio==NULL)
         {
		
	     printk("%d,%d %ld %ld.%ld 'C'    \n",MAJOR(dev),MINOR(dev),seq,diff_sec,diff_nsec);
 	 }		
         else	 
	 {	
         RW=(bio->bi_rw)?'R':'W';
	 F='C';
         sector=(bio->bi_io_vec->bv_offset);
         size=bio_sectors(bio);
       	 printk("%d,%d %d %ld  %ld.%ld %c  %c %lld+%d \n",MAJOR(dev),MINOR(dev),rq->cpu,seq,diff_sec,diff_nsec,F,RW,sector,size);
	}
       
}



static void my_block_fun(struct task_struct *task,struct bio* bio)
{
	getrawmonotonic(&current_time);
	if(strcmp(disk->disk_name,bio->bi_bdev->bd_disk->disk_name)==0)
	{
	        	seq++;
			RW=(bio->bi_rw)?'R':'W';
			F=(task->bio_list)?'M':'I';
			sector=(bio->bi_io_vec->bv_offset);
			size=bio_sectors(bio);
			cpu=task_thread_info(task)->cpu;
			diff_sec=current_time.tv_sec-init_time.tv_sec;
                        diff_nsec=current_time.tv_nsec-init_time.tv_nsec;
			printk("%d,%d %d %ld %ld.%ld %d %c %c %lld+%d %s\n",MAJOR(dev),MINOR(dev),cpu,seq,diff_sec,diff_nsec,task->pid,F,RW,sector,size,task->comm); 
		
	}
	if((current_time.tv_sec-init_time.tv_sec)>timer)
	{
		block_fun=NULL;
		block_requeue=NULL;
		block_comp=NULL;
	}
}

static __init int start_module(void)
{
  	dev=name_to_dev_t(device);
	disk = get_gendisk(dev,&dummy);
	if(!disk)
	return 0;
	queue=disk->queue;
	getrawmonotonic(&init_time);
	block_fun=my_block_fun;
	block_requeue=my_block_requeue;
	block_comp=my_block_comp;
return 0;
}
static __exit void end_module(void)
{
	block_fun=NULL;
	block_requeue=NULL;
	block_comp=NULL;
}







module_init(start_module);
module_exit(end_module);
MODULE_LICENSE("Dual BSD/GPL");

