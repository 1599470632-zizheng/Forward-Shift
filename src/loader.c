#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "loader.h"
#include "index.h"

Document *find_document(SearchEngine *e, int id) {
    Document *d;
    for (d = e->documents; d; d = d->next) {
        if (d->id == id) return d;
    }
    return NULL;
}

int is_word_char(unsigned char c) {
    return isalpha(c) || c == '\'';
}

static void trim_apostrophes(char *token) {
    size_t len = strlen(token), start = 0;
    while (start < len && token[start] == '\'') start++;
    while (len > start && token[len - 1] == '\'') len--;
    if (start > 0) memmove(token, token + start, len - start);
    token[len - start] = '\0';
}

static int process_token(SearchEngine *e, Document *d,
                         char *token, size_t *len) {
    if (*len == 0) return 1;
    token[*len] = '\0';
    trim_apostrophes(token);
    if (token[0] == '\0') {
        (*len) = 0;
        return 1;
    }
    if (!add_occurrence(e, d->id, token)) return 0;
    d->total_terms++;
    (*len) = 0;
    return 1;
}

int index_document(SearchEngine *e, Document *d) {
    FILE *fp = fopen(d->path, "r");
    int c;
    char token[MAX_TOKEN];
    size_t len = 0;
    if (!fp) {
        printf("Warning: cannot open %s; skipped.\n", d->path);
        return 0;
    }
    while ((c = fgetc(fp)) != EOF) {
        unsigned char ch = (unsigned char)c;
        if (is_word_char(ch)) {
            if (len < MAX_TOKEN - 1) {
                token[len++] = (char)tolower(ch);
            }
        } else {
            if (!process_token(e, d, token, &len)) {
                fclose(fp);
                return 0;
            }
        }
    }
    if (!process_token(e, d, token, &len)) {
        fclose(fp);
        return 0;
    }
    fclose(fp);
    e->total_terms += d->total_terms;
    return 1;
}

int load_collection(SearchEngine *e, const char *manifest) {
    FILE *fp = fopen(manifest, "r");
    char line[MAX_LINE];
    int next_id = 1;
    if (!fp) {
        printf("Cannot open manifest file: %s\n", manifest);
        return 0;
    }
    while (fgets(line, sizeof(line), fp)) {
        size_t n = strlen(line);
        while (n && (line[n - 1] == '\n' || line[n - 1] == '\r')) {
            line[--n] = '\0';
        }
        if (n == 0 || line[0] == '#') continue;

        Document *d = (Document *)malloc(sizeof(*d));
        if (!d) {
            fclose(fp);
            return 0;
        }
        memset(d, 0, sizeof(*d));
        d->id = next_id++;
        d->path = duplicate_string(line);
        if (!d->path) {
            free(d);
            fclose(fp);
            return 0;
        }
        d->next = e->documents;
        e->documents = d;
        e->document_count++;
    }
    fclose(fp);

    Document *rev = NULL, *cur = e->documents;
    while (cur) {
        Document *next = cur->next;
        cur->next = rev;
        rev = cur;
        cur = next;
    }
    e->documents = rev;

    for (cur = e->documents; cur; cur = cur->next) {
        index_document(e, cur);
    }
    return 1;
}

static void add_query_term(char terms[][MAX_TOKEN], int *count, char *token) {
    trim_apostrophes(token);
    if (token[0] == '\0' || *count >= MAX_QUERY_TERMS) return;
    for (int j = 0; j < *count; j++) {
        if (strcmp(terms[j], token) == 0) return;
    }
    strcpy(terms[(*count)++], token);
}

int tokenize_query(const char *query, char terms[][MAX_TOKEN]) {
    int count = 0;
    char token[MAX_TOKEN];
    size_t len = 0;
    for (size_t i = 0; query[i] != '\0'; i++) {
        unsigned char c = (unsigned char)query[i];
        if (is_word_char(c)) {
            if (len < MAX_TOKEN - 1) {
                token[len++] = (char)tolower(c);
            }
        } else if (len > 0) {
            token[len] = '\0';
            add_query_term(terms, &count, token);
            len = 0;
        }
    }
    if (len > 0) {
        token[len] = '\0';
        add_query_term(terms, &count, token);
    }
    return count;
}
