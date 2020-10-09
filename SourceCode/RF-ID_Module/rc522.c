#include "rc522_api.h"

#define  N_1  1
#define  N_2  2
#define  N_3  3
#define  N_4  4
#define  N_NOP  255

#define  RELOAD_COUNT  0xfb

static IOCTLDescription_t ioctls[] = {
		{ .ioctlcode = CHANGE_PASSWD, .name = "CHANGE PASSWD", },
		{ .ioctlcode = CHANGE_BLOCK, .name = "CHANGE BLOCK", },
		{ .ioctlcode = READ_CARD, .name = "READ CARD" },
		{ .ioctlcode = WRITE_CARD, .name = "WRITE CARD" },
		{ .ioctlcode = CHANGE_KEY, .name = "CHANGE KEY" },
		{ .ioctlcode = GET_ID, .name = "GET ID" },
		{ .ioctlcode = BEEP, .name = "BEEP" },

};

typedef unsigned char uchar;
uchar NewKey[16]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x07,0x80,0x69,0x00,0x00,0x00,0x00,0x00,0x00};

static unsigned char Read_Data[16]={0x00};
static unsigned char read_data_buff[16];

static uchar PassWd[6]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static uchar WriteData[17];
static unsigned char RevBuffer[30];
static unsigned char MLastSelectedSnr[4];

uint blockAddr,operationcard;

struct spi_device *rc522_spi;

static struct gpio_desc *gpiodBuzz;

dev_t dev;
static struct cdev rfid_cdev;
static struct class *dev_class;
static struct device *dev_struct;

void delay_ms(uint tms)
{
	mdelay(tms);
}

void buzzerFunc(void)
{
	gpiod_set_value_cansleep(gpiodBuzz,1);
	delay_ms(100);
	gpiod_set_value_cansleep(gpiodBuzz,0);
}

void InitRc522(void)
{
	unsigned char a;
	PcdReset();
	a = ReadRawRC(TReloadRegL);
	if(a != 30)
		printk(KERN_DEBUG"NO RC522 - %d\n",a);
	else
		printk(KERN_DEBUG"RC522 exist\n");
	PcdAntennaOff();  
	PcdAntennaOn();
	M500PcdConfigISOType( 'A' );
}

static char rc522_loop_work(uint opnd)
{
	char *pdata = read_data_buff;
	char status;

	PcdReset();
	status=PcdRequest(PICC_REQIDL,&RevBuffer[0]);
	if(status!=MI_OK)
	{
		printk(KERN_DEBUG"search card: no card\n");
		return -EFAULT;
	}
	status=PcdAnticoll(&RevBuffer[2]);
	if(status!=MI_OK)
	{
		printk(KERN_DEBUG"get card nu: no number\n");
		return -EFAULT;
	} 
	memcpy(MLastSelectedSnr,&RevBuffer[2],4);

	status=PcdSelect(MLastSelectedSnr);
	if(status!=MI_OK)
	{
		printk(KERN_DEBUG"select card: no card\n");
		return -EFAULT;            
	}
	if (opnd == GET_ID) {
		PcdHalt();	
		return 0;	
	}
	else if (opnd == READ_CARD) {
		status=PcdAuthState(PICC_AUTHENT1A,blockAddr,PassWd,MLastSelectedSnr);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"read authorize card err\n");
			return -EFAULT;
		}
		status=PcdRead(blockAddr,Read_Data);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"read card err\n");
			return -EFAULT;
		} else {
			int i;
			memcpy(pdata, Read_Data, sizeof(Read_Data));
			printk(KERN_DEBUG"read block %d info:", blockAddr);
			for(i = 0; i < 16; i++) {
				printk(KERN_DEBUG"%2.2X",pdata[i]);
			}
			printk(KERN_DEBUG"\n");
		}
	} else if (opnd == CHANGE_KEY) {
		status=PcdAuthState(PICC_AUTHENT1A,blockAddr,PassWd,MLastSelectedSnr);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"card authorize err");
			return -EFAULT;
		}
		status=PcdWrite(blockAddr,&NewKey[0]);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"change password err");
			return -EFAULT;
		} else
			printk(KERN_DEBUG"set password success");
	} else if (opnd == WRITE_CARD) {
		status=PcdAuthState(PICC_AUTHENT1A,blockAddr,PassWd,MLastSelectedSnr);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"write authrioze err\n");
			return -EFAULT;
		}
		status=PcdWrite(blockAddr,&WriteData[0]);
		if(status!=MI_OK)
		{
			printk(KERN_DEBUG"write data err\n");
			return -EFAULT;
		} else {
			printk(KERN_DEBUG"write data to block %d sucess\n", blockAddr);
		}
	}
	PcdHalt();
	return 0;
}



static int rc522_open(struct inode *inode,struct file *filp)
{
	InitRc522();
	printk(KERN_DEBUG"rc522 start work!\n");
	return 0;
}

static ssize_t rc522_read (struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	/*PcdReset();*/
	operationcard = READ_CARD;
	if(rc522_loop_work(operationcard))
		return 0;
	printk(KERN_DEBUG"card info:%2.2X\n",Read_Data[0]);
	if (copy_to_user(buf, read_data_buff, sizeof(read_data_buff))) {
		printk(KERN_DEBUG"copy card number to userspace err\n");
		return 0;
	}
	return sizeof(read_data_buff);
}

static ssize_t rc522_write (struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	if (blockAddr == 0) {
		printk(KERN_DEBUG"block[0] is reserveed, can't write\n");
		return 0;
	}
	if (blockAddr < 0 || blockAddr > 63) {
		printk(KERN_DEBUG"block[%d] unreachable, please set the write block first", blockAddr);
		return -0;
	} 
	if ((blockAddr % 4) == 3) {
		printk(KERN_DEBUG"block[%d] is key block, not data block\n", blockAddr);
		return -0;
	}
	memset(WriteData, 0, sizeof(WriteData));
	if (copy_from_user(WriteData, (char *)buf, count)) {
		printk(KERN_DEBUG"%s, [line %d] copy from user err.\n", __FILE__, __LINE__);
		return 0;
	}
	/*PcdReset();*/
	operationcard =  WRITE_CARD;
	if(rc522_loop_work(operationcard))
		return -EFAULT;
	return 0;
}

static int rc522_release(struct inode *inode,struct file *filp)
{
	printk(KERN_DEBUG"%s\n", __func__);
	return 0;
}

static long rc522_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printk(KERN_DEBUG "CMD = 0x%x\n", cmd);
	switch(cmd) {
	case CHANGE_PASSWD:
		operationcard = CHANGE_PASSWD;
		if (copy_from_user(PassWd, (char *)arg, sizeof(PassWd))) {
			printk(KERN_DEBUG"%s:change pass word err", __func__);
			return -EFAULT;
		}
		break;
	case CHANGE_BLOCK:
		if (arg < 0 || arg > 63) {
			printk(KERN_DEBUG"block number err %lu\n", arg);
			return -EFAULT;
		}
		blockAddr = (int)arg;
		printk(KERN_INFO "block = %d", blockAddr);
		break;
	case READ_CARD:
		break;
	case WRITE_CARD:
		break;
	case CHANGE_KEY:
		operationcard = CHANGE_KEY;
		break;
	case GET_ID:
		operationcard =  GET_ID;
		if(!rc522_loop_work(operationcard)){
			if (copy_to_user((char *)arg, MLastSelectedSnr,4)) {
				printk(KERN_DEBUG"%s, [line %d] copy to user err.\n", __FILE__, __LINE__);
				return -EFAULT;
			}
		}
		else
			return -EFAULT;
		break;
	case BEEP:
		buzzerFunc();
		break;
	default:
		break;
	}
	return 0;
}


static int rc522_remove(struct spi_device *spi)
{

	return 0;
}
static int rc522_probe(struct spi_device *spi)
{
	printk(KERN_DEBUG "SPI number = %d",spi->controller->bus_num);
	blockAddr = 1;
	printk(KERN_DEBUG"%s\n", __func__);
	rc522_spi = spi;
	gpiodBuzz = devm_gpiod_get(&spi->dev, "buzz", GPIOD_OUT_LOW); // Get gpio desc from device tree component //
	if(IS_ERR(gpiodBuzz)) {
		printk(KERN_INFO "Cant get gpio - buzz - err num %d", IS_ERR(gpiodBuzz));
		return -1;
	}
	printk(KERN_INFO "GPIO claimed successfully - number %d", desc_to_gpio(gpiodBuzz));
	if(gpiod_direction_output(gpiodBuzz, 0)) { // Set the gpio pin on output //
		printk(KERN_INFO "Cant set direction to gpio");
		return -1;
	}
	return 0;
};

static struct of_device_id spi_rfid_dt_ids[] = { // DTS compatible //
		{ .compatible = "shtl,rfid_rc522" },
		{},
};

MODULE_DEVICE_TABLE(of, spi_rfid_dt_ids); // Add our device to Devices Table - for "Hot Pluggin" //

static struct spi_driver rc522_driver = {
		.driver = {
				.name			= "rfid_rc522",
				.owner  		= THIS_MODULE,
				.of_match_table = of_match_ptr(spi_rfid_dt_ids),
		},
		.probe = rc522_probe,
		.remove = rc522_remove,
};

static struct file_operations rc522_fops = {
		.owner = THIS_MODULE,
		.open = rc522_open,
		.release = rc522_release,
		.read = rc522_read,
		.write = rc522_write,
		.unlocked_ioctl = rc522_ioctl,
};

static int RC522_init(void)
{
	printk(KERN_DEBUG"RFID_RC522 module init.\n");

	if((alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME)) < 0) {
		printk(KERN_INFO "Error allocation Major number !\n");
		return -1;
	}
	printk(KERN_INFO "RFID_RC522 module has been inserted, Major num: %d | Minor num: %d\n", MAJOR(dev), MINOR(dev));

	cdev_init(&rfid_cdev,&rc522_fops);

	if((cdev_add(&rfid_cdev, dev, 1)) < 0) {
		printk(KERN_INFO "Cannot add rfid_cdev to the system\n");
		goto rem_cdev;
	}
	printk(KERN_INFO "Char device for RC522_RFID module has been added");

	if((dev_class = class_create(THIS_MODULE,CLASS_NAME)) == NULL) {
		printk(KERN_INFO "Couldnt create a class");
		goto rem_class;
	}

	if((dev_struct = device_create(dev_class,NULL,dev,NULL,DEVICE_NAME)) == NULL) {
		printk(KERN_INFO "Talos device cannot be created");
		goto rem_device;
	}

	if(spi_register_driver(&rc522_driver) < 0) {
		printk(KERN_DEBUG"SPI driver register failed\n");
		goto rem_device;
	}

	for(int i=0; i < (sizeof(ioctls) / sizeof(IOCTLDescription_t)); i++)
		printk(KERN_INFO"IOCTL Codes:\t%s=0x%02X\n", ioctls[i].name, ioctls[i].ioctlcode);

	return 0;

rem_device:
	device_destroy(dev_class,dev);
rem_class:
	class_destroy(dev_class);
rem_cdev:
	cdev_del(&rfid_cdev);
	unregister_chrdev_region(dev,1);
	return -1;
}

static void RC522_exit(void)
{
	spi_unregister_driver(&rc522_driver);
	device_destroy(dev_class,dev);
	class_destroy(dev_class);
	cdev_del(&rfid_cdev);
	unregister_chrdev_region(dev,1);
	printk(KERN_DEBUG"RC522_RFID module has been removed\n");
}

module_init(RC522_init);
module_exit(RC522_exit);

MODULE_AUTHOR("RealTime Goup-Tal,Alex,Shay,Avi");
MODULE_DESCRIPTION("Linux Kernel Device Drivers Final Project");
MODULE_LICENSE("Dual BSD/GPL");	
MODULE_VERSION("0.1.0");
