#include "parser.h"
#include "ast.h"
#include <stdio.h>
#include <string.h>

#define CUR p->cur
#define can_parse(p, i) p->token_count > p->cursor + i

Parser_t *parser_new(Lexer_t *l, Token *tokens, size_t token_count)
{
    Parser_t *p = malloc(sizeof(Parser_t));
    p->tokens = tokens,
    p->token_count = token_count,
    p->cursor = 0,
    p->content = l->content,
    p->content_len = l->content_len,
    p->prev = NULL;
    p->cur = &p->tokens[p->cursor];
    if (can_parse(p, 1))
    {
        p->next = &p->tokens[p->cursor + 1];
    }

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
    if (is_parser_parsed(p, ast))
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
                error("parsed bad\n", NULL);
            }
            break;
        }
    }
    return 0;
}

// commands only in tasks
static AstNode_t *parse_command(Parser_t *p, Ast_t *ast)
{
    char *start = p->cur->text;
    while (p->cur->kind != TOKEN_NEWLINE)
    {
        parser_eat(p, p->cur->kind);
    }
    size_t len = p->cur->text - start;
    char *string = malloc(sizeof(char) * len + 1);
    strncpy(string, start, len);
    AstNode_t *node =
        ast_new_node(ast, (struct AstNode_t){
                              .text = start,
                              .len = len,
                              .kind = AST_COMMAND,
                              .data.AST_COMMAND = (struct AST_COMMAND){
                                  .text = string}});
    printf("NOWW: %s\n", token_kind_name(p->cur->kind));
    if (node != NULL)
    {
        return node;
    }
    return NULL;
}
static AstNode_t *parse_task_decl(Parser_t *p, Ast_t *ast)
{
    if (p->cur->kind == TOKEN_SYMBOL)
    {
        bool prev;
        if (p->prev != NULL)
        {
            if (p->prev->kind == TOKEN_NEWLINE)
            {
                prev = true;
            }
        }
        else
        {
            prev = true;
        }
        if (!prev || p->next->kind != TOKEN_COLON)
        {
            return NULL;
        }
        char *task_start = p->cur->text;
        char *task_name = malloc(p->cur->text_len * sizeof(char) + 1);
        strncpy(task_name, p->cur->text, p->cur->text_len);
        task_name[p->cur->text_len] = '\0';
        printf("%s | %s | %s\n", token_kind_name(p->prev->kind), token_kind_name(p->cur->kind), token_kind_name(p->next->kind));
        parser_eat(p, TOKEN_SYMBOL);
        parser_eat(p, TOKEN_COLON);
        size_t space_count = 0;
        Token next;
        size_t arg_count = 0;
        // FIX: MAX TASK DEPENDENTS 10 TODO:
        char **temp_dependent_names = malloc(sizeof(char *) * 1);
        size_t temp_dependent_names_len = 0;
        AstNode_t **temp_orders = malloc(sizeof(AstNode_t *) * 1);
        size_t temp_orders_len = 0;
        while (can_parse(p, 1) && p->cur->kind != TOKEN_NEWLINE)
        {
            if ((space_count = peek_through_blankspaces(p, &next)) != -1)
            {
                parser_eat_spaces(p, space_count);
            }
            if (p->cur->kind == TOKEN_SPACE)
            {
                parser_eat(p, TOKEN_SPACE);
            }
            // parse dependents
            char *start = p->cur->text;
            while (!is_space_token(p->cur->kind) && p->cur->kind != TOKEN_NEWLINE)
            {
                if (p->cur->kind != TOKEN_SYMBOL && p->cur->kind != TOKEN_DOT)
                {
                    parser_token_error(p, p->cur, "Unexpected token `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
                }
                parser_eat(p, p->cur->kind);
            }
            size_t len = p->cur->text - start;
            printf("%.*s\n", (int)len, start);
            char *name = malloc(sizeof(char) * len) + 1;
            strncpy(name, start, len);
            name[len] = '\0';
            temp_dependent_names = realloc(temp_dependent_names, sizeof(char *) * (temp_dependent_names_len + 1));
            temp_dependent_names[temp_dependent_names_len] = name;
            temp_dependent_names_len += 1;
            arg_count += 1;
        }
        printf("Arg count: %zu\n", arg_count);
        parser_eat(p, TOKEN_NEWLINE);
        // parse task body
        if (p->cur->kind != TOKEN_TASK_SPACE && p->cur->kind != TOKEN_TASK_TABSPACE)
        {
            parser_token_error(p, p->cur, "missing task body", NULL);
        }
        TokenKind body_space_kind = p->cur->kind;
        size_t body_space_kind_len = p->cur->text_len;
        size_t order_count = 0;
        while (p->cur->kind == body_space_kind)
        {
            parser_eat(p, body_space_kind);
            printf("start %s\n", token_kind_name(p->cur->kind));
            AstNode_t *parsed_node = parse_var_decl(p, ast);
            if (parsed_node == NULL)
                parsed_node = parse_percent_directive(p, ast);
            if (parsed_node == NULL)
                parsed_node = parse_var_decl(p, ast);
            if (parsed_node == NULL)
                parsed_node = parse_func_call(p, ast);
            if (parsed_node == NULL)
                parsed_node = parse_command(p, ast);
            if (parsed_node == NULL)
            {
                parser_token_error(p, p->cur, "Failed to parse token `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
            }
            // while (is_parser_parsed(p, ast))
            // {
            //     printf("ok %s\n", token_kind_name(p->cur->kind));
            // }
            temp_orders = realloc(temp_orders, sizeof(AstNode_t) * (temp_orders_len + 1));
            temp_orders[temp_orders_len] = parsed_node;
            temp_orders_len += 1;
            printf("end %s\n", token_kind_name(p->cur->kind));
            parser_eat(p, TOKEN_NEWLINE);
            order_count += 1;
        }
        size_t len = p->prev->text - task_start;
        AstNode_t *task_node =
            ast_new_node(ast, (struct AstNode_t){
                                  .text = task_start,
                                  .len = len,
                                  .kind = AST_TASK_DECL,
                                  .data.AST_TASK_DECL = (struct AST_TASK_DECL){
                                      .name = task_name,
                                      .dependents = temp_dependent_names,
                                      .dependents_len = temp_dependent_names_len,
                                      .order_count = order_count,
                                      .orders = temp_orders}});

        return task_node;
    }
    return NULL;
}
static AstNode_t *parse_func_call(Parser_t *p, Ast_t *ast)
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
                    // FIX: TEMPORARY 10 ARGUMENTS MAX TODO:
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

                    return func_call_node;
                }
                else
                {
                    parser_token_error(p, p->cur, "expected function name, got `%.*s` (%s)", (int)p->cur->text_len, p->cur->text, token_kind_name(p->cur->kind));
                }
            }
        }
    }
    return NULL;
}
static AstNode_t *parse_percent_directive(Parser_t *p, Ast_t *ast)
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
        return node;
    }
    return NULL;
}
static AstNode_t *parse_var_decl(Parser_t *p, Ast_t *ast)
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
                    return node;
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
    return NULL;
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
    // for (int i = 1; i < 100; ++i)
    // {
    //     if (can_parse(p, i))
    //     {
    //     }
    // }
    // while (can_parse(p, spaces_count + 1))
    // {
    //     printf("can parse, next is: %s\n", token_kind_name(p->tokens[p->cursor + spaces_count + 1].kind));
    //     if (p->tokens[p->cursor + spaces_count + 1].kind == TOKEN_SPACE)
    //     {
    //         spaces_count++;
    //         printf("- found space on %zu ", p->cursor + spaces_count);
    //         continue;
    //     }
    //     else if (p->tokens[p->cursor + spaces_count + 1].kind == TOKEN_END)
    //     {
    //         return -1;
    //     }
    //     else /// if (can_parse(p, spaces_count))
    //     {
    //         *t = p->tokens[p->cursor + spaces_count + 1];
    //         printf("peeked to `%s` spaces: %zu\n", token_kind_name(t->kind), spaces_count);
    //         return spaces_count;
    //     }
    // }
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