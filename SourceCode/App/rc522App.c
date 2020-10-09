#include "rc522App.h"

static char* readBlockFromCard(int blockAdd);


int main(int argc, char** argv) 
{ 
	Init();

	do{
		PrintMenu();
		int i=-1;
		scanf("%d",&i);
		if (i >= 0 && i < totalItems && UserMenu[i].pfunc)
			UserMenu[i].pfunc();
	}while (1);

	return 0; 
} 

//-------------------------------------------------
// Functions definitions - START

void Init(void)
{
	printf("Init RFID_RC522\n");
	rc522_fd = open("/dev/rfid_rc522_dev", O_RDWR);
	if(rc522_fd == -1)
	{
		printf("test: Error Opening rc522 file\n");
		exit(1);
	}
	sleep(1);

	printf("Init LCD File\n");
	lcd_fd = open("/dev/lcdi2c", O_RDWR);
	if(lcd_fd == -1)
	{
		printf("test: Error Opening LCD file\n");
		exit(1);
	}
	sleep(1);
}


void PrintMenu (void)
{
	PUSER_MENU_ITEM pMenuItem = UserMenu;
	while (pMenuItem->Descr != NULL) {
		printf("%s",pMenuItem->Descr);
		pMenuItem++;
		totalItems++;
	}
	printf("\r Enter Your choice: ");
}

void MFRC522_ReadUnameAndUid(void)
{
	char* uName = readBlockFromCard(1);
	sleep(0.1);
	char* uid = readBlockFromCard(2);
	sleep(0.1);
	sendCurl(uName,uid);
	char* uid_uname = (char*)calloc((BLOCK_SIZE << 1) + 1, sizeof(char));
	uid_uname = strcat(strcat(uName, "\n"), uid);
	ioctl(lcd_fd, RESET, "1");
	sleep(0.1);
	write(lcd_fd, uid_uname, strlen(uid_uname));
	ioctl(rc522_fd, BEEP);
	sleep(5);
	ioctl(lcd_fd, RESET, "1");
	printf("User Name & ID on card are: %s\n", uid_uname);
}

void MFRC522_ReadSerialNum()
{
	char a[16] = {0};
	ioctl(rc522_fd, GET_ID, a);
	printf("UID = 0x%x%x%x%x \n", a[0],a[1],a[2],a[3]);
}

void MFRC522_WriteUserNameToCard(void)
{
	ioctl(rc522_fd, CHANGE_BLOCK, 1);

	char uName[BLOCK_SIZE+1] = {0};
	printf("Please enter user name:\n");
	scanf("%16s", uName);
	write(rc522_fd, uName, sizeof(uName));

	write(lcd_fd, "Writing to card:", sizeof("Writing to card:"));
	write(lcd_fd, uName, BLOCK_SIZE);
	ioctl(rc522_fd, BEEP);
	sleep(5);
	ioctl(lcd_fd, RESET, "1");
}

void MFRC522_WriteUidToCard(void)
{
	ioctl(rc522_fd, CHANGE_BLOCK, 2);

	char uid[BLOCK_SIZE+1] = {0};
	printf("Please enter UID: ");
	scanf("%16s",uid);
	write(rc522_fd, uid, BLOCK_SIZE);

	write(lcd_fd, "Writing to card:", sizeof("Writing to card:"));
	write(lcd_fd, uid, BLOCK_SIZE);
	ioctl(rc522_fd, BEEP);
	sleep(5);
	ioctl(lcd_fd, RESET, "1");
}

void MFRC522_ReadUserNameFromCard(void)
{
	char* uName = readBlockFromCard(1);
	printf("User Name on card is: %s\n", uName);
	ioctl(lcd_fd, RESET, "1");
	sleep(0.1);
	write(lcd_fd, uName, BLOCK_SIZE);
	ioctl(rc522_fd, BEEP);
	sleep(5);
	ioctl(lcd_fd, RESET, "1");
}
void MFRC522_ReadUidFromCard(void)
{
	char* uid = readBlockFromCard(2);
	printf("User ID on card is: %s\n", uid);
	ioctl(lcd_fd, RESET, "1");
	sleep(1);
	write(lcd_fd, uid, BLOCK_SIZE);
	ioctl(rc522_fd, BEEP);
	sleep(5);
	ioctl(lcd_fd, RESET, "1");
}

// Functions definitions - END
//-------------------------------------------------


char* readBlockFromCard(int blockAdd)
{
	char* uid = (char*)calloc(BLOCK_SIZE, sizeof(char));
	ioctl(rc522_fd, CHANGE_BLOCK, blockAdd);
	sleep(1);
	read(rc522_fd, uid, 0);
	return uid;
}

void sendCurl(char* name,char* id)
{
	CURL *curl;
	CURLcode res;

	curl = curl_easy_init();
	if(curl) {
		char url[1024]="https://alexshoyhit.com/kernel.php?name=";
		strcat(url,name);
		strcat(url,"&id=");
		strcat(url,id);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		/* example.com is redirected, so we tell libcurl to follow redirection */
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
}
