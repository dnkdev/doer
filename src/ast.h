#ifndef _AST_H
#define _AST_H

#include <stdlib.h>
#include "lexer.h"

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
    AST_COMMAND,
    AST_VAR_DECL,
    AST_VAR,
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
        struct Ast_t *value;
    } AST_VAR_DECL;
    struct AST_VAR
    {
        char *name;
    } AST_VAR;
    struct AST_STRING
    {
        char *value;
    } AST_STRING;
    struct AST_EXPRESSION
    {
        struct Ast_t *left;
        struct Ast_t *right;
    } AST_EXPRESSION;
    struct AST_ASSIGN
    {
        char *name;
        char *value;
    } AST_ASSIGN;
    struct AST_FUNC_CALL
    {
        const char *name;
        struct Ast_t **arguments;
        size_t arg_count;
    } AST_FUNC_CALL;
    struct AST_FUNC_DECL
    {
        char *name;
        struct Ast_t **arguments;
        size_t arg_count;
        struct Ast_t **nodes;

    } AST_FUNC_DECL;
    struct AST_TASK
    {
        char *name;
        struct Ast_t **dependents;
        size_t dependents_len;
        struct Ast_t **nodes;
    } AST_TASK;
    struct AST_CONDITION
    {
        struct Ast_t *left;
        struct Ast_t *right;
        LogicalOperator op;
    } AST_CONDITION;
} AstKind_u;

typedef struct Ast_t
{
    AstKind kind;
    AstKind_u data;
    char *text;
    size_t len;
} Ast_t;

Ast_t *ast_new(Ast_t ast);
#define AST_NEW(t, ...) ast_new((struct Ast_t){.kind = t, .data.t = (struct t){__VA_ARGS__}})

const char *ast_kind_name(AstKind k);

#endif //_AST_H