#pragma once

// MAGIC_NB: constante arbitraire
#define MAGIC_NB 1000

enum type
{
    rien,
    condition,
    texte,
    ponctuation,
    instruction,
    boucle,
    variable
};

enum redirection
{
    droite2,
    gauche2,
    droite2tiret,
    droite_et,
    gauche_et,
    droite_barre,
    gauche_droite,
    barre,
    droite,
    gauche,
    non_redir
};

struct list
{
    enum type type;
    enum redirection redirection;
    char *valeur;
    int space; // If the current character is preceded by a space
    int quoted; // If it is quoted
    int backslashed; // If it is backslashed
    struct list *next;
};

#include "../tools/variables.h"
#include "../42sh.h"

/* add_a_word:
 * ajoute un token lorsque la fin d'un mot est reconnu. */
void add_a_word(struct list *head, char *input, int *dbt_mot, int *i);

/* lexer:
 * creer la list de tokens a partir de l'input. */
struct list *lexer(char *input, struct list *head);

/* list_init:
* initialize la liste. */
struct list *list_init(void);

/* list_append:
 * Ajoute l'element "valeur" (deja allouee par l'utilisateur) a la liste "head"
 * et met automatiquement a jour le type. */
struct list *list_append(char *valeur, struct list *head);

/* find_token:
 * Renvoie le premier emplacement comportant le token, NULL sinon. */
struct list *find_token(struct list *head, char *token);

/* print_list:
 * affiche la liste de tokens ainsi que leur type:
 * - 0: rien (ne devrait jamais s'afficher)
 * - 1: condition: if, fi, then, else
 * - 2: texte quelconque
 * - 3: ponctuation: "&;[]\n!" */
void print_list(struct list *head);

/* destroy_list:
 * libere entierement la liste de tokens. */
void destroy_list(struct list *head);
