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

void free_interpreter(Interpret_t **inter)
{

    if ((*inter)->ast != NULL)
    {
        // free_(Ast_t *, (*inter)->ast);
        free((*inter)->ast);
        (*inter)->ast = (Ast_t *)NULL;
    }
    while ((*inter)->node_count >= 1)
    {
        // free_((*(*inter)->call_queue))
        (*inter)->node_count--;
        Ast_t *a;
        a = (*inter)->call_queue[(*inter)->node_count];
        assert(a != NULL);
        switch (a->kind)
        {
        case AST_COMMAND:
            // free_(a->data.AST_COMMAND.text);
            break;
        case AST_VAR_DECL:
            free_(char *, a->data.AST_VAR_DECL.name);
            free_(Ast_t *, a->data.AST_VAR_DECL.value);
            break;
        case AST_VAR:
            break;
        case AST_PERCENT:
            free_(char *, a->data.AST_PERCENT.value);
            break;
        case AST_STRING:
            free_(char *, a->data.AST_STRING.value);
            break;
        case AST_EXPRESSION:
            break;
        case AST_ASSIGN:
            break;
        case AST_FUNC_CALL:
            break;
        case AST_FUNC_DECL:
            break;
        case AST_TASK:
            break;
        case AST_CONDITION:
            break;
        }
    }
    free_(Ast_t **, (*inter)->call_queue);
    while ((*inter)->commands_len >= 1)
    {
        (*inter)->commands_len--;
        free_(Ast_t *, (*inter)->commands[(*inter)->commands_len]);
    }
    free_(Ast_t **, (*inter)->commands);
    while ((*inter)->variables_len >= 1)
    {
        (*inter)->variables_len--;
        free_(Ast_t *, (*inter)->variables[(*inter)->variables_len]);
    }
    free_(Ast_t **, (*inter)->variables);
    while ((*inter)->percent_dvs_len >= 1)
    {
        (*inter)->percent_dvs_len--;
        if ((*inter)->percent_dvs[(*inter)->percent_dvs_len] != NULL)
        {
            free((*inter)->percent_dvs[(*inter)->percent_dvs_len]);
            (*inter)->percent_dvs[(*inter)->percent_dvs_len] = NULL;
            // free_(Ast_t *, (*inter)->percent_dvs[(*inter)->percent_dvs_len]);
        }
    }
    free_(Ast_t **, (*inter)->percent_dvs);
    while ((*inter)->strings_len >= 1)
    {
        (*inter)->strings_len--;
        free_(Ast_t *, (*inter)->strings[(*inter)->strings_len]);
    }
    free_(Ast_t **, (*inter)->strings);
    while ((*inter)->tasks_len >= 1)
    {
        (*inter)->tasks_len--;
        free_(Ast_t *, (*inter)->tasks[(*inter)->tasks_len]);
    }
    free_(Ast_t **, (*inter)->tasks);
    while ((*inter)->functions_len >= 1)
    {
        (*inter)->functions_len--;
        free_(Ast_t *, (*inter)->functions[(*inter)->functions_len]);
    }
    free_(Ast_t **, (*inter)->functions);
    free((*inter));
    *inter = NULL;
    // (*inter)->ast = (Ast_t *)NULL;
    // Ast_t *ast;
    // Ast_t **call_queue;
    // size_t node_count;
    // Ast_t **commands;
    // size_t commands_len;
    // Ast_t **variables;
    // size_t variables_len;
    // Ast_t **percent_dvs;
    // size_t percent_dvs_len;
    // Ast_t **strings;
    // size_t strings_len;
    // Ast_t **tasks;
    // size_t tasks_len;
}