#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "symbol.h"

#define HASH_SIZE 1024

typedef struct _Hash_node {
    char *key;
    Symbol *value;
    struct _Hash_node *next;
} Hash_node;

typedef struct _Hash_map {
    Hash_node *buckets[HASH_SIZE];
} Hash_map;

unsigned int hash(char *key);

Hash_map *create_hash_map();

// Insert a key-value pair into the hash map
void insert(Hash_map *map, char *key, Symbol *value);

// Lookup a key in the hash map
Symbol *lookup(Hash_map *map, char *key);

#endif