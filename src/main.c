#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <search.h>

#pragma GCC diagnostic ignored "-Wincompatible-pointer-types"
//-- Grammar Structures --
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

// external linkage
extern struct lhs_type *lhs;
extern size_t lhs_size;
extern char **rhs;
extern size_t rhs_size;
extern void load_grammar(FILE *);

//-- Brute-Force Structures --
#define HIST_MAX 64
struct hist_type
{
    char symb;
    size_t p_num;
};

struct hist_type hist[HIST_MAX];
size_t hist_sz = 0;
#define hist_top (hist_sz - 1)

#define SENT_MAX 64
char sent[SENT_MAX];
size_t sent_sz = 0;
#define sent_top (sent_sz - 1)

enum state_enum
{
    PARSE,
    BACKTRACK,
    TERMINATION
} state = PARSE;

//-- prototypes --
inline void put_usage(void);
void print_lhs(void);
int parse(char *);

int lhs_compare(const char *, const struct lhs_type *);
int lhs_find(const char *);

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
    parse(argv[1]);

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

int parse(char *T)
{
    int conf_case;

    /// 1. Initialize
    size_t idx = 0;
    // Using \0 as end marker
    size_t t_len = strlen(T);
    // \e is a stand in for empty character
    hist[hist_sz++] = (struct hist_type){
        .symb = '\e',
        .p_num = 0,
    };
    memcpy(sent, (char[2]){'\0', lhs[0].nt}, 2 * sizeof(char));
    sent_sz += 2;

    /// 2. loop
    while (1)
    {
        /// 3. get stack-top elements and determine configuration
        struct hist_type hist_curr = hist[hist_top];
        char sent_tchar = sent[sent_top];
        if (state == PARSE && idx == t_len && sent_top == '\0')
        {
            conf_case = 3;
        }
        else
        {
            if (state == PARSE)
            {
                // look for top character in lhs
                int ft = lhs_find(&sent_tchar);
                if (ft != -1)
                {
                    conf_case = 1;
                }
                else
                {
                    if (sent_tchar == T[idx])
                    {
                        conf_case = 2;
                    }
                    else
                    {
                        conf_case = 4;
                    }
                }
            }
            else
            {
                // backtrack
                // look for top history in lhs
                int fs = lhs_find(&(hist_curr.symb));
                if (fs == -1)
                {
                    conf_case = 5;
                }
                else
                {
                    if (hist_curr.p_num < lhs[fs].max)
                    {
                        conf_case = 6;
                    }
                    else
                    {
                        if (idx == 0 && hist_curr.symb == lhs[0].nt)
                        {
                            // unsuccessful parse
                            return 1;
                        }
                        else
                        {
                            conf_case = 7;
                        }
                    }
                }
            }
        }

        /// 4. select case
    }

    return 0;
}

int lhs_compare(const char *key, const struct lhs_type *elem)
{
    // returns 0 if equal
    return !(*key == elem->nt);
}

/**
 * Find non-terminal character in lhs
 * @return index of matching key in lhs, -1 if not found.
*/
int lhs_find(const char *key)
{
    struct lhs_type *fs_ptr = (struct lhs_type *)lfind(key, lhs, &lhs_size, sizeof(struct lhs_type), &lhs_compare);
    return (fs_ptr) ? (fs_ptr - lhs) : -1;
}
