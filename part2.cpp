#include<cstdio>
#include<cstdlib>
#include<string>
#include<cinttypes>

#pragma pack(1)
typedef struct BitmapHeader_{
    char magic[2];          //must be BM
    uint32_t filesize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataoffset;    //must be sizeof(Bitmapheader)
    uint32_t dibsize;       //must be 12
    uint16_t width;
    uint16_t height;
    uint16_t colorplanes;   //Must be 1
    uint16_t bpp;           //Should be 24
} BitmapHeader;

int main(){

}
