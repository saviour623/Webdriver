## Webdriver
<table><tr><td> <strong><em> Automating Webdrivers </em></strong> </td></tr></table>

## Basic Example
```C

#include <webdriver.h>
#include <stdlib.h>

int main(void) {
       Webdriver handle = webdriver("chrome"); // Start session

        if (webdriver_Error(Handle)) {
            fprintf(stderr, webdriver_strerror());
            exit(EXIT_FAILURE);
        }
        handle.open("www.example.com");
        WebdriverClass element = handle.findElement(handle, By.ID, "box");
        element.sendKeys("hello world");
        WebdriverClose(handle); // Session is running

        WebdriverQuit(handle); // close all session
        return 0;
}
```
<br>


