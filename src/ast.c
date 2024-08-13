#include "ast.h"

Ast_t *ast_new(Ast_t ast)
{
    Ast_t *ptr = malloc(sizeof(Ast_t));
    if (ptr)
        *ptr = ast;
    return ptr;
}

const char *ast_kind_name(AstKind k)
{
    switch (k)
    {
    case AST_COMMAND:
        return "command";
    case AST_VAR:
        return "variable";
    case AST_VAR_DECL:
        return "var decl";
    case AST_PERCENT:
        return "percent directive";
    case AST_STRING:
        return "string";
    case AST_EXPRESSION:
        return "expression";
    case AST_ASSIGN:
        return "assign";
    case AST_FUNC_CALL:
        return "func call";
    case AST_FUNC_DECL:
        return "func decl";
    case AST_TASK:
        return "task";
    case AST_CONDITION:
        return "condition";
    }
}