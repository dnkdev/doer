#include "ast.h"
#include <stdio.h>

Ast_t *ast_new()
{
    Ast_t *ptr = calloc(1, sizeof(Ast_t));
    ptr->nodes = malloc(sizeof(AstNode_t));
    ptr->commands = malloc(sizeof(AstNode_t *));
    ptr->variables = malloc(sizeof(AstNode_t *));
    ptr->percent_dvs = malloc(sizeof(AstNode_t *));
    ptr->strings = malloc(sizeof(AstNode_t *));
    ptr->tasks = malloc(sizeof(AstNode_t *));
    ptr->functions = malloc(sizeof(AstNode_t *));
    return ptr;
}

AstNode_t *ast_new_node(Ast_t *ast, AstNode_t node)
{
    ast->nodes_count++;
    if (ast->nodes_count > 1)
    {
        AstNode_t *temp = realloc(ast->nodes, ast->nodes_count * sizeof(AstNode_t));
        MEM_REALLOC_CHECK(temp);
        ast->nodes = temp;
    }
    ast->nodes[ast->nodes_count - 1] = node;

    printf("creating %s: %.*s\n", ast_kind_name(node.kind), (int)node.len, node.text);
    switch (node.kind)
    {
    case AST_TASK_DECL:
    {
        AstNode_t **temp = realloc(ast->tasks, (ast->tasks_len + 1) * sizeof(AstNode_t *));
        MEM_REALLOC_CHECK(temp);
        ast->tasks = temp;
        ast->tasks[ast->tasks_len] = &ast->nodes[ast->nodes_count - 1];
        ast->tasks_len += 1;
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    case AST_COMMAND:
    {
        AstNode_t **temp = realloc(ast->commands, (ast->commands_len + 1) * sizeof(AstNode_t *));
        MEM_REALLOC_CHECK(temp);
        ast->commands = temp;
        ast->commands[ast->commands_len] = &ast->nodes[ast->nodes_count - 1];
        ast->commands_len += 1;
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    case AST_FUNC_CALL:
    {
        // AstNode_t **temp = realloc(ast->percent_dvs, (ast->percent_dvs_len + 1) * sizeof(AstNode_t *));
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    case AST_PERCENT:
    {
        AstNode_t **temp = realloc(ast->percent_dvs, (ast->percent_dvs_len + 1) * sizeof(AstNode_t *));
        MEM_REALLOC_CHECK(temp);
        ast->percent_dvs = temp;
        ast->percent_dvs[ast->percent_dvs_len] = &ast->nodes[ast->nodes_count - 1];
        ast->percent_dvs_len += 1;
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    case AST_VAR_DECL:
    {
        AstNode_t **temp = realloc(ast->variables, (ast->variables_len + 1) * sizeof(AstNode_t *));
        MEM_REALLOC_CHECK(temp);
        ast->variables = temp;
        ast->variables[ast->variables_len] = &ast->nodes[ast->nodes_count - 1];
        ast->variables_len += 1;
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    case AST_STRING:
    {
        AstNode_t **temp = realloc(ast->strings, (ast->strings_len + 1) * sizeof(AstNode_t *));
        MEM_REALLOC_CHECK(temp);
        ast->strings = temp;
        ast->strings[ast->strings_len] = &ast->nodes[ast->nodes_count - 1];
        ast->strings_len += 1;
        return &ast->nodes[ast->nodes_count - 1];
    }
    break;
    default:
        error("unimplemented other ast kinds: (%s)", ast_kind_name(node.kind));
        break;
    }
    return NULL;
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
    case AST_TASK_CALL:
        return "task call";
    case AST_TASK_DECL:
        return "task decl";
    case AST_CONDITION:
        return "condition";
    }
}