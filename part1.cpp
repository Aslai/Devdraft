//I'm a sucker for C I/O
#include<stdint.h>
#include<vector>
#include<string>
#include<cstdio>
#include<cmath>

#define USE_FILESTREAMS
const uint64_t Mask32Bit = 0xFFFFFFFFull;

//!Note to readers of my code:
//!
//!I willfully used some magic numbers in array access relating to accessing the numbers for the game.
//!I figured that the amount of work required to make the game work for N digits is far beyond the scope of
//!this challenge, and my entire algorithm would crash and burn if you tried to use it on any other
//!number of inputs.
//!

class Number{
    //This is just a very simple big integer implementation. I only implemented functionality as needed for
    //the project, and it isn't really fit for any other use. I won't stop anyone from expanding upon it, but
    //even the GMP is less painful to use than this.
    //
    //!THIS IS A MUTABLE TYPE! OPERATIONS PERFORMED ON INSTANCES AFFECT THE INSTANCE IN QUESTION AND WILL NOT
    //!CREATE AN IMMUTABLE COPY! You have been warned.


    std::vector<uint64_t> data;
    public:
    Number(){

    }
    Number( const Number& other ){
        data = other.data;
    }
    int Compare( Number& value ){
        int i = data.size();
        int j = value.data.size();
        //Start one of the counters at a negative index to keep the numbers aligned to their correct place.
        //Negative offsets just get treated as holding a value of 0.
        if( i < j ){
            i -= j;
            j = 0;
        }
        else{
            j -= i;
            i = 0;
        }
        while( i < (int) data.size() ){
            uint32_t a = i >= 0 ? data[i] : 0;
            uint32_t b = j >= 0 ? value.data[j] : 0;
            if( a < b ){
                return -1;
            }
            if( a > b ){
                return 1;
            }
            i++;
            j++;
        }
        return 0;
    }
    Number& Add( uint32_t value ){

        if( data.size() == 0 )
        {
            //0 + x is simply x
            data.push_back( value );
        }
        else{
            uint32_t overflow = value;
            for( size_t i = data.size(); i > 0; --i ){
                data[i-1] += overflow;
                overflow = data[i-1] >> 32;
                data[i-1] &= Mask32Bit;
            }
            if( overflow != 0 ){
                data.insert( data.begin(), overflow );
            }
        }
        return *this;
    }
    Number& Add( Number& value ){
        //For simplicity's sake, make both the numbers equally sized.
        while( data.size() < value.data.size() ){
            data.insert( data.begin(), 0 );
        }
        while( value.data.size() < data.size() ){
            value.data.insert( value.data.begin(), 0 );
        }
        for( size_t i = 0; i < data.size(); ++i ){
            data[i] += value.data[i] & Mask32Bit;
        }
        Add(0); //Add automatically handles carry propagation
        return *this;
    }
    Number& Subtract( Number& value ){
        //For simplicity's sake, make both the numbers equally sized.
        while( data.size() < value.data.size() ){
            data.insert( data.begin(), 0 );
        }
        while( value.data.size() < data.size() ){
            value.data.insert( value.data.begin(), 0 );
        }

        int i = data.size();
        int j = value.data.size();
        uint32_t carry = 0;
        while( i > 0 && j > 0 ){
            uint32_t a = i > 0 ? data[i-1]& Mask32Bit : 0;
            uint32_t b = j > 0 ? value.data[j-1]& Mask32Bit : 0;
            uint32_t result = a - b - carry;
            carry = 0;
            if( a < b ){
                carry = a;
            }
            if( i > 0 ){
                data[i-1] = result & Mask32Bit;
            }
            i-=i>0?1:0;
            j-=j>0?1:0;
        }
        return *this;
    }

    Number& Multiply( uint32_t value ){
        for( size_t i = 0; i < data.size(); ++i ){
            data[i] *= value;
        }
        Add(0); //Add automatically handles carry propagation

        return *this;
    }
    Number& Set( std::string value ){
        //Fill the internal binary structure with a decimal integer represented by a string
        data.clear();
        for( size_t i = 0; i < value.length(); ++i ){
            if( value[i] >= '0' && value[i] <= '9' ){
                Multiply( 10 );
                Add( value[i] - '0' );
            }
        }
        return *this;
    }
};

struct Range{ Number Minimum, Maximum; };
std::vector<Range> Patterns;


void DeterminePattern(){
    //This can be determined more trivially, but the algorithm is left as-is because it's fast enough and more clear.
    //The pattern is simply:
    //0x1
    //0x3
    //0xB
    //0x2B
    //0xAB
    //0x2AB
    //0xAAB
    //0x2AAB
    //0xAAAB
    //0x2AAAB
    //0xAAAAB
    //... And on forever
    Range patternrange;
    Number n, prev;
    Number one;
    one.Set("1");
    prev.Set( "1" );
    n.Set( "3" );

    //Hardcode the first region for convenience
    patternrange.Minimum = prev;
    patternrange.Maximum = prev;
    Patterns.push_back( patternrange );
    Number maximum;
    //Store pattern info all the way up to 10^100 since provided numbers are defined as 0 <= T <= 10^100
    maximum.Set("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    while( n.Compare( maximum ) < 0 ){
        //Get the end of the region where second move advantages occur
        Number diff = n;
        diff.Add( n );
        diff.Subtract( one );

        //Store it for later use
        patternrange.Minimum = n;
        patternrange.Maximum = diff;
        Patterns.push_back( patternrange );

        //Find the beginning of the next region
        diff = n;
        diff.Subtract( prev );
        diff.Multiply( 4 );
        prev = n;
        n.Add( diff );
    }
}

//I should be using a scoped enum here...
enum GameState{
    GoSecond,
    GoFirst
};


GameState CheckMovesFast(std::vector<Number> numbers){
    //The meat of the implementation.
    //The methods and math used are derived from observed behavior of the output from a previous iteration of this function.
    //Honestly, I don't know WHY this method works, but it works according to spec as far as I can tell.
    //This is merely my attempt to recreate the behavior of the data in a highly efficient manner.
    //I derive no pleasure from fostering this black magic, but it's so fast!

    //Ensure that the edge case of all identical numbers is handled immediately
    if( numbers[1].Compare(numbers[2]) == 0 && numbers[1].Compare(numbers[0]) == 0 )
        return GoSecond;

    //Get the difference between number 1 and number 2, and the difference between number 0 and number 1.
    //Sort them in ascending order.
    numbers[2].Subtract(numbers[1]);
    numbers[1].Subtract(numbers[0]);
    //From this point on, numbers[0] is scratch space.
    if( numbers[1].Compare(numbers[2]) > 0 ){
        numbers[0] = numbers[1];
        numbers[1] = numbers[2];
        numbers[2] = numbers[0];
    }

    //Find which area of the pattern the larger of the two differences falls in.
    size_t i;
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[2]) < 0; ++i ){};
    //If it's not actually in the range of this area of the pattern, go first.
    if( Patterns[i].Minimum.Compare(numbers[2]) > 0 )
        return GoFirst;

    //Store the maximum extent of the current pattern region for later use
    numbers[0] = Patterns[i].Maximum;

    //Find which area of the pattern the smaller of the two differences falls in.
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[1]) < 0; ++i ){};
    //If it's not actually in the range of this area of the pattern, go first.
    if( Patterns[i].Minimum.Compare(numbers[1]) > 0 )
        return GoFirst;

    Number num1plus1;
    num1plus1 = numbers[1];
    num1plus1.Add( 1 );

    //Special case: When the two differences are equal, or the larger one is only one unit ahead of the smaller
    //difference, then I believe that going second is guaranteed to allow a forced victory.
    if( numbers[1].Compare(numbers[2]) != 0 && num1plus1.Compare(numbers[2]) != 0){
        //Another special case: When the value of the large difference is greater than the value of the farthest reach
        //of the current pattern area MINUS the value of the smaller difference, I believe going first allows you to force a victory.
        //Confusing? I think so. Essentially, if your two differences are 9 and 80, then the large difference is in
        //the 43-85 template area. The furthest extent is 85, so 85-9=76. 76 is smaller than 80, so we need to go first
        //in order to force victory.

        numbers[0].Subtract(numbers[1]);
        if( numbers[0].Compare( numbers[2] ) < 0 ){
            return GoFirst;
        }
    }

    //If we've made it through all of that, then we go second to force a victory.
    return GoSecond;
}

int problem1(){
    DeterminePattern();
    char buffer[10000]; //The spec specifies 10^100 as the largest possible number,
                        //so only 303 characters are really needed.
    #ifdef USE_FILESTREAMS
    freopen( "input.txt", "r", stdin );
    freopen( "output.txt", "w", stdout );
    #endif
    fgets( buffer, 10000, stdin );
    int rounds = 0;
    int result = sscanf( buffer, "%d", &rounds );
    if( result == 0 || rounds < 0 ){
        //Invalid input was supplied at this point.
        //The behavior in the situation is undefined, so I figured that I would supply an informative
        //error message instead of failing silently.
        printf("Failure: Invalid round count specified,\r\n");
        return 1;
    }
    int roundcounter = 0;
    while( rounds-- > 0 ){
        roundcounter++;
        fgets( buffer, 10000, stdin );
        std::string stringbuffer = buffer;
        std::vector<Number> numbers(3);
        size_t start = 0;
        size_t position = 0;
        for( int i = 0; i < 3; ++i ){
            start = position;
            position = stringbuffer.find_first_of(' ', start);
            if( (i != 2 && position == std::string::npos)){
                printf("Failure: Invalid number of numbers for round %d\r\n", roundcounter );
                return 1;
            }
            std::string num = stringbuffer.substr(start, position - start);
            numbers[i].Set( num );

            position++;
        }
        //Sort the array of numbers in ascending order
        Number temp;
        if( numbers[0].Compare(numbers[1]) > 0 ){
            temp = numbers[0];
            numbers[0] = numbers[1];
            numbers[1] = temp;
        }
        if( numbers[1].Compare(numbers[2]) > 0 ){
            temp = numbers[1];
            numbers[1] = numbers[2];
            numbers[2] = temp;
        }
        if( numbers[0].Compare(numbers[1]) > 0 ){
            temp = numbers[0];
            numbers[0] = numbers[1];
            numbers[1] = temp;
        }

        GameState state = CheckMovesFast( numbers );

        if( state == GoFirst ){
            printf("First\n");
        }
        else {
            printf("Second\n");
        }
    }
    return 0;
}
