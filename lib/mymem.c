/*
#include<mymem.h>
#include<stdbool.h>
#include<ACIA.h>
#include<SDSPI.h> //for printhex
struct _myHeapStruct
{
    void* ptr;
    uint16_t size;
};
struct _myHeapStruct _myHeap[_HEAP_MAX_ID];
bool _malocInitTrue = false;
uint16_t lastAdd = (uint16_t)&_MDATA;
void* mymalloc(uint16_t size)
{
    void *ptr;
    if(_malocInitTrue == false)
    {
        for(int i = 0; i < _HEAP_MAX_ID; i++)
        {
            _myHeap[i].size = 0;
        }
    }
    uint16_t offset = 0;
    for(int n = 0; n < _HEAP_MAX_ID; n++)
    {
        
        if(_myHeap[n].size = 0 && lastAdd+size<=(uint16_t)(&_MDATALAST) && lastAdd+size<=_LAST_DATA_ADD)
        {
            ptr = &lastAdd;
            lastAdd += size;
            _myHeap[n].size = size;
            _myHeap[n].ptr = ptr;
            
        }
    }
    
    return ptr;
}

void _printMemAdd(void* ptr)
{
    puts("0x");
    print_hex((uint8_t)((((uint16_t)ptr)&0xFF00)>>8));
    print_hex((uint8_t)((uint16_t)ptr)&0xFF);
}
*/