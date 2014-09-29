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

int compute( Field f ){
    printf("%d %d %d\n", f.width, f.height, f.coins.size() );
    for( int i = 0; i < f.coins.size(); ++i ){
        printf("%d %d %d\n", f.coins[i].x, f.coins[i].y, f.coins[i].value);
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
