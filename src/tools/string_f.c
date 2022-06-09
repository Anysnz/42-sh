#include "string_f.h"

void compute_var(char *input)
{
    int i = 0;
    int j = 0;
    int saveOp = 0;
    size_t saven1;
    int saven2;
    size_t len = strlen(input);
    char *n1 = xmalloc1(len, 1);
    char *n2 = xmalloc1(len, 1);
    char *res = xmalloc1(len, 1);
    char *output = xmalloc1(len, 1);
    while (input[i] < '0' || input[i] > '9')
        i++;
    saven1 = i;
    while (input[i] >= '0' && input[i] <= '9')
    {
        n1[j] = input[i];
        j++;
        i++;
    }
    while (input[i] != '+' && input[i] != '-' && input[i] != '*'
            && input[i] != '/')
        i++;
    saveOp = i;
    while (input[i] < '0' || input[i] > '9')
        i++;
    j = 0;
    while (input[i] >= '0' && input[i] <= '9')
    {
        n2[j] = input[i];
        j++;
        i++;
    }
    saven2 = i;
    if (input[saveOp] == '+')
        my_itoa(atoi(n1) + atoi(n2), res);
    else if (input[saveOp] == '/')
        my_itoa(atoi(n1) / atoi(n2), res);
    else if (input[saveOp] == '-')
        my_itoa(atoi(n1) - atoi(n2), res);
    else if (input[saveOp] == '*')
        my_itoa(atoi(n1) * atoi(n2), res);
    strncat(output, input, saven1);
    strcat(output, res);
    len = strlen(output);
    while (input[saven2] != 0)
    {
        if (input[saven2] == ')' && input[saven2 + 1] == ')')
            saven2 += 2;
        output[len] = input[saven2];
        saven2++;
        len++;
    }
    strcpy(input, output);
    free(output);
}

int extract_var_name(char *input, char *name, int *compute)
{
    int i = 1;
    int k = 0;
    if (input[i] == '{')
    {
        i++;
        while (input[i] != '}')
        {
            name[k] = input[i];
            k++;
            i++;
        }
        i++;
    }
    else if (input[i] == '(' && input[i + 1] == '(')
    {
        i += 2;
        *compute = 1;
        while (input[i] != '+' && input[i] != '-' && input[i] != '/'
                && input[i] != '*')
        {
            name[k] = input[i];
            k++;
            i++;
        }
    }
    else
    {
        while (input[i] != ' ' && input[i] != 0)
        {
            name[k] = input[i];
            k++;
            i++;
        }
    }
    return i;
}

int check_if_countain_only(char *input, char c1, char c2)
{
    int i = 0;
    while (input[i] != 0)
    {
        if (input[i] != c1 && input[i] != c2)
            return 0;
        i++;
    }
    return 1;
}

char *my_itoa(int value, char *s)
{
    if (value == 0)
    {
        s[0] = '0';
        s[1] = 0;
        return s;
    }
    int negat = 0;
    if (value < 0)
    {
        value = - value;
        negat = 1;
    }
    int i = value;
    int taille = 0;
    while (i > 9)
    {
        taille++;
        i /= 10;
    }
    i = 0;
    if (negat == 1)
    {
        s[0] = '-';
        taille++;
        i = 1;
    }
    while (value > 0)
    {
        s[taille] = value % 10 + '0';
        value /= 10;
        taille--;
        i++;
    }
    return s;
}

void split_cond(char *s, char *c1, char *c2, char *c3, char *c4)
{
    int i = 0;
    i += append_to_space_or_tab(s, c1) + 1;
    i += append_to_space_or_tab(s + i, c2) + 1;
    i += append_to_space_or_tab(s + i, c3) + 1;
    append_to_space_or_tab(s + i, c4);
}

int append_to_space_or_tab(char *input, char *output)
{
    int i = 0;
    while ((input[i] == ' ' || input[i] == '\t') && input[i] != 0)
        i++;
    while (input[i] != ' ' && input[i] != '\t' && input[i] != 0)
    {
        output[i] = input[i];
        i++;
    }
    return i;
}
