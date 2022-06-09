#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

enum typ
{
    root,
    instruct,
    cond,
    nextIsCond,
    nextIsWhile,
    var,
    nextIsUntil
};

enum redir
{
    Droite2,
    Gauche2,
    Droite2tiret,
    Droite_et,
    Gauche_et,
    Droite_barre,
    Gauche_droite,
    Barre,
    Droite,
    Gauche,
    Non_redir
};

struct tree
{
    enum typ typ;
    enum redir redir;
    char *key;
    int number_brother;
    struct tree *first_child;
    struct tree *brother_node;
};

#include "../42sh.h"

int treeSize(struct tree *root);
struct tree *treeInit(void);
struct tree *appendNode(struct tree *parent, char *value, enum typ typ
                                    , enum redirection redirection);
struct tree *appendNodeBas(struct tree *parent, char *value, enum typ typ
                               , enum redirection redirection);
struct tree *appendNodeDroite(struct tree *parent, char *value, enum typ typ
                               , enum redirection redirection);
void delete_all_nodes(struct tree *root);
void *xmalloc1(size_t nmemb, size_t size);
