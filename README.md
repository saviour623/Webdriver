##MVPG
An easy tool to send/recieve files over the internet

```
#include <mvpg.h>
#define CLIENT_ADDRESS "https://10.10.0.1:80000"
#define KEEP_ALIVE      MVPG_CONNECT_STABLE

int main(void) {
    mvpgObject newTransfer = mvpgInitTransfer("./project.mp4", mvpgRegular);
    mvpgErrorHandler( tranferObject(newTransfer, CLIENT_ADDRESS, KEEP_ALIVE), "failed to initiate transfer");
    return 0;
}
```

