#include <stdio.h>
#define C_LEXER_IMPL
#include "c_lexer.h"

int main(){
    char *symbols[] = {
        "+", "+="
    };
    char *str_dels[] = {
        "\"", "'"
    };
    c_lexer lexer = create_lexer(
        "\"This is a ++ string \\\" \" 'this is a \" string' ' this is dead",
        symbols,
        2,
        str_dels,
        2,
        '\\'
    );

    c_token token = {0};

    while(next_token(&lexer, &token)){
        printf("type: %d:%d - '%s'\n",token.type, token.id, token.token.str);
    }

    delete_token(&token);
    delete_lexer(&lexer);

    return 0;
}