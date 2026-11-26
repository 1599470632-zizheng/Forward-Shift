#ifndef INDEX_H
#define INDEX_H

#include "engine_types.h"

char *duplicate_string(const char *s);
void init_engine(SearchEngine *e);
void free_engine(SearchEngine *e);
TermNode *find_term(SearchEngine *e, const char *word);
TermNode *get_or_create_term(SearchEngine *e, const char *word);
int add_occurrence(SearchEngine *e, int doc_id, const char *word);

#endif
