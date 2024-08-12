
#include <stdio.h>

#include "lexer.h"
#include "string.h"
#include "common.h"

#include <threads.h>

#define CUR l->content[l->cursor]

static char peek(Lexer *l);
static char look_back(Lexer *l);
static bool can_peek(Lexer *l);
static bool can_look_back(Lexer *l);

static bool collect_newline(Lexer *l, Token *token);
static bool collect_task_spaces(Lexer *l, Token *token);
static bool collect_string(Lexer *l, Token *token);
static bool collect_symbol(Lexer *l, Token *token);
static bool collect_number(Lexer *l, Token *token);
static bool collect_literals(Lexer *l, Token *token);
static bool collect_comment(Lexer *l, Token *token);

static const char *token_kind_name(TokenKind name);
static Lexer *lexer_new(char *content, size_t content_len);
static Token lexer_next(Lexer *l);

typedef struct
{
    TokenKind kind;
    const char *text;
} Literal_Token;

Literal_Token literal_tokens[] = {
    {.text = "$", .kind = TOKEN_DOLLAR},
    {.text = "(", .kind = TOKEN_OPAREN},
    {.text = ")", .kind = TOKEN_CPAREN},
    {.text = "*", .kind = TOKEN_ASTERISK},
    {.text = ".", .kind = TOKEN_DOT},
    {.text = "{", .kind = TOKEN_OCURLY},
    {.text = "}", .kind = TOKEN_CCURLY},
    {.text = ";", .kind = TOKEN_SEMICOLON},
    {.text = ":=", .kind = TOKEN_DECL_SIGN},
    {.text = ":", .kind = TOKEN_COLON},
    {.text = "/", .kind = TOKEN_SLASH},
    {.text = "+", .kind = TOKEN_PLUS},
    {.text = "-", .kind = TOKEN_MINUS},
    {.text = "!=", .kind = TOKEN_NOTEQ},
    {.text = "==", .kind = TOKEN_EQEQ},
    {.text = "=", .kind = TOKEN_EQ},
    {.text = "@", .kind = TOKEN_AT},
    {.text = "!", .kind = TOKEN_EXCL},
    {.text = "<=", .kind = TOKEN_LESS_EQ},
    {.text = "<", .kind = TOKEN_LESS},
    {.text = ">=", .kind = TOKEN_GREATER_EQ},
    {.text = ">", .kind = TOKEN_GREATER},
    {.text = "->", .kind = TOKEN_ARROW_RIGHT}};
#define literal_tokens_count (sizeof(literal_tokens) / sizeof(literal_tokens[0]))

static const char *token_kind_name(TokenKind kind)
{
    switch (kind)
    {
    case TOKEN_END:
        return "end token";
    case TOKEN_INVALID:
        return "invalid token";
    case TOKEN_DOLLAR:
        return "dollar token";
    case TOKEN_PREPROC:
        return "preprocessor token";
    case TOKEN_SYMBOL:
        return "symbol token";
    case TOKEN_NUMBER:
        return "number token";
    case TOKEN_ASTERISK:
        return "asterisk token";
    case TOKEN_DOT:
        return "dot token";
    case TOKEN_OPAREN:
        return "open paren token";
    case TOKEN_CPAREN:
        return "close paren token";
    case TOKEN_OCURLY:
        return "open curly token";
    case TOKEN_CCURLY:
        return "close curly token";
    case TOKEN_SEMICOLON:
        return "semicolon token";
    case TOKEN_COLON:
        return "colon token";
    case TOKEN_DECL_SIGN:
        return "declaration sign token";
    case TOKEN_COMMENT:
        return "comment token";
    case TOKEN_STRING:
        return "string token";
    case TOKEN_ARROW_RIGHT:
        return "arrow-right token";
    case TOKEN_TASK_SPACE:
        return "tasl blank space token";
    case TOKEN_TASK_TABSPACE:
        return "task tab space token";
    case TOKEN_SPACE:
        return "blank space token";
    case TOKEN_TABSPACE:
        return "tab space token";
    case TOKEN_NEWLINE:
        return "new-line token";
    case TOKEN_SLASH:
        return "slash token";
    case TOKEN_PLUS:
        return "plus token";
    case TOKEN_MINUS:
        return "minus token";
    case TOKEN_NOTEQ:
        return "not-eq token";
    case TOKEN_EQ:
        return "eq token";
    case TOKEN_EQEQ:
        return "eqeq token";
    case TOKEN_AT:
        return "at token";
    case TOKEN_LESS:
        return "less token";
    case TOKEN_GREATER:
        return "greater token";
    case TOKEN_LESS_EQ:
        return "less eq token";
    case TOKEN_GREATER_EQ:
        return "greater eq token";
    default:
        return "unnamed";
    }
    return NULL;
}

static bool can_peek(Lexer *l)
{
    if (l->content_len > l->cursor + 1)
    {
        return true;
    }
    return false;
}
static char peek(Lexer *l)
{
    return l->content[l->cursor + 1];
}
static bool can_look_back(Lexer *l)
{
    if (l->cursor - 1 >= 0 && l->content_len > l->cursor - 1)
    {
        return true;
    }
    return false;
}
static char look_back(Lexer *l)
{
    return l->content[l->cursor - 1];
}
static Lexer *lexer_new(char *content, size_t content_len)
{
    Lexer *l = calloc(1, sizeof(Lexer));
    l->content = content;
    l->content_len = content_len;
    l->line = 1;
    return l;
}

static bool lexer_starts_with(Lexer *l, const char *prefix)
{
    size_t prefix_len = strlen(prefix);
    if (prefix_len == 0)
    {
        return true;
    }
    if (l->cursor + prefix_len - 1 >= l->content_len)
    {
        return false;
    }
    for (size_t i = 0; i < prefix_len; ++i)
    {
        if (prefix[i] != l->content[l->cursor + i])
        {
            return false;
        }
    }
    return true;
}

static char lexer_eat(Lexer *l, size_t len)
{
    assert(l->cursor < l->content_len);
    char x = l->content[l->cursor];
    l->cursor += 1;
    if (x == '\n')
    {
        l->line += 1;
        l->bol = l->cursor;
    }
    if (len > 1)
        return lexer_eat(l, --len);
    return x;
}

static bool is_symbol_start(char x)
{
    return isalpha(x) || x == '_';
}
static bool is_symbol(char x)
{
    return isalnum(x) || x == '_';
}

size_t get_file_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    size_t file_size = ftell(file);
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

    size_t file_size = get_file_size(file);

    *dest = (char *)malloc(file_size * sizeof(char) + 1);
    fread(*dest, file_size, 1, file);
    (*dest)[file_size] = '\0';
    fclose(file);
    return file_size;
}

Lexer *lexer_collect_file(char *file_path, Token **tokens, size_t *token_count)
{
    char *content;
    size_t count = 0;
    size_t content_len = read_file(file_path, &content);
    Lexer *l = lexer_new(content, content_len);
    Token t = lexer_next(l);
    while (t.kind != TOKEN_END)
    {
        // fprintf(stdout, "'%.*s' %zu (%s) %zu %zu\n", (int)t.text_len, t.text, t.text_len, token_kind_name(t.kind), t.pos.row, t.pos.col);
        t = lexer_next(l);
        t.pos.file_path = file_path;
        count += 1;
        if (count > 1)
        {
            Token *temp = realloc(*tokens, count * sizeof(Token));
            if (temp == NULL)
            {
                error("Memory reallocation failed!", NULL);
                return NULL;
            }
            *tokens = temp;
            (*tokens)[count - 1] = t;
        }
    }
    printf("Token count: %zu\n", count);
    return l;
}
static Token lexer_next(Lexer *l)
{
    Token token = {.text = &CUR};
    if (l->cursor >= l->content_len)
        return token;

    if (
        collect_newline(l, &token) ||
        collect_task_spaces(l, &token) ||
        collect_symbol(l, &token) ||
        collect_string(l, &token) ||
        collect_number(l, &token) ||
        collect_literals(l, &token) ||
        collect_comment(l, &token))
    {
        return token;
    }
    token.kind = TOKEN_INVALID;
    token.text_len = 1;
    token.pos.row = l->line;
    token.pos.col = l->cursor - l->bol;
    lexer_eat(l, 1);

    return token;
}

static bool collect_task_spaces(Lexer *l, Token *token)
{
    if (can_look_back(l) && look_back(l) == '\n')
    {
        if (is_space(CUR))
        {
            if (CUR == '\t')
                token->kind = TOKEN_TASK_TABSPACE;
            else
            {
                token->kind = TOKEN_TASK_SPACE;
            }
            token->pos.row = l->line;
            token->pos.col = l->cursor - l->bol;
            while (l->cursor < l->content_len && is_space(CUR))
            {
                token->text_len += 1;
                lexer_eat(l, 1);
            }
            return true;
        }
    }
    else
    {
        if (is_space(CUR))
        {
            if (CUR == '\t')
                token->kind = TOKEN_TABSPACE;
            else
            {
                token->kind = TOKEN_SPACE;
            }
            token->pos.row = l->line;
            token->pos.col = l->cursor - l->bol;
            while (l->cursor < l->content_len && is_space(CUR))
            {
                token->text_len += 1;
                lexer_eat(l, 1);
            }
            return true;
        }
    }
    return false;
}
static bool collect_newline(Lexer *l, Token *token)
{
    if (CUR == '\n' || CUR == '\r')
    {
        token->kind = TOKEN_NEWLINE;
        token->text_len = 1;
        token->pos.row = l->line;
        token->pos.col = l->cursor - l->bol;
        lexer_eat(l, 1);
        return true;
    }
    return false;
}

static bool collect_symbol(Lexer *l, Token *token)
{
    if (is_symbol_start(CUR))
    {
        token->kind = TOKEN_SYMBOL;
        token->pos.row = l->line;
        token->pos.col = l->cursor - l->bol;
        while (l->cursor < l->content_len && is_symbol(CUR))
        {
            token->text_len += 1;
            lexer_eat(l, 1);
        }
        return true;
    }
    return false;
}
static bool collect_number(Lexer *l, Token *token)
{
    if (isdigit(CUR))
    {
        token->kind = TOKEN_NUMBER;
        token->pos.row = l->line;
        token->pos.col = l->cursor - l->bol;
        while (l->cursor < l->content_len && isdigit(CUR))
        {
            token->text_len += 1;
            lexer_eat(l, 1);
        }
        if (CUR == '.')
        {
            token->text_len += 1;
            lexer_eat(l, 1);
            while (l->cursor < l->content_len && isdigit(CUR))
            {
                token->text_len += 1;
                lexer_eat(l, 1);
            }
        }
        return true;
    }
    return false;
}

static bool collect_string(Lexer *l, Token *token)
{
    if (CUR == '"')
    {
        // TODO: TOKEN_STRING should also handle escape sequences
        token->kind = TOKEN_STRING;
        token->pos.row = l->line;
        token->pos.col = l->cursor - l->bol;
        lexer_eat(l, 1);
        while (l->cursor < l->content_len && CUR != '"' && CUR != '\n')
        {
            lexer_eat(l, 1);
        }
        if (l->cursor < l->content_len)
        {
            lexer_eat(l, 1);
        }
        token->text_len = &CUR - token->text;

        return true;
    }
    return false;
}
static bool collect_literals(Lexer *l, Token *token)
{
    for (size_t i = 0; i < literal_tokens_count; ++i)
    {
        if (lexer_starts_with(l, literal_tokens[i].text))
        {
            // NOTE: this code assumes that there is no newlines in literal_tokens[i].text
            size_t text_len = strlen(literal_tokens[i].text);
            token->kind = literal_tokens[i].kind;
            token->text_len = text_len;
            token->pos.row = l->line;
            token->pos.col = l->cursor - l->bol;
            lexer_eat(l, text_len);
            return true;
        }
    }
    return false;
}
static bool collect_comment(Lexer *l, Token *token)
{
    if (CUR == '#')
    {
        token->kind = TOKEN_COMMENT;
        token->pos.row = l->line;
        token->pos.col = l->cursor - l->bol;
        while (l->cursor < l->content_len && CUR != '\n')
        {
            lexer_eat(l, 1);
            token->text_len += 1;
        }

        return true;
    }
    return false;
}
