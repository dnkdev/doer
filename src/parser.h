#ifndef _PARSER_H
#define _PARSER_H

#include "lexer.h"
#include "ast.h"
#include "interpret.h"
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
size_t parser_parse(Parser_t *p, Ast_t *ast);
void parser_advance(Parser_t *p, int count);
static Token look(Parser_t *p, int count);
static size_t peek_to(Parser_t *p, TokenKind kind, Token *t);
static size_t peek_through_blankspaces(Parser_t *p, Token *token);

#define MAX_FUNC_NAME 32
#define MAX_PERCENT_DIRECTIVE_NAME 8

static AstNode_t *parse_command(Parser_t *p, Ast_t *ast);
static AstNode_t *parse_task_decl(Parser_t *p, Ast_t *ast);
static AstNode_t *parse_func_call(Parser_t *p, Ast_t *ast);
static AstNode_t *parse_var_decl(Parser_t *p, Ast_t *ast);
static AstNode_t *parse_percent_directive(Parser_t *p, Ast_t *ast);

#define parser_eat_spaces(p, spaces_count) \
    for (int i = 0; i < spaces_count; i++) \
    {                                      \
        parser_eat(p, TOKEN_SPACE);        \
    }

#define parser_ast_error(p, ast, ...)                               \
    fprintf(stderr, "%.*s", (int)ast->len, ast->text, __VA_ARGS__); \
    exit(1);

#define parser_token_error(p, t, s, ...)                                                                                                                        \
    fprintf(stderr, TERM_RED TERM_BOLD "error: " TERM_RESET TERM_BOLD "%s:%zu:%zu: " s TERM_RESET "\n", t->pos.file_path, t->pos.row, t->pos.col, __VA_ARGS__); \
    exit(1);

#define pwarning(p, t, s, ...)                                                                                                                                       \
    fprintf(stderr, TERM_MAGENTA TERM_BOLD "warning: " TERM_RESET TERM_BOLD "%s:%zu:%zu " s TERM_RESET "\n", t->pos.file_path, t->pos.row, t->pos.col, __VA_ARGS__); \
    exit(1);

#define is_parser_parsed(p, ast) \
    (parse_task_decl(p, ast) != NULL || parse_var_decl(p, ast) != NULL || parse_percent_directive(p, ast) != NULL || parse_func_call(p, ast) != NULL)

#endif //_PARSER_H