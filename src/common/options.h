#ifndef OPTIONS_H
#define OPTIONS_H

extern int showPrecedenceConflict;

enum option_type
{
    OPT_FLAG = 1,
    OPT_INT,
    OPT_DBL,
    OPT_STR,
    OPT_FFLAG,
    OPT_FINT,
    OPT_FDBL,
    OPT_FSTR
};

struct s_options
{
    enum option_type type;
    const char *label;
    char *arg;
    void (*fn)();
    const char *message;
};

struct pstate
{
    char *filename;             /* Name of the input file */
    int tokenlineno;            /* Linenumber at which current token starts */
    int errorcnt;               /* Number of errors so far */
    char *tokenstart;           /* Text of current token */
    struct lemon *gp;           /* Global state vector */
    enum e_state state;         /* The state of the parser */
    struct symbol *fallback;    /* The fallback token */
    struct symbol *tkclass;     /* Token class symbol */
    struct symbol *lhs;         /* Left-hand side of current rule */
    const char *lhsalias;       /* Alias for the LHS */
    int nrhs;                   /* Number of right-hand side symbols seen */
    struct symbol *rhs[MAXRHS]; /* RHS symbols */
    const char *alias[MAXRHS];  /* Aliases for each RHS symbol (or NULL) */
    struct rule *prevrule;      /* Previous rule parsed */
    const char *declkeyword;    /* Keyword of a declaration */
    char **declargslot;         /* Where the declaration argument should be put */
    int insertLineMacro;        /* Add #line before declaration insert */
    int *decllinenoslot;        /* Where to write declaration line number */
    enum e_assoc declassoc;     /* Assign this association to decl arguments */
    int preccounter;            /* Assign this precedence to decl arguments */
    struct rule *firstrule;     /* Pointer to first rule in the grammar */
    struct rule *lastrule;      /* Pointer to the most recently parsed rule */
};

char *OptArg(int n);
int OptNArgs(void);
int OptInit(char **a, struct s_options *o, FILE *err);

void handle_d_option(char *z);
void handle_D_option(char *z);
void handle_T_option(char *z);

#endif