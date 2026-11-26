#include <stdlib.h>
#include <string.h>
#include "index.h"

char *duplicate_string(const char *s) {
    size_t n = strlen(s) + 1;
    char *p = (char *)malloc(n);
    if (p) memcpy(p, s, n);
    return p;
}

void init_engine(SearchEngine *e) {
    memset(e, 0, sizeof(*e));
}

void free_engine(SearchEngine *e) {
    Document *d = e->documents;
    while (d) {
        Document *next = d->next;
        free(d->path);
        free(d);
        d = next;
    }
    TermNode *t = e->index;
    while (t) {
        TermNode *tn = t->next;
        Posting *p = t->postings;
        while (p) {
            Posting *pn = p->next;
            free(p);
            p = pn;
        }
        free(t->term);
        free(t);
        t = tn;
    }
    init_engine(e);
}

TermNode *find_term(SearchEngine *e, const char *word) {
    TermNode *t;
    for (t = e->index; t; t = t->next) {
        if (strcmp(t->term, word) == 0) return t;
    }
    return NULL;
}

TermNode *get_or_create_term(SearchEngine *e, const char *word) {
    TermNode *t = find_term(e, word);
    if (t) return t;

    t = (TermNode *)malloc(sizeof(*t));
    if (!t) return NULL;
    memset(t, 0, sizeof(*t));
    t->term = duplicate_string(word);
    if (!t->term) {
        free(t);
        return NULL;
    }
    t->next = e->index;
    e->index = t;
    e->unique_terms++;
    return t;
}

int add_occurrence(SearchEngine *e, int doc_id, const char *word) {
    TermNode *t = get_or_create_term(e, word);
    Posting *p;
    if (!t) return 0;

    for (p = t->postings; p; p = p->next) {
        if (p->doc_id == doc_id) {
            p->frequency++;
            return 1;
        }
    }
    p = (Posting *)malloc(sizeof(*p));
    if (!p) return 0;
    memset(p, 0, sizeof(*p));
    p->doc_id = doc_id;
    p->frequency = 1;
    p->next = t->postings;
    t->postings = p;
    e->posting_count++;

    for (Document *d = e->documents; d; d = d->next) {
        if (d->id == doc_id) {
            d->unique_terms++;
            break;
        }
    }
    return 1;
}
