#include "execute.h"

#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

size_t take_address_recipe(char *recipe)
{
    size_t addr = 0;
    while (recipe[addr] != ' ' && recipe[addr] != '\t')
    {
        if (recipe[addr] == '\0')
            return addr;

        addr++;
    }

    return addr;
}

void execute_with_execvp(char *recipe)
{
    size_t ind = take_address_recipe(recipe);
    char *instr = xmalloc1(ind + 1, sizeof(char));
    for (size_t y = 0; y < ind; y++)
        instr[y] = recipe[y];

    if (recipe[strlen(recipe) - 1] == ' ')
        recipe[strlen(recipe) - 1] = '\0';

    char *real_recipe = recipe + ind + 1;
    if (strcmp(instr, "echo") == 0)
      //  echo(real_recipe);
        return;
    else if (strcmp(instr, "cd") == 0)
    {
        chdir(real_recipe);
        kill(getpid(), SIGTERM);
    }
    else
    {
        char *argv[] = {"42sh", real_recipe, NULL};
        if (recipe[ind + 1] == '\0')
            argv[1] = NULL;

        if (execvp(instr, argv) == -1)
        {
            warn("execute_with_execvp in execute.c: %s", instr);
            kill(getpid(), SIGTERM);
        }
    }

    free(instr);

    return;
}

void execute_condition(char *c1, char *c2, char *c3, char *c4)
{
    if (strcmp(c2, "") == 0)
    {
        if (execlp("[", "42sh", c1, "]", NULL) == -1)
            warn("issue during evaluation of condition");
    }
    else if (strcmp(c3, "") == 0)
    {
        if (execlp("[", "42sh", c1, c2, "]", NULL) == -1)
            warn("issue during evaluation of condition");
    }
    else if (strcmp(c4, "") == 0)
    {
        if (execlp("[", "42sh", c1, c2, c3, "]", NULL) == -1)
            warn("issue during evaluation of condition");
    }
    else
    {
        if (execlp("[", "42sh", c1, c2, c3, c4, "]", NULL) == -1)
            warn("issue during evaluation of condition");
    }
    return;
}

int condition_is_true(char *s)
{
    if (check_if_countain_only(s, ' ', '\t') == 1)
        return 0;
    int pid = fork();
    size_t len = strlen(s);
    char *c1 = xmalloc1(len, sizeof(char));
    char *c2 = xmalloc1(len, sizeof(char));
    char *c3 = xmalloc1(len, sizeof(char));
    char *c4 = xmalloc1(len, sizeof(char));
    split_cond(s, c1, c2, c3, c4);
    if (pid == 0)
        execute_condition(c1, c2, c3, c4);

    int wstatus = 0;
    if (waitpid(0, &wstatus, 0) == -1)
        err(1, "error while waiting child process");

    free(c1);
    free(c2);
    free(c3);
    free(c4);

    return !WEXITSTATUS(wstatus);
}

void inverse_cond(struct tree *ast)
{
    size_t len = strlen(ast->key);
    char *output = xmalloc1(len, 1);
    int i = 0;
    int j = 0;
    int s = 0;
    while (ast->key[i] != 0 && s == 0)
    {
        if (ast->key[i] == ' ' || ast->key[i] == '\t')
        {
            output[j] = ast->key[i];
            j++;
        }
        else if (ast->key[i] != '!')
        {
            output[j] = '!';
            j++;
            output[j] = ' ';
            j++;
            output[j] = ast->key[i];
            j++;
            s = 1;

        }
        i++;
    }
    while (ast->key[i] != 0)
    {
        output[j] = ast->key[i];
        i++;
        j++;
    }
    strcpy(ast->key, output);
    free(output);
}

int execute_ast(struct tree *ast, struct var *mesVars)
{
    if (ast)
    {
        char *instruction = xmalloc1(1, MAGIC_NB);
        if (ast->key)
            replace_string(ast->key, instruction, mesVars);
        if (ast->typ == root)
            execute_ast(ast->first_child, mesVars);
        else if (ast->typ == instruct)
        {
            if (ast->redir == Droite || ast->redir == Droite2
                || ast->redir == Droite_et || ast->redir == Droite_barre)
                execute_with_redirections(instruction, ast->redir);
            else
            {
                execute_recipe(instruction);
                execute_ast(ast->first_child, mesVars);
            }
        }
        else if (ast->typ == nextIsCond)
        {
            execute_ast(ast->brother_node, mesVars);
            execute_ast(ast->first_child, mesVars);
        }
        else if (ast->typ == nextIsWhile)
        {
            int a = 1;
            while (a == 1)
                a = execute_ast(ast->brother_node, mesVars);
            execute_ast(ast->first_child, mesVars);
        }
        else if (ast->typ == nextIsUntil)
        {
            int a = 1;
            inverse_cond(ast->brother_node);
            while (a == 1)
                a = execute_ast(ast->brother_node, mesVars);
            execute_ast(ast->first_child, mesVars);
        }
        else if (ast->typ == var)
        {
            extract_name_value_and_add(instruction, mesVars);
            execute_ast(ast->first_child, mesVars);
        }
        else if (ast->typ == cond)
        {
            if (condition_is_true(instruction))
                execute_ast(ast->first_child, mesVars);
            else
            {
                execute_ast(ast->brother_node, mesVars);
                free(instruction);
                return 0;
            }
        }
        free(instruction);
        return 1;
    }
    return 0;
}

void execute_recipe(char *recipe)
{
    pid_t pid = fork();

    if (pid == 0)
        execute_with_execvp(recipe);

    int wstatus = 0;
    if (waitpid(0, &wstatus, 0) == -1)
        warn("error waitpid in execute_recipe in execute.c");

    return;
}

void execute_with_redirections(char *recipe, int red)
{
    int pid = fork();
    if (pid == -1)
        err(118, "%s", recipe);

    if (pid == 0)
    {
        char real_recipe[MAGIC_NB] = {'\0'};
        char out_file[MAGIC_NB] = {'\0'};

        take_recipe_redir(real_recipe, recipe);
        take_out_file_redir(out_file, recipe);

        int flags = O_RDWR | O_CREAT | O_TRUNC;
        if (red == Droite2)
            flags = O_RDWR | O_CREAT | O_APPEND;

        int out = open(out_file, flags, 0644);
        if (out == -1)
            err(134, "execute_with_redirections");

        if (dup2(out, fileno(stdout)) == -1)
            err(137, "execute_with_redirections");
        if (red == Droite_et)
            if (dup2(out, fileno(stderr)) == -1)
                err(140, "execute_with_redirections");

        execute_with_execvp(real_recipe);

        close(out);
    }

    else
    {
        int status = 0;
        if (waitpid(pid, &status, 0) == -1)
                err(151, "42sh");
    }

    return;
}

char *take_recipe_redir(char *in, char *s)
{
    char *ptr = strstr(s, ">");
    int ite = 0;

    while (s + ite != ptr)
    {
        in[ite] = s[ite];
        ite++;
    }

    return in;
}

char *take_out_file_redir(char *in, char *s)
{
    char *ptr = strstr(s, ">");
    int ite = ptr - s;
    int depart = 0;

    while (s[ite] != '\0')
    {
        if (s[ite] == ' ' || s[ite] == '>' || s[ite] == '<' || s[ite] == '-'
                        || s[ite] == '\t' || s[ite] == '|' || s[ite] == '&')
        {
            ite++;
            continue;
        }

        in[depart] = s[ite];
        depart++;
        ite++;
    }

    return in;
}
