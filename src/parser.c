#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>

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
}
void parser_next(Parser_t *p, Ast_t *ast)
{
    if (
        parse_func_call(p, ast) ||
        parse_var_decl(p, ast) ||
        parse_var(p, ast))
        return;
}
size_t parser_parse(Parser_t *p)
{
    Ast_t ast;
    while (p->cursor < p->token_count)
    {
        parser_next(p, &ast);
        Token t = p->tokens[p->cursor];
        fprintf(stdout, "'%.*s' (%s) %.*s\n", 0, (char *)0, ast_kind_name(ast.kind), (int)t.text_len, t.text);
        parser_advance(p, 1);
    }
    return 0;
}
static bool parse_var_decl(Parser_t *p, Ast_t *ast)
{
    if (CUR.kind == TOKEN_SYMBOL)
    {
        size_t count = 1;
        while (can_parse(p, count))
        {
            Token next = look(p, count);
            count++;
            switch (next.kind)
            {
            case TOKEN_SPACE:
                continue;
            case TOKEN_EQ:
                // count++;
                while (can_parse(p, count))
                {
                    printf("OK");
                    Token next_assign = look(p, count);
                    switch (next_assign.kind)
                    {
                    case TOKEN_SPACE:
                        count++;
                        continue;
                    case TOKEN_SYMBOL:
                        ast = AST_NEW(AST_VAR, malloc(sizeof(char) * next_assign.text_len + 1));
                        strncpy(ast->data.AST_VAR.name, next_assign.text, (int)next_assign.text_len);
                        ast->data.AST_VAR.name[next_assign.text_len] = '\0';
                        break;
                    case TOKEN_STRING:
                        ast = AST_NEW(AST_STRING, malloc(sizeof(char) * next_assign.text_len + 1));
                        strncpy(ast->data.AST_VAR.name, next_assign.text, (int)next_assign.text_len);
                        ast->data.AST_VAR.name[next_assign.text_len] = '\0';
                        break;
                    default:
                        return parser_token_error(p, next_assign, "unexpected %s", token_kind_name(next_assign.kind));
                    }
                }
                break;
            default:
                return parser_token_error(p, next, "unexpected %s", token_kind_name(next.kind));
            }
            // count++;
        }
        return true;
    }
    return false;
}
static bool parse_var(Parser_t *p, Ast_t *ast)
{
    if (CUR.kind == TOKEN_SYMBOL)
    {
        ast = AST_NEW(AST_VAR, malloc(sizeof(char) * CUR.text_len + 1));
        strncpy(ast->data.AST_VAR.name, CUR.text, (int)CUR.text_len);
        ast->data.AST_VAR.name[CUR.text_len] = '\0';
        return true;
    }
    return false;
}
static bool parse_func_call(Parser_t *p, Ast_t *ast)
{
    if (CUR.kind == TOKEN_DOLLAR)
    {
        // Ast_t func = {.kind = AST_FUNC_CALL, .data = (struct AST_FUNC_CALL){.name = t.text}};
        Token next = look(p, 1);
        parser_advance(p, 1);
        Token endtoken;
        size_t expr_len;
        if (next.kind == TOKEN_OPAREN)
        {
            size_t count = 0;
            while (can_parse(p, count))
            {
                parser_advance(p, 1);
                count += 1;
                Token t = p->tokens[p->cursor + count];
                printf("'%.*s' %s\n", (int)t.text_len, t.text, token_kind_name(t.kind));
                Ast_t *arguments;
                size_t arg_count;
                char func_name[MAX_FUNC_NAME];
                switch (t.kind)
                {
                case TOKEN_SPACE:
                    continue;
                    break;
                case TOKEN_SYMBOL:
                    if (count == 2) // function name
                    {
                        size_t len = (t.text_len < MAX_FUNC_NAME) ? (t.text_len) : (MAX_FUNC_NAME - 1);
                        strncpy(func_name, t.text, (int)len);
                        continue;
                    }
                    arguments = AST_NEW(AST_VAR, malloc(sizeof(char) * t.text_len + 1));
                    strncpy(arguments->data.AST_VAR.name, t.text, (int)t.text_len);
                    arguments->data.AST_VAR.name[t.text_len] = '\0';
                    break;
                case TOKEN_NEWLINE:
                    parser_token_error(p, p->tokens[p->cursor + count], "Can't be newlines in function call expression.", NULL);
                    break;
                case TOKEN_CPAREN:
                    ast = AST_NEW(AST_FUNC_CALL, next.text, &arguments, arg_count);
                    endtoken = t;
                    break;
                default:
                    break;
                }
                // if (t.kind == TOKEN_NEWLINE)
                // {
                // }
                // if (t.kind == TOKEN_CPAREN)
                // {
                // }
            }
            parser_token_error(p, next, "Can't found a function end.", NULL);
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