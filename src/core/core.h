
#ifndef CORE_H
#define CORE_H

enum symbol_type
{
    TERMINAL,
    NONTERMINAL,
    MULTITERMINAL
};

struct state
{
    struct config *bp;        /* The basis configurations for this state */
    struct config *cfp;       /* All configurations in this set */
    int statenum;             /* Sequential number for this state */
    struct action *ap;        /* List of actions for this state */
    int nTknAct, nNtAct;      /* Number of actions on terminals and nonterminals */
    int iTknOfst, iNtOfst;    /* yy_action[] offset for terminals and nonterms */
    int iDfltReduce;          /* Default action is to REDUCE by this rule */
    struct rule *pDfltReduce; /* The default REDUCE rule. */
    int autoReduce;           /* True if this is an auto-reduce state */
};

struct symbol
{
    const char *name;        /* Name of the symbol */
    int index;               /* Index number for this symbol */
    enum symbol_type type;   /* Symbols are all either TERMINALS or NTs */
    struct rule *rule;       /* Linked list of rules of this (if an NT) */
    struct symbol *fallback; /* fallback token in case this token doesn't parse */
    int prec;                /* Precedence if defined (-1 otherwise) */
    enum e_assoc assoc;      /* Associativity if precedence is defined */
    char *firstset;          /* First-set for all rules of this symbol */
    Boolean lambda;          /* True if NT and can generate an empty string */
    int useCnt;              /* Number of times used */
    char *destructor;        /* Code which executes whenever this symbol is
                             ** popped from the stack during error processing */
    int destLineno;          /* Line number for start of destructor.  Set to
                             ** -1 for duplicate destructors. */
    char *datatype;          /* The data type of information held by this
                             ** object. Only used if type==NONTERMINAL */
    int dtnum;               /* The data type number.  In the parser, the value
                             ** stack is a union.  The .yy%d element of this
                             ** union is the correct data type for this object */
    int bContent;            /* True if this symbol ever carries content - if
                             ** it is ever more than just syntax */
    /* The following fields are used by MULTITERMINALs only */
    int nsubsym;            /* Number of constituent symbols in the MULTI */
    struct symbol **subsym; /* Array of constituent symbols */
};

struct rule
{
    struct symbol *lhs;     /* Left-hand side of the rule */
    const char *lhsalias;   /* Alias for the LHS (NULL if none) */
    int lhsStart;           /* True if left-hand side is the start symbol */
    int ruleline;           /* Line number for the rule */
    int nrhs;               /* Number of RHS symbols */
    struct symbol **rhs;    /* The RHS symbols */
    const char **rhsalias;  /* An alias for each RHS symbol (NULL if none) */
    int line;               /* Line number at which code begins */
    const char *code;       /* The code executed when this rule is reduced */
    const char *codePrefix; /* Setup code before code[] above */
    const char *codeSuffix; /* Breakdown code after code[] above */
    struct symbol *precsym; /* Precedence symbol for this rule */
    int index;              /* An index number for this rule */
    int iRule;              /* Rule number as used in the generated tables */
    Boolean noCode;         /* True if this rule has no associated C code */
    Boolean codeEmitted;    /* True if the code has been emitted already */
    Boolean canReduce;      /* True if this rule is ever reduced */
    Boolean doesReduce;     /* Reduce actions occur after optimization */
    Boolean neverReduce;    /* Reduce is theoretically possible, but prevented
                            ** by actions or other outside implementation */
    struct rule *nextlhs;   /* Next rule with the same LHS */
    struct rule *next;      /* Next rule in the global list */
};

struct lemon
{
    struct state **sorted;   /* Table of states sorted by state number */
    struct rule *rule;       /* List of all rules */
    struct rule *startRule;  /* First rule */
    int nstate;              /* Number of states */
    int nxstate;             /* nstate with tail degenerate states removed */
    int nrule;               /* Number of rules */
    int nruleWithAction;     /* Number of rules with actions */
    int nsymbol;             /* Number of terminal and nonterminal symbols */
    int nterminal;           /* Number of terminal symbols */
    int minShiftReduce;      /* Minimum shift-reduce action value */
    int errAction;           /* Error action value */
    int accAction;           /* Accept action value */
    int noAction;            /* No-op action value */
    int minReduce;           /* Minimum reduce action */
    int maxAction;           /* Maximum action value of any kind */
    struct symbol **symbols; /* Sorted array of pointers to symbols */
    int errorcnt;            /* Number of errors */
    struct symbol *errsym;   /* The error symbol */
    struct symbol *wildcard; /* Token that matches anything */
    char *name;              /* Name of the generated parser */
    char *arg;               /* Declaration of the 3rd argument to parser */
    char *ctx;               /* Declaration of 2nd argument to constructor */
    char *tokentype;         /* Type of terminal symbols in the parser stack */
    char *vartype;           /* The default type of non-terminal symbols */
    char *start;             /* Name of the start symbol for the grammar */
    char *stacksize;         /* Size of the parser stack */
    char *include;           /* Code to put at the start of the C file */
    char *error;             /* Code to execute when an error is seen */
    char *overflow;          /* Code to execute on a stack overflow */
    char *failure;           /* Code to execute on parser failure */
    char *accept;            /* Code to execute when the parser excepts */
    char *extracode;         /* Code appended to the generated file */
    char *tokendest;         /* Code to execute to destroy token data */
    char *vardest;           /* Code for the default non-terminal destructor */
    char *filename;          /* Name of the input file */
    char *outname;           /* Name of the current output file */
    char *tokenprefix;       /* A prefix added to token names in the .h file */
    int nconflict;           /* Number of parsing conflicts */
    int nactiontab;          /* Number of entries in the yy_action[] table */
    int nlookaheadtab;       /* Number of entries in yy_lookahead[] */
    int tablesize;           /* Total table size of all tables in bytes */
    int basisflag;           /* Print only basis configurations */
    int printPreprocessed;   /* Show preprocessor output on stdout */
    int has_fallback;        /* True if any %fallback is seen in the grammar */
    int nolinenosflag;       /* True if #line statements should not be printed */
    char *argv0;             /* Name of the program */
};

#endif