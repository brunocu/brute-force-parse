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
#define hist_curr (hist[hist_top])

#define SENT_MAX 64
char sent[SENT_MAX];
size_t sent_sz = 0;
#define sent_top (sent_sz - 1)
#define sent_tchar (sent[sent_top])

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
void sent_insert(const char *);
void print_state(int, const char*);

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
    //print_lhs();
    int result = parse(argv[1]);
    if(result == 0){
        puts("String has been accepted");
    }else{
        //Ñao ñao amigao
        puts("String has not been accepted");
    }
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
    size_t t_len = strlen(T);
    // \a is a stand in for empty character
    hist[0] = (struct hist_type){
        .symb = '\a',
        .p_num = 0,
    };
    // Using \0 as end marker
    memcpy(sent, (char[2]){'\0', lhs[0].nt}, 2 * sizeof(char));
    sent_sz += 2;
    int loop = 1;
    /// 2. loop
    while (1)
    {
        print_state(idx, T);
        /// 3. get stack-top elements and determine configuration
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
                            return EXIT_FAILURE;
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
        size_t rhs_idx;
        size_t rhs_len;
        printf("%d. Using case %d \n", loop++ , conf_case);
        switch (conf_case)
        {
        case 1:
            //Tree expansion (First production of a nonterminal)
            hist_sz++;
            hist_curr.p_num = 1;
            hist_curr.symb = sent_tchar;
            rhs_idx = lhs[lhs_find(&sent_tchar)].first;
            sent_sz--;
            sent_insert(rhs[rhs_idx]);
            break;
        case 2:
            //Input symbol match (Advance the index into the input string)
            hist_sz++;
            hist_curr.p_num = 0;
            hist_curr.symb = sent_tchar;
            idx++;
            sent_sz--;
            break;
        case 3:
            //Terminate (Successful parse has been performed)
            state = TERMINATION;
            return EXIT_SUCCESS;
        case 4:
            //Input symbol mismatch (A generated substring failed to match the input)
            state = BACKTRACK;
            break;
        case 5:
            // Backtrack on input
            idx--;
            sent[sent_sz++] = hist_curr.symb;
            hist_sz--;
            break;
        case 6:
            //6a. Next alternate, replace with next production
            state = PARSE;
            rhs_idx = lhs[lhs_find(&(hist_curr.symb))].first + (hist_curr.p_num - 1);
            rhs_len = strlen(rhs[rhs_idx]);
            sent_sz -= rhs_len;
            hist_curr.p_num++;
            sent_insert(rhs[rhs_idx + 1]);
            break;
        case 7:
            //6c. the alternates for A has been exhausted; remove Aj)
            rhs_idx = lhs[lhs_find(&(hist_curr.symb))].first + (hist_curr.p_num - 1);
            rhs_len = strlen(rhs[rhs_idx]);
            sent_sz -= rhs_len;
            sent[sent_sz++] = hist_curr.symb;
            hist_sz--;
            break;
        default:
            //You shouldnt be here
            return EXIT_FAILURE;
        }
    }
    
    return 0;
}

int lhs_compare(const char *key, const struct lhs_type *elem)
{
    // returns 0 if equal
    return !(*key == elem->nt);
}

void sent_insert(const char *key){
    size_t key_len = strlen(key);
    //size_t j = 0;
    for (int i = key_len - 1; i >= 0; i--)
    {
        sent[sent_sz++] = key[i];
    }
}

void print_state(int idx, const char* key){
    char state_c;
    switch (state)
    {
    case PARSE:
        state_c = 'q';
        break;
    case BACKTRACK:
        state_c = 'b';
        break;
    case TERMINATION:
        state_c = 't';
        break;
    }
    printf("[ %c ; %d ; ", state_c, idx);
    for (size_t i = 0; i < hist_sz; i++)
    {
        if( hist[i].p_num != 0){
            printf("%c%d ", hist[i].symb, hist[i].p_num);
        }else{
            printf("%c ", hist[i].symb);
        }
        
    }
    printf("; ");
    for (int i = sent_top; i > 0; i--)
    {
        printf("%c", sent[i]);
    }
    printf(" ] %s \n", (key+idx));
    
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
