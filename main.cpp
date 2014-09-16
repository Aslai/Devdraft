#include<stdint.h>
#include<vector>
#include<string>
#include<cstdio>
#include<cmath>

class Number{
    std::vector<uint64_t> data;
    public:
    Number( int precision = 0 ){
        data.resize( precision/32, 0 );
    }
    Number( const Number& other ){
        data = other.data;
    }
    uint32_t GetLSD(){
        return data[data.size()-1];
    }
    void print(){
        for( size_t i = 0; i < data.size(); ++i ){
            printf("%08X", data[i]);
        }
        printf("\n");
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
                data[i-1] &= 0xFFFFFFFFull;
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
            data[i] += value.data[i] & 0xFFFFFFFFull;
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
            uint32_t a = i > 0 ? data[i-1]& 0xFFFFFFFFull : 0;
            uint32_t b = j > 0 ? value.data[j-1]& 0xFFFFFFFFull : 0;
            uint32_t result = a - b - carry;
            carry = 0;
            if( a < b ){
                carry = a;
            }
            if( i > 0 ){
                data[i-1]=result & 0xFFFFFFFFull;
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
    Number& ShiftRightSelf(){

        uint64_t carry = 0;
        for( size_t i = 0; i < data.size(); ++i ){
            data[i] |= carry << 32;
            carry = data[i] & 1;
            data[i] >>= 1;
        }

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
    //maximum.SetSelf("10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    maximum.SetSelf("100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    while( n.Compare( maximum ) < 0 ){
        //n.print();
        //printf("\n");

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
    ForcedWin,
    ForcedLoss,
    Indeterminable,
    GoSecond,
    GoFirst
};
enum CurrentMove{
    Player,
    AI
};

std::vector<bool> primelist(100000, false);
bool hasinitialized = false;

bool isPrime(uint32_t number){
    if(hasinitialized == false){
        primelist[0] = primelist[1] = true;
        for( int i = 2; i < 100; ++i ){
            for( int j = 2; j < 100; ++j ){
                primelist[i*j] = true;
            }
        }
        hasinitialized = true;
    }
    return !primelist[number];
}

GameState CheckMovesFast(std::vector<Number> numbers, CurrentMove who){

    if( numbers[1].Compare(numbers[2]) == 0 && numbers[1].Compare(numbers[0]) == 0 )
        return GoSecond;
    numbers[2].SubtractSelf(numbers[1]);

    numbers[1].SubtractSelf(numbers[0]);
    if( numbers[1].Compare(numbers[2]) > 0 ){
        numbers[0] = numbers[1];
        numbers[1] = numbers[2];
        numbers[2] = numbers[0];
    }
    //numbers[1].print();
    //numbers[2].print();
    size_t i;
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[2]) < 0; ++i ){};
    //if( i > 0 && Patterns[i].Minimum.Compare(numbers[2]) > 0 )
    //    --i;
    //printf("i: %d\t", i);
    if( Patterns[i].Minimum.Compare(numbers[2]) > 0 )
        return GoFirst;
    numbers[0] = Patterns[i].Maximum;
    for(i = 0; i < Patterns.size() && Patterns[i].Maximum.Compare(numbers[1]) < 0; ++i ){};
    //if( i > 0 && Patterns[i].Minimum.Compare(numbers[1]) > 0 )
    //    --i;
    //printf("%d\n", i);
    //Patterns[i].Minimum.print();
    //Patterns[i].Maximum.print();
    if( Patterns[i].Minimum.Compare(numbers[1]) > 0 )
        return GoFirst;
    Number num1plus1(0);
    num1plus1 = numbers[1];
    num1plus1.AddSelf( 1 );
    if( numbers[1].Compare(numbers[2]) != 0 && num1plus1.Compare(numbers[2]) != 0){
        numbers[0].SubtractSelf(numbers[1]);
        //printf("%d\n", numbers[0].GetLSD() );
        if( numbers[0].Compare( numbers[2] ) < 0 ){
            return GoFirst;
        }
    }

    return GoSecond;
}
int Depth = 0;
GameState CheckMoves(std::vector<Number> numbers, CurrentMove who, int depth){
    if( Depth < depth )
        Depth = depth;
    std::vector<GameState> states;
    if( (numbers[0].Compare(numbers[1]) == 0) && (numbers[0].Compare(numbers[2]) == 0) ){
        if( who == Player ){
            return ForcedLoss;
        }
        return ForcedWin;
    }

    for( int i = 0; i < 3; ++i ){
        std::vector<Number> numberstester = numbers;
        //Average the other two numbers
        numberstester[i] = numberstester[(i+1)%3];
        numberstester[i].AddSelf(numberstester[(i+2)%3]);
        numberstester[i].ShiftRightSelf();

        if( numberstester[i].Compare( numbers[i] ) == 0 ){ //If the new value is equal to the old value, don't test this case
            continue;
        }
        GameState state = CheckMoves(numberstester, who==Player?AI:Player, depth + 1 );
        if( state == ForcedWin && who == Player )
            return ForcedWin;
        states.push_back( state );
    }
    int losscount = 0;
    int wincount = 0;
    for( size_t i = 0; i < states.size(); ++i ){
        if( states[i] == ForcedLoss ){
            if( who == AI ){
                return ForcedLoss;
            }
            losscount++;
        }
        if( states[i] == ForcedWin ){
            if( who == Player ){
                return ForcedWin;
            }
            wincount++;
        }
    }
    if( wincount == states.size() ){
        return ForcedWin;
    }
    if( losscount == states.size() ){
        return ForcedLoss;
    }
    return Indeterminable;
}

int main(){
    DeterminePattern();

    /*FILE* input = fopen( "input.txt", "w" );
    fprintf(input, "10000\n");
    for( int i = 0; i < 100; ++i ){
        for( int j = 0; j < 100; ++j ){
            fprintf( input, "%d %d %d\n", 10, 10 + i, 10 + i + j );
        }
    }
    fclose(input);*/
    char buffer[10000];
    freopen( "input.txt", "r", stdin );
    freopen( "output.txt", "w", stdout );
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
            //printf("%s|\n", num.c_str());
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
        /*int a, b, c;
        a = numbers[1].GetLSD() - numbers[0].GetLSD();
        b = numbers[2].GetLSD() - numbers[1].GetLSD();
        c = numbers[2].GetLSD() - numbers[0].GetLSD();*/
        /*printf( "%02d %02d %02d\t%03d%c %03d%c\t%03d%c\t%d\t%d\t%d\t%d\t%d\t%d", numbers[0].GetLSD(), numbers[1].GetLSD(), numbers[2].GetLSD(),
                            a,
                            isPrime(a)?'P':' ',
                            b,
                            isPrime(b)?'P':' ',
                            c,
                            isPrime(c)?'P':' ',
                            (a + b) / 2 + c,
                            a*b+c,
                            c?(a+b) / c:0,
                            c?a*b/c:0,
                            a*b/2+c,
                            a*b
                            );*/
        //printf("%02d %02d\t", a, b );
        /*Depth = 0;
        GameState first = CheckMoves( numbers, Player, 1 );
        GameState realstate = Indeterminable;
        if( first == ForcedWin ){
            realstate = GoFirst;
        }
        else{
            GameState second = CheckMoves( numbers, AI, 1 );
            if( second == ForcedWin ){
                realstate = GoSecond;
            }
            else if( first == ForcedLoss && second == ForcedLoss ){
                printf("Neither\n");
            }
            else if( first == Indeterminable || second == Indeterminable ){
                printf("Indeterminable\n");
            }
        }*/

        GameState first = CheckMovesFast( numbers, Player );
        //if( first == GoSecond )
        //printf("Game Depth %d \t", Depth);
        if( first == GoFirst ){
            printf("First\n");
        }
        else {
            printf("Second\n");
        }

        //double depth = log(a + b) / log(3) * 2;
        /*if( first == GoSecond && ((int)floor( depth )) % 2 == 1 ){
                first = GoFirst;
        }*/
        /*if( first != realstate ){

            if( first == GoFirst )
                printf("Pattern says Go First, real check does not.\t");
            if( first == GoSecond )
                printf("Pattern says Go Second, real check does not.\t");
            printf("Round: %d\t%d %d %d\t%d %d\tDepth %f\n\n", roundcounter, numbers[0].GetLSD(), numbers[1].GetLSD(), numbers[2].GetLSD(), a, b, depth );
        }*/


    }
    return 0;
}
