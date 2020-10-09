#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
void sendCurl(char* name,char* id);
int main(void)
{
  sendCurl("abc","123");
  return 0;
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
