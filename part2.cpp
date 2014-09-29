#include<cstdio>
#include<cstdlib>
#include<string>
#include<cinttypes>
#include<vector>

#pragma pack(1)
struct BitmapHeader{
    char magic[2];          //must be BM
    uint32_t filesize;
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t dataoffset;    //must be sizeof(Bitmapheader)
    uint32_t dibsize;       //must be 12
    uint16_t width;
    uint16_t height;
    uint16_t colorplanes;   //Must be 1
    uint16_t bpp;
};

enum class Slope{
    Positive,
    Negative
};

void generatebmp( std::string file, int width, int height, int x, int y, Slope s ){
    std::vector<bool> pixels(width * height, false);
    struct{
        int x, y;
    }speed = {1, 1};

    if( s == Slope::Negative ){
        speed.y = -1;
    }

    for( int i = 0; i < width * height; ++i ){
        pixels[x + y * width] = true;
        x += speed.x;
        if( x >= width || x < 0 ){
            x -= 2 * speed.x;
            speed.x = -speed.x;
        }
        y += speed.y;
        if( y >= height || y < 0 ){
            y -= 2 * speed.y;
            speed.y = -speed.y;
        }
    }

    int widthpadding = (4 - width * 3 % 4) % 4;
    BitmapHeader head;
    head.magic[0] = 'B';
    head.magic[1] = 'M';
    head.filesize = sizeof( BitmapHeader ) + (width*3 + widthpadding) * height;
    head.dataoffset = sizeof( BitmapHeader );
    head.dibsize = 12;
    head.width = width;
    head.height = height;
    head.colorplanes = 1;
    head.bpp = 24;


    FILE* f = fopen( file.c_str(), "wb" );
    fwrite( &head, 1, sizeof( BitmapHeader), f );

    for( size_t i = 0; i < pixels.size(); ++i ){
        if( pixels[i] ){
            fputc( 0, f );
            fputc( 0, f );
            fputc( 0, f );
        }
        else{
            fputc( 255, f );
            fputc( 255, f );
            fputc( 255, f );
        }
        if( i % width == width - 1 ){
            for( int j = 0; j < widthpadding; ++j ){
                fputc(0, f);
            }
        }
    }
    fclose( f );
}

int main(){
    for( int i = 0; i < 40; ++i ){
        for( int j = 0; j < 40; ++j ){
            char str[100];
            sprintf( str, "ss/img-%d-%d.bmp", i, j );
            generatebmp( str, i, j, 0, 0, Slope::Positive );
        }
    }
}
