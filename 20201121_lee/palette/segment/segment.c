/*
 * 7-Segment Device Driver
 *  Hanback Electronics Co.,ltd
 * File : segment.c
 * Date : April,2009
 */ 

// ¸ğµâÀÇ Çì´õÆÄÀÏ ¼±¾ğ
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <asm/fcntl.h>
#include <linux/ioport.h>
#include <linux/delay.h>

#include <asm/ioctl.h>
#include <asm/uaccess.h>
#include <asm/io.h>

#define DRIVER_AUTHOR		"hanback"		// ¸ğµâÀÇ ÀúÀÛÀÚ
#define DRIVER_DESC		"7-Segment program"	// ¸ğµâ¿¡ ´ëÇÑ ¼³¸í

#define	SEGMENT_MAJOR		242			// µğ¹ÙÀÌ½º ÁÖ¹øÈ£
#define	SEGMENT_NAME		"SEGMENT"		// µğ¹ÙÀÌ½º ÀÌ¸§
#define SEGMENT_MODULE_VERSION	"SEGMENT PORT V0.1"	// µğ¹ÙÀÌ½º ¹öÀü

#define SEGMENT_ADDRESS_GRID	0x88000030	// 7-segÀÇ Digit ¸¦ ¼±ÅÃÇÏ±â À§ÇÑ ·¹Áö½ºÅÍ 
#define SEGMENT_ADDRESS_DATA	0x88000032	// 7-seg¸¦ µğ½ºÇÃ·¹ÀÌ ÇÏ±â À§ÇÑ ·¹Áö½ºÅÍ
#define SEGMENT_ADDRESS_RANGE	0x1000		// I/O ¿µ¿ªÀÇ Å©±â

//Global variable
static unsigned int segment_usage = 0;
static unsigned long *segment_data;
static unsigned long *segment_grid;
int input_count = 3;

// define functions...
// ÀÀ¿ë ÇÁ·Î±×·¥¿¡¼­ µğ¹ÙÀÌ½º¸¦ Ã³À½ »ç¿ëÇÏ´Â °æ¿ì¸¦ Ã³¸®ÇÏ´Â ÇÔ¼ö
int segment_open (struct inode *inode, struct file *filp)
{
	// µğ¹ÙÀÌ½º°¡ ¿­·Á ÀÖ´ÂÁö È®ÀÎ.
	if(segment_usage != 0) return -EBUSY;
	
	// GRID¿Í DATAÀÇ °¡»ó ÁÖ¼Ò ¸ÅÇÎ
	segment_grid =  ioremap(SEGMENT_ADDRESS_GRID, SEGMENT_ADDRESS_RANGE);
	segment_data =  ioremap(SEGMENT_ADDRESS_DATA, SEGMENT_ADDRESS_RANGE);
	
	// µî·ÏÇÒ ¼ö ÀÖ´Â I/O ¿µ¿ªÀÎÁö È®ÀÎ
	if(!check_mem_region((unsigned long)segment_data,SEGMENT_ADDRESS_RANGE) && !check_mem_region((unsigned long)segment_grid, SEGMENT_ADDRESS_RANGE))
	{
		// I/O ¸Ş¸ğ¸® ¿µ¿ªÀ» µî·Ï
		request_region((unsigned long)segment_grid, SEGMENT_ADDRESS_RANGE, SEGMENT_NAME);
		request_region((unsigned long)segment_data, SEGMENT_ADDRESS_RANGE, SEGMENT_NAME);
	}
	else printk("driver : unable to register this!\n");

	segment_usage = 1;	
	return 0; 
}

// ÀÀ¿ë ÇÁ·Î±×·¥¿¡¼­ µğ¹ÙÀÌ½º¸¦ ´õÀÌ»ó »ç¿ëÇÏÁö ¾Ê¾Æ¼­ ´İ±â¸¦ ±¸ÇöÇÏ´Â ÇÔ¼ö
int segment_release (struct inode *inode, struct file *filp)
{
	// ¸ÅÇÎµÈ °¡»óÁÖ¼Ò¸¦ ÇØÁ¦
	iounmap(segment_grid);
	iounmap(segment_data);

	// µî·ÏµÈ I/O ¸Ş¸ğ¸® ¿µ¿ªÀ» ÇØÁ¦
	release_region((unsigned long)segment_data, SEGMENT_ADDRESS_RANGE);
	release_region((unsigned long)segment_grid, SEGMENT_ADDRESS_RANGE);

	segment_usage = 0;
	return 0;
}

// x¿¡ ´ëÇÑ LEDÀÇ ÄÚµå·Î º¯È¯ÇÏ¿© ¹İÈ¯
unsigned short Getsegmentcode (short x)
{
	unsigned short code;
	switch (x) {
		case 0x0 : code = 0xfc; break;
		case 0x1 : code = 0x60; break;
		case 0x2 : code = 0xda; break;
		case 0x3 : code = 0xf2; break;
		case 0x4 : code = 0x66; break;
		case 0x5 : code = 0xb6; break;
		case 0x6 : code = 0xbe; break;
		case 0x7 : code = 0xe4; break;
		case 0x8 : code = 0xfe; break;
		case 0x9 : code = 0xf6; break;
		
		case 0xa : code = 0xfa; break;
		case 0xb : code = 0x3e; break;
		case 0xc : code = 0x1a; break;
		case 0xd : code = 0x7a; break;						
		case 0xe : code = 0x9e; break;
		case 0xf : code = 0x8e; break;				
		default : code = 0; break;
	}
	return code;
}						

// µğ¹ÙÀÌ½º µå¶óÀÌ¹öÀÇ ¾²±â¸¦ ±¸ÇöÇÏ´Â ÇÔ¼ö
ssize_t segment_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
//	static int input_count = 3;
	unsigned char data[6];
	unsigned char digit[6]={0x20, 0x10, 0x08, 0x04, 0x02, 0x01};
	unsigned int i,j,num,ret;
	unsigned int count=0,temp1,temp2;

	// »ç¿ëÀÚ ¸Ş¸ğ¸® gdata¸¦ Ä¿³Î ¸Ş¸ğ¸® numÀ¸·Î n¸¸Å­ º¹»ç 
	ret=copy_from_user(&num,gdata,4);	

        if(num!=0) { // num ä¸æ˜¯ 0æ—¶
	                data[5]=Getsegmentcode(num/100000);
                        temp1=num%100000;

                        data[4]=Getsegmentcode(temp1/10000);
                        temp2=temp1%10000;

                        data[3]=Getsegmentcode(temp2/1000);
                        temp1=temp2%1000;

                        data[2]=Getsegmentcode(temp1/100);
                        temp2=temp1%100;

                        data[1]=Getsegmentcode(temp2/10);

                        data[0]=Getsegmentcode(temp2%10);

                        for(i=0;i<6;i++) {
                              *segment_grid = digit[i];
                              *segment_data = data[i];
                              udelay(500);
                        }
       
       }
      
	
      
        *segment_grid = ~digit[0];
        *segment_data = 0;

	return length;
}

// ÆÄÀÏ ¿ÀÆÛ·¹ÀÌ¼Ç ±¸Á¶Ã¼
// ÆÄÀÏÀ» ¿­¶§ open()À» »ç¿ëÇÑ´Ù. open()´Â ½Ã½ºÅÛ ÄİÀ» È£ÃâÇÏ¿© Ä¿³Î ³»ºÎ·Î µé¾î°£´Ù.
// ÇØ´ç ½Ã½ºÅÛ Äİ°ú °ü·ÃµÈ ÆÄÀÏ ¿¬»êÀÚ ±¸Á¶Ã¼ ³»ºÎÀÇ open¿¡ ÇØ´çÇÏ´Â ÇÊµå°¡ µå¶óÀÌ¹ö ³»¿¡¼­
// segment_open()À¸·Î Á¤ÀÇµÇ¾î ÀÖÀ¸¹Ç·Î segment_open()°¡ È£ÃâµÈ´Ù.
// write¿Í releaseµµ ¸¶Âù°¡Áö·Î µ¿ÀÛÇÑ´Ù. ¸¸¾à µî·ÏµÇÁö ¾ÊÀº µ¿ÀÛ¿¡ ´ëÇØ¼­´Â Ä¿³Î¿¡¼­ Á¤ÀÇÇØ
// ³õÀº default µ¿ÀÛÀ» ÇÏµµ·Ï µÇ¾î ÀÖ´Ù.
struct file_operations segment_fops = 
{
	.owner		= THIS_MODULE,
	.open		= segment_open,
	.write		= segment_write,
	.release	= segment_release,
};

// ¸ğµâÀ» Ä¿³Î ³»ºÎ·Î »ğÀÔ
// ¸ğµâ ÇÁ·Î±×·¥ÀÇ ÇÙ½ÉÀûÀÎ ¸ñÀûÀº Ä¿³Î ³»ºÎ·Î µé¾î°¡¼­ ¼­ºñ½º¸¦ Á¦°ø¹Ş´Â °ÍÀÌ¹Ç·Î
// Ä¿³Î ³»ºÎ·Î µé¾î°¡´Â init()À» ¸ÕÀú ½ÃÀÛÇÑ´Ù.
// ÀÀ¿ë ÇÁ·Î±×·¥Àº ¼Ò½º ³»ºÎ¿¡¼­ Á¤ÀÇµÇÁö ¾ÊÀº ¸¹Àº ÇÔ¼ö¸¦ »ç¿ëÇÑ´Ù. ±×°ÍÀº ¿ÜºÎ
// ¶óÀÌºê·¯¸®°¡ ÄÄÆÄÀÏ °úÁ¤¿¡¼­ ¸µÅ©µÇ¾î »ç¿ëµÇ±â ¶§¹®ÀÌ´Ù. ¸ğµâ ÇÁ·Î±×·¥Àº Ä¿³Î
// ³»ºÎÇÏ°í¸¸ ¸µÅ©µÇ±â ¶§¹®¿¡ Ä¿³Î¿¡¼­ Á¤ÀÇÇÏ°í Çã¿ëÇÏ´Â ÇÔ¼ö¸¸À» »ç¿ëÇÒ ¼ö ÀÖ´Ù.
int segment_init(void)
{
	int result;

	// ¹®ÀÚ µğ¹ÙÀÌ½º µå¶óÀÌ¹ö¸¦ µî·ÏÇÑ´Ù.
	result = register_chrdev(SEGMENT_MAJOR, SEGMENT_NAME, &segment_fops);
	if (result < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return result;
	}

	// major ¹øÈ£¸¦ Ãâ·ÂÇÑ´Ù.
	printk(KERN_INFO"Init Module, 7-Segment Major Number : %d\n", SEGMENT_MAJOR);
	return 0;
}

// ¸ğµâÀ» Ä¿³Î¿¡¼­ Á¦°Å
void segment_exit(void)
{
	// ¹®ÀÚ µğ¹ÙÀÌ½º µå¶óÀÌ¹ö¸¦ Á¦°ÅÇÑ´Ù.
	unregister_chrdev(SEGMENT_MAJOR,SEGMENT_NAME);

	printk("driver: %s DRIVER EXIT\n", SEGMENT_NAME);
}

module_init(segment_init);	// ¸ğµâ ÀûÀç ½Ã È£ÃâµÇ´Â ÇÔ¼ö
module_exit(segment_exit);	// ¸ğµâ Á¦°Å ½Ã È£ÃâµÇ´Â ÇÔ¼ö

MODULE_AUTHOR(DRIVER_AUTHOR);	// ¸ğµâÀÇ ÀúÀÛÀÚ
MODULE_DESCRIPTION(DRIVER_DESC);// ¸ğµâ¿¡ ´ëÇÑ ¼³¸í
MODULE_LICENSE("Dual BSD/GPL");	// ¸ğµâÀÇ ¶óÀÌ¼±½º µî·Ï

