
#include<stdint.h>
#define _HEAP_MAX_ID 16
#define _LAST_DATA_ADD 0x7FFFF
extern void* _MDATA;
extern void* _MDATALAST;
void* mymalloc(uint16_t size);
void _printMemAdd(void* ptr);
uint8_t myfree(void *ptr);

