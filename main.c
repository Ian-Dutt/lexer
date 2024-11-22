#include <stdio.h>
#define C_LEXER_IMPL
#include "c_lexer.h"

int main(){
    char *symbols[] = {
        "+", "+="
    };
    c_lexer lexer = create_lexer(
        "++= +=+ ++ _name_of_thing()",
        symbols,
        2,
        0,
        NULL,
        0,
        '\0'
    );

    c_token token = {0};

    while(next_token(&lexer, &token)){
        printf("type: %d:%d - '%s'\n",token.type, token.id, token.token.str);
    }

    delete_token(&token);
    delete_lexer(&lexer);

    return 0;
}