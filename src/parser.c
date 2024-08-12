#include "parser.h"
#include "ast.h"
#include <stdio.h>

#define CUR p->tokens[p->cursor]
#define can_parse(p, x) p->token_count > p->cursor + x

Parser_t *parser_new(Lexer_t *l, Token *tokens, size_t token_count)
{

    Parser_t *p = malloc(sizeof(Parser_t));
    p->tokens = tokens, p->token_count = token_count, p->cursor = 0, p->content = l->content, p->content_len = l->content_len;
    return p;
}
void parser_advance(Parser_t *p, int count)
{
    p->cursor += count;
    p->token_count -= count;
}
// Ast_t parser_next(Parser_t *p)
// {
//     // parse_func_call(p);
// }
size_t parser_parse(Parser_t *p)
{
    Ast_t ast;
    while (p->token_count > 0)
    {
        // ast = parser_next(p);
        Token t = p->tokens[p->cursor];
        parser_advance(p, 1);
        fprintf(stdout, "'%.*s' %zu %zu\n", (int)t.text_len, t.text, t.pos.row, t.pos.col);
    }
    return 0;
}
static bool parse_func_call(Parser_t *p)
{
    if (CUR.kind == TOKEN_DOLLAR)
    {
        // Ast_t func = {.kind = AST_FUNC_CALL, .data = (struct AST_FUNC_CALL){.name = t.text}};
        Token next = look(p, 1);
        Token endtoken;
        size_t expr_len;
        if (next.kind == TOKEN_OPAREN)
        {
            size_t count = 0;
            while (can_parse(p, count))
            {
                if (p->tokens[p->cursor + count].kind == TOKEN_NEWLINE)
                {
                    parser_token_error(p, p->tokens[p->cursor + count], "Can't be newline in function call", NULL);
                }
                count += 1;
                if (p->tokens[p->cursor + count].kind == TOKEN_CPAREN)
                {
                    endtoken = p->tokens[p->cursor + count];
                    break;
                }
            }
            // size_t expr_tokens_count = peek_to(p, TOKEN_CPAREN, &endtoken) + 1; // +1 for TOKEN_CPAREN itself
            // printf("founded %zu | %d\n", expr_tokens_count, 0);
            // for (int i = 0; i < expr_tokens_count; i++)
            // {
            //     // Token t = ;
            //     if (p->tokens[p->cursor + i].kind == TOKEN_NEWLINE)
            //     {
            //         parser_token_error(p, p->tokens[p->cursor + i], "Can't be newline in function call", NULL);
            //         // fprintf(stderr, "%s:%zu:%zu error: CANT BE NEWLINE\n",
            //         //         p->tokens[p->cursor + i].pos.file_path, p->tokens[p->cursor + i].pos.row, p->tokens[p->cursor + i].pos.col);
            //     }
            //     // fprintf(stdout, "'%.*s' %zu %zu\n", (int)t.text_len, t.text, t.pos.row, t.pos.col);
            // }
        }
        else if (next.kind == TOKEN_SYMBOL)
        {
        }
    }
    return false;
}
static size_t peek_to(Parser_t *p, TokenKind kind, Token *t)
{
    size_t len = 0;
    while (can_parse(p, 1))
    {
        len += 1;
        if (p->tokens[p->cursor + len].kind == kind)
        {
            t = &p->tokens[p->cursor + len];
            return len;
        }
    }
    return len;
}
static Token look(Parser_t *p, int count)
{
    Token t;
    assert(count != 0);
    if (can_parse(p, count))
    {
        if (count > 0)
        {
            t = p->tokens[p->cursor + count];
        }
        else
        {
            t = p->tokens[p->cursor - count];
        }
    }
    return t;
}