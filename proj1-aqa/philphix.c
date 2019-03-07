
#include "hashtable.h"
#include "philphix.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

HashTable *dictionary;

int main(int argc, char **argv){
  if(argc != 2){
    /* Note, you can print everything you want to standard error, it will be
       ignored by the autograder */
    fprintf(stderr, "Specify a dictionary\n");
    return 0;
  }

  fprintf(stderr, "Creating hashtable\n");
  dictionary = createHashTable(2255, &stringHash, &stringEquals);

  fprintf(stderr, "Loading dictionary %s\n", argv[1]);
  readDictionary(argv[1]);
  fprintf(stderr, "Dictionary loaded\n");

  fprintf(stderr, "Processing stdin\n");
  processInput();

  return 0;
}

/** You need to implement this function.
* Define a hash function, referenced from djb2 by Dan Bernstein */
unsigned int stringHash(void *s){
  char *string = (void *) s;
  unsigned int hash = 5381;
  int c;
  while ((c = *string++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

/** You need to implement this function.
* Checks if two strings are equal. Returns 1 is equal, 0 if not equal. */
int stringEquals(void *s1, void *s2){
  char *string1 = (char *) s1;
  char *string2 = (char *) s2;
  if (strcmp(string1, string2) == 0) {
    return 1;
  } else {
    return 0;
  }

}
/** You need to implement this function.
* This function should read in every word in the dictionary and store it in the
* dictionary. You should first open the file specified, then read the content
* and insert them into the dictionary (use `insertData` function defined in
* `hashtable.h`). As described in the specs, you can initially assume that no
* word is longer than 60 characters.  However, for the final 30% of your grade,
* you cannot assumed that words have a bounded length.*/
void readDictionary(char *name) {
    FILE * input = fopen(name, "r");
    if (input != NULL) {
        int c = fgetc(input);
        while (c != EOF) {
            int size = 60;
            int length = 0;
            char *key = malloc(sizeof(char) * size + 1);
            char *value = malloc(sizeof(char) * size + 1);
            while ((char) c != ' '
                   && isalnum((char) c)) {
                if (length >= size) {
                    size *= 2;
                    key = realloc(key, sizeof(char) * size);
                }
                key[length] = (char) c;
                c = fgetc(input);
                length++;
            }
            c = fgetc(input);
            key[length] = '\0';
            length = 0;
            size = 60;
            while ((char) c != '\n'
                   && c != EOF
                    && isprint((char) c)) {
                if (length >= size) {
                    size *= 2;
                    value = realloc(value, sizeof(char) * size);
                }
                value[length] = (char) c;
                c = fgetc(input);
                length++;
            }
            value[length] = '\0';
            insertData(dictionary, key, value);
            c = fgetc(input);
        }
    } else {
        fprintf(stderr, "File %s does not exist\n", name);
        exit(0);
    }
}

/** You need to implement this function.*/
void processInput(){
    int size = 60;
    int length = 0;
    char * word = malloc(sizeof(char) * size + 1);
    int i;
    while((i = getchar()) != EOF) {
        char c = (char) i;
        if (isalnum(c)) {
            if (length >= size) {
                size *= 2;
                word = realloc(word, sizeof(char) * size);
            }
            word[length] = c;
            length++;
        } else {
            word[length] = '\0';
            length = 0;
            int printed = 0;
            char *wordCopy = malloc(sizeof(char) * size + 1);
            strncpy(wordCopy, word, strlen(word) + 1);
            if (findData(dictionary, wordCopy) != NULL
                && !printed) {
                printed = 1;
                fprintf(stdout, "%s", (char *) findData(dictionary, wordCopy));
            }
            int i;
            for (i = 1; wordCopy[i] != '\0'; i++) {
                wordCopy[i] = tolower(wordCopy[i]);
            }
            if (findData(dictionary, wordCopy) != NULL
                && !printed) {
                printed = 1;
                fprintf(stdout, "%s", (char *) findData(dictionary, wordCopy));
            }
            wordCopy[0] = tolower(wordCopy[0]);
            if (findData(dictionary, wordCopy) != NULL
                && !printed) {
                printed = 1;
                fprintf(stdout, "%s", (char *) findData(dictionary, wordCopy));
            }
            if (!printed) {
                fprintf(stdout, "%s", word);
            }
            putchar(c);
        }
    }
}
