#pragma once

#include "tools/variables.h"
#include "tools/tests.h"
#include "lexer/lexer.h"
#include "tools/ast.h"
#include "builtins/echo.h"
#include "parser/parser.h"
#include "parser/grammaire.h"
#include "tools/ast_print.h"
#include "execution/execute.h"
#include "tools/string_f.h"
#include <stdio.h>
#include <err.h>
#include <string.h>
#include <unistd.h>

struct global // this struct allows us to have many global variables in one.
{
    // Lexer variables:

    int space; // linked to the field of the same name in struct list.

    int special; // if a token should be lexered as a text or with its
                 // special meaning. Can be 0 (only text), 1 (example: with "
                 // or echo) or 2 (example: with ').

    int after_anti_slash; // special == 2 but for \.

    int beg_command; // equal 1 if we are at the beginning of a command.

    // Parser variables:

    int quote_open; // useful in parser.c/getInstr function. Assert that quotes
                    // are " open (1), ' open (2) or closed (0).

    int lwasb; // some location. Assert that last list was backslashed.

    // Global variables:

    int error; // exit value of 42sh.

    struct list *tail; // last token. To perform faster.
};

void sig_handler(int signo);

void call_lexer_pipe(int argc, char **argv);
void call_lexer_stdin(void);
void call_lexer_for_file(char *path, int argc, char **argv);
void call_lexer_for_c(char **s, int argc);
