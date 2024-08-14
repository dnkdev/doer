#ifndef _LEXER_H
#define _LEXER_H

#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum
{
    TOKEN_END = 0,
    TOKEN_INVALID,
    TOKEN_DOLLAR,
    TOKEN_DECL_SIGN,
    TOKEN_DOT,
    TOKEN_PERCENT,
    TOKEN_SYMBOL,
    TOKEN_NUMBER,
    TOKEN_ASTERISK,
    TOKEN_OPAREN,
    TOKEN_CPAREN,
    TOKEN_OCURLY,
    TOKEN_CCURLY,
    TOKEN_SEMICOLON,
    TOKEN_COLON,
    TOKEN_COMMENT,
    TOKEN_STRING,
    TOKEN_ARROW_RIGHT,
    TOKEN_TASK_SPACE,
    TOKEN_TASK_TABSPACE,
    TOKEN_SPACE,
    TOKEN_TABSPACE,
    TOKEN_SLASH,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_NOTEQ,
    TOKEN_EQ,
    TOKEN_EQEQ,
    TOKEN_AT,
    TOKEN_EXCL,
    TOKEN_LESS,
    TOKEN_GREATER,
    TOKEN_LESS_EQ,
    TOKEN_GREATER_EQ,
    TOKEN_NEWLINE,
} TokenKind;

typedef struct
{
    char *file_path;
    size_t row;
    size_t col;
} Loc;

typedef struct
{
    TokenKind kind;
    char *text;
    size_t text_len;
    Loc pos;
} Token;

typedef struct
{
    char *content;
    size_t content_len;
    size_t cursor;
    size_t line;
    size_t bol;       // beginning of the line
    bool is_var_decl; // if lexer detects :=, then all values after that are as one string
} Lexer_t;

Lexer_t *lexer_collect_file(char *file_path, Token **tokens, size_t *token_count);
const char *token_kind_name(TokenKind kind);

#define token_error(t, s, ...)                                                                                                         \
    fprintf(stderr, "%s:%zu:%zu" TERM_RED TERM_BOLD " error: " TERM_RESET s "\n", t.pos.file_path, t.pos.row, t.pos.col, __VA_ARGS__); \
    exit(1);

#define ptrtoken_error(t, s, ...)                                                                                                         \
    fprintf(stderr, "%s:%zu:%zu" TERM_RED TERM_BOLD " error: " TERM_RESET s "\n", t->pos.file_path, t->pos.row, t->pos.col, __VA_ARGS__); \
    exit(1);

#define is_space_token(kind) (kind == TOKEN_SPACE || kind == TOKEN_TABSPACE)

#endif // _LEXER_H