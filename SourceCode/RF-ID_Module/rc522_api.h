#ifndef __rc522_h__
#define __rc522_h__

#include <linux/workqueue.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/mutex.h>
#include <linux/spi/spi.h>
#include <linux/cdev.h>

#define PCD_IDLE              0x00
#define PCD_AUTHENT           0x0E
#define PCD_RECEIVE           0x08
#define PCD_TRANSMIT          0x04
#define PCD_TRANSCEIVE        0x0C
#define PCD_RESETPHASE        0x0F
#define PCD_CALCCRC           0x03


#define PICC_REQIDL           0x26
#define PICC_REQALL           0x52
#define PICC_ANTICOLL1        0x93
#define PICC_ANTICOLL2        0x95
#define PICC_AUTHENT1A        0x60
#define PICC_AUTHENT1B        0x61
#define PICC_READ             0x30
#define PICC_WRITE            0xA0
#define PICC_DECREMENT        0xC0
#define PICC_INCREMENT        0xC1
#define PICC_RESTORE          0xC2
#define PICC_TRANSFER         0xB0
#define PICC_HALT             0x50


#define DEF_FIFO_LENGTH       64                 //FIFO size=64byte
#define MAXRLEN  18


#define     RFU00                 0x00    
#define     CommandReg            0x01    
#define     ComIEnReg             0x02    
#define     DivlEnReg             0x03    
#define     ComIrqReg             0x04    
#define     DivIrqReg             0x05
#define     ErrorReg              0x06    
#define     Status1Reg            0x07    
#define     Status2Reg            0x08    
#define     FIFODataReg           0x09
#define     FIFOLevelReg          0x0A
#define     WaterLevelReg         0x0B
#define     ControlReg            0x0C
#define     BitFramingReg         0x0D
#define     CollReg               0x0E
#define     RFU0F                 0x0F
// PAGE 1     
#define     RFU10                 0x10
#define     ModeReg               0x11
#define     TxModeReg             0x12
#define     RxModeReg             0x13
#define     TxControlReg          0x14
#define     TxAutoReg             0x15
#define     TxSelReg              0x16
#define     RxSelReg              0x17
#define     RxThresholdReg        0x18
#define     DemodReg              0x19
#define     RFU1A                 0x1A
#define     RFU1B                 0x1B
#define     MifareReg             0x1C
#define     RFU1D                 0x1D
#define     RFU1E                 0x1E
#define     SerialSpeedReg        0x1F
// PAGE 2    
#define     RFU20                 0x20  
#define     CRCResultRegM         0x21
#define     CRCResultRegL         0x22
#define     RFU23                 0x23
#define     ModWidthReg           0x24
#define     RFU25                 0x25
#define     RFCfgReg              0x26
#define     GsNReg                0x27
#define     CWGsCfgReg            0x28
#define     ModGsCfgReg           0x29
#define     TModeReg              0x2A
#define     TPrescalerReg         0x2B
#define     TReloadRegH           0x2C
#define     TReloadRegL           0x2D
#define     TCounterValueRegH     0x2E
#define     TCounterValueRegL     0x2F
// PAGE 3      
#define     RFU30                 0x30
#define     TestSel1Reg           0x31
#define     TestSel2Reg           0x32
#define     TestPinEnReg          0x33
#define     TestPinValueReg       0x34
#define     TestBusReg            0x35
#define     AutoTestReg           0x36
#define     VersionReg            0x37
#define     AnalogTestReg         0x38
#define     TestDAC1Reg           0x39  
#define     TestDAC2Reg           0x3A   
#define     TestADCReg            0x3B   
#define     RFU3C                 0x3C   
#define     RFU3D                 0x3D   
#define     RFU3E                 0x3E   
#define     RFU3F		  		  0x3F

#define 	MI_OK                 0
#define 	MI_NOTAGERR           (-1)
#define 	MI_ERR                (-2)

#define SET_SPI_CS  do{}while(0)
#define CLR_SPI_CS  do{}while(0)

#define SET_SPI_CK  do{}while(0)
#define CLR_SPI_CK  do{}while(0)
#define SET_SPI_MOSI  do{}while(0)
#define CLR_SPI_MOSI  do{}while(0)
#define STU_SPI_MISO  do{}while(0)

#define SET_RC522RST do{}while(0) 
#define CLR_RC522RST do{}while(0)

#define RFID_IOCTL_BASE 0xCE
#define CHANGE_PASSWD _IOW(RFID_IOCTL_BASE, 0x01, char *)
#define CHANGE_BLOCK _IOW(RFID_IOCTL_BASE, (0x02 << 1), int)
#define READ_CARD _IOR(RFID_IOCTL_BASE, (0x03 << 1), char *)
#define WRITE_CARD _IOW(RFID_IOCTL_BASE, (0x04 << 1), char *)
#define CHANGE_KEY _IO(RFID_IOCTL_BASE, (0x05 << 1))
#define GET_ID _IOR(RFID_IOCTL_BASE, (0x06 << 1), char *)
#define BEEP _IO(RFID_IOCTL_BASE, (0x07 << 1))

#define CLASS_NAME "RC522"
#define DEVICE_NAME "rfid_rc522_dev"

typedef struct ioctl_description {
  uint32_t ioctlcode;
  char	name[24];
} IOCTLDescription_t;

extern char PcdReset(void);
extern char PcdRequest(unsigned char req_code,unsigned char *pTagType);
extern void PcdAntennaOn(void);
extern void PcdAntennaOff(void);
extern char M500PcdConfigISOType(unsigned char type);
extern char PcdAnticoll(unsigned char *pSnr);
extern char PcdSelect(unsigned char *pSnr);
extern char PcdAuthState(unsigned char auth_mode,unsigned char addr,unsigned char *pKey,unsigned char *pSnr);
extern char PcdWrite(unsigned char addr,unsigned char *pData);
extern char PcdRead(unsigned char addr,unsigned char *pData);
extern char PcdHalt(void);
extern struct spi_device *rc522_spi;
extern unsigned char ReadRawRC(unsigned char Address);

#endif
