#include<stdint.h>
#include<vector>
#include<string>
#include<cstdio>
#include<cmath>

#define USE_FILESTREAMS
const uint64_t Mask32Bit = 0xFFFFFFFFull;

class Number{
    std::vector<uint64_t> data;
    public:
    Number( int precision = 0 ){
        data.resize( precision/32, 0 );
    }
    Number( const Number& other ){
        data = other.data;
    }
    int Compare( Number& value ){
        int i = data.size();
        int j = value.data.size();
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
    Number& AddSelf( uint32_t value ){

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
    Number& AddSelf( Number& value ){
        while( data.size() < value.data.size() ){
            data.insert( data.begin(), 0 );
        }
        while( value.data.size() < data.size() ){
            value.data.insert( value.data.begin(), 0 );
        }
        for( size_t i = 0; i < data.size(); ++i ){
            data[i] += value.data[i] & Mask32Bit;
        }
        AddSelf(0); //AddSelf automatically handles carry propagation
        return *this;
    }
    Number& SubtractSelf( Number& value ){
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

    Number& MultiplySelf( uint32_t value ){
        for( size_t i = 0; i < data.size(); ++i ){
            data[i] *= value;
        }
        AddSelf(0); //AddSelf automatically handles carry propagation

        return *this;
    }
    Number& SetSelf( std::string value ){
        data.clear();
        for( size_t i = 0; i < value.length(); ++i ){
            if( value[i] >= '0' && value[i] <= '9' ){
                MultiplySelf( 10 );
                AddSelf( value[i] - '0' );
            }
        }
        return *this;
    }
};

struct Range{ Number Minimum, Maximum; };
std::vector<Range> Patterns;


void DeterminePattern(){
    //This can be determined more trivially, but the algorithm is left as is because it's fast enough and more clear.
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
    Number n(0), prev(0);
    Number one(0);
    one.SetSelf("1");
    prev.SetSelf( "1" );
    n.SetSelf( "3" );
    patternrange.Minimum = prev;
    patternrange.Maximum = prev;
    Patterns.push_back( patternrange );
    Number maximum(0);
    maximum.SetSelf("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    while( n.Compare( maximum ) < 0 ){
        Number diff = n;

        patternrange.Minimum = n;
        diff.AddSelf( n );
        diff.SubtractSelf( one );
        patternrange.Maximum = diff;
        diff = n;
        diff.SubtractSelf( prev );
        diff.MultiplySelf( 4 );
        prev = n;
        n.AddSelf( diff );
        Patterns.push_back( patternrange );
    }
}

enum GameState{
    GoSecond,
    GoFirst
};


GameState CheckMovesFast(std::vector<Number> numbers){

    if( numbers[1].Compare(numbers[2]) == 0 && numbers[1].Compare(numbers[0]) == 0 )
        return GoSecond;
    numbers[2].SubtractSelf(numbers[1]);

    numbers[1].SubtractSelf(numbers[0]);
    if( numbers[1].Compare(numbers[2]) > 0 ){
        numbers[0] = numbers[1];
        numbers[1] = numbers[2];
        numbers[2] = numbers[0];
    }

    size_t i;
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[2]) < 0; ++i ){};
    if( Patterns[i].Minimum.Compare(numbers[2]) > 0 )
        return GoFirst;
    numbers[0] = Patterns[i].Maximum;
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[1]) < 0; ++i ){};
    if( Patterns[i].Minimum.Compare(numbers[1]) > 0 )
        return GoFirst;
    Number num1plus1(0);
    num1plus1 = numbers[1];
    num1plus1.AddSelf( 1 );
    if( numbers[1].Compare(numbers[2]) != 0 && num1plus1.Compare(numbers[2]) != 0){
        numbers[0].SubtractSelf(numbers[1]);
        if( numbers[0].Compare( numbers[2] ) < 0 ){
            return GoFirst;
        }
    }
    return GoSecond;
}

int main(){
    DeterminePattern();
    char buffer[10000];
    #ifdef USE_FILESTREAMS
    freopen( "input.txt", "r", stdin );
    freopen( "output.txt", "w", stdout );
    #endif
    fgets( buffer, 10000, stdin );
    int rounds = 0;
    int result = sscanf( buffer, "%d", &rounds );
    if( result == 0 || rounds < 0 ){
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
            numbers[i].SetSelf( num );

            position++;
        }
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

        GameState first = CheckMovesFast( numbers );

        if( first == GoFirst ){
            printf("First\n");
        }
        else {
            printf("Second\n");
        }
    }
    return 0;
}
