## C_Selenium_Server
<table><tr><td> <strong><em> C selenuim wrapper for automating browsers </em></strong> </td></tr></table>

## Basic Example
An example to transfer a "project.mp4" file to `https://10.10.0.1:80000`
```C

#include <mvpg.h>
#define CLIENT_ADDRESS https://10.10.0.1:80000
#define KEEP_ALIVE      MVPG_CONNECT_STABLE

int main(void) {
    /* initialize transfer */
    mvpgObject newTransfer = mvpgInitTransfer("./project.mp4", MVPG_REGULAR);

    /* transfer file to address specified by CLIENT_ADDRESS; report error if tranfer fails */
    mvpgErrorHandler( mvpgTranferObject(newTransfer, CLIENT_ADDRESS, KEEP_ALIVE), "failed to initiate transfer");

    return 0;
}
```
<br>

_IMPORTANT: Project is currently ongoing.

