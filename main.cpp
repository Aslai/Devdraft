#include<cstdio>

int problem1();
int problem2();

int main( int argc, char* argv[] ){
    if( argc > 1 ){
        switch( argv[1][0] ){
            case '1': return problem1();
            case '2': return problem2();
            default:
                break;
        }
    }
    printf("Usage: devdraft [problem number]\nValid numbers are 1 and 2.\n");
    return 1;
}
