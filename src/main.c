#include <stdio.h>
#include "engine_types.h"
#include "index.h"
#include "loader.h"

static void show_statistics(SearchEngine *e) {
    Document *d;
    printf("\n--- Collection Statistics ---\n");
    printf("Documents loaded: %d\n", e->document_count);
    printf("Total indexed terms: %ld\n", e->total_terms);
    printf("Unique indexed terms: %d\n", e->unique_terms);
    printf("Posting relationships: %ld\n", e->posting_count);
    printf("\nPer-document statistics:\n");
    for (d = e->documents; d; d = d->next) {
        printf("ID %d | %-40s | %ld terms (%ld unique)\n",
               d->id, d->path, d->total_terms, d->unique_terms);
    }
}

static void view_document(SearchEngine *e) {
    int id, ch;
    char line[MAX_LINE];
    printf("Enter document ID: ");
    if (scanf("%d", &id) != 1) {
        while ((ch = getchar()) != '\n' && ch != EOF) {}
        printf("Invalid document ID.\n");
        return;
    }
    while ((ch = getchar()) != '\n' && ch != EOF) {}

    Document *d = find_document(e, id);
    if (!d) {
        printf("Document not found.\n");
        return;
    }
    FILE *fp = fopen(d->path, "r");
    if (!fp) {
        printf("Cannot open document.\n");
        return;
    }
    printf("\n--- %s ---\n", d->path);
    while (fgets(line, sizeof(line), fp)) {
        fputs(line, stdout);
    }
    fclose(fp);
    printf("\n");
}

int main(void) {
    SearchEngine engine;
    int choice, ch;
    init_engine(&engine);

    printf("===============================================\n");
    printf(" Document Search and Information Retrieval\n");
    printf("===============================================\n");
    printf("Loading collection from documents.txt...\n");
    if (!load_collection(&engine, "documents.txt")) {
        printf("Create documents.txt and list one .txt path per line.\n");
        free_engine(&engine);
        return 1;
    }
    printf("Loaded %d document entries; index contains %d unique terms.\n",
           engine.document_count, engine.unique_terms);

    do {
        printf("\n========== MAIN MENU ==========\n");
        printf("1. Search documents (In Progress)\n");
        printf("2. Show collection and index statistics\n");
        printf("3. View a document\n");
        printf("4. Exit\n");
        printf("Select an option: ");
        if (scanf("%d", &choice) != 1) {
            while ((ch = getchar()) != '\n' && ch != EOF) {}
            printf("Invalid input. Enter a number from 1 to 4.\n");
            continue;
        }
        while ((ch = getchar()) != '\n' && ch != EOF) {}

        switch (choice) {
            case 1:
                printf("\n[Notice] Search and ranking module is under development (scheduled for next week).\n");
                break;
            case 2:
                show_statistics(&engine);
                break;
            case 3:
                view_document(&engine);
                break;
            case 4:
                printf("Goodbye.\n");
                break;
            default:
                printf("Invalid choice. Please select 1-4.\n");
        }
    } while (choice != 4);

    free_engine(&engine);
    return 0;
}
