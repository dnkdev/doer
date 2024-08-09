#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "common.h"

long get_file_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);
    return file_size;
}

size_t read_file(const char *file_path, char **dest)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
    {
        error("Can't open file `%s`", file_path);
    }

    long file_size = get_file_size(file);

    *dest = (char *)malloc(file_size * sizeof(char) + 1);
    fread(*dest, file_size, 1, file);
    (*dest)[file_size] = '\0';
    fclose(file);
    return file_size;
}

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
    char *content;
    size_t content_len = read_file(file_name, &content);
    size_t token_count = lexer_collect_from(content, content_len, &tokens);
}