#include "ast.h"

int treeSize(struct tree *root)
{
    int a = 1;
    if (root->first_child)
        a += treeSize(root->first_child);
    if (root->brother_node)
        a += treeSize(root->brother_node);
    return  a;
}

struct tree *treeInit(void)
{
    struct tree *new = xmalloc1(sizeof(struct tree), 1);
    new->first_child = NULL;
    new->brother_node = NULL;
    new->key = NULL;
    new->typ = root;
    new->redir = Non_redir;
    new->number_brother = 0;
    return new;
}

struct tree *appendNode(struct tree *parent, char *value, enum typ typ
                               , enum redirection redirection)
{
    struct tree *new = xmalloc1(sizeof(struct tree), 1);
    new->first_child = NULL;
    new->brother_node = NULL;
    new->key = value;
    new->number_brother = 0;
    new->typ = typ;
    new->redir = (enum redir) redirection;
    parent->number_brother++;
    if (parent->first_child == NULL)
        parent->first_child = new;
    else
    {
        parent = parent->first_child;
        while (parent->brother_node != NULL)
            parent = parent->brother_node;
        parent->brother_node = new;
    }
    return new;
}

struct tree *appendNodeBas(struct tree *parent, char *value, enum typ typ
                               , enum redirection redirection)
{
    struct tree *new = xmalloc1(sizeof(struct tree), 1);
    new->first_child = NULL;
    new->brother_node = NULL;
    new->key = value;
    new->number_brother = 0;
    new->typ = typ;
    parent->first_child = new;
    new->redir = (enum redir)redirection;
    return new;
}

struct tree *appendNodeDroite(struct tree *parent, char *value, enum typ typ
                               , enum redirection redirection)
{
    struct tree *new = xmalloc1(sizeof(struct tree), 1);
    new->first_child = NULL;
    new->brother_node = NULL;
    new->key = value;
    new->number_brother = 0;
    new->typ = typ;
    parent->number_brother++;
    parent->brother_node = new;
    new->redir = (enum redir)redirection;
    return new;
}

void delete_all_nodes(struct tree *root)
{
    if (!root)
        return;

    if (root->first_child)
        delete_all_nodes(root->first_child);
    if (root->brother_node)
        delete_all_nodes(root->brother_node);
    if (root->key)
        free(root->key);
    free(root);

    return;
}

void *xmalloc1(size_t nmemb, size_t size)
{
    void *ptr = calloc(nmemb, size);
    if (!ptr)
        err(1, "NULL allocation");
    return ptr;
}
