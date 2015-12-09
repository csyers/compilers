#include "scope.h"
#include "symbol.h"

extern struct hash_table* h;

// enter a scope by adding a new hash table and linking it to the current ones
// @input:
// @output: a new HT is inserted into a linked list of hashtables, h now points to the new table
void scope_enter()
{
	struct hash_table *h_previous;				
	h_previous = h;						// pointer to lowest current level	
	hash_table_insert(h,"0next",hash_table_create(0,0));	// adds a hash table as member "0next" of the current lowest level
	h = hash_table_lookup(h,"0next");			// moves h to the new lowest hash table
	hash_table_insert(h,"0prev",h_previous);		// inserts a link to the previous table into the new lowest level
	// insert two counters for the scope: the number of params and the number of local variables
	if(scope_level() == 2){					// if this is a function scope, add counts for params and local variables
		int *param_count = (int*) malloc(sizeof(int));
		int *local_count = (int*) malloc(sizeof(int));
		*param_count = 0;
		*local_count = 0;
		hash_table_insert(h,"0param_count",param_count);	// insert the counter variables into the table
		hash_table_insert(h,"0local_count",local_count);
        }
}

int scope_increment_local_count()
{
	struct hash_table *cursor = h;
	int *local_count;
	while(!hash_table_lookup(cursor,"0local_count") || !cursor){
		cursor = hash_table_lookup(cursor,"0prev");
	}
	if(cursor){
		local_count = hash_table_lookup(cursor,"0local_count");
		(*local_count)++;
		return (*local_count)-1;
	}
	else return -1;
}
// leave a scope by deleting it and removing the next link to it
// @input:
// @output: h now points to the previous hash table
void scope_exit()
{
	h = hash_table_lookup(h,"0prev");
	hash_table_delete(hash_table_lookup(h,"0next"));
	hash_table_remove(h,"0next");
}

// counts the number of hash tables in the linked list of HTs
// @input: 
// @output: the number of hash tables in the linked list
int scope_level()
{
	int count = 0;
	struct hash_table *cursor = h;
	while(cursor){
		cursor = hash_table_lookup(cursor,"0prev");
		count++;
	}
	return count;
}

// inserts name and symbol into the hash table
void scope_bind(const char *name, struct symbol *symbol)
{
	hash_table_insert(h,name,symbol);
}

// looks for a name in the lowest level of the hash table
// @output: a symbol that the name maps to, or null if not found
struct symbol* scope_lookup_single(const char *name)
{
	return hash_table_lookup(h,name);
}

// looks for a name in all levels of the hash table
// @output: a symbol that the name maps to, or null if not found
struct symbol* scope_lookup_multiple(const char *name)
{
	struct hash_table *cursor = h;
	while(cursor){
		if(hash_table_lookup(cursor,name)){
			return hash_table_lookup(cursor,name);
		}
		cursor = hash_table_lookup(cursor,"0prev");
	}
	return NULL;
}
