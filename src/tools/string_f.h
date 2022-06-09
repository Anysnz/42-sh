#pragma once
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"

int extract_var_name(char *input, char *output, int *compute);
void split_cond(char *s, char *c1, char *c2, char *c3, char *c4);
char *my_itoa(int value, char *s);
void compute_var(char *input);
int append_to_space_or_tab(char *input, char *output);
int check_if_countain_only(char *input, char c, char c2);
