#ifndef _COMMON_H
#define _COMMON_H

#include <stdio.h>

#define UNIMPLEMENTED(...)                                                                                            \
    do                                                                                                                \
    {                                                                                                                 \
        fprintf(stderr, "%s:%d: " TERM_YELLOW "UNIMPLEMENTED:" TERM_RESET " %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                                                                      \
    } while (0)
#define UNREACHABLE(...)                                                                                          \
    do                                                                                                            \
    {                                                                                                             \
        fprintf(stderr, "%s:%d: " TERM_CYAN "UNREACHABLE:" TERM_RESET " %s \n", __FILE__, __LINE__, __VA_ARGS__); \
        exit(1);                                                                                                  \
    } while (0)

#define UNUSED(x) (void)(x)

#define TERM_BLACK "\033[30m"
#define TERM_RED "\033[31m"
#define TERM_GREEN "\033[32m"
#define TERM_YELLOW "\033[33m"
#define TERM_BLUE "\033[34m"
#define TERM_MAGENTA "\033[35m"
#define TERM_CYAN "\033[36m"
#define TERM_WHITE "\033[37m"

#define TERM_RESET "\033[0m"

#define TERM_BOLD "\033[1m"
#define TERM_BOLD_OFF "\033[22m"
#define TERM_BOLD_GREEN "\033[1;32m"
#define TERM_DIM "\033[2m"
#define TERM_ITALIC "\033[3m"
#define TERM_UNDERLINE "\033[4m"
#define TERM_BLINK_SLOW "\033[5m"
#define TERM_BLINK_RAPID "\033[6m"
#define TERM_COLOR_SWAP "\033[7m"
#define TERM_HIDE "\033[8m"
#define TERM_CROSSED "\033[9m"

#define error(s, ...)                                                                                                           \
    fprintf(stderr, "%s:%d:%s" TERM_RED TERM_BOLD " error: " TERM_RESET s "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
    exit(1);

#define warning(s, ...)                                                                                                               \
    fprintf(stderr, "%s:%d:%s" TERM_MAGENTA TERM_BOLD " warning: " TERM_RESET s "\n", __FILE__, __LINE__, __FUNCTION__, __VA_ARGS__); \
    exit(1);

#define is_space(x) (x == ' ') || (x == '\t')

#endif // _COMMON_H