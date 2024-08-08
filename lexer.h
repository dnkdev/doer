#ifndef _LEXER_H
#define _LEXER_H

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

// #include "common.h"

typedef enum
{
    TOKEN_END = 0,
    TOKEN_INVALID,
    TOKEN_DOLLAR,
    TOKEN_SFUNC,
    TOKEN_PREPROC,
    TOKEN_SYMBOL,
    TOKEN_OPAREN,
    TOKEN_CPAREN,
    TOKEN_OCURLY,
    TOKEN_CCURLY,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMENT,
    TOKEN_STRING
} TokenKind;

typedef struct
{
    const char *file_path;
    size_t row;
    size_t col;
} Loc;

typedef struct
{
    TokenKind kind;
    const char *text;
    size_t text_len;
    Loc pos;
} Token;

typedef struct
{
    const char *content;
    size_t content_len;
    size_t cursor;
    size_t line;
    size_t bol; // beginning of the line
} Lexer;

size_t lexer_collect_from(char *content, size_t content_len, Token *tokens);

#endif // _LEXER_H