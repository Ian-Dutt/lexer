#include <stdio.h>
#define C_LEXER_IMPL
#include "c_lexer.h"

char *read_file(const char *filename, long *length){
    FILE *fp = fopen(filename, "r");
    long size;
    char *contents;
    if(fp == NULL){
        ERR("Unable to open file %s\n", filename);
        exit(1);
    }

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    contents = calloc(size + 1, sizeof(char));
    if(contents == NULL){
        ERR("Unable to allocate memory for file\n");
        fclose(fp);
        exit(1);
    }

    fread(contents, size, sizeof(char), fp);
    fclose(fp);
    *length = size;

    return contents;
}

int main(){
    long length;
    char *symbols[] = {
        "func", "{", "}", "(", ")", "->", "return", "+", ";", ","
    };
    char *str_dels[] = {
        "\"", "'"
    };
    c_lexer lexer = create_lexer(
        read_file("lang.va", &length),
        symbols,
        10,
        NULL,
        0,
        '\0'
    );

    c_token token = {0};

    while(next_token(&lexer, &token)){
        printf("type: %d - '%s'\n",token.type, token.token.str);
    }

    delete_token(&token);
    delete_lexer(&lexer);

    return 0;
}