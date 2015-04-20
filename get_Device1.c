#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/device.h>         //for class_dev_iter_init
#include<linux/genhd.h>          //for block_class
#include<linux/fcntl.h>		//for FCDWD
#include<linux/namei.h>		// for user_path_at
#include<linux/dcache.h>       //for dentry
#include<linux/path.h>		//for path
unsigned long long int _makedev (unsigned int __major,unsigned int __minor)
{
	return ((__major<<20)|(((1U<<20)-1)&&__minor));
}
static __init int start_module(void)
{
 struct path path;
 unsigned int lookup_flags = 0;
 struct gendisk * disk;
 struct dentry *dentry;
 struct super_block *spb;
 struct block_device *bd;
 char filename[]="/dev/sda1";
 
 user_path_at(AT_FDCWD, filename, lookup_flags, &path);
 {
   printk("Inside path\n");
   dentry=path.dentry;
   if(dentry)
   {
     spb=dentry->d_sb;
      printk("Inside dentry\n"); 
     if(spb)
     { 
       printk("inside superblock\n");
       bd=spb->s_bdev;
       if(bd)
       {
	 printk("inside block_device\n");
	 disk=bd->bd_disk;
       }
     }
    }
   }  
 //struct class_dev_iter iter;
 //struct device *dev;
// struct char_device_struct **cp,*chrdevs[CHRDEV_MAJOR_HASH_SIZE];
// int i=0;
 //class_dev_iter_init(&iter, &block_class, NULL,NULL);
// while ((dev = class_dev_iter_next(&iter))) {
// struct gendisk *disk = dev_to_disk(dev);
//	printk("%s\n",dev_name(dev));
// if (strcmp(dev_name(dev), name)
 
// }
// dev_t dev=MKDEV(8,1);
// printk(KERN_INFO "Major %d minor %d\n",MAJOR(dev),MINOR(dev));
// struct gendisk *disk = get_gendisk(dev,0);
 
 return 0;
}
static __exit void end_module(void)
{
	printk(KERN_INFO "exited\n");
}







module_init(start_module);
module_exit(end_module);

