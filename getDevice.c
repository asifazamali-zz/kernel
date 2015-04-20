
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h> 
#include <linux/fs.h>     
#include <linux/errno.h>  
#include <linux/types.h>  
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>
#include <linux/root_dev.h>
#include <linux/ctype.h>
//jbd specific includes
#include <linux/jbd.h>
#include <linux/byteorder/generic.h>
#include <linux/ext2_fs.h>
#include <linux/fs.h>


MODULE_LICENSE("Dual BSD/GPL");

//static char *Version = "1.0";
static int major_num = 0;
module_param(major_num, int, 0);
extern int start_recover_map(int mode);
unsigned long long int _makedev (unsigned int __major,unsigned int __minor)
{
	return ((__major<<20)|(((1U<<20)-1)&&__minor));
}

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
		goto done;

	/*
	 * try non-existant, but valid partition, which may only exist
	 * after revalidating the disk, like partitioned md devices
	 */
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


static __init int start_module(void)
{
 int dummy;
 struct request_queue *queue;
 struct gendisk *disk; 
 struct request *req;
 struct bio *bio;
 char *buf;
//class_dev_iter_init(&iter, &block_class, NULL,NULL);
// while ((dev = class_dev_iter_next(&iter))) {
// struct gendisk *disk = dev_to_disk(dev);
//	printk("%s\n",dev_name(dev));
// if (strcmp(dev_name(dev), name)
 
// }
 //dev_t dev=MKDEV(23,0);
 dev_t dev=name_to_dev_t("/dev/sda1");
 printk(KERN_INFO "Major %d minor %d\n",MAJOR(dev),MINOR(dev));
 disk = get_gendisk(dev,&dummy);
 printk(KERN_INFO "%s\n",disk->disk_name);
 if(disk->part_tbl->len<1+0) 
 {
   put_disk(disk);
   disk=0;
   return -1;
 }
 printf("readbytes_tryKM2\n");
    buf = (unsigned char*)vmalloc(0x800);
    memset( buf , 0xFE , 0x800 );


    bio = bio_map_kern( disk->queue , buf , 0x400 , GFP_KERNEL );
    if( IS_ERR(bio) )
        {vfree(buf);return;}

    bio->bi_sector = 0;
 bio->bi_bdev = bdget_disk(disk,0);
 printk("   bi_bdev = %016lX\n",(unsigned long)(bio->bi_bdev));
 printk("   bi_bdev->bd_disk = %s\n",(bio->bi_bdev->bd_disk->disk_name));
 if(bio->bi_sector)
   printk(" sector %lld \n",bio->bi_sector);
 if(bio->bi_flags)
    printk("%lu\n",bio->bi_flags);
 if(bio->bi_rw)
    printk("%lu\n",bio->bi_rw);
// queue=disk->queue;
// req=queue->boundary_rq;
 //printk(KERN_INFO "Pending requests:%d\n",(queue->nr_pending)); 
 return 0;
}
static __exit void end_module(void)
{
	printk(KERN_INFO "exited\n");
}







module_init(start_module);
module_exit(end_module);

