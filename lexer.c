
#include <stdio.h>

#include "lexer.h"
#include "common.h"
#include "string.h"

static const char *token_kind_name(TokenKind name);
static Lexer lexer_new(const char *content, size_t content_len);
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
    {.text = "{", .kind = TOKEN_OCURLY},
    {.text = "}", .kind = TOKEN_CCURLY},
    {.text = ";", .kind = TOKEN_SEMICOLON},
    {.text = ":", .kind = TOKEN_COLON},
};
#define literal_tokens_count (sizeof(literal_tokens) / sizeof(literal_tokens[0]))

#define CUR l->content[l->cursor]

static const char *token_kind_name(TokenKind kind)
{
    switch (kind)
    {
    case TOKEN_END:
        return "end token";
    case TOKEN_INVALID:
        return "invalid token";
    case TOKEN_PREPROC:
        return "preprocessor token";
    case TOKEN_SYMBOL:
        return "symbol token";
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
    case TOKEN_COMMENT:
        return "comment token";
    case TOKEN_STRING:
        return "string token";
    default:
        return "unnamed";
    }
    return NULL;
}

static Lexer lexer_new(const char *content, size_t content_len)
{
    Lexer l = {0};
    l.content = content;
    l.content_len = content_len;
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
static int lexer_trim_left(Lexer *l)
{
    int count = 0;
    while (l->cursor < l->content_len && isspace(l->content[l->cursor]))
    {
        count++;
        lexer_eat(l, 1);
    }
    return count;
}

static bool is_symbol_start(char x)
{
    return isalpha(x) || x == '_';
}
static bool is_symbol(char x)
{
    return isalnum(x) || x == '_';
}
static Token lexer_next(Lexer *l)
{
    lexer_trim_left(l);

    Token token = {
        .text = &l->content[l->cursor]};

    if (l->cursor >= l->content_len)
        return token;

    if (CUR == '#')
    {
        token.kind = TOKEN_PREPROC;
        bool new_line = false;
        do
        {
            if (CUR == '\n')
            {
                lexer_eat(l, 1);
                token.text_len++;
            }

            while (l->cursor < l->content_len && CUR != '\n')
            {
                if (CUR == '\\')
                {
                    new_line = true;
                }
                else if (!isspace(CUR) && new_line == true)
                {
                    new_line = false;
                }
                lexer_eat(l, 1);
                token.text_len += 1;
            }
        } while (new_line == true);

        return token;
    }

    if (is_symbol_start(CUR))
    {
        token.kind = TOKEN_SYMBOL;
        while (l->cursor < l->content_len && is_symbol(CUR))
        {
            l->cursor += 1;
            token.text_len += 1;
        }
        return token;
    }

    for (size_t i = 0; i < literal_tokens_count; ++i)
    {
        if (lexer_starts_with(l, literal_tokens[i].text))
        {
            // NOTE: this code assumes that there is no newlines in literal_tokens[i].text
            size_t text_len = strlen(literal_tokens[i].text);
            token.kind = literal_tokens[i].kind;
            token.text_len = text_len;
            lexer_eat(l, text_len);
            return token;
        }
    }

    token.kind = TOKEN_INVALID;
    token.text_len = 1;

    l->cursor += 1;
    return token;
}

size_t lexer_collect_from(char *content, size_t content_len, Token *tokens)
{
    Lexer l = lexer_new(content, content_len);
    Token t;
    t = lexer_next(&l);
    while (t.kind != TOKEN_END)
    {
        fprintf(stdout, "'%.*s' (%s)\n", (int)t.text_len, t.text, token_kind_name(t.kind));
        // fprintf(stdout, "token info: %zu %.*s\n", t.text_len, (int)t.text_len, t.text);
        t = lexer_next(&l);
    }
    return 0;
}