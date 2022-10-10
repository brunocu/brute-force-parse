#define CVECTOR_LOGARITHMIC_GROWTH
#include <stdio.h>

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

// external linkage
extern struct lhs_t *lhs;
extern size_t lhs_size;
extern char **rhs;
extern size_t rhs_size;
extern void load_grammar(FILE *);

inline void put_usage(void);

void print_lhs(struct lhs_t);

int main(int argc, char **argv)
{
    ++argv;
    --argc;
    FILE *fin = NULL;
    if (argc > 0)
        fin = fopen(argv[0], "r");
    else
    {
        put_usage();
        return (1);
    }

    load_grammar(fin);
    for (size_t i = 0; i < lhs_size; i++)
    {
        print_lhs(lhs[i]);
    }
    
    return 0;
}

void put_usage(void)
{
    puts("usage: fuerza-bruta <grammar path> <string>");
}

void print_lhs(struct lhs_t lhs)
{
    printf("NT=%c, MAX=%d, FIRST=%d\n", lhs.nt, lhs.max, lhs.first);
    for (size_t rhs_idx = 0; rhs_idx < lhs.max; rhs_idx++)
    {
        printf("%c := %s\n", lhs.nt, rhs[lhs.first + rhs_idx]);
    }
    
}