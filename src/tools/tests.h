#pragma once

#include "../42sh.h"

#include <string.h>

/* test_end_of_word:
 * return true si le caractere "c" se situe juste apres la fin d'un mot. */
int test_end_of_word(char c);

/* est_une_condition:
 * return true si "input" fait parti de la grammaire de la condition. */
int est_une_condition(char *input);

int est_une_ponct(char c);
int est_une_instr(char *s);
int est_une_boucle(char *c);
int est_une_redirection(char *s);
