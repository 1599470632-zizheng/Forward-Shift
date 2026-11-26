#ifndef LOADER_H
#define LOADER_H

#include "engine_types.h"

Document *find_document(SearchEngine *e, int id);
int is_word_char(unsigned char c);
int index_document(SearchEngine *e, Document *d);
int load_collection(SearchEngine *e, const char *manifest);
int tokenize_query(const char *query, char terms[][MAX_TOKEN]);

#endif
