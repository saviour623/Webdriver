## Mvpg
<table><tr><td> <strong><em> An easy tool to send/recieve files over the internet </em></strong> </td></tr></table>

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

_IMPORTANT: This project is stated to officially begin by 2nd July 2025, and so if you're free to feature in it, send me a mail at ```saviourme33@gmail.com``` including a message of interest and your github username, or you may wait until it's open for contribution._
