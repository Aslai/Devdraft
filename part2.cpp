#include<cstdio>
#include<cstdlib>
#include<string>
#include<cinttypes>
#include<vector>


#define USE_FILESTREAMS

struct Field{
    struct Coin{
        int x, y, value;
    };
    int width;
    int height;
    std::vector<Coin> coins;
};

struct Speed{
    int x, y;
};

int compute( Field f ){
    int major = f.width;
    int minor = f.height;
    if( minor > major ){
        major = f.height;
        minor = f.width;
    }

    std::vector<int> classes(minor, 0);
    int classnumber = 1;

    for( int xstart = 0; xstart < minor; ++xstart ){
        if( classes[xstart] != 0 )
            continue;
        Speed s = {1, 1};
        if( minor == 1 ){
            s.y = 0;
        }
        if( major == 1 ){
            s.x = 0;
        }

        Speed start = s;
        int x = xstart;
        int y = 0;
        int step = 0;
        do{
            ++step;
            if( y == 0 && classes[x] > 0 ){
                classes[xstart] = classes[x];
            }
            x += s.x;
            if( x <= 0 || x >= minor-1 ){ //Bounce off of vertical walls
                s.x = -s.x;
            }
            y += s.y;
            if( y < 0 || y >= major ){ //Bounce off of horizontal walls
                s.y = -s.y;
            }
        } while(    step < 5 ||
                  !(x == xstart && //While not back at the start with the starting speed
                    y == 0 &&
                    s.x == start.x &&
                    s.y == start.y) );

        if( classes[xstart] == 0 ){
            classes[xstart] = classnumber++;
        }
    }
    for( int i = 0; i < classes.size(); ++i ){
        printf("%d\t", classes[i] );
    }
    printf("\n");
}

int main(){
    #ifdef USE_FILESTREAMS
    freopen( "input.txt", "r", stdin );
    freopen( "output.txt", "w", stdout );
    #endif
    while( !feof( stdin ) ){
        Field f;
        Field::Coin c;
        int coins;
        int result = fscanf( stdin, "%d %d %d", &f.width, &f.height, &coins );
        if( result != 3 ){
            fprintf( stderr, "Failed to parse values.\n" );
            continue;
        }
        while( coins-- > 0 ){
            result = fscanf( stdin, "%d %d %d", &c.x, &c.y, &c.value );
            if( result != 3 ){
                fprintf( stderr, "Failed to parse values.\n" );
                break;
            }
            f.coins.push_back( c );
        }
        if( coins > 0 ){
            fprintf( stderr, "Too many coins specified.\n" );
            continue;
        }
        compute( f );

    }
    return 0;
}
