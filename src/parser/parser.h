#pragma once

#include "grammaire.h"

int arret_if(struct list *tokens);
int arret_boucle(struct list *tokens);
struct tree *else_clause_parseur(struct list *token, struct tree *tree,
        struct tree *saveIf);
char *concatenate(struct list *tokens);
struct tree *parser(struct list *token);
char *getCond(char *cond, struct list *list);
struct list *getInstruct(char *instruct, struct list *list, int *type);
struct tree *rule_if_parseur(struct list *token, struct tree *tree);
void replace(struct tree *root, struct tree *replaceMe, struct tree *replace);
struct tree *rule_while_parseur(struct list *token, struct tree *new);
struct tree *rule_until_parseur(struct list *token, struct tree *new);
