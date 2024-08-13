#ifndef _INTERPRET_H
#define _INTERPRET_H

#include "ast.h"
#include "common.h"

typedef struct
{

} Scope_t;
typedef struct
{
    Ast_t *ast;
    Ast_t **call_queue;
    size_t node_count;
    Ast_t **commands;
    size_t commands_len;
    Ast_t **variables;
    size_t variables_len;
    Ast_t **percent_dvs;
    size_t percent_dvs_len;
    Ast_t **strings;
    size_t strings_len;
    Ast_t **tasks;
    size_t tasks_len;
    Ast_t **functions;
    size_t functions_len;
    // AST_COMMAND,
    //     AST_VAR_DECL,
    //     AST_VAR,
    //     AST_PERCENT,
    //     AST_STRING,
    //     AST_EXPRESSION,
    //     AST_ASSIGN,
    //     AST_FUNC_CALL,
    //     AST_FUNC_DECL,
    //     AST_TASK,
    //     AST_CONDITION,
} Interpret_t;

Interpret_t *inter_new();
void inter_collect(Interpret_t *inter);
void free_interpreter(Interpret_t **inter);

#define free_(type, x) free(x), x = (type)NULL;

#endif // _INTERPRET_H