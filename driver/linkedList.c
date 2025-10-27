#include <stdio.h>
#include <stdint.h>

typedef struct XList {
  void *data;
  uintptr_t xtranv;
} XList;

void *XListNew(void)
{
  XList *obxList = malloc(sizeof( XList ));

  obxList->data = obxList->next = 0;
  return obxList;
}

void *XListPush(XList obxList, void *data)
{
  obxNode = XListNew();
  obxList->data = data
  if (obxList == NULL)
	return obxList;
  
  *(uintptr_t *)&obxNode ^= (uintptr_t)(obxList);
}

int main(void) {
  XList *names = XListNew();
}
