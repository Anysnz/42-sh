#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "../tools/ast.h"
#include "../lexer/lexer.h"

struct list *cmpAndPass(struct list *list, const char valeur[10], enum type t);
struct list *else_clause(struct list *list);
struct list *rule_if(struct list *list);
struct list *skipLesTextes(struct list *list);
struct list *skipLesRiens(struct list *list);
struct list *skipLesTextesEtLesRiens(struct list *list);
struct list *do_group(struct list *list);
struct list *rule_for(struct list *list);
struct list *rule_while(struct list *list);
struct list *skipAllExeptCond(struct list *list);
struct list *skipAllExeptBoucle(struct list *list); 
struct list *rule_until(struct list *list);    
