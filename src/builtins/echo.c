#include "echo.h"

int glob = 0;

int init_opt(int *n, int *e, char *input)
{
    int to_ret = 0;
    int i = 0;
    while (1)
    {
        char c = input[i];

        if (c == ' ' || c == '\t')
            i++;
        else if (c == '-' && (input[i + 1] == 'e' || input[i + 1] == 'n'
                    || input[i + 1] == 'E') && (input[i + 2] == ' '
                    || input[i + 2] == '\t' || input[i + 2] == '\0'
                    || input[i + 2] == '\\'))
        {
            if (input[i + 1] == 'e')
                *e = 1;
            else if (input[i + 1] == 'E')
                *e = 0;
            else
                *n = 1;

            i += 3;
            if (input[i + 2] == '\\')
                i--;
            glob = 1;
            to_ret = i;
        }
        else if (c == '-' && input[i + 1] == 'e' && input[i + 2] == 'n' &&
            (input[i + 3] == ' ' || input[i + 3] == '\t' || input[i + 3] == 0))
        {
            *e = 1;
            *n = 1;
            i += 4;
            to_ret = i;
        }
        else
            break;
    }

    return to_ret;
}

int count_backsl(char *s, int i)
{
    int nb = 0;
    for (int p = i - 1; p >= 0 && s[p] == '\\'; p--)
        nb = p;

    return nb;
}

int evaluate_quotes(char *s, int *i, int quotes)
{
    int bck = count_backsl(s, *i) % 2;
    char c = s[*i];
    if (c == '\'' && quotes == 0 && bck == 0)
    {
        *i += 1;
        if (s[*i] != '\\')
            *i += 1;
        quotes = 2;
    }
    else if (c == '\"' && quotes == 0 && bck == 0)
    {
        *i += 1;
        if (s[*i] != '\\')
            *i += 1;
        quotes = 1;
    }
    else if (c == ' ' && s[*i + 1] == '\'' && quotes == 2)
    {
        *i += 2;
        quotes = 0;
    }
    else if (c == ' ' && s[*i + 1] == '\"' && quotes == 1)
    {
        *i += 2;
        quotes = 0;
    }

    return quotes;
}

int sequences(char *s, char *input, int i, int p)
{
    switch (input[p])
    {
        case 'a':
            s[i++] = 7;
            break;

        case 'b':
            s[i++] = 8;
            break;

        case 'c':
            i = -1;
            break;

        case 'e':
            s[i++] = 27;
            break;

        case 'f':
            s[i++] = '\f';
            break;

        case 'n':
            s[i++] = '\n';
            break;

        case 'r':
            s[i++] = 13;
            break;

        case 't':
            s[i++] = '\t';
            break;

        case 'v':
            s[i++] = '\v';
            break;

        default:
            s[i++] = input[p - 1];
            s[i++] = input[p];
    }

    return i;
}

void echo(char *input)
{
    int n = 0;
    int e = 0;
    int len = strlen(input);
    char *s = calloc(sizeof(char), len + 100);
    if (!s)
        err(144, "echo: NULL allocation");

    int quotes = 0;
    int p = init_opt(&n, &e, input);
    int i = 0;
    while (p < len && input[p] != '\0')
    {
        char c = input[p];
        if (c == '\\' && e == 1 && quotes != 0)
        {
            p++;
            i = sequences(s, input, i, p);
            if (i == -1)
                break;
            p++;
        }
        else
        {
            int tmp = evaluate_quotes(input, &p, quotes);
            if (tmp != quotes)
            {
                quotes = tmp;
                continue;
            }

            s[i++] = input[p++];
        }
    }
    if (n == 1 && i != -1 && s[i] == '\n')
        s[i] = '\0';
    else if (n == 0 && i != -1 && s[i] != '\n')
        s[i] = '\n';
    if (glob == 1)
        putchar('\\');
    printf("%s", s);
    free(s);

    if (quotes != 0 && i != -1)
    {
        warn("syntax: guillemets");
        exit(2);
    }
    exit(0);
}
