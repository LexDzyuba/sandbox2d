#pragma once

#include <stddef.h>

struct Node {
	void *key;
	void *data;
	struct Node *next;
} Node;

struct Table {
	size_t size;
	struct Node **list;
};

struct Table *createTable(size_t size);
size_t hashStringNoCase(char *str, size_t m);
void insert(struct Table *t, void *key, void *data, size_t(*pHashFunc)(void*, size_t m), int(*pCmp)(void*, void*));
void *lookup(struct Table *t, void *key, size_t(*pHashFunc)(void*, size_t m), int(*pCmp)(void*, void*));
