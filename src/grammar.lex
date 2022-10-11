%{
#include <stdlib.h>
#include <string.h>

#ifndef LHS_STRUCT
#define LHS_STRUCT
struct lhs_type
{
    char nt;
    int max;
    int first;
};
#else
struct lhs_type;
#endif

struct lhs_type *lhs_curr = NULL;

struct lhs_type *lhs = NULL;
size_t lhs_size = 0;
char **rhs = NULL;
size_t rhs_size = 0;
%}
%option nodefault
%option noyywrap
%option array

%x LHS
%x RHS

NEWLINE [\r\n]
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
                            lhs = (struct lhs_type *) reallocarray(lhs, ++lhs_size, sizeof(struct lhs_type));
                            lhs[lhs_size - 1] = (struct lhs_type) {
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
<RHS>{NEWLINE}{1,2} BEGIN(LHS);
%%
void load_grammar(FILE *filein)
{
    yyin = filein;
    BEGIN(LHS);
    yylex();
}