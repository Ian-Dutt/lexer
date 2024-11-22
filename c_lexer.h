#ifndef __C_LEXER__
#define __C_LEXER__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct _const_str{
    char *str;
    size_t length;
} const_str;

typedef struct _mut_str{
    char *str;
    size_t length;
} mut_str;

typedef struct _c_lexer_ {
    char *stream;
    size_t length;
    size_t index;
    int loc;
    int line;
    const_str *symbols;
    int symbols_length;
    // FILE *log;
    const_str *string_delims;
    int delims_length;
    char escape_char;
} c_lexer;

typedef struct _c_token_{
    mut_str token;
    int type;
    int id;
} c_token;

c_lexer create_lexer(const char *stream, char **symbols, int num_symbols, char **string_tokens, int num_strs, char escape_char);
void delete_lexer(c_lexer *lexer);
void reset_lexer(c_lexer *lexer);

int next_token(c_lexer *lexer, c_token *token);

#define C_LEXER_IMPL
#ifdef C_LEXER_IMPL

#include <stdarg.h>
#define ERR(...) fprintf(stderr, __VA_ARGS__)

char *c_strdup(const char *s) {
    size_t size = strlen(s) + 1;
    char *p = (char *)malloc(size);
    if (p != NULL) {
        memcpy(p, s, size);
    }
    return p;
}

void c_str_realloc(char **dest, const char *s) {
    size_t size = strlen(s) + 1;
    char *p = (char *)realloc(*dest, size);
    if (p != NULL) {
        memcpy(p, s, size);
        *dest = p;
    }else{
        free(p);
    }
}

c_lexer create_lexer(const char *stream, char **symbols, int num_symbols, char **string_tokens, int num_strs, char escape_char){
    c_lexer lexer = {0};
    int i, j;

    for(i = 0; i < num_strs; ++i){
        for(j = 0; j < num_symbols; ++j){
            if(strstr(string_tokens[i], symbols[j])){
                ERR("Unable to create lexer. Start of string '%s' is in symbol '%s'\n", string_tokens[i], symbols[j]);
                exit(1);
            }
        }
    }

    lexer.length = strlen(stream);
    lexer.stream = c_strdup(stream);
    
    if(num_symbols){
        lexer.symbols_length = num_symbols;
        // lexer.symbols = symbols;
        lexer.symbols = malloc(num_symbols * sizeof(const_str));

        if(lexer.symbols == NULL){
            ERR("Unable to allocate memory\n");
            exit(1);
        }

        for(i = 0; i < num_symbols; ++i){
            lexer.symbols[i] = (const_str){symbols[i], strlen(symbols[i])};
        }
    }

    if(num_strs){
        lexer.delims_length = num_strs;
        lexer.escape_char = escape_char;

        lexer.string_delims = malloc(num_symbols * sizeof(const_str));

        if(lexer.symbols == NULL){
            ERR("Unable to allocate memory\n");
            free(lexer.symbols);
            free(lexer.stream);
            exit(1);
        }

        for(i = 0; i < num_strs; ++i){
            lexer.string_delims[i] = (const_str){string_tokens[i], strlen(string_tokens[i])};
        }
    }

    return lexer;
}

void delete_lexer(c_lexer *lexer){
    free(lexer->stream);
    free(lexer->string_delims);
    free(lexer->symbols);
    memset(lexer, 0, sizeof(c_lexer));
}
void reset_lexer(c_lexer *lexer){
    lexer->line = 0;
    lexer->loc = 0;
    lexer->index = 0;
}

int is_new_token(c_lexer *lexer, size_t i){
    size_t max_length = 0;
    int index = 0;
    const char *start = lexer->stream + i;
    if(isspace(*start)){
        return -1;
    }

    for(int j = 0; j < lexer->delims_length; ++j){
        if(strncmp(start, lexer->string_delims[j].str, lexer->string_delims[j].length) == 0){
            return -1;
        }
    }

    for(int j = 0; j < lexer->symbols_length; ++j){
        if(strncmp(start, lexer->symbols[j].str, lexer->symbols[j].length) == 0){
            if(lexer->symbols[j].length > max_length){
                index = j + 1;
                max_length = lexer->symbols[j].length;
            }
        }
    }

    return index;
}

int lexer_type(c_lexer *lexer, const char *token){
    int i = 0;
    for(; i < lexer->symbols_length; ++i){
        // puts(lexer->symbols[i].str);
        if(strcmp(lexer->symbols[i].str, token) == 0){
            return 1;
        }
    }
    return 0;
}

int lexer_id(c_lexer *lexer, const char *token){
    int i = 0;
    for(; i < lexer->symbols_length; ++i){
        if(strcmp(lexer->symbols[i].str, token) == 0){
            return i;
        }
    }
    return -1;
}

int is_word(const char *token){
    if(token[0] == '\0' || (!isalpha(token[0]) && token[0] != '_')){
        return -1;
    }

    for(int i = 1; token[i]; ++i){
        if(!isalnum(token[i]) && token[i] != '_'){
            return -i - 1;
        }
    }

    return 1;
}

int next_token(c_lexer *lexer, c_token *token){
    size_t index = lexer->index;
    size_t i = 0;
    char tmp[1000];

    while(index < lexer->length && isspace(lexer->stream[index])){
        index++;
    }

    int sym = is_new_token(lexer, index);

    if(sym){
        const_str *symbol = &lexer->symbols[sym - 1];
        // printf("%d\n", sym);

        // printf("%s\n", symbol->str);
        strcpy(tmp, symbol->str);

        lexer->index = index + symbol->length;

        i = symbol->length;

    }else{

        while(index < lexer->length && !is_new_token(lexer, index)){
            tmp[i++] = lexer->stream[index++];
        }

        lexer->index = index;

        if(i == 0){
            return 0;
        }
        
        tmp[i] = '\0';
        int word = is_word(tmp);

        if(word < 0){
            fprintf(stderr, "Unknown Symbol '%s' found in '%s'\n", tmp + (-word - 1), tmp);
            return 0;
        }
    }
    token->token.length = i;
    c_str_realloc(&(token->token.str), tmp);
    token->type = lexer_type(lexer, tmp);
    token->id = token->type == 1 ? lexer_id(lexer, tmp) : 0;

    return 1;
}

void delete_token(c_token *token){
    free(token->token.str);
}
#endif

#endif