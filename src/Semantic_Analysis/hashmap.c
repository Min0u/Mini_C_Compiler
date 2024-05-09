#include "hashmap.h"

unsigned int hash(char *key)
{
    unsigned int hash = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        hash = (hash << 5) + key[i];
    }
    return hash % HASH_SIZE;
}

////////////////////////////////////////////////////////////////////////////////////////

Hash_map *create_hash_map()
{
    Hash_map *map = (Hash_map *)malloc(sizeof(Hash_map));
    for (int i = 0; i < HASH_SIZE; i++)
    {
        map->buckets[i] = NULL;
    }
    return map;
}

////////////////////////////////////////////////////////////////////////////////////////

void insert(Hash_map *map, char *key, Symbol *value)
{
    unsigned int index = hash(key);
    Hash_node *node = (Hash_node *)malloc(sizeof(Hash_node));
    node->key = strdup(key);
    node->value = value;
    node->next = NULL;
    if (map->buckets[index] == NULL)
    {
        map->buckets[index] = node;
    }
    else
    {
        Hash_node *temp = map->buckets[index];
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
    }
}

////////////////////////////////////////////////////////////////////////////////////////

Symbol *lookup(Hash_map *map, char *key)
{
    unsigned int index = hash(key);
    Hash_node *node = map->buckets[index];
    while (node != NULL)
    {
        if (strcmp(node->key, key) == 0)
        {
            return node->value;
        }
        node = node->next;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////

void print_hashmap(Hash_map *map)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        Hash_node *node = map->buckets[i];
        while (node != NULL)
        {
            printf("\t%s -> %s\n", node->key, node->value->id);
            node = node->next;
        }
    }
}