#pragma once

#include "../42sh.h"

#include <sys/types.h>
#include <sys/wait.h>

/* Takes the condition 's' and returns its evaluation: 1 (true), 0 (false),
 * -1 (error, operation not recognized). */

int condition_is_true(char *s);

/* Executes the ast. */

int execute_ast(struct tree *ast, struct var *mesVars);

/* Executes a recipe. */

void execute_recipe(char *recipe);

/* Executes a recipe with redirection. */

void execute_with_redirections(char *recipe, int red);

char *take_recipe_redir(char *in, char *s);
char *take_out_file_redir(char *in, char *s);
void execute_condition(char *c1, char *c2, char *c3, char *c4);
void inverse_cond(struct tree *ast);
