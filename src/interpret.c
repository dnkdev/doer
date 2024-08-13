#include "interpret.h"

Interpret_t *inter_new()
{
    Interpret_t *inter = calloc(1, sizeof(Interpret_t));
    inter->ast = malloc(sizeof(Ast_t));
    inter->call_queue = malloc(sizeof(Ast_t *));
    inter->variables = malloc(sizeof(Ast_t *));
    inter->percent_dvs = malloc(sizeof(Ast_t *));
    return inter;
}

void inter_collect(Interpret_t *inter)
{
    Ast_t **temp = realloc(inter->call_queue, inter->node_count + 1 * sizeof(Ast_t *));
    inter->call_queue = temp;
    inter->call_queue[inter->node_count] = inter->ast;
    assert(inter->call_queue[inter->node_count] != NULL);
    inter->node_count += 1;
    if (inter->ast->kind == AST_VAR_DECL)
    {
        Ast_t **temp = realloc(inter->variables, inter->variables_len + 1 * sizeof(Ast_t *));
        inter->variables = temp;
        inter->variables[inter->variables_len] = inter->ast;
        inter->variables_len += 1;
    }
    else if (inter->ast->kind == AST_PERCENT)
    {
        Ast_t **temp = realloc(inter->percent_dvs, inter->percent_dvs_len + 1 * sizeof(Ast_t *));
        inter->percent_dvs = temp;
        inter->percent_dvs[inter->percent_dvs_len] = inter->ast;
        inter->percent_dvs_len += 1;
    }
    else
    {
        error("unhandled node %.*s", (int)inter->ast->len, inter->ast->text);
    }
}
