
#ifndef UTILS_H
#define UTILS_H

void Strsafe_init(void);
void Symbol_init(void);
void State_init(void);
struct symbol *Symbol_new(const char *x);
struct symbol *Symbol_find(const char *key);
int Symbol_count();
struct symbol **Symbol_arrayof();
static struct rule *Rule_sort(struct rule *rp);
void Reprint(struct lemon *lemp);
void SetSize(int n);
void FindRulePrecedences(struct lemon *xp);
void FindFirstSets(struct lemon *lemp);
void FindStates(struct lemon *lemp);
struct state **State_arrayof(void);
void FindLinks(struct lemon *lemp);
void FindFollowSets(struct lemon *lemp);
void FindActions(struct lemon *lemp);
void CompressTables(struct lemon *lemp);
void ResortStates(struct lemon *lemp);
void ReportTable(
    struct lemon *lemp,
    int mhflag, /* Output in makeheaders format if true */
    int sqlFlag /* Generate the *.sql file too */
);
void ReportHeader(struct lemon *lemp);
void stats_line(const char *zLabel, int iValue);
int Symbolcmpp(const void *_a, const void *_b);

#endif
