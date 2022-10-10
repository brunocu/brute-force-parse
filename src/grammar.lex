%{
#include <stdlib.h>
#include <string.h>

#ifndef LHS_STRUCT
#define LHS_STRUCT
struct lhs_t
{
    char nt;
    int max;
    int first;
};
#else
struct lhs_t;
#endif

struct lhs_t *lhs_curr = NULL;

struct lhs_t *lhs = NULL;
size_t lhs_size = 0;
char **rhs = NULL;
size_t rhs_size = 0;
%}
%option nodefault
%option noyywrap
%option array

%x LHS
%x RHS
%%
 // TODO memory safety
<LHS>[[:upper:]]    {
                        if(lhs_curr && lhs_curr->nt == yytext[0])
                        {
                            // same symbol
                            lhs_curr->max++;
                        }
                        else
                        {
                            // new symbol
                            lhs = (struct lhs_t *) reallocarray(lhs, ++lhs_size, sizeof(struct lhs_t));
                            lhs[lhs_size - 1] = (struct lhs_t) {
                                .nt = yytext[0],
                                .max = 1,
                                .first = rhs_size
                            };
                            lhs_curr = &(lhs[lhs_size - 1]);
                        }
                    }
<LHS>[[:blank:]]+   BEGIN(RHS);
<RHS>[[:graph:]]+   {
                        rhs = (char **) reallocarray(rhs, ++rhs_size, sizeof(char *));
                        rhs[rhs_size - 1] = strdup(yytext);
                    }
<RHS>[[:space:]]+   BEGIN(LHS);
%%
void load_grammar(FILE *filein)
{
    yyin = filein;
    BEGIN(LHS);
    yylex();
}