## Webdriver
<table><tr><td> <strong><em> C wrapper for webdrivers </em></strong> </td></tr></table>

## Basic Example
```C

#include <Webdriver.h>
#include <stdlib.h>

int main(void) {
       Webdriver Handle = WebdriverInit("chrome"); // Start session

        if (Handle == WEBDRIVER_INIT_FAILED) {
            fprintf(stderr, Webdriver_strerror());
            exit(EXIT_FAILURE);
        }
        Handle.open("www.example.com");
        WebdriverClass element = Handle.findElement(By.ID, "box");
        element.sendKeys("hello world");
        WebdriverClose(Handle); // Session is running

        WebdriverQuit(Handle); // close all session
        return 0;
}
```
<br>

_Project is currently ongoing._


