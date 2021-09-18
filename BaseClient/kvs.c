#include "iostream"
#include "util.h"
#include "stdio.h"
#include "string.h"
#include "time.h"

using namespace std;

static const size_t kvs_pair_size = sizeof(CybsTable);

static const size_t kvs_store_size = sizeof(CybsMap);

static int kvs_sort_compare(const void *a, const void *b) {
	const CybsTable *pairA = (const CybsTable*) a;
	const CybsTable *pairB = (const CybsTable*) b;

	const char *x = (const char *)pairA->key;
	const char *y = (const char *)pairB->key;
	return strcmp(x, y);
}

static int search_compare(const void *key, const void *element) {
    const CybsTable *pair = (const CybsTable*) element;

    if (strcmp((const char*)key, (const char *)pair->key) > 0) {
        return 1;
    }
    if (strcmp((const char*)key, (const char *)pair->key) < 0) {
        return -1;
    }
    return 0;
}

static CybsTable *get_pair(CybsMap *store, const void *key) {
    if ((!store) || (!store->pairs)) {
        return NULL;
    }
    return (CybsTable *)bsearch(key, store->pairs, store->length, kvs_pair_size,
                    search_compare);
}

static void sort_pairs(CybsMap *store) {
    if ((!store) || (!store->pairs)) {
        return;
    }
    qsort(store->pairs, store->length, kvs_pair_size, kvs_sort_compare);
}

static void resize_pairs(CybsMap *store) {
    if (!store) {
        return;
    }
	store->pairs = (CybsTable*)realloc(store->pairs, kvs_pair_size * store->length);
}

static void mem_alloc_error()
{
    fprintf(stderr, "Failed to allocate memory");
}

static void create_pair(CybsMap *store, const void *key, void *value) {
	CybsTable *pair;
    if (!store) {
        return;
    }
	string keyCopy((char *)key);
    string valueCopy((char *)value);
    ++store->length;
	store->totallength = store->totallength + (keyCopy.size()) + valueCopy.size();
    resize_pairs(store);
    pair = &store->pairs[store->length - 1];
	pair->key = (char *) malloc(keyCopy.size() + sizeof(char));
	keyCopy.copy((char *)pair->key, keyCopy.size(), 0);
	((char *)pair->key)[keyCopy.size()]='\0';
    //pair->key = key;
    //pair->value = value;
	pair->value = (char *) malloc(valueCopy.size() + sizeof(char));
        if(!pair->value) {
           mem_alloc_error();
           exit(0);
        }
	valueCopy.copy((char *)pair->value, valueCopy.size(), 0);
	((char *) pair->value)[valueCopy.size()]='\0';
    sort_pairs(store);
}

static void remove_pair(CybsMap *store, CybsTable *pair) {
    if ((!store) || (!pair)) {
        return;
    }
    sort_pairs(store);
    --store->length;
	string pairKeyCopy((char *)pair->key);
	string pairKeyValue((char *)pair->value);
	store->totallength = store->totallength - (pairKeyCopy.size() + pairKeyValue.size());
    resize_pairs(store);
}

CybsMap *cybs_create_map(void) {
	CybsMap *store = (CybsMap *)malloc(kvs_store_size);
    if(!store) {
       mem_alloc_error();
       exit(0);
    }
    store->pairs = NULL;
    store->length = 0;
	store->totallength = 0;
    return store;
}

void cybs_destroy_map(CybsMap *store) {
    if (!store) {
        return;
    }
	int i = 0;
	CybsTable pair;
	for (i = 0; i < store->length; i++) {
		pair = store->pairs[i];
		free ((void *)pair.key);
		free (pair.value);
	}

	free(store->pairs);
	store->length = 0;
	store->totallength = 0;
    free(store);
}

char *cybs_strdup( const char * szStringToDup )
{
	string szStringToDupCopy(szStringToDup);
	char *szDup
		= (char *) malloc( szStringToDupCopy.size() + sizeof( char ) );

	if (szDup)
	{
		szStringToDupCopy.copy(szDup, szStringToDupCopy.size(), 0);
		szDup[szStringToDupCopy.size()]='\0';
		return( szDup );
	}

	return( 0 );
}

void cybs_add(CybsMap *store, const void *key, void *value) {
	char *szNameDup = 0, *szValueDup = 0;
	CybsTable *pair = get_pair(store, key);
    if (pair) {
		if (value) {
			free (pair->value);
			string valueCopy((char *)value);
			pair->value = (char *) malloc(valueCopy.size() + sizeof(char));
                        if(!pair->value) {
                            mem_alloc_error();
                            exit(0);
                        }                       
			valueCopy.copy((char *) pair->value, valueCopy.size(), 0);
			((char *) pair->value)[valueCopy.size()]='\0';
        } else {
            remove_pair(store, pair);
        }
    } else if (value) {
		szNameDup = cybs_strdup( (const char *)key );
		szValueDup = cybs_strdup( (const char *)value );
        create_pair(store, szNameDup, szValueDup);
		free(szNameDup);
		free(szValueDup);
    }
}

void *cybs_get(CybsMap *store, const void *key) {
	CybsTable *pair = get_pair(store, key);
    return pair ? pair->value : NULL;
}

