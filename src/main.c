#include <string.h>
#include <stdio.h>

#include "common.h"
#include "lexer.h"
#include "parser.h"

int main(int c, char *argv[])
{
    char *file_name = "Dofile";
    if (c > 1)
    {
        if (argv[1][0] == '$')
        {
            UNIMPLEMENTED("the 'repl' mode");
        }
        file_name = argv[1];
    }

    Token *tokens = malloc(sizeof(Token));
    size_t token_count;
    Lexer *lexer = lexer_collect_file(file_name, &tokens, &token_count);
    Parser_t p = parser_new(lexer, tokens, token_count);

    // size_t token_count = lexer_collect_file(file_name, &tokens);
    // Parser_t p = parser_new(tokens, token_count);
    size_t node_count = parser_parse(&p);
}