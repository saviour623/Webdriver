#include "../client.c"
#include <stdio.h>

int main(void)
{
  uint8_t index = webdriverObjectKeyHash("mka9kl", 8);

  printf("%u\n", index);
  return 0;
}
