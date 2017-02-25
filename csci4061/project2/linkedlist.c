#include"linkedlist.h"
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

void print_list(node_t * head) {
	node_t * current = head;

	while (current != NULL) {
			printf("%s\n", current->name);
			current = current->next;
	}
}

void push_node(node_t *head, char *name) {
	node_t *current = head;
	if(current->name == NULL) {
		current->name = name;
		current->next = NULL;
	}
	else {
		while (current->next != NULL) {
				current = current->next;
		}
		current->next = malloc(sizeof(node_t));
		current->next->name = name;
		current->next->next = NULL;
	}

}

char* remove_node(node_t *head, char *name) {
	node_t *current = head;
	node_t *previous = NULL;
	if(head == NULL) {
		return NULL;
	}
	while (strcmp(current->name ,name)){
		if(current->next == NULL) {
			printf("Could not remove %s: name not found", name);
		}
		else {
			previous = current;
			current = current->next;
		}
	}
	previous->next = current->next;
	return current->name;
}

int isEmpty(node_t *head) {
	if(head == NULL) {
		return 1;
	}
	else {
		return 0;
	}
}

char* search_nodes(node_t *head, char *pattern) {
	node_t *current = head;
	char *lastDot;
	if(head == NULL) {
		return NULL;
	}
	while(current->next != NULL) {
		lastDot = strrchr(current->name, '.');
		if(lastDot && strcmp(current->name, pattern)) {
			return current->name;
		}
		current = current->next;
	}
	return NULL;
}
