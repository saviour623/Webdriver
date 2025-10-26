#include "client.h"
#include <stdio.h>

int main(void) {
  //char bf[102];

  Webdriver_Config conf = {
						   .port = 32432
  };
  Webdriver_Client client = webdriverCreateClient(&conf);

  if (webdriverError(client)){
	fprintf(stderr, "Could not start web client");
	return EXIT_FAILURE;
  }

  int stat = (long)webdriverSetbuf(client, NULL, 0);
  if (stat != WEBDR_SUCCESS){
	fprintf(stderr, "setbuf failed with: %d\n",stat);
	exit(EXIT_FAILURE);
  };
  webdriverSetHttpCmd(client, GET, "/");
  webdriverAddHttpHeader(client, "Host", "www.example.com");
  webdriverAddHttpHeader(client, "Content-Type", "application/json");
  webdriverAddHttpHeader(client, "Allow", "*/*");
  webdriverAddHttpHeader(client, "Content-Type", "application/json");

  webdriverShowHttpHeaders(client);

  webdriverDestroyClient(client);
  return EXIT_SUCCESS;
}
