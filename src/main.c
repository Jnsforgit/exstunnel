
int main(int argc, char **argv)
{
    int i = 0;
    int exitcode = 0;
    struct rule *rp = NULL;
    struct lemon lem;

    static int version = 0;
    static int rpflag = 0;
    static int basisflag = 0;
    static int compress = 0;
    static int quiet = 0;
    static int statistics = 0;
    static int mhflag = 0;
    static int nolinenosflag = 0;
    static int noResort = 0;
    static int sqlFlag = 0;
    static int printPP = 0;

    static struct s_options options[] = 
    {
        {OPT_FLAG, "b", (char*)&basisflag, 0, "Print only the basis in report."},
        {OPT_FLAG, "c", (char*)&compress, 0, "Don't compress the action table."},
        {OPT_FSTR, "d", 0, handle_d_option, "Output directory.  Default '.'"},
        {OPT_FSTR, "D", 0, handle_D_option, "Define an %ifdef macro."},
        {OPT_FLAG, "E", (char*)&printPP, 0, "Print input file after preprocessing."},
        {OPT_FSTR, "f", 0, 0, "Ignored.  (Placeholder for -f compiler options.)"},
        {OPT_FLAG, "g", (char*)&rpflag, 0, "Print grammar without actions."},
        {OPT_FSTR, "I", 0, 0, "Ignored.  (Placeholder for '-I' compiler options.)"},
        {OPT_FLAG, "m", (char*)&mhflag, 0, "Output a makeheaders compatible file."},
        {OPT_FLAG, "l", (char*)&nolinenosflag, 0, "Do not print #line statements."},
        {OPT_FSTR, "O", 0, 0, "Ignored.  (Placeholder for '-O' compiler options.)"},
        {OPT_FLAG, "p", (char*)&showPrecedenceConflict, 0, "Show conflicts resolved by precedence rules"},
        {OPT_FLAG, "q", (char*)&quiet, 0, "(Quiet) Don't print the report file."},
        {OPT_FLAG, "r", (char*)&noResort, 0, "Do not sort or renumber states"},
        {OPT_FLAG, "s", (char*)&statistics, 0, "Print parser stats to standard output."},
        {OPT_FLAG, "S", (char*)&sqlFlag, 0, "Generate the *.sql file describing the parser tables."},
        {OPT_FLAG, "x", (char*)&version, 0, "Print the version number."},
        {OPT_FSTR, "T", 0, handle_T_option, "Specify a template file."},
        {OPT_FSTR, "W", 0, 0, "Ignored.  (Placeholder for '-W' compiler options.)"},
        {OPT_FLAG, 0,0,0,0}
    };

    (void)argc;
    OptInit(argv,options,stderr);
    if (version)
    {
        printf("Lemon version 1.0\n");
        exit(0);
    }

    if (OptNArgs() != 1)
    {
        fprintf(stderr,"Exactly one filename argument is required.\n");
        exit(1);
    }

    memset(&lem, 0, sizeof(lem));
    lem.errorcnt = 0;

    /* Initialize the machine */
    Strsafe_init();
    Symbol_init();
    State_init();
    lem.argv0 = argv[0];
    lem.filename = OptArg(0);
    lem.basisflag = basisflag;
    lem.nolinenosflag = nolinenosflag;
    lem.printPreprocessed = printPP;
    Symbol_new("$");

    /* Parse the input file */
    Parse(&lem);
    if( lem.printPreprocessed || lem.errorcnt )
    {
        exit(lem.errorcnt);
    }

    if( lem.nrule==0 )
    {
        fprintf(stderr,"Empty grammar.\n");
        exit(1);
    }

    lem.errsym = Symbol_find("error");

    /* Count and index the symbols of the grammar */
    Symbol_new("{default}");
    lem.nsymbol = Symbol_count();
    lem.symbols = Symbol_arrayof();
    for(i=0; i<lem.nsymbol; i++) 
    {
        lem.symbols[i]->index = i;
    }

    qsort(lem.symbols,lem.nsymbol,sizeof(struct symbol*), Symbolcmpp);
    for(i=0; i<lem.nsymbol; i++) 
    {
        lem.symbols[i]->index = i;
    }

    while( lem.symbols[i-1]->type==MULTITERMINAL )
    { 
        i--; 
    }

    assert( strcmp(lem.symbols[i-1]->name,"{default}")==0 );

    lem.nsymbol = i - 1;
    for (i=1; ISUPPER(lem.symbols[i]->name[0]); i++)
    {

    }

    lem.nterminal = i;

    /* Assign sequential rule numbers.  Start with 0.  Put rules that have no
    ** reduce action C-code associated with them last, so that the switch()
    ** statement that selects reduction actions will have a smaller jump table.
    */
    for (i = 0, rp = lem.rule; rp; rp = rp->next)
    {
        rp->iRule = rp->code ? i++ : -1;
    }

    lem.nruleWithAction = i;
    for (rp = lem.rule; rp; rp = rp->next)
    {
        if( rp->iRule<0 )
        {
            rp->iRule = i++;
        }
    }

    lem.startRule = lem.rule;
    lem.rule = Rule_sort(lem.rule);

    /* Generate a reprint of the grammar, if requested on the command line */
    if ( rpflag ) 
    {
        Reprint(&lem);
    }
    else
    {
        /* Initialize the size for all follow and first sets */
        SetSize(lem.nterminal+1);

        /* Find the precedence for every production rule (that has one) */
        FindRulePrecedences(&lem);

        /* Compute the lambda-nonterminals and the first-sets for every
        ** nonterminal */
        FindFirstSets(&lem);

        /* Compute all LR(0) states.  Also record follow-set propagation
        ** links so that the follow-set can be computed later */
        lem.nstate = 0;
        FindStates(&lem);
        lem.sorted = State_arrayof();

        /* Tie up loose ends on the propagation links */
        FindLinks(&lem);

        /* Compute the follow set of every reducible configuration */
        FindFollowSets(&lem);

        /* Compute the action tables */
        FindActions(&lem);

        /* Compress the action tables */
        if( compress==0 ) CompressTables(&lem);

        /* Reorder and renumber the states so that states with fewer choices
        ** occur at the end.  This is an optimization that helps make the
        ** generated parser tables smaller. */
        if( noResort==0 ) ResortStates(&lem);

        /* Generate a report of the parser generated.  (the "y.output" file) */
        if( !quiet ) ReportOutput(&lem);

        /* Generate the source code for the parser */
        ReportTable(&lem, mhflag, sqlFlag);

        /* Produce a header file for use by the scanner.  (This step is
        ** omitted if the "-m" option is used because makeheaders will
        ** generate the file for us.) */
        if( !mhflag ) ReportHeader(&lem);
    }

    if( statistics )
    {
        printf("Parser statistics:\n");
        stats_line("terminal symbols", lem.nterminal);
        stats_line("non-terminal symbols", lem.nsymbol - lem.nterminal);
        stats_line("total symbols", lem.nsymbol);
        stats_line("rules", lem.nrule);
        stats_line("states", lem.nxstate);
        stats_line("conflicts", lem.nconflict);
        stats_line("action table entries", lem.nactiontab);
        stats_line("lookahead table entries", lem.nlookaheadtab);
        stats_line("total table size (bytes)", lem.tablesize);
    }

    if( lem.nconflict > 0 )
    {
        fprintf(stderr,"%d parsing conflicts.\n",lem.nconflict);
    }

    /* return 0 on success, 1 on failure. */
    exitcode = ((lem.errorcnt > 0) || (lem.nconflict > 0)) ? 1 : 0;
    exit(exitcode);

    return (exitcode);
}