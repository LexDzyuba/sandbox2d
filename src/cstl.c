#include "cstl.h"

#include <ctype.h>
#include <stdlib.h>


struct Table *createTable(size_t size) {
	if (size == 0)
		return NULL;
	struct Table *t = (struct Table*)malloc(sizeof(struct Table));
	t->size = size;
	t->list = (struct Node**)malloc(sizeof(struct Node*)*size);
	size_t i;
	for (i = 0; i < size; i++)
		t->list[i] = NULL;
	return t;
}

size_t hashStringNoCase(char *str, size_t m) {
	size_t result = 5381;
	unsigned char *p;

	p = (unsigned char *)str;

	while (*p != '\0') {
		result = (result << 5) + result + (unsigned int)tolower(*p);
		++p;
	}

	return result % m;
}

void insert(struct Table *t, void *key, void *data, size_t(*pHashFunc)(void*, size_t m), int(*pCmp)(void*, void*)) {
	size_t pos = pHashFunc(key, t->size);
	struct Node *list = t->list[pos];
	struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
	struct Node *temp = list;
	while (temp) {
		if (pCmp(temp->key, key) == 0) {
			temp->data = data;
			return;
		}
		temp = temp->next;
	}
	newNode->key = key;
	newNode->data = data;
	newNode->next = list;
	t->list[pos] = newNode;
}

void *lookup(struct Table *t, void *key, size_t(*pHashFunc)(void*, size_t m), int(*pCmp)(void*, void*)) {
	int pos = pHashFunc(key, t->size);
	struct Node *list = t->list[pos];
	struct Node *temp = list;
	while (temp) {
		if (pCmp(temp->key, key) == 0) {
			return temp->data;
		}
		temp = temp->next;
	}
	return NULL;
}

struct Node **popListTable(struct Table *t) {
	struct Node *n = NULL;
	if (t->size > 0)
		return &t->list[--t->size];
	return NULL;
}

List *newList() {
	List *l = malloc(sizeof(List));
	l->node = NULL;
	return l;
}

void pushBackList(List **l, void *data) {
	if (*l == NULL) {
		*l = newList();
		(*l)->data = data;
		return;
	}
	pushBackList(&(*l)->node, data);
}

void pushFrontList(List **l, void *data) {
	if (l == NULL) {
		*l = newList();
		(*l)->data = data;
		return;
	}
	List *tmp = newList();
	tmp->data = data;
	tmp->node = *l;
	*l = tmp;
}

void *foreachList(List **tmpPointerToList) {
	if (tmpPointerToList == NULL)
		return NULL;
	void *data = (*tmpPointerToList)->data;
	*tmpPointerToList = (*tmpPointerToList)->node;
	return data;
}