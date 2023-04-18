
#include "common.h"

int showPrecedenceConflict = 0;

static char **g_argv = NULL;
static struct s_options *op = NULL;
static FILE *errstream = NULL;
static char *outputDir = NULL;
static int nDefine = 0;     /* Number of -D options on the command line */
static char **azDefine = 0; /* Name of the -D macros */
static char *user_templatename = NULL;
static char emsg[] = "Command line syntax error: ";

/*
** Return the index of the N-th non-switch argument.  Return -1
** if N is out of range.
*/
static int argindex(int n)
{
    int i;
    int dashdash = 0;
    if (g_argv != 0 && *g_argv != 0)
    {
        for (i = 1; g_argv[i]; i++)
        {
            if (dashdash || !ISOPT(g_argv[i]))
            {
                if (n == 0)
                    return i;
                n--;
            }
            if (strcmp(g_argv[i], "--") == 0)
                dashdash = 1;
        }
    }
    return -1;
}

/*
** Print the command line with a carrot pointing to the k-th character
** of the n-th field.
*/
static void errline(int n, int k, FILE *err)
{
    int spcnt, i;
    if (g_argv[0])
    {
        fprintf(err, "%s", g_argv[0]);
        spcnt = lemonStrlen(g_argv[0]) + 1;
    }
    else
    {
        spcnt = 0;
    }
    for (i = 1; i < n && g_argv[i]; i++)
    {
        fprintf(err, " %s", g_argv[i]);
        spcnt += lemonStrlen(g_argv[i]) + 1;
    }
    spcnt += k;
    for (; g_argv[i]; i++)
        fprintf(err, " %s", g_argv[i]);
    if (spcnt < 20)
    {
        fprintf(err, "\n%*s^-- here\n", spcnt, "");
    }
    else
    {
        fprintf(err, "\n%*shere --^\n", spcnt - 7, "");
    }
}

/*
** Process a command line switch which has an argument.
*/
static int handleswitch(int i, FILE *err)
{
    int lv = 0;
    double dv = 0.0;
    char *sv = 0, *end;
    char *cp;
    int j;
    int errcnt = 0;
    cp = strchr(g_argv[i], '=');
    assert(cp != 0);
    *cp = 0;
    for (j = 0; op[j].label; j++)
    {
        if (strcmp(g_argv[i], op[j].label) == 0)
            break;
    }
    *cp = '=';
    if (op[j].label == 0)
    {
        if (err)
        {
            fprintf(err, "%sundefined option.\n", emsg);
            errline(i, 0, err);
        }
        errcnt++;
    }
    else
    {
        cp++;
        switch (op[j].type)
        {
        case OPT_FLAG:
        case OPT_FFLAG:
            if (err)
            {
                fprintf(err, "%soption requires an argument.\n", emsg);
                errline(i, 0, err);
            }
            errcnt++;
            break;
        case OPT_DBL:
        case OPT_FDBL:
            dv = strtod(cp, &end);
            if (*end)
            {
                if (err)
                {
                    fprintf(err,
                            "%sillegal character in floating-point argument.\n", emsg);
                    errline(i, (int)((char *)end - (char *)g_argv[i]), err);
                }
                errcnt++;
            }
            break;
        case OPT_INT:
        case OPT_FINT:
            lv = strtol(cp, &end, 0);
            if (*end)
            {
                if (err)
                {
                    fprintf(err, "%sillegal character in integer argument.\n", emsg);
                    errline(i, (int)((char *)end - (char *)g_argv[i]), err);
                }
                errcnt++;
            }
            break;
        case OPT_STR:
        case OPT_FSTR:
            sv = cp;
            break;
        }
        switch (op[j].type)
        {
        case OPT_FLAG:
        case OPT_FFLAG:
            break;
        case OPT_DBL:
            *(double *)(op[j].arg) = dv;
            break;
        case OPT_FDBL:
            op[j].fn(dv);
            break;
        case OPT_INT:
            *(int *)(op[j].arg) = lv;
            break;
        case OPT_FINT:
            op[j].fn((int)lv);
            break;
        case OPT_STR:
            *(char **)(op[j].arg) = sv;
            break;
        case OPT_FSTR:
            op[j].fn(sv);
            break;
        }
    }
    return errcnt;
}

/*
** Process a flag command line argument.
*/
static int handleflags(int i, FILE *err)
{
    int v = 0;
    int j = 0;
    int errcnt = 0;

    for (j = 0; op[j].label; j++)
    {
        if (strncmp(&g_argv[i][1], op[j].label, lemonStrlen(op[j].label)) == 0)
        {
            break;
        }
    }

    v = g_argv[i][0] == '-' ? 1 : 0;
    if (op[j].label == 0)
    {
        if (err)
        {
            fprintf(err, "%sundefined option.\n", emsg);
            errline(i, 1, err);
        }
        errcnt++;
    }
    else if (op[j].arg == 0 && op[j].fn == 0)
    {
        /* Ignore this option */
    }
    else if (op[j].type == OPT_FLAG)
    {
        *((int *)op[j].arg) = v;
    }
    else if (op[j].type == OPT_FFLAG)
    {
        op[j].fn(v);
    }
    else if (op[j].type == OPT_FSTR)
    {
        op[j].fn(&g_argv[i][2]);
    }
    else
    {
        if (err)
        {
            fprintf(err, "%smissing argument on switch.\n", emsg);
            errline(i, 1, err);
        }
        errcnt++;
    }

    return errcnt;
}

char *OptArg(int n)
{
    int i;
    i = argindex(n);
    return i >= 0 ? g_argv[i] : 0;
}

int OptNArgs(void)
{
    int i = 0;
    int cnt = 0;
    int dashdash = 0;

    if (g_argv != 0 && g_argv[0] != 0)
    {
        for (i = 1; g_argv[i]; i++)
        {
            if (dashdash || !ISOPT(g_argv[i]))
            {
                cnt++;
            }

            if (strcmp(g_argv[i], "--") == 0)
            {
                dashdash = 1;
            }
        }
    }

    return cnt;
}

void OptErr(int n)
{
    int i;
    i = argindex(n);
    if (i >= 0)
    {
        errline(i, 0, errstream);
    }
}

void OptPrint(void)
{
    int i;
    int max, len;
    max = 0;
    for (i = 0; op[i].label; i++)
    {
        len = lemonStrlen(op[i].label) + 1;
        switch (op[i].type)
        {
        case OPT_FLAG:
        case OPT_FFLAG:
            break;
        case OPT_INT:
        case OPT_FINT:
            len += 9; /* length of "<integer>" */
            break;
        case OPT_DBL:
        case OPT_FDBL:
            len += 6; /* length of "<real>" */
            break;
        case OPT_STR:
        case OPT_FSTR:
            len += 8; /* length of "<string>" */
            break;
        }
        if (len > max)
            max = len;
    }
    for (i = 0; op[i].label; i++)
    {
        switch (op[i].type)
        {
        case OPT_FLAG:
        case OPT_FFLAG:
            fprintf(errstream, "  -%-*s  %s\n", max, op[i].label, op[i].message);
            break;
        case OPT_INT:
        case OPT_FINT:
            fprintf(errstream, "  -%s<integer>%*s  %s\n", op[i].label,
                    (int)(max - lemonStrlen(op[i].label) - 9), "", op[i].message);
            break;
        case OPT_DBL:
        case OPT_FDBL:
            fprintf(errstream, "  -%s<real>%*s  %s\n", op[i].label,
                    (int)(max - lemonStrlen(op[i].label) - 6), "", op[i].message);
            break;
        case OPT_STR:
        case OPT_FSTR:
            fprintf(errstream, "  -%s<string>%*s  %s\n", op[i].label,
                    (int)(max - lemonStrlen(op[i].label) - 8), "", op[i].message);
            break;
        }
    }
}

int OptInit(char **a, struct s_options *o, FILE *err)
{
    int errcnt = 0;
    g_argv = a;
    op = o;
    errstream = err;

    if (g_argv && *g_argv && op)
    {
        int i;
        for (i = 1; g_argv[i]; i++)
        {
            if (g_argv[i][0] == '+' || g_argv[i][0] == '-')
            {
                errcnt += handleflags(i, err);
            }
            else if (strchr(g_argv[i], '='))
            {
                errcnt += handleswitch(i, err);
            }
        }
    }

    if (errcnt > 0)
    {
        fprintf(err, "Valid command line options for \"%s\" are:\n", *a);
        OptPrint();
        exit(1);
    }

    return 0;
}

/* In spite of its name, this function is really a scanner.  It read
** in the entire input file (all at once) then tokenizes it.  Each
** token is passed to the function "parseonetoken" which builds all
** the appropriate data structures in the global state vector "gp".
*/
void Parse(struct lemon *gp)
{
    int c;
    int lineno;
    int startline = 0;
    unsigned int filesize;
    char *cp = NULL;
    char *nextcp = NULL;
    char *filebuf = NULL;
    FILE *fp = NULL;
    struct pstate ps;

    memset(&ps, '\0', sizeof(ps));
    ps.gp = gp;
    ps.filename = gp->filename;
    ps.errorcnt = 0;
    ps.state = INITIALIZE;

    /* Begin by reading the input file */
    fp = fopen(ps.filename, "rb");
    if (fp == 0)
    {
        ErrorMsg(ps.filename, 0, "Can't open this file for reading.");
        gp->errorcnt++;
        return;
    }
    fseek(fp, 0, 2);
    filesize = ftell(fp);
    rewind(fp);
    filebuf = (char *)malloc(filesize + 1);
    if (filesize > 100000000 || filebuf == 0)
    {
        ErrorMsg(ps.filename, 0, "Input file too large.");
        free(filebuf);
        gp->errorcnt++;
        fclose(fp);
        return;
    }
    if (fread(filebuf, 1, filesize, fp) != filesize)
    {
        ErrorMsg(ps.filename, 0, "Can't read in all %d bytes of this file.",
                 filesize);
        free(filebuf);
        gp->errorcnt++;
        fclose(fp);
        return;
    }
    fclose(fp);
    filebuf[filesize] = 0;

    /* Make an initial pass through the file to handle %ifdef and %ifndef */
    preprocess_input(filebuf);
    if (gp->printPreprocessed)
    {
        printf("%s\n", filebuf);
        return;
    }

    /* Now scan the text of the input file */
    lineno = 1;
    for (cp = filebuf; (c = *cp) != 0;)
    {
        if (c == '\n')
            lineno++; /* Keep track of the line number */
        if (ISSPACE(c))
        {
            cp++;
            continue;
        } /* Skip all white space */
        if (c == '/' && cp[1] == '/')
        { /* Skip C++ style comments */
            cp += 2;
            while ((c = *cp) != 0 && c != '\n')
                cp++;
            continue;
        }
        if (c == '/' && cp[1] == '*')
        { /* Skip C style comments */
            cp += 2;
            if ((*cp) == '/')
                cp++;
            while ((c = *cp) != 0 && (c != '/' || cp[-1] != '*'))
            {
                if (c == '\n')
                    lineno++;
                cp++;
            }
            if (c)
                cp++;
            continue;
        }
        ps.tokenstart = cp;      /* Mark the beginning of the token */
        ps.tokenlineno = lineno; /* Linenumber on which token begins */
        if (c == '\"')
        { /* String literals */
            cp++;
            while ((c = *cp) != 0 && c != '\"')
            {
                if (c == '\n')
                    lineno++;
                cp++;
            }
            if (c == 0)
            {
                ErrorMsg(ps.filename, startline,
                         "String starting on this line is not terminated before "
                         "the end of the file.");
                ps.errorcnt++;
                nextcp = cp;
            }
            else
            {
                nextcp = cp + 1;
            }
        }
        else if (c == '{')
        { /* A block of C code */
            int level;
            cp++;
            for (level = 1; (c = *cp) != 0 && (level > 1 || c != '}'); cp++)
            {
                if (c == '\n')
                    lineno++;
                else if (c == '{')
                    level++;
                else if (c == '}')
                    level--;
                else if (c == '/' && cp[1] == '*')
                { /* Skip comments */
                    int prevc;
                    cp = &cp[2];
                    prevc = 0;
                    while ((c = *cp) != 0 && (c != '/' || prevc != '*'))
                    {
                        if (c == '\n')
                            lineno++;
                        prevc = c;
                        cp++;
                    }
                }
                else if (c == '/' && cp[1] == '/')
                { /* Skip C++ style comments too */
                    cp = &cp[2];
                    while ((c = *cp) != 0 && c != '\n')
                        cp++;
                    if (c)
                        lineno++;
                }
                else if (c == '\'' || c == '\"')
                { /* String a character literals */
                    int startchar, prevc;
                    startchar = c;
                    prevc = 0;
                    for (cp++; (c = *cp) != 0 && (c != startchar || prevc == '\\'); cp++)
                    {
                        if (c == '\n')
                            lineno++;
                        if (prevc == '\\')
                            prevc = 0;
                        else
                            prevc = c;
                    }
                }
            }
            if (c == 0)
            {
                ErrorMsg(ps.filename, ps.tokenlineno,
                         "C code starting on this line is not terminated before "
                         "the end of the file.");
                ps.errorcnt++;
                nextcp = cp;
            }
            else
            {
                nextcp = cp + 1;
            }
        }
        else if (ISALNUM(c))
        { /* Identifiers */
            while ((c = *cp) != 0 && (ISALNUM(c) || c == '_'))
                cp++;
            nextcp = cp;
        }
        else if (c == ':' && cp[1] == ':' && cp[2] == '=')
        { /* The operator "::=" */
            cp += 3;
            nextcp = cp;
        }
        else if ((c == '/' || c == '|') && ISALPHA(cp[1]))
        {
            cp += 2;
            while ((c = *cp) != 0 && (ISALNUM(c) || c == '_'))
                cp++;
            nextcp = cp;
        }
        else
        { /* All other (one character) operators */
            cp++;
            nextcp = cp;
        }
        c = *cp;
        *cp = 0;            /* Null terminate the token */
        parseonetoken(&ps); /* Parse the token */
        *cp = (char)c;      /* Restore the buffer */
        cp = nextcp;
    }
    free(filebuf); /* Release the buffer after parsing */
    gp->rule = ps.firstrule;
    gp->errorcnt = ps.errorcnt;
}

/* Rember the name of the output directory
 */
void handle_d_option(char *z)
{
    outputDir = (char *)malloc(lemonStrlen(z) + 1);
    if (outputDir == 0)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    
    lemon_strcpy(outputDir, z);
}

/* This routine is called with the argument to each -D command-line option.
** Add the macro defined to the azDefine array.
*/
void handle_D_option(char *z)
{
    char **paz = NULL;

    nDefine++;
    azDefine = (char **)realloc(azDefine, sizeof(azDefine[0]) * nDefine);
    if (azDefine == 0)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    paz = &azDefine[nDefine - 1];
    *paz = (char *)malloc(lemonStrlen(z) + 1);
    if (*paz == 0)
    {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }

    lemon_strcpy(*paz, z);
    for (z = *paz; *z && *z != '='; z++)
    {

    }
    *z = 0;
}

void handle_T_option(char *z)
{
    user_templatename = (char *)malloc(lemonStrlen(z) + 1);
    if (user_templatename == 0)
    {
        memory_error();
    }

    lemon_strcpy(user_templatename, z);
}

/* Print an action to the given file descriptor.  Return FALSE if
** nothing was actually printed.
*/
int PrintAction(
    struct action *ap, /* The action to print */
    FILE *fp,          /* Print the action here */
    int indent         /* Indent by this amount */
)
{
    int result = 1;
    switch (ap->type)
    {
    case SHIFT:
    {
        struct state *stp = ap->x.stp;
        fprintf(fp, "%*s shift        %-7d", indent, ap->sp->name, stp->statenum);
        break;
    }
    case REDUCE:
    {
        struct rule *rp = ap->x.rp;
        fprintf(fp, "%*s reduce       %-7d", indent, ap->sp->name, rp->iRule);
        RulePrint(fp, rp, -1);
        break;
    }
    case SHIFTREDUCE:
    {
        struct rule *rp = ap->x.rp;
        fprintf(fp, "%*s shift-reduce %-7d", indent, ap->sp->name, rp->iRule);
        RulePrint(fp, rp, -1);
        break;
    }
    case ACCEPT:
        fprintf(fp, "%*s accept", indent, ap->sp->name);
        break;
    case ERROR:
        fprintf(fp, "%*s error", indent, ap->sp->name);
        break;
    case SRCONFLICT:
    case RRCONFLICT:
        fprintf(fp, "%*s reduce       %-7d ** Parsing conflict **",
                indent, ap->sp->name, ap->x.rp->iRule);
        break;
    case SSCONFLICT:
        fprintf(fp, "%*s shift        %-7d ** Parsing conflict **",
                indent, ap->sp->name, ap->x.stp->statenum);
        break;
    case SH_RESOLVED:
        if (showPrecedenceConflict)
        {
            fprintf(fp, "%*s shift        %-7d -- dropped by precedence",
                    indent, ap->sp->name, ap->x.stp->statenum);
        }
        else
        {
            result = 0;
        }
        break;
    case RD_RESOLVED:
        if (showPrecedenceConflict)
        {
            fprintf(fp, "%*s reduce %-7d -- dropped by precedence",
                    indent, ap->sp->name, ap->x.rp->iRule);
        }
        else
        {
            result = 0;
        }
        break;
    case NOT_USED:
        result = 0;
        break;
    }
    if (result && ap->spOpt)
    {
        fprintf(fp, "  /* because %s==%s */", ap->sp->name, ap->spOpt->name);
    }
    return result;
}

