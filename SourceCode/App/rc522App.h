#ifndef RFID_LINUX_DRIVER_TEST_RC522APP_H_
#define RFID_LINUX_DRIVER_TEST_RC522APP_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <math.h>
#include <curl/curl.h>

//-------------------------------------------------
// Types declarations - START

#define BLOCK_SIZE 16

typedef enum _IO_CMD_RFID {
	CHANGE_PASSWD		=0x4004CE01,
	CHANGE_BLOCK		=0x4004CE04,
	READ_CARD			=0x8004CE06,
	WRITE_CARD			=0x4004CE08,
	CHANGE_KEY			=0xCE0A,
	GET_ID				=0x8004CE0C,
	BEEP				=0xCE0E,
}IO_CMD_RFID;

typedef enum _IO_CMD_LCD {
	GETCHAR				= 0x8004F506,
	SETCHAR				= 0x4004F506,
	GETPOSITION			= 0x8004F50D,
	SETPOSITION			= 0x4004F511,
	RESET				= 0x4004F516,
	HOME				= 0x4004F51A,
	GETBACKLIGHT		= 0x8004F51E,
	SETBACKLIGHT		= 0x4004F51E,
	GETCURSOR			= 0x8004F522,
	SETCURSOR			= 0x4004F522,
	GETBLINK			= 0x8004F526,
	SETBLINK			= 0x4004F526,
	SCROLLHZ			= 0x4004F52A,
	GETCUSTOMCHAR		= 0x8004F52D,
	SETCUSTOMCHAR		= 0x4004F52D,
	CLEAR 				= 0x4004F532,
}IO_CMD_LCD;


typedef struct _USER_MENU_ITEM
{
	char *Descr;
	void (*pfunc) ();
} USER_MENU_ITEM, *PUSER_MENU_ITEM;

// Types declarations - END
//-------------------------------------------------

//-------------------------------------------------
// Functions declarations - START
void Init(void);
void PrintMenu (void);
void MFRC522_ReadUnameAndUid(void);
void MFRC522_ReadSerialNum(void);
void MFRC522_WriteUserNameToCard(void);
void MFRC522_WriteUidToCard(void);
void MFRC522_ReadUserNameFromCard(void);
void MFRC522_ReadUidFromCard(void);
void sendCurl(char* name,char* id);

// Functions declarations - END
//-------------------------------------------------


//-------------------------------------------------
// Globals declarations - START
static int rc522_fd;
static int lcd_fd;
static unsigned totalItems = 0;
static USER_MENU_ITEM UserMenu[] =
{
	{"\r 0- Print menu\n"						, PrintMenu},
	{"\r 1- RFID Read card SerialNum\n"			, MFRC522_ReadSerialNum},
	{"\r 2- RFID Write user name\n"				, MFRC522_WriteUserNameToCard},
	{"\r 3- RFID Write user ID\n"				, MFRC522_WriteUidToCard},
	{"\r 4- RFID Read user name\n"				, MFRC522_ReadUserNameFromCard},
	{"\r 5- RFID Read user ID\n"				, MFRC522_ReadUidFromCard},
	{"\r 6- RFID Read User ID & User Name\n"	, MFRC522_ReadUnameAndUid},
//	{"\r 5- RFID Write DATA Block\n"			, MFRC522_WriteDataBlock},
//	{"\r 6- RFID Read Data Block\n"				, MFRC522_ReadCardDataBlock},
//	{"\r 7- RFID Write VALUE Block\n"			, MFRC522_WriteValueBlock},
//	{"\r 8- RFID Read Value Block\n"			, MFRC522_ReadCardValueBlock},
//	{"\r 9- RFID Read Block Access Bits\n"		, MFRC522_ReadSectorTrailerCardBlock},
//	{"\r 10-RFID Increment Block\n"				, MFRC522_IncrementCardBlock},
//	{"\r 11-RFID Decrement Block\n"				, MFRC522_DecrementCardBlock},
//	{"\r 12-RFID Transfer Block\n"				, MFRC522_TransferCardBlock},
//	{"\r 13-DS18B20 read Temperature\n"			, DS18B20_ReadTemperature},
//	{"\r 14-Bluetooth HC-06 Receive Data\n"		, HC06_ReadData},
//	{"\r 15-Read RFID and Set Air Conditioner\n", PRV_AcReady},
	{NULL, NULL}
};


// Globals declarations - END
//-------------------------------------------------





#endif /* RFID_LINUX_DRIVER_TEST_RC522APP_H_ */
