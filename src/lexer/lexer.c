#include "lexer.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct global global;

int indication_fin(struct list *l)
{
    char *s = l->valeur;

    if ((strcmp(s, "&") == 0 || strcmp(s, ";") == 0 || strcmp(s, "\n") == 0) &&
            l->type == ponctuation)
        return 1;

    return 0;
}

void add_a_word(struct list *head, char *input, int *dbt_mot, int *i)
{
    if (head->next)
        head = global.tail; // jump directly to the tail of tokens

    if ((input[*dbt_mot] != ' ' && input[*dbt_mot] != '\t')
                                        || global.after_anti_slash == 1)
    {
        char *mot = NULL;
        if (*dbt_mot == *i) // cas particulier
        {
            mot = xmalloc1(sizeof(char), 2);
            mot[0] = input[*i];
            *i += 1;
        }
        else
        {
            mot = xmalloc1(sizeof(char), *i - *dbt_mot + 1);
            strncpy(mot, input + *dbt_mot, *i - *dbt_mot);
        }
        global.tail = list_append(mot, head);
    }
    else
        global.space++;

    while (est_une_ponct(input[*i]) && input[*i] != '\\') // '\n' bug fixed
    {
        char *mot = xmalloc1(sizeof(char), 2);
        mot[0] = input[*i];
        list_append(mot, head);
        *i += 1;
    }

    if (global.special == 0)
    {
        while (input[*i] == ' ' || input[*dbt_mot] == '\t')
        {
            *i += 1;
            global.space++;
        }
    }
    *dbt_mot = *i;

    return;
}

void add_a_word_1(struct list *head, char *input, int *dbt_mot, int *i)
{
    if (head->next)
        head = global.tail; // jump directly to the tail of tokens

    char *mot = NULL;
    if (*dbt_mot == *i) // cas particulier
    {
        mot = xmalloc1(sizeof(char), 2);
        mot[0] = input[*i];
        *i += 1;
    }
    else
    {
        mot = xmalloc1(sizeof(char), *i - *dbt_mot + 1);
        strncpy(mot, input + *dbt_mot, *i - *dbt_mot);
    }
    global.tail = list_append(mot, head);

    *dbt_mot = *i;

    return;
}

void variable_special_1(char *input, struct list *head, int *i, int *dbt_mot)
{
    char c = input[*i];
    *dbt_mot = *i;
    if (input[*i + 1] == '{')
    {
        while (c != '}')
        {
            if (c == '\0')
            {
                warn("variable definition");
                global.error = 2;
                break;
            }
            else if (c == '\\')
            {
                add_a_word_1(head, input, dbt_mot, i);
                add_a_word_1(head, input, dbt_mot, i);
            }
            else
                *i += 1;
            c = input[*i];
        }
    }
    else
    {
        while (c != ' ' && c != '\t')
        {
            if (c == '\0')
            {
                warn("variable definition");
                global.error = 2;
                break;
            }
            else if (c == '\\')
            {
                add_a_word_1(head, input, dbt_mot, i);
                add_a_word_1(head, input, dbt_mot, i);
            }
            else
                *i += 1;
            c = input[*i];
            if (c == '\"')
                break;
        }
    }

    add_a_word_1(head, input, dbt_mot, i);
}

void lexer_quotes_1(char *input, struct list *head, int *i, int *dbt_mot)
{
    char c = input[*i];
    while (c != '"')
    {
        if (c == '$' && global.special == 1)
            variable_special_1(input, head, i, dbt_mot);
        else if (c == '\0')
        {
            warn("quoting");
            global.error = 2;
            break;
        }
        else if (c == '\\')
        {
            add_a_word_1(head, input, dbt_mot, i);
            add_a_word_1(head, input, dbt_mot, i);
        }
        else
            *i += 1;
        c = input[*i];
    }
    add_a_word_1(head, input, dbt_mot, i);
}

void lexer_quotes_2(char *input, struct list *head, int *i, int *dbt_mot)
{
    char c = input[*i];
    while (c != '\'')
    {
        if (c == '\0')
        {
            warn("quoting");
            global.error = 2;
            break;
        }
        else
            *i += 1;
        c = input[*i];
    }
    add_a_word_1(head, input, dbt_mot, i);
}

struct list *lexer(char *input, struct list *head)
{
    int dbt_mot = 0;
    int i = 0;
    int shebang = 0;
    char c = 1;
    while (c != '\0')
    {
        c = input[i];
        if (c == '#')
        {
            if ((i == 0 || input[i - 1] == ' ' || input[i - 1] == '\t'
                              || input[i - 1] == '\n') && global.special == 0)
                shebang = 1;
        }
        else if (c == '\n')
            shebang = 0;

        if (global.special == 1)
            lexer_quotes_1(input, head, &i, &dbt_mot);
        else if (global.special == 2)
            lexer_quotes_2(input, head, &i, &dbt_mot);
        else if ((test_end_of_word(input[i]) || i == MAGIC_NB || c == '\\'
                        || global.after_anti_slash == 1) && shebang == 0)
            add_a_word(head, input, &dbt_mot, &i);
        else
        {
            i++;
            if (shebang == 1)
                dbt_mot = i;
        }
    }

    if (global.after_anti_slash == 1)
    {
        struct list *tmp = head;
        while (tmp->next)
            tmp = tmp->next;
        tmp->valeur = '\0';
    }

    return head;
}

struct list *list_init(void)
{
    struct list *new = xmalloc1(sizeof(struct list), 1);
    new->type = rien;
    new->valeur = 0;
    new->next = NULL;

    return new;
}

void change_global_special(struct list *l)
{
    if (global.special != 0)
        l->quoted = 1;

    if (l->type != ponctuation)
        return;

    char *valeur = l->valeur;
    if (strcmp(valeur, "\"") == 0)
    {
        if (global.special == 1)
            global.special = 0;
        else if (global.special == 0)
            global.special = 1;
    }
    else if (strcmp(valeur, "\'") == 0)
    {
        if (global.special == 2)
            global.special = 0;
        else if (global.special == 0)
            global.special = 2;
    }

    return;
}

void attribute_type(struct list *new)
{
    char *valeur = new->valeur;

    if (est_une_ponct(valeur[0]) == 1)
        new->type = ponctuation;
    else if (is_variable_declaration(valeur) == 1)
        new->type = variable;
    else if (valeur[0] == 0)
        new->type = rien;
    else
        new->type = texte;

    return;
}

void attribute_type_first(struct list *new)
{
    char *valeur = new->valeur;

    if (est_une_condition(valeur) == 1)
        new->type = condition;
    else if (est_une_ponct(valeur[0]) == 1)
        new->type = ponctuation;
    else if (est_une_instr(valeur) == 1)
        new->type = instruction;
    else if (est_une_boucle(valeur) == 1)
        new->type = boucle;
    else if (is_variable_declaration(valeur) == 1)
        new->type = variable;
    else if (valeur[0] == 0)
        new->type = rien;
    else
        new->type = texte;

    return;
}

struct list *list_append(char *valeur, struct list *head)
{
    while (head->next)
        head = head->next;

    struct list *new = NULL;
    if (strcmp(valeur, "\\") == 0 && global.after_anti_slash == 1
                                             && global.special == 1)
    {
        free(head->valeur);
        new = head;
        new->backslashed = 1;
    }
    else if (global.after_anti_slash == 1 && global.special == 0)
    {
        free(head->valeur);
        new = head;
        new->backslashed = 1;
    }
    else
    {
        new = xmalloc1(sizeof(struct list), 1);
        head->next = new;
        new->space = global.space;
    }
    global.space = 0;
    new->valeur = valeur;
    new->next = NULL;

    global.tail = new;

    if (global.after_anti_slash == 0 && global.special == 0)
    {
        if (global.beg_command == 1)
            attribute_type_first(new);
        else
            attribute_type(new);

        if (strcmp(valeur, "\\") == 0)
            global.after_anti_slash = 1;

        new->redirection = est_une_redirection(valeur);
    }
    else if (global.after_anti_slash == 1)
    {
        new->type = texte;
        if (strcmp(valeur, "\n") == 0)
            new->type = ponctuation;
        else
            global.after_anti_slash = 0;

        new->redirection = non_redir;
    }
    else if (global.special == 1)
    {
        if (strcmp(valeur, "\\") == 0)
        {
            global.after_anti_slash = 1;
            new->type = ponctuation;
        }
        else if (valeur[0] == '$')
            new->type = variable;
        else if (strcmp(valeur, "\"") == 0)
            new->type = 3;
        else
            new->type = texte;

        new->redirection = non_redir;
    }
    else // global.special == 2
    {
        new->type = texte;
        new->redirection = non_redir;
        if (strcmp(valeur, "\'") == 0)
            new->type = 3;
    }

    change_global_special(new);
    if (indication_fin(new))
        global.beg_command = 1;
    else
        global.beg_command = 0;

    return new;
}

struct list *find_token(struct list *head, char *token)
{
    if (head->next)
        head = head->next;

    while (head)
    {
        if (strcmp(head->valeur, token) == 0)
            return head;

        head = head->next;
    }

    return NULL;
}

void print_list(struct list *head)
{
    if (head->next)
        head = head->next;

    while (head)
    {
        if (strcmp(head->valeur, "\n") == 0)
            printf ("\"\\n (newline)\" est de classe %d\n", head->type);
        else if (strcmp(head->valeur, "\0") == 0)
            printf ("\"\\0 (null character)\" est de classe %d\n", head->type);
        else
            printf ("\"%s\" est de classe %d\n", head->valeur, head->type);
        head = head->next;
    }
}

void destroy_list(struct list *head)
{
    if (!head)
        return;

    destroy_list(head->next);
    if (head->valeur)
        free(head->valeur);
    free(head);

    return;
}
