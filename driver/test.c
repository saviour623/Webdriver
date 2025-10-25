#include "client.h"
#include <stdio.h>

int main(void) {
  char bf[1024];

  Webdriver_Config conf = {
						   .port = 32432
  };
  Webdriver_Client client = webdriverCreateClient(&conf);

  if (webdriverError(client)){
	fprintf(stderr, "Could not start web client");
	return EXIT_FAILURE;
  }
  int stat = (long)webdriverSetbuf(client, bf, 1024);
  if (stat != WEBDR_SUCCESS){
	fprintf(stderr, "setbuf failed with: %d\n",stat);
	exit(EXIT_FAILURE);
  };
  webdriverSetHttpCmd(client, GET, "/");
  // webdriverAddHttpHeader(client, "Host", "www.example.com");

  webdriverShowHttpHeaders(client);

  webdriverDestroyClient(client);
  return EXIT_SUCCESS;
}
