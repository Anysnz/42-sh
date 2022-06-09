#include "variables.h"
#include "ast.h"
#include <string.h>

struct var *init_var()
{
    struct var *new = xmalloc1(sizeof(struct var), 1);
    new->nom = NULL;
    new->valeur = NULL;
    new->next = NULL;
    return new;
}

char *get_var_value(struct var *mesVars, char *nom)
{
    while (mesVars)
    {
        if (mesVars->valeur)
            if (mesVars->nom)
                if (strcmp(mesVars->nom, nom) == 0)
                    return mesVars->valeur;
        mesVars = mesVars->next;
    }
    return NULL;
}

struct var *modifie_or_add_var(struct var *mesVars, char *nom, char *valeur)
{
    struct var *saveLast = NULL;
    while (mesVars)
    {
        if (mesVars->nom)
        {
            if (strcmp(mesVars->nom, nom) == 0)
            {
                if (mesVars->valeur)
                    free(mesVars->valeur);
                mesVars->valeur = valeur;
                return mesVars;
            }
        }
        saveLast = mesVars;
        mesVars = mesVars->next;
    }
    struct var *new = xmalloc1(sizeof(struct var), 1);
    new->next = NULL;
    new->valeur = valeur;
    new->nom = nom;
    saveLast->next = new;
    return new;
}

void dellete_all_vars(struct var *mesVars)
{
    if (mesVars)
    {
        dellete_all_vars(mesVars->next);
        if (mesVars->valeur)
            free(mesVars->valeur);
        if (mesVars->nom)
            free(mesVars->nom);
        free(mesVars);
    }
}

int is_variable_declaration(char *input)
{
    int is_equal = 0;
    int betwin_quote = 0;
    int i = 0;
    while (input[i] != 0)
    {
        if (input[i] == ' ' && betwin_quote == 0)
            return -1;
        if (input[i] == '\t' && betwin_quote == 0)
            return -1;
        if (input[i] == '"' && is_equal == 0)
            return -1;
        if (input[i] == '"' && betwin_quote == 0)
            betwin_quote = 1;
        else if (input[i] == '"' && betwin_quote == 1)
            betwin_quote = 0;
        if (input[i] == '=' && is_equal == 0)
            is_equal = 1;
        else if (input[i] == '=' && is_equal == 1)
            return -1;
        i++;
    }
    return is_equal;
}

void extract_name_value_and_add(char *input, struct var *mesVars)
{
    int i = 0;
    char *name = xmalloc1(sizeof(char), MAGIC_NB);
    char *value = xmalloc1(sizeof(char), MAGIC_NB);
    int j = 0;
    while (input[i] != '=')
    {
        name[i] = input[i];
        i++;
    }
    i++;
    while (input[i] != 0)
    {
        if (input[i] != '"')
        {
            value[j] = input[i];
            j++;
        }
        i++;
    }
    modifie_or_add_var(mesVars, name, value);
}

void replace_string(char *input, char *output, struct var *mesVars)
{
    int i = 0;
    int j = 0;
    int l = 0;
    int compute = 0;
    char *value = NULL;
    while (input[i] != 0)
    {
        if (input[i] == '$')
        {
            char *name = xmalloc1(1, MAGIC_NB);
            i += extract_var_name(input + i, name, &compute);
            if (strcmp(name, "RANDOM") == 0)
            {
                srand(time(NULL));
                int random = rand() % 32768;
                char generated[6] = {'\0'};
                my_itoa(random, generated);
                while (generated[l] != 0)
                {
                    output[j] = generated[l];
                    j++;
                    l++;
                }
                l = 0;
            }
            else
                value = get_var_value(mesVars, name);
            l = 0;
            if (value)
            {
                while (value[l] != 0)
                {
                    output[j] = value[l];
                    j++;
                    l++;
                }
                l = 0;
            }
            free(name);
        }
        else
        {
            output[j] = input[i];
            j++;
            i++;
        }
    }
    if (compute == 1)
        compute_var(output);
}

void add_start_variable(struct var *mesVars, int argc, char **argv)
{
    char *hashtag = xmalloc1(1, 1);
    char *hashtagc = xmalloc1(MAGIC_NB, 1);
    hashtag[0] = '#';
    my_itoa(argc - 2, hashtagc);
    modifie_or_add_var(mesVars, hashtag, hashtagc);
    char *etoile = xmalloc1(1, 1);
    char *etoilec = xmalloc1(MAGIC_NB, 1);
    etoile[0] = '*';
    for (int i = 2; i < argc; i++)
    {
        strcat(etoilec, argv[i]);
        if (i + 1 < argc)
            strcat(etoilec, " ");
    }
    modifie_or_add_var(mesVars, etoile, etoilec);
    char *arobase = xmalloc1(1, 1);
    char *arobasec = xmalloc1(MAGIC_NB, 1);
    arobase[0] = '@';
    strcpy(arobasec, etoilec);
    modifie_or_add_var(mesVars, arobase, arobasec);
    char *dolar = xmalloc1(1, 1);
    dolar[0] = '$';
    char *dolarc = xmalloc1(10, 10);
    int pid = getpid();
    my_itoa(pid, dolarc);
    modifie_or_add_var(mesVars, dolar, dolarc);
}
