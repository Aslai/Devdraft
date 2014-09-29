#include<cstdio>
#include<vector>
#include<map>

#define USE_FILESTREAMS

//!This code is a lot more straightforward that the entry I submitted for the first challenge.
//!I was looking at the patterns generated with the laser in this problem, and it appears that an O(n)
//!algorithm is possible, where n is the number of coins. After a few hours of testing, I couldn't
//!derive the patterns mathematically, so I opted for a more obvious approach. With more diligence,
//!I imagine the patterns could be derived mathematically.

struct Field {
    struct Coin {
        int x, y, value;
        int touch_x, touch_y, touch_t;
        bool check_x, check_y, check_d1, check_d2;
    };
    int width;
    int height;
    std::vector<Coin> coins;
    std::map<int,Coin> coinmap;
};

struct Speed {
    int x, y;
};

int hash( int x, int y, int width ) { //This is much faster than using std::pair in std::map
    return x + y * width * 2;
}

int maxpath( Field &f, Field::Coin c ) {
    int ret = 0;
    int trackx = 0, tracky = 0;
    if( c.check_x == false || c.check_y == false ) { //If this coin has been checked by another coin, don't recheck it.
        for( size_t i = 0; i < f.coins.size(); ++i ) { //Determine the value of straight vertical or horizontal shots.
            if( f.coins[i].x == c.x ) {
                trackx += f.coins[i].value;
                f.coins[i].check_x = true; //Prevent checking this coin redundantly
            }
            if( f.coins[i].y == c.y ) {
                tracky += f.coins[i].value;
                f.coins[i].check_y = true; //Prevent checking this coin redundantly
            }
        }
    }
    ret = tracky;
    if( trackx > tracky) {
        ret = trackx;
    }
    if( f.width > 1 && f.height > 1 ) { //Don't check diagonals if there's no need to
        for( int trial = -1; trial <= 1; trial += 2 ) { //check both diagonals
            int amount = 0;
            Speed s = { trial, 1 };
            int x = c.x;
            int y = c.y;
            int passes = 0;
            do {
                if( x == c.x && y == c.y ) { //This keeps track of how many times the origin has been passed.
                    ++passes;
                }
                if( f.coinmap.find( hash( x, y, f.width ) ) != f.coinmap.end() ) { //Prevent adding empty entries into the map
                    Field::Coin &test_coin = f.coinmap[hash( x, y, f.width )];

                    if( !( test_coin.touch_x == c.x && test_coin.touch_y == c.y && test_coin.touch_t == trial ) ) {
                        //If we haven't already visited this coin during this trial, mark it as having been visited.
                        test_coin.touch_x = c.x;
                        test_coin.touch_y = c.y;
                        test_coin.touch_t = trial;
                        amount += test_coin.value;
                        if( s.x == -1 ) {
                            test_coin.check_d1 = true; //Prevent checking this coin redundantly
                        } else {
                            test_coin.check_d2 = true; //Prevent checking this coin redundantly
                        }
                    }
                }

                //Handle laser reflection
                if( (x <= 0 && s.x < 0) || (x >= f.width && s.x > 0) ) {
                    s.x = -s.x;
                }
                if( (y <= 0 && s.y < 0) || (y >= f.height && s.y > 0) ) {
                    s.y = -s.y;
                }
                x += s.x;
                y += s.y;
            } while( passes <= 2 );

            if( amount > ret ) {
                ret = amount;
            }
        }
    }
    return ret;
}

int compute( Field f ) {
    int maxval = 0;
    for( size_t i = 0; i < f.coins.size(); ++i ) {
        int value = maxpath( f, f.coins[i] );
        if( value > maxval ) {
            maxval = value;
        }
    }
    return maxval;
}

int main() {
    #ifdef USE_FILESTREAMS
    freopen( "input.txt", "r", stdin );
    freopen( "output.txt", "w", stdout );
    #endif

    while( !feof( stdin ) ) {
        Field f;
        Field::Coin c;
        int coins;
        int result = fscanf( stdin, "%d %d %d", &f.width, &f.height, &coins );
        if( result != 3 ) {
            fprintf( stderr, "Failed to parse values.\n" );
            continue;
        }
        while( coins-- > 0 ) {
            result = fscanf( stdin, "%d %d %d", &c.x, &c.y, &c.value );
            c.check_x = c.check_y = c.check_d1 = c.check_d2 = false;
            c.touch_t = c.touch_x = c.touch_y = -10;
            if( result != 3 ) {
                fprintf( stderr, "Failed to parse values.\n" );
                break;
            }
            f.coins.push_back( c );
            f.coinmap[hash( c.x, c.y, f.width )] = c;
        }
        if( coins > 0 ) {
            fprintf( stderr, "Too many coins specified.\n" );
            continue;
        }
        printf("%d\n", compute( f ) );

    }
    return 0;
}
