#ifndef __HASH_H__
#define __HASH_H__

#include "main.h"

typedef struct hash_node* hnptr;

typedef struct hash_node
{
	void* data;
	hnptr link;
}hash_node;

void add_hash(hnptr hash[], void* data, int hash_number);
void clear_hash(hnptr hash[]);
int hash_function(char str[]);

#endif