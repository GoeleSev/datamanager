#include "dplist.h"
#include <stddef.h>  // voor NULL
#include <stdbool.h> // voor bool

dplist_t *dpl_create(
    void *(*element_copy)(void *),
    void (*element_free)(void **),
    int (*element_compare)(void *, void *)
) {
    (void) element_copy;
    (void) element_free;
    (void) element_compare;
    return NULL; // placeholder
}

void dpl_free(dplist_t **list, bool free_element) {
    (void) list;
    (void) free_element;
}
