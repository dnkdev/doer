#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>

#define CUR p->cur
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
    if (kind == CUR->kind)
    {
        p->prev = &p->tokens[p->cursor];
        p->cursor += 1;
        p->cur = &p->tokens[p->cursor];
        p->next = (can_parse(p, 1)) ? (&p->tokens[p->cursor + 1]) : (CUR); // p->tokens[p->cursor];
    }
    else
    {
        parser_token_error(p, CUR, "Unexpected (%s) '%.*s', expected (%s)", token_kind_name(CUR->kind), (int)CUR->text_len, CUR->text, token_kind_name(kind));
    }
    return false;
}
void parser_advance(Parser_t *p, int count)
{
    p->prev = &p->tokens[p->cursor];
    p->cursor += count;
    p->cur = &p->tokens[p->cursor];
    p->next = (can_parse(p, 1)) ? (&p->tokens[p->cursor + 1]) : (CUR); // p->tokens[p->cursor];
}
bool parser_next(Parser_t *p, Ast_t *ast)
{
    if (
        parse_var_decl(p, ast) ||
        parse_percent_directive(p, ast) ||
        // parse_var(p, ast) ||
        parse_func_call(p, ast))
        return true;
    else
    {
        if (p->cur->kind != TOKEN_NEWLINE && p->cur->kind != TOKEN_SPACE && p->cur->kind != TOKEN_END)
        {
            parser_token_error(p, p->cur, "unhandled token `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
        }
    }
    return false;
}
size_t parser_parse(Parser_t *p, Ast_t *ast)
{
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
        case TOKEN_END:
            return ast->nodes_count;
        default:
            if (parser_next(p, ast))
            {
                // printf("parsed good %s\n", token_kind_name(p->cur->kind));
                //     // inter_collect(inter);
                parser_advance(p, 1);
            }
            else
            {

                printf("parsed bad\n");
            }
            break;
        }
    }
    return 0;
}
static bool parse_func_call(Parser_t *p, Ast_t *ast)
{
    if (p->cur->kind == TOKEN_DOLLAR)
    {
        if (can_parse(p, 1))
        {
            Token after_dollar = look(p, 1);
            if (after_dollar.kind == TOKEN_OPAREN)
            {
                char *func_start = p->cur->text;
                parser_eat(p, TOKEN_DOLLAR);
                parser_eat(p, TOKEN_OPAREN);
                if (p->cur->kind == TOKEN_SYMBOL)
                {
                    char *func_name = malloc(sizeof(char) * p->cur->text_len + 1);
                    strncpy(func_name, p->cur->text, p->cur->text_len);
                    func_name[p->cur->text_len] = '\0';
                    parser_eat(p, TOKEN_SYMBOL);
                    size_t arg_count = 0;
                    // FIX: TEMPORARY 10 ARGUMENTS MAX
                    AstNode_t **temp_arguments = malloc(10 * sizeof(AstNode_t *));
                    while (p->cur->kind != TOKEN_CPAREN)
                    {
                        while (p->cur->kind == TOKEN_SPACE)
                        {
                            parser_eat(p, TOKEN_SPACE);
                        }
                        char *argument_start = p->cur->text;
                        size_t argument_len;
                        while (!is_space_token(p->cur->kind) && p->cur->kind != TOKEN_CPAREN && p->cur->kind != TOKEN_NEWLINE)
                        {
                            parser_eat(p, p->cur->kind);
                        }
                        argument_len = p->cur->text - argument_start;
                        char *string = malloc(sizeof(char) * argument_len + 1);
                        strncpy(string, argument_start, argument_len);
                        string[argument_len] = '\0';
                        AstNode_t *node =
                            ast_new_node(ast, (struct AstNode_t){
                                                  .text = argument_start,
                                                  .len = argument_len,
                                                  .kind = AST_STRING,
                                                  .data.AST_STRING = (struct AST_STRING){
                                                      .value = string}});
                        temp_arguments[arg_count] = node;
                        arg_count += 1;
                    }
                    parser_eat(p, TOKEN_CPAREN);
                    AstNode_t **arguments = malloc(sizeof(AstNode_t *) * arg_count);
                    arguments = temp_arguments;
                    printf("Argument count: %zu\n", arg_count);
                    free(temp_arguments);

                    AstNode_t *func_call_node = ast_new_node(ast, (struct AstNode_t){
                                                                      .text = func_start,
                                                                      .len = (p->cur->text - func_start),
                                                                      .kind = AST_FUNC_CALL,
                                                                      .data.AST_FUNC_CALL = (struct AST_FUNC_CALL){
                                                                          .name = func_name,
                                                                          .arguments = arguments,
                                                                          .arg_count = arg_count}});
                }
                else
                {
                    parser_token_error(p, p->cur, "expected function name, got `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
                }
            }
            return true;
        }
    }
    return false;
}
static bool parse_percent_directive(Parser_t *p, Ast_t *ast)
{
    if (p->cur->kind == TOKEN_PERCENT)
    {
        char *string = malloc(sizeof(char) * p->cur->text_len);
        strncpy(string, p->cur->text + 1, p->cur->text_len - 1);
        string[p->cur->text_len - 1] = '\0';
        AstNode_t *node =
            ast_new_node(ast, (struct AstNode_t){
                                  .text = p->cur->text,
                                  .len = p->cur->text_len,
                                  .kind = AST_PERCENT,
                                  .data.AST_PERCENT = (struct AST_PERCENT){
                                      .value = string}});

        parser_eat(p, TOKEN_PERCENT);
        return true;
    }
    return false;
}
static bool parse_var_decl(Parser_t *p, Ast_t *ast)
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
                    char *name = malloc(sizeof(char) * name_token->text_len + 1);
                    char *string = malloc(sizeof(char) * p->cur->text_len + 1);
                    strncpy(name, name_token->text, (int)name_token->text_len);
                    strncpy(string, p->cur->text, (int)p->cur->text_len);
                    name[name_token->text_len] = '\0';
                    string[p->cur->text_len] = '\0';
                    AstNode_t *string_node =
                        ast_new_node(ast, (struct AstNode_t){
                                              .text = p->cur->text,
                                              .len = p->cur->text_len,
                                              .kind = AST_STRING,
                                              .data.AST_STRING = (struct AST_STRING){
                                                  .value = string}});
                    AstNode_t *node =
                        ast_new_node(ast, (struct AstNode_t){
                                              .text = name_token->text,
                                              .len = p->cur->text - name_token->text + p->cur->text_len,
                                              .kind = AST_VAR_DECL,
                                              .data.AST_VAR_DECL = (struct AST_VAR_DECL){
                                                  .name = name,
                                                  .value = string_node}});

                    parser_eat(p, TOKEN_STRING);
                    return true;
                }
                else
                {
                    parser_token_error(p, p->cur, "can't parse declaration, expected string", NULL);
                }
            }
        }
        // else
        // {
        //     parser_token_error(p, p->cur, "can't parse", NULL);
        // }
    }
    return false;
}
static bool parse_var(Parser_t *p, Ast_t **ast)
{
    if (CUR->kind == TOKEN_SYMBOL)
    {

        return true;
    }
    return false;
}

static size_t peek_to(Parser_t *p, TokenKind kind, Token *t)
{
    size_t len = 0;
    while (can_parse(p, len + 1))
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