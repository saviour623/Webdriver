##MVPG
An easy tool to send/recieve programs (files) over the internet

```
#include <mvpg.h>

int main(void) {
    mvpgObject newTransfer = mvpgInitTransfer("./project.mp4", mvpgRegular);
    mvpgErrorHandler( tranferObject(newTransfer), "failed to initiate transfer");
    return 0;
}
```

