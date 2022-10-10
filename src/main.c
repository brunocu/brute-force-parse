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

void print_lhs(void);

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
    print_lhs();

    return 0;
}

void put_usage(void)
{
    puts("usage: fuerza-bruta <grammar path> <string>");
}

void print_lhs(void)
{
    size_t r = 1;
    for (size_t i = 0; i < lhs_size; i++)
    {
        printf("NT=%c, MAX=%d, FIRST=%d\n", lhs[i].nt, lhs[i].max, lhs[i].first);
        for (size_t rhs_idx = 0; rhs_idx < lhs[i].max; rhs_idx++)
        {
            printf("%d. %c := %s\n", r, lhs[i].nt, rhs[lhs[i].first + rhs_idx]);
            r++;
        }
    }
}