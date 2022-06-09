#include "ast_print.h"

size_t number_of_occurence(char *s, char c)
{
    size_t len = strlen(s);
    size_t number = 0;

    for (size_t p = 0; p < len; p++)
        if (s[p] == c)
            number++;

    return number;
}

char *what_to_print(struct tree *ast)
{
    char *to_ret = NULL;
    if (ast->key)
    {
        size_t more_alloc = number_of_occurence(ast->key, '"');
        size_t len = strlen(ast->key);
        to_ret = xmalloc1(sizeof(char), len + 3 + more_alloc);
        size_t ind = 0; // number of '"' encountered
        to_ret[0] = '"';
        for (size_t p = 0; p < len; p++)
        {
            if (ast->key[p] == '"')
            {
                to_ret[p + 1 + ind++] = '\\';
                to_ret[p + 1 + ind] = '"';
            }
            else
                to_ret[p + 1 + ind] = ast->key[p];
        }
        to_ret[len + ind + 1] = '"';
    }
    else if (ast->typ == root)
    {
        to_ret = xmalloc1(sizeof(char), strlen("\"ROOT\"") + 1);
        strcat(to_ret, "\"ROOT\"");
    }
    else if (ast->typ == nextIsCond)
    {
        to_ret = xmalloc1(sizeof(char), strlen("\"IF\"") + 1);
        strcat(to_ret, "\"IF\"");
    }
    else if (ast->typ == nextIsWhile)
    {
        to_ret = xmalloc1(sizeof(char), strlen("\"WHILE\"") + 1);
        strcat(to_ret, "\"WHILE\"");
    }

    return to_ret;
}

void concate_for_graph(char *text, char *s1, char *s2)
{
    strcat(text, "\n    ");
    strcat(text, s1);
    strcat(text, " -> ");
    strcat(text, s2);
    strcat(text, ";");

    return;
}

void rec_printTree(struct tree *ast, FILE *file)
{
    if (ast->first_child)
    {
        char *root = what_to_print(ast);
        char *child = what_to_print(ast->first_child);
        char *text = xmalloc1(strlen(root) + strlen(child) + 11, 1);

        concate_for_graph(text, root, child);
        fwrite(text, sizeof(char), strlen(text), file);

        free(root);
        free(child);
        free(text);

        rec_printTree(ast->first_child, file);
    }

    if (ast->brother_node)
    {
        char *root = what_to_print(ast);
        char *brother = what_to_print(ast->brother_node);
        char *texte = xmalloc1(strlen(root) + strlen(brother) + 11, 1);

        concate_for_graph(texte, root, brother);
        fwrite(texte, sizeof(char), strlen(texte), file);

        free(root);
        free(brother);
        free(texte);

        rec_printTree(ast->brother_node, file);
    }

    return;
}

void printTree(struct tree *root)
{
    FILE *file = fopen("tree.dot", "w");
    if (file == NULL)
        fprintf(stderr, "error in printTree\n");

    fwrite("digraph Ultimate_Answer_AST\n{\n", sizeof(char), 30, file);
    char *ROOT = "    ROOT [shape=polygon,sides=10,peripheries=3 \
                                            ,color=blue,style=filled]\n";
    fwrite(ROOT, sizeof(char), strlen(ROOT), file);
    char *IF = "    IF [shape=polygon,sides=4,color=red,style=filled]\n";
    fwrite(IF, sizeof(char), strlen(IF), file);
    char *WHILE = "    WHILE [shape=polygon,sides=4,color=green4 \
                                            ,style=filled]\n";
    fwrite(WHILE, sizeof(char), strlen(WHILE), file);
    rec_printTree(root, file);
    fwrite("\n}\n\0", sizeof(char), 3, file);

    fclose(file);

    return;
}
