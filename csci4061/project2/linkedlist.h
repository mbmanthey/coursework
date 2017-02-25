#ifndef LINKEDLIST_H_INCLUDED
#define LINKEDLIST_H_INCLUDED
typedef struct node {
	char *name;
	struct node *next;
} node_t;
void print_list(node_t * head);
void push_node(node_t *head, char *name);
char* remove_node(node_t *head, char *name);
int isEmpty(node_t *head);
char* search_nodes(node_t *head, char *pattern);

#endif
