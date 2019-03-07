
/*
 * This is so the C preprocessor does not try to include multiple copies
 * of the header file if someone uses multiple #include directives.
 */
#ifndef _HASHTABLE_H_
#define _HASHTABLE_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

/*
 * This header file defines an interface to a generic hashtable.
 * It stores void* data, and uses two function, int (*) (void *) to
 * compute the hash and an int (*) (void *, void *) for equal true/false
 */

struct HashBucket {
  void *key;
  void *data;
  struct HashBucket *next;
};

typedef struct HashTable {
  unsigned int (*hashFunction) (void *);
  int (*equalFunction) (void *, void *);
  struct HashBucket **data;
  int size;
} HashTable;

extern HashTable *createHashTable(int size, unsigned int (*hashFunction)(void *),
			   int (*equalFunction)(void *, void *));

extern void insertData(HashTable *table, void *key, void *data);

extern void *findData(HashTable *table, void *key);

#endif

