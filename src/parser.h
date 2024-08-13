#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"
#include "ast.h"
#include "common.h"

typedef struct
{
    char *content;
    size_t content_len;
    Token *tokens;
    Token *prev;
    Token *cur;
    Token *next;
    size_t cursor;
    size_t token_count;
} Parser_t;

Parser_t *parser_new(Lexer_t *l, Token *tokens, size_t token_count);
size_t parser_parse(Parser_t *p);
void parser_advance(Parser_t *p, int count);
static Token look(Parser_t *p, int count);
static size_t peek_to(Parser_t *p, TokenKind kind, Token *t);
static size_t peek_through_blankspaces(Parser_t *p, Token *token);

#define MAX_FUNC_NAME 32

static bool parse_func_call(Parser_t *p, Ast_t **ast);
static bool parse_var_decl(Parser_t *p, Ast_t **ast);
static bool parse_percent_directive(Parser_t *p, Ast_t **ast);
static bool parse_var(Parser_t *p, Ast_t **ast);

#define parser_ptrtoken_error(p, t, s, ...)                                                                                               \
    fprintf(stderr, "%s:%zu:%zu" TERM_RED TERM_BOLD " error: " TERM_RESET s "\n", t->pos.file_path, t->pos.row, t->pos.col, __VA_ARGS__); \
    exit(1);

#define parser_token_error(p, t, s, ...)                                                                                               \
    fprintf(stderr, "%s:%zu:%zu" TERM_RED TERM_BOLD " error: " TERM_RESET s "\n", t.pos.file_path, t.pos.row, t.pos.col, __VA_ARGS__); \
    exit(1);

#define pwarning(p, s, ...)                                                                                                             \
    fprintf(stderr, "%s:%zu:%zu" TERM_MAGENTA TERM_BOLD " warning: " TERM_RESET s "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
    exit(1);

#endif //_PARSER_H