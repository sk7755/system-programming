#include "hash.h"

void add_hash(hnptr hash[], void* data, int hash_number)
{
	hnptr pnew = (hnptr)malloc(sizeof(*pnew));
	pnew->data = data;
	pnew->link = hash[hash_number];
	hash[hash_number] = pnew;
}

//free all of dynamic memories in hash table
void clear_hash(hnptr hash[])
{
	for (int i = 0; i < HASH_SIZE; i++)
	{
		hnptr p = hash[i];

		while (p)
		{
			hnptr next = p->link;
			free(p->data);
			free(p);
			p = next;
		}
		hash[i] = NULL;
	}
}

//according to string, decide hash number
int hash_function(char str[])
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
	{
		hash = ((hash << 5) + hash) + c;
	}

	return hash % HASH_SIZE;
}
