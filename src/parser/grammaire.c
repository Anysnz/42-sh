#include "grammaire.h"

struct list *skipLesTextesEtLesRiens_if(struct list *list)
{
    if (list->valeur[0] == '\n')
    {
        if (list->next)
            return skipLesTextesEtLesRiens(list->next);
        else
            return NULL;
    }
    while (list->type == 0 || list->type == 2 || strcmp(list->valeur, "!") == 0)
        if (list->next)
            list = list->next;
    return list;
}

struct list *rule_if(struct list *list)
{
    if (!list->valeur)
        list = list->next;

    if ((list = cmpAndPass(list, "if", condition)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "[", ponctuation)) == NULL)
        return NULL;
    if ((list = skipLesTextesEtLesRiens_if(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "]", ponctuation)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, ";", ponctuation)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "then", condition)) == NULL)
        return NULL;
    if ((list = skipAllExeptCond(list)) == NULL)
        return NULL;
    if ((list = else_clause(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "fi", condition)) == NULL)
        return NULL;
    return list;
}

struct list *else_clause(struct list *list)
{
    if (cmpAndPass(list, "fi", condition) != NULL)
        return list;
    else if (cmpAndPass(list, "else", condition) != NULL)
    {
        list = cmpAndPass(list, "else", condition);
        if ((list = skipAllExeptCond(list)) == NULL)
            return NULL;
        else
            return list;
    }
    else if (cmpAndPass(list, "elif", condition) != NULL)
    {
        list = cmpAndPass(list, "elif", condition);
        if ((list = skipLesRiens(list)) == NULL)
            return NULL;
        if ((list = cmpAndPass(list, "[", ponctuation)) == NULL)
            return NULL;
        if ((list = skipLesTextesEtLesRiens(list)) == NULL)
            return NULL;
        if ((list = cmpAndPass(list, "]", ponctuation)) == NULL)
            return NULL;
        if ((list = skipLesRiens(list)) == NULL)
            return NULL;
        if ((list = cmpAndPass(list, ";", ponctuation)) == NULL)
            return NULL;
        if ((list = skipLesRiens(list)) == NULL)
            return NULL;
        if ((list = cmpAndPass(list, "then", condition)) == NULL)
            return NULL;
        if ((list = skipAllExeptCond(list)) == NULL)
            return NULL;
        return else_clause(list);
    }
    else
        return NULL;
}

struct list *cmpAndPass(struct list *list, const char valeur[10], enum type t)
{
    if (strcmp(list->valeur, valeur) == 0 && list->type == t)
        if (list->next)
            return list->next;
        else
            return NULL;
    else if (list->valeur[0] == '\n')
    {
        if (list->next)
            return cmpAndPass(list->next, valeur, t);
        else
            return NULL;
    }
    else
        return NULL;
}

struct list *skipLesRiens(struct list *list)
{
    if (list->valeur[0] == '\n')
    {
        if (list->next)
            return skipLesRiens(list->next);
        else
            return NULL;
    }
    while (list->type == 0)
        if (list->next)
            list = list->next;
    return list;
}

struct list *skipAllExeptBoucle(struct list *list)
{
    if (list->valeur[0] == '\n')
    {
        if (list->next)
            return skipAllExeptBoucle(list->next);
        else
            return NULL;
    }
    while (list->type != boucle)
        if (list->next)
            list = list->next;
    return list;
}

struct list *skipAllExeptCond(struct list *list)
{
    if (list->valeur[0] == '\n')
    {
        if (list->next)
            return skipAllExeptCond(list->next);
        else
            return NULL;
    }
    while (list->type != condition)
    {
        if (list->next)
            list = list->next;
        else
            return NULL;
    }
    return list;
}

struct list *skipLesTextesEtLesRiens(struct list *list)
{
    if (list->valeur[0] == '\n')
    {
        if (list->next)
            return skipLesTextesEtLesRiens(list->next);
        else
            return NULL;
    }
    while (list->type == 0 || list->type == 2)
        if (list->next)
            list = list->next;
    return list;
}


struct list *rule_while(struct list *list)
{
    if (!list->valeur)
        list = list->next;
    if ((list = cmpAndPass(list, "while", boucle)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "[", ponctuation)) == NULL )
        return NULL;
    if ((list = skipLesTextesEtLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "]", ponctuation)) == NULL )
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, ";", ponctuation)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = do_group(list)) == NULL)
        return NULL;
    return list;
}

struct list *rule_for(struct list *list)
{
    if (!list->valeur)
        list = list->next;
    if ((list = cmpAndPass(list, "for", boucle)) == NULL)
        return NULL;
    if ((list = skipLesTextesEtLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "in", boucle)) == NULL)
        return NULL;
    if ((list = skipAllExeptBoucle(list)) == NULL)
        return NULL;
    if ((list = do_group(list)) == NULL)
        return NULL;
    return list;

}

struct list *do_group(struct list *list)
{
    if ((list = cmpAndPass(list, "do", boucle)) == NULL)
        return NULL;
    if ((list = skipAllExeptBoucle(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "done", boucle)) == NULL)
        return NULL;
    return list;
}

struct list *rule_until(struct list *list)
{
    if (!list->valeur)
        list = list->next;
    if ((list = cmpAndPass(list, "until", boucle)) == NULL)
        return NULL;
    if ((list = skipLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "[", ponctuation)) == NULL)
        return NULL;
    if ((list = skipLesTextesEtLesRiens(list)) == NULL)
        return NULL;
    if ((list = cmpAndPass(list, "]", ponctuation)) == NULL)
        return NULL;
    if ((list = skipAllExeptBoucle(list)) == NULL)
        return NULL;
    if (( list = do_group(list)) == NULL)
        return NULL;
    return list;
}
