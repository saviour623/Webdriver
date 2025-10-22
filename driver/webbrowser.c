#include "client.h"

int main(void) {
  Webdriver_Config conf = {.server = true};
  Webdriver_Client handle = WebdriverCreateClient(&conf);
  char bf[1024];
  int wfd, fd;
  ssize_t len;

  if (Webdriver_Error(handle)) {
    // Handle error
    exit(EXIT_FAILURE);
  }
  wfd = Webdriver_ClientGetFd(handle);
  if (connect(wfd, Webdriver_ClientGetHost(handle), sizeof (struct sockaddr)) < 0)
    {
      fprintf(stderr, "[Webdriver Client]: connect <%s>\n", strerror(Webdriver_ClientErrorCode));
      exit(EXIT_FAILURE);
    }

  if ((fd = open("request.json", O_RDONLY, 777)) < 0) {
    perror("opening request.json");
    exit(-1);
  }
  len = read(fd, bf, 1024);
  close(fd);

  errno = 0;
  if (len != send(wfd, bf, len, 0)){
    errno == 0 ?  puts("unable to send a complete request") : perror("connect");
    exit(EXIT_FAILURE);
  }

  memset(bf, 0, len);
  if ((len = recv(wfd, bf, 1024, 0)) < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  // Print Chromedriver response
  fd = open("log", O_RDWR, S_IRUSR | S_IWUSR);

  if ((fd < 0) && (fd != EEXIST)) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  if (write(fd, response, strlen(response)) < 0) {
    perror("write");
    exit(-1);
  }

  freeaddrinfo(info);
  close(fd);
  WebdriverDestroyClient(Handle);

  return 0;
}
