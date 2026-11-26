#ifndef ENGINE_TYPES_H
#define ENGINE_TYPES_H

#define MAX_LINE 4096
#define MAX_TOKEN 128
#define MAX_QUERY_TERMS 128
#define DEFAULT_TOP_K 10

typedef struct Posting {
    int doc_id;
    int frequency;
    struct Posting *next;
} Posting;

typedef struct TermNode {
    char *term;
    Posting *postings;
    struct TermNode *next;
} TermNode;

typedef struct Document {
    int id;
    char *path;
    long total_terms;
    long unique_terms;
    struct Document *next;
} Document;

typedef struct {
    Document *documents;
    TermNode *index;
    int document_count;
    long total_terms;
    long posting_count;
    int unique_terms;
} SearchEngine;

typedef struct {
    int doc_id;
    long score;
    char matched[1024];
} SearchResult;

#endif
