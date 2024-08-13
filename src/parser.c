#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>

#define CUR p->tokens[p->cursor]
#define can_parse(p, x) p->token_count > p->cursor + x

Parser_t *parser_new(Lexer_t *l, Token *tokens, size_t token_count)
{
    Parser_t *p = malloc(sizeof(Parser_t));
    p->tokens = tokens,
    p->token_count = token_count,
    p->cursor = 0,
    p->content = l->content,
    p->content_len = l->content_len,
    p->cur = &p->tokens[p->cursor];

    return p;
}
bool parser_eat(Parser_t *p, TokenKind kind)
{
    if (kind == CUR.kind)
    {
        p->prev = &p->tokens[p->cursor];
        p->cursor += 1;
        p->cur = &p->tokens[p->cursor];
        p->next = (can_parse(p, 1)) ? (&p->tokens[p->cursor + 1]) : (&CUR); // p->tokens[p->cursor];
    }
    else
    {
        parser_token_error(p, CUR, "Unexpected (%s) '%.*s', expected (%s)", token_kind_name(CUR.kind), (int)CUR.text_len, CUR.text, token_kind_name(kind));
    }
    return false;
}
void parser_advance(Parser_t *p, int count)
{
    p->prev = &p->tokens[p->cursor];
    p->cursor += count;
    p->cur = &p->tokens[p->cursor];
    p->next = (can_parse(p, 1)) ? (&p->tokens[p->cursor + 1]) : (&CUR); // p->tokens[p->cursor];
}
bool parser_next(Parser_t *p, Ast_t **ast)
{
    if (
        parse_var_decl(p, ast))
        // parse_var(p, ast) ||
        // parse_func_call(p, ast))
        return true;
    else
    {
        if (p->cur->kind != TOKEN_NEWLINE)
        {
            parser_ptrtoken_error(p, p->cur, "unhandled token `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
        }
    }
    return false;
}
size_t parser_parse(Parser_t *p)
{
    size_t node_count;
    Ast_t *ast = malloc(sizeof(Ast_t));
    while (p->cursor < p->token_count)
    {
        switch (p->cur->kind)
        {
        case TOKEN_NEWLINE:
        case TOKEN_SPACE:
        case TOKEN_TABSPACE:
            parser_advance(p, 1);
            continue;
            break;
        default:
            break;
        }
        if (p->cur->kind == TOKEN_END)
        {
            return node_count;
        }
        if (parser_next(p, &ast))
        {
            printf("added node: %s; %.*s\n", ast_kind_name(ast->kind), (int)ast->len, ast->text);
            node_count++;
            parser_advance(p, 1);
        }
        // Token t = *p->cur;
        // fprintf(stdout, "(%s) %.*s %s\n", ast_kind_name(ast.kind), (int)t.text_len, t.text, token_kind_name(t.kind));
    }
    return 0;
}
static bool parse_var_decl(Parser_t *p, Ast_t **ast)
{
    if (p->cur->kind == TOKEN_SYMBOL)
    {
        Token *name_token = p->cur;
        Token next;
        size_t spaces_count = 0;
        if ((spaces_count = peek_through_blankspaces(p, &next)) != -1)
        {
            // printf("= here is `%s` spaces: %zu\n", token_kind_name(next.kind), spaces_count);
            if (next.kind == TOKEN_DECL_SIGN)
            {
                parser_eat(p, TOKEN_SYMBOL);
                for (int i = 0; i < spaces_count; i++)
                {
                    parser_eat(p, TOKEN_SPACE);
                }
                spaces_count = 0;
                parser_eat(p, TOKEN_DECL_SIGN);
                while (p->cur->kind == TOKEN_SPACE)
                {
                    spaces_count++;
                    parser_eat(p, TOKEN_SPACE);
                }
                if (p->cur->kind == TOKEN_STRING)
                {
                    *ast = AST_NEW(AST_VAR_DECL,
                                   malloc(sizeof(char) * name_token->text_len + 1),
                                   AST_NEW(AST_STRING, malloc(sizeof(char) * p->cur->text_len + 1)));

                    strncpy((*ast)->data.AST_VAR_DECL.name, name_token->text, (int)name_token->text_len);
                    strncpy((*ast)->data.AST_VAR_DECL.value->data.AST_STRING.value, p->cur->text, (int)p->cur->text_len);
                    (*ast)->data.AST_VAR_DECL.value->data.AST_STRING.value[p->cur->text_len] = '\0';
                    (*ast)->data.AST_VAR_DECL.name[name_token->text_len] = '\0';
                    (*ast)->len = p->cur->text - name_token->text + p->cur->text_len;
                    (*ast)->text = name_token->text;
                    parser_eat(p, TOKEN_STRING);
                    return true;
                }
                else
                {
                    parser_ptrtoken_error(p, p->cur, "can't parse declaration", NULL);
                }
            }
        }
        // else
        // {
        //     parser_ptrtoken_error(p, p->cur, "can't parse", NULL);
        // }
    }
    return false;
}
static bool parse_var(Parser_t *p, Ast_t **ast)
{
    if (CUR.kind == TOKEN_SYMBOL)
    {

        return true;
    }
    return false;
}
static bool parse_func_call(Parser_t *p, Ast_t **ast)
{
    if (CUR.kind == TOKEN_DOLLAR)
    {
        return true;
    }
    return false;
}
static size_t peek_to(Parser_t *p, TokenKind kind, Token *t)
{
    size_t len = 0;
    while (can_parse(p, len))
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
static size_t peek_through_blankspaces(Parser_t *p, Token *t)
{
    size_t spaces_count = 0;
    int i = 1;
    while (can_parse(p, i))
    {
        if (p->tokens[p->cursor + i].kind == TOKEN_SPACE)
        {

            i++;
            spaces_count++;
            printf("- found space on %zu ", p->cursor + spaces_count);
            continue;
        }
        else if (p->tokens[p->cursor + i].kind == TOKEN_END)
        {
            return -1;
        }
        else /// if (can_parse(p, spaces_count))
        {
            *t = p->tokens[p->cursor + i];
            printf("peeked to `%s` spaces: %zu\n", token_kind_name(t->kind), spaces_count);
            return spaces_count;
        }
        i++;
    }
    for (int i = 1; i < 100; ++i)
    {
        if (can_parse(p, i))
        {
        }
    }
    while (can_parse(p, spaces_count + 1))
    {
        printf("can parse, next is: %s\n", token_kind_name(p->tokens[p->cursor + spaces_count + 1].kind));
        if (p->tokens[p->cursor + spaces_count + 1].kind == TOKEN_SPACE)
        {
            spaces_count++;
            printf("- found space on %zu ", p->cursor + spaces_count);
            continue;
        }
        else if (p->tokens[p->cursor + spaces_count + 1].kind == TOKEN_END)
        {
            return -1;
        }
        else /// if (can_parse(p, spaces_count))
        {
            *t = p->tokens[p->cursor + spaces_count + 1];
            printf("peeked to `%s` spaces: %zu\n", token_kind_name(t->kind), spaces_count);
            return spaces_count;
        }
    }
    printf("yeo\n");
    return -1;
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