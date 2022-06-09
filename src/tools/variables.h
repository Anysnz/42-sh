#pragma once
#include <time.h>
struct var
{
    struct var *next;
    char *nom;
    char *valeur;
};

char *get_var_value(struct var *mesVars, char *nom);
struct var *init_var();
struct var *modifie_or_add_var(struct var *mesVars, char *nom, char *valeur);
void dellete_all_vars(struct var *mesVars);
void extract_name_value_and_add(char *input, struct var *mesVars);
int is_variable_declaration(char *input);
void replace_string(char *input, char *output, struct var *mesVars);
void add_start_variable(struct var *mesVars, int argc, char **argv);
