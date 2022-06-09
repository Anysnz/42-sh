#include "parser.h"
#include <sys/types.h>
#include <sys/wait.h>

struct global global;

int arret_if(struct list *tokens)
{
    char *s = tokens->valeur;

    return tokens->type == 1 && (strcmp(s, "elif") || strcmp(s, "else")
            || strcmp(s, "fi"));
}

int arret_boucle(struct list *tokens)
{
    char *s = tokens->valeur;
    if (strcmp(s, "do") == 0 || strcmp(s, "done") == 0)
        return 1;
    return 0;

}

struct tree *rule_if_parseur(struct list *token, struct tree *tree)
{
    char *condStr = xmalloc1(sizeof(char), MAGIC_NB);
    condStr = getCond(condStr, token);
    token = token->next;
    while (cmpAndPass(token, ";", ponctuation) == NULL)
        token = token->next;
    tree = appendNodeBas(tree, NULL, nextIsCond, (enum redirection)Non_redir);
    struct tree *saveIf = tree;
    tree = appendNodeDroite(tree, condStr, cond, (enum redirection)Non_redir);

    while (token && arret_if(token) == 0)
    {
        int type = 1;
        char *instructStr = xmalloc1(sizeof(char), MAGIC_NB);
        token = getInstruct(instructStr, token, &type);
        tree = appendNodeBas(tree, instructStr, type, token->redirection);
    }

    return else_clause_parseur(token, saveIf->brother_node, saveIf);
}

struct tree *else_clause_parseur(struct list *token, struct tree *tree,
        struct tree *saveIf)
{
    while (token->type != 1 || cmpAndPass(token, "then", condition) != NULL)
        token = token->next;
    if (cmpAndPass(token, "fi", condition) != NULL)
        return saveIf;
    else if (cmpAndPass(token, "else", condition) != NULL)
    {
        int type = 1;
        char *mot = xmalloc1(sizeof(char), MAGIC_NB);
        token = getInstruct(mot, token, &type);
        tree = appendNodeDroite(tree, mot, instruct, token->redirection);
        while (token && arret_if(token) == 0)
        {
            char *instr = xmalloc1(sizeof(char), MAGIC_NB);
            token = getInstruct(instr, token, &type);
            tree = appendNodeBas(tree, instr, type, token->redirection);
        }

        return saveIf;
    }
    else if (cmpAndPass(token, "elif", condition) != NULL)
    {
        char *condStr = xmalloc1(sizeof(char), MAGIC_NB);
        condStr = getCond(condStr, token);
        token = token->next;
        while (cmpAndPass(token, ";", ponctuation) == NULL)
            token = token->next;
        tree = appendNodeDroite(tree, condStr, cond, (enum redirection)Non_redir);
        struct tree *save_tree = tree;

        while (token && arret_if(token) == 0)
        {
            int type = 1;
            char *ins = xmalloc1(sizeof(char), MAGIC_NB);
            token = getInstruct(ins, token, &type);
            tree = appendNodeBas(tree, ins, type, token->redirection);
        }
        return else_clause_parseur(token, save_tree, saveIf);
    }
    return tree;
}

void replace(struct tree *root, struct tree *replaceMe, struct tree *withMe)
{
    if (root->brother_node == replaceMe)
        root->brother_node = withMe;
    if (root->first_child == replaceMe)
        root->first_child = withMe;
    if (root->brother_node)
        replace(root->brother_node, replaceMe, withMe);
    if (root->first_child)
        replace(root->first_child, replaceMe, withMe);
}

struct tree *parser(struct list *token)
{
    struct tree *new = treeInit();
    token = token->next;
    struct tree *saveRoot = new;
    while (token)
    {
        struct list *next = NULL;
        if ((next = rule_if(token)) != NULL)
        {
            new = rule_if_parseur(token, new);
            token = next;
        }
        else if ((next = rule_until(token)) != NULL)
        {
            new = rule_until_parseur(token, new);
            token = next;
        }
        else if ((next = rule_while(token)) != NULL)
        {
            new = rule_while_parseur(token, new);
            token = next;
        }
        else if (token->type == instruction || token->type == texte 
                || token->type == variable)
        {
            int type = 1;
            char *instructStr = xmalloc1(sizeof(char), MAGIC_NB);
            token = getInstruct(instructStr, token, &type);
            if (!token)
            {
                new = appendNodeBas(new, instructStr, type, non_redir);

                return saveRoot;
            }
            new = appendNodeBas(new, instructStr, type, token->redirection);
        }
        else
            token = token->next;
    }
    return saveRoot;
}

struct tree *rule_while_parseur(struct list *token, struct tree *tree)
{
    char *condStr = xmalloc1(sizeof(char), MAGIC_NB);
    condStr = getCond(condStr, token);
    token = token->next;
    while (cmpAndPass(token, ";", ponctuation) == NULL)
        token = token->next;
    tree = appendNodeBas(tree, NULL, nextIsWhile, (enum redirection)Non_redir);
    struct tree *saveIf = tree;
    tree = appendNodeDroite(tree, condStr, cond, (enum redirection)Non_redir);
    while (token && arret_boucle(token) == 0)
    {
        int type = 1;
        char *instructStr = xmalloc1(sizeof(char), MAGIC_NB);
        token = getInstruct(instructStr, token, &type);
        tree = appendNodeBas(tree, instructStr, type, token->redirection);
    }
    return saveIf;
}

struct tree *rule_until_parseur(struct list *token, struct tree *tree)
{
    char *condStr = xmalloc1(sizeof(char), MAGIC_NB);
    condStr = getCond(condStr, token);
    token = skipAllExeptBoucle(token->next)->next;
    tree = appendNodeBas(tree, NULL, nextIsUntil, (enum redirection)Non_redir);
    struct tree *saveIf = tree;
    tree = appendNodeDroite(tree, condStr, cond, (enum redirection)Non_redir);
    while (token && arret_boucle(token) == 0)
    {
        int type = 1;
        char *instructStr = xmalloc1(sizeof(char), MAGIC_NB);
        token = getInstruct(instructStr, token, &type);
        tree = appendNodeBas(tree, instructStr, type, token->redirection);
    }
    return saveIf;
}

char *getCond(char *cond, struct list *list)
{
    int a = 0;
    while (a == 0)
    {
        if (list->valeur)
            if (strcmp(list->valeur, "]") == 0)
                a = 1;
        if (list->type == 2 || strcmp(list->valeur, "!") == 0)
        {
            strcat(cond, list->valeur);
            strcat(cond, " ");
        }
        list = list->next;
    }
    return cond;
}

int fin_de_commande(struct list *l)
{
    if (!l)
        return 1;

    char *s = l->valeur;
    if ((l->type == ponctuation && (strcmp(s, "\n") == 0 || strcmp(s, ";") == 0
    || strcmp(s, "&") == 0) && global.quote_open == 0) || strcmp(s, "\0") == 0)
        return 1;

    return 0;
}

int do_not_take_the_quote(struct list *l)
{
    char *s = l->valeur;
    int b0 = l->type == ponctuation;
    int b1 = strcmp(s, "\"") == 0;
    int b2 = strcmp(s, "\'") == 0;
    int g0 = global.quote_open == 0;
    int g1 = global.quote_open == 1;

    if (!b0)
        return 0;

    if (b1)
    {
        if (g0)
            global.quote_open = 1;
        else if (g1)
            global.quote_open = 0;
        else
            return 0;

        return 1;
    }

    if (b2)
    {
        if (g0)
            global.quote_open = 2;
        else if (g1)
            return 0;
        else
            global.quote_open = 0;

        return 1;
    }

    return 0;
}

struct list *getInstruct(char *instruct, struct list *list, int *type)
{
    while (list->type != instruction && list->type != texte
            && list->type != variable)
        list = list->next;
    if (list->type == variable)
    {
        strcat(instruct, list->valeur);
        *type *= 5;
        return list->next->next;
    }

    int first = 1;
    while (!fin_de_commande(list))
    {
        strcat(instruct, list->valeur);
        if ((list->next && (list->next->space != 0 || list->next->quoted == 1))
                    && strcmp(list->valeur, "\\") != 0 && global.lwasb == 0
                                && strcmp(list->next->valeur, "\\") != 0)
        {
            global.lwasb = 0;
            strcat(instruct, " ");
        }
        else if (first == 1)
            strcat(instruct, " ");

        list = list->next;
        first = 0;
    }

    if (global.quote_open != 0) // error
    {
        global.error = 2;
        global.quote_open = 0;
        strcpy(instruct, "\0");
    }

    if (list)
        return list->next;

    return list;
}
