#ifndef _AST_H
#define _AST_H

#include <stdlib.h>
#include "lexer.h"
#include "common.h"

typedef enum
{
    OP_AND, // Logical AND: &&
    OP_OR,  // Logical OR: ||
    OP_NOT, // Logical NOT: !
    OP_EQ,  // Equality: ==
    OP_NEQ, // Not equal: !=
    OP_LT,  // Less than: <
    OP_LE,  // Less than or equal: <=
    OP_GT,  // Greater than: >
    OP_GE   // Greater than or equal: >=
} LogicalOperator;

typedef enum
{
    AST_COMMAND = 0,
    AST_VAR_DECL,
    AST_VAR,
    AST_PERCENT,
    AST_STRING,
    AST_EXPRESSION,
    AST_ASSIGN,
    AST_FUNC_CALL,
    AST_FUNC_DECL,
    AST_TASK,
    AST_CONDITION,
} AstKind;

typedef union
{
    struct AST_COMMAND
    {
        char *text;
        size_t text_len;
    } AST_COMMAND;
    struct AST_VAR_DECL
    {
        char *name;
        struct AstNode_t *value;
    } AST_VAR_DECL;
    struct AST_VAR
    {
        char *name;
    } AST_VAR;
    struct AST_PERCENT
    {
        char *value;
    } AST_PERCENT;
    struct AST_STRING
    {
        char *value;
    } AST_STRING;
    struct AST_EXPRESSION
    {
        struct AstNode_t *left;
        struct AstNode_t *right;
    } AST_EXPRESSION;
    struct AST_ASSIGN
    {
        char *name;
        char *value;
    } AST_ASSIGN;
    struct AST_FUNC_CALL
    {
        const char *name;
        struct AstNode_t **arguments;
        size_t arg_count;
    } AST_FUNC_CALL;
    struct AST_FUNC_DECL
    {
        char *name;
        struct AstNode_t **arguments;
        size_t arg_count;
        struct AstNode_t **nodes;

    } AST_FUNC_DECL;
    struct AST_TASK
    {
        char *name;
        struct AstNode_t **dependents;
        size_t dependents_len;
        struct AstNode_t **nodes;
    } AST_TASK;
    struct AST_CONDITION
    {
        struct AstNode_t *left;
        struct AstNode_t *right;
        LogicalOperator op;
    } AST_CONDITION;
} AstKind_u;

typedef struct AstNode_t
{
    AstKind kind;
    AstKind_u data;
    char *text;
    size_t len;
} AstNode_t;

typedef struct Ast_t
{
    AstNode_t *nodes;
    size_t nodes_count;

    AstNode_t **commands;
    size_t commands_len;
    AstNode_t **variables;
    size_t variables_len;
    AstNode_t **percent_dvs;
    size_t percent_dvs_len;
    AstNode_t **strings;
    size_t strings_len;
    AstNode_t **tasks;
    size_t tasks_len;
    AstNode_t **functions;
    size_t functions_len;
} Ast_t;

Ast_t *ast_new();
AstNode_t *ast_new_node(Ast_t *ast, AstNode_t node);
#define AST_NEW_NODE(ast, t, ...) ast_new_node(ast, (struct AstNode_t){.kind = t, .data.t = (struct t){__VA_ARGS__}})

const char *ast_kind_name(AstKind k);

#define MEM_REALLOC_CHECK(x)                        \
    if (x == NULL)                                  \
    {                                               \
        error("Memory reallocation failed!", NULL); \
        return NULL;                                \
    }

#endif //_AST_H