#include "tests.h"

int test_end_of_word(char c)
{
    return (c == '\t' || c == ' ' || c == '\0' || est_une_ponct(c));
}

int est_une_condition(char *input)
{
    char commandes[5][5] = {"if", "then", "else", "fi", "elif"};

    for (int i = 0; i < 5; i++)
    {
        if (strcmp(input, commandes[i]) == 0)
            return 1;
    }

    return 0;
}

int est_une_instr(char *s)
{
    char instructions[3][5] = {"echo", "ls", "cat"};

    for (int i = 0; i < 3; i++)
    {
        if (strcmp(s, instructions[i]) == 0)
            return 1;
    }

    return 0;
}

int est_une_ponct(char c)
{
    char ponct[9] = "\"\'\\&;[]\n!";

    for (int i = 0; i < 9; i++)
    {
        if (c == ponct[i])
            return 1;
    }

    return 0;
}

int est_une_boucle(char *c)
{
    char boucle[6][6] = {"do", "done", "while", "in", "for", "until"};

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(boucle[i], c) == 0)
            return 1;
    }

    return 0;
}

int est_une_redirection(char *s)
{
    char red[10][4] = {">>", "<<", "<<-", ">&", "<&", ">|", "<>", "|", ">"
                                                    , "<"};

    for (int i = 0; i < 10; i++)
    {
        if (strstr(s, red[i]) != NULL)
            return i;
    }

    return 10;
}
