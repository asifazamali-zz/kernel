

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
#include <linux/ext3_fs.h>
#include <linux/fs.h>

#include "../include/maptable.h"

MODULE_LICENSE("Dual BSD/GPL");

//static char *Version = "1.0";
static int major_num = 0;
module_param(major_num, int, 0);
extern int start_recover_map(int mode);
//#define CLOUD_BLOCK_SIZE 4096

/*
 * The internal representation of our device.
 */
static struct testbd_device {
    struct block_device *bdev;
    struct gendisk *gd;
} Device;




// FROM init/do_mounts.c
/*
 *	Convert a name into device number.  We accept the following variants:
 *
 *	1) device number in hexadecimal	represents itself
 *	2) /dev/nfs represents Root_NFS (0xff)
 *	3) /dev/<disk_name> represents the device number of disk
 *	4) /dev/<disk_name><decimal> represents the device number
 *         of partition - device number of disk plus the partition number
 *	5) /dev/<disk_name>p<decimal> - same as the above, that form is
 *	   used when disk name of partitioned disk ends on a digit.
 *	If name doesn't have fall into the categories above, we return (0,0).
 *	block_class is used to check if something is a disk name. If the disk
 *	name contains slashes, the device name has them replaced with
 *	bangs.
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


// FROM drivers/md
/*
 * prevent the device from being mounted, repartitioned or
 * otherwise reused by a RAID array (or any other kernel
 * subsystem), by bd_claiming the device.
 */
int lock_rdev(struct testbd_device *rdev, dev_t dev)
{
	int err = 0;
	struct block_device *bdev;
	char b[BDEVNAME_SIZE];

	rdev->bdev = 0;
	bdev = open_by_devnum(dev, FMODE_READ|FMODE_WRITE);
	if (IS_ERR(bdev)) {
		printk(KERN_ERR "testbd: could not open %s.\n",
			__bdevname(dev, b));
		return PTR_ERR(bdev);
	}
	err = bd_claim(bdev, rdev);
	if (err) {
		printk(KERN_ERR "testbd: could not bd_claim %s.\n",
			bdevname(bdev, b));
		blkdev_put(bdev);
		return err;
	}

	rdev->bdev = bdev;
	return err;
}

static void unlock_rdev(struct testbd_device *rdev)
{
	struct block_device *bdev = rdev->bdev;
	rdev->bdev = NULL;
	bd_release(bdev);
	blkdev_put(bdev);
}


/*
 * Ioctl.
 */
int testbd_ioctl (struct inode *inode, struct file *filp,
			unsigned int cmd, unsigned long arg)
{
    printk(KERN_ALERT "testbd_ioctl\n");
    return 0;
}





static int testbd_open(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "testbd_open\n");
    return 0;
}


static int testbd_release(struct inode* inode,struct file *filp)
{
    printk(KERN_ALERT "testbd_release\n");
    return 0;
}


/*
 * The device operations structure.
 */
static struct block_device_operations testbd_ops = {
	.owner			    = THIS_MODULE,
	.open		       	    = testbd_open,
	.release		    = testbd_release,
        .ioctl	                    = testbd_ioctl,
	.media_changed		    = NULL,
	.revalidate_disk	    = NULL
};


void testbd_end_io(struct bio *testbd_bio, int error)
{

	struct bio *org_bio = (struct bio *)testbd_bio->bi_private;
	int uptodate = test_bit(BIO_UPTODATE, &testbd_bio->bi_flags);
 //   printk(KERN_ALERT "testbd_end_io\n");
	

	if (uptodate) {
		bio_endio(org_bio, 0);
	}
	else {
		bio_endio(org_bio, -EIO);
	}

	bio_put(testbd_bio);

	return;
}

static int is_journal_commit_block(struct bio* bio)
{
	printk("testing for journal commit block");
	 char * buffer = page_address(bio->bi_io_vec[0].bv_page)+bio->bi_io_vec[0].bv_offset;
	 journal_header_t *tmp = (journal_header_t*)buffer;
	 printk("blocktype = %d",tmp->h_blocktype);
	 if(tmp->h_blocktype == JFS_COMMIT_BLOCK)   //cpu_to_be32(JFS_COMMIT_BLOCK))
	 {
		printk("journal commit block\n");
		return 1;
	 }
	return 0;
}

static int is_journal_super_block(struct bio* bio)
{
	int blocksize = 4096;
	int min_block_size = 1024;
	int sb_block = min_block_size/blocksize;
	int start = sb_block+1;
	printk("sector should be = %d",(start*blocksize/512));
	if(bio->bi_sector == (start*blocksize/512))
	{
		printk("journal super block\n");
		return 1;
	}
	return 0;

}


static int testbd_make_request(struct request_queue *q, struct bio *bio)
{
	struct bio *testbd_bio_clone;
	//printk(KERN_ALERT "testbd_make_request block = %d\n",bio->bi_sector/8);

	testbd_bio_clone = bio_clone(bio, GFP_NOIO);
	testbd_bio_clone->bi_bdev = Device.bdev;
	testbd_bio_clone->bi_sector = bio->bi_sector;
	testbd_bio_clone->bi_end_io = testbd_end_io;
	testbd_bio_clone->bi_private = bio;
	testbd_bio_clone->bi_rw = bio->bi_rw;//bio_data_dir(bio);
	bio->bi_bdev = Device.bdev;
	generic_make_request(testbd_bio_clone);

	return 0;

}


static int __init testbd_init(void)
{
    int err;
    dev_t dev;
    char* devname = "/dev/sda3";
//    char* devname = "/dev/sda1";
  
    Device.bdev = 0;
    dev = name_to_dev_t(devname);
    if (!dev) {
      printk(KERN_WARNING "testbd: Unknown device name: %s\n", devname);
      goto out;
    }



    err = lock_rdev(&Device, dev);
    if (err)
        goto out;
   
/*
 * Get registered.
 */
    major_num = register_blkdev(major_num, "testbd");
    if (major_num <= 0) {
        printk(KERN_WARNING "testbd: unable to get major number\n");
        goto out;
    }
    
/*
 * And the gendisk structure.
 */
    Device.gd = alloc_disk(16);
    if (! Device.gd)
        goto out_unregister;
    Device.gd->major = major_num;
    Device.gd->first_minor = 0;
    Device.gd->fops = &testbd_ops;
    Device.gd->private_data = &Device;
    strcpy (Device.gd->disk_name, "testbd");
    Device.gd->queue = blk_alloc_queue(GFP_KERNEL);
    Device.gd->queue->queuedata = &Device;


    // printk(KERN_ALERT "size from nr_sects: %i\n", Device.bdev->bd_disk->part0.nr_sects);    
	set_capacity(Device.gd,6144*4*1024);
	blk_queue_make_request(Device.gd->queue, testbd_make_request);


    add_disk(Device.gd);

    return 0;

  out_unregister:
    unregister_blkdev(major_num, "testbd");
  out:
    return -ENOMEM;
}

static void __exit testbd_exit(void)
{
    printk(KERN_ALERT "Bye, world\n");
    if(Device.bdev)
       unlock_rdev(&Device);
    del_gendisk(Device.gd);
    put_disk(Device.gd);
    unregister_blkdev(major_num, "testbd");
    if(Device.gd->queue)
        blk_cleanup_queue(Device.gd->queue);
}
	
module_init(testbd_init);
module_exit(testbd_exit);
