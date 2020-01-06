#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>

#include "htable.h"
#define BUFFERSIZE 4096

uint32_t htable[256] = {0};
char ctable[256][256];
char *buff;

/* Opens file and returns the file descriptor */
int open_file(char *argv[]){
	int fd = open(argv[1], O_RDONLY); 
	if(fd == -1){
		printf("%s: %s\n",argv[1],strerror(errno));
		return -1;
	}
	return fd;
}

/* read new page into buffer */
int read_from_file(char *argv[], int fd){
	int i = read(fd, buff, BUFFERSIZE);
	if(i == -1){
		printf("%s: %s\n",argv[1],strerror(errno));
		return -1;
	}
	return i;
}

/* Takes in a file and returns a chartable of character counts */
int read_and_place(char *argv[]){
	int fd;
	uint8_t charval;
	if((fd = open_file(argv)) == -1){
		return -1;
	}
	while(read(fd, &charval, sizeof(uint8_t)) != 0){
		htable[charval]++;
	}
	close(fd);
	return 1;
}

/* Takes chartable and builds a linked list */
struct node *create_linked(void){
	unsigned int i;
	struct node *head = NULL;
	struct node *nextn;
	for(i = 0; i < 256; i++){
		if(htable[i] != 0){
			nextn = (struct node *) malloc(sizeof(struct node));
			nextn->character = i;
			nextn->count = htable[i];
			nextn->next = NULL;
			nextn->left = NULL;
			nextn->right = NULL;
			head = insert_node(head, nextn);
		}
	}
	return head;
}

/* inserts node into sorted linked list */
struct node *insert_node(struct node *head, struct node *nextn){
	struct node *curr;
	if(head == NULL){
		return nextn;
	}else{
		if(nextn->count < head->count){
			nextn->next = head;
			return nextn;
		}else if(nextn->count == head->count){
			curr = head;
			while((curr->next != NULL) && (nextn->count == curr->next->count) && 
										(curr->next->character < nextn->character)){
				curr = curr->next;
			}
			if(curr->next == NULL){
				curr->next = nextn;
				return head;
			}else{
				nextn->next = curr->next;
				curr->next = nextn;
				return head;
			}
		}else{
			curr = head;
			while((curr->next != NULL) && (curr->next->count < nextn->count)){
				curr = curr->next;
			}
			if(curr->next == NULL){
				curr->next = nextn;
				return head;
			}
			if(curr->next->count == nextn->count){
				while((curr->next != NULL) && (nextn->count == curr->next->count) && 
										(curr->next->character < nextn->character)){
					curr = curr->next;
				}
				if(curr->next == NULL){
					curr->next = nextn;
					return head;
				}else{
					nextn->next = curr->next;
					curr->next = nextn;
					return head;
				}
			}else{
				nextn->next = curr->next;
				curr->next = nextn;
				return head;
			}
		}
	}
}

/* build Tree from linked list */
struct node *build_tree(struct node *head){
	if(head == NULL){
		return NULL;
	}else if(head->next == NULL){
		return head;
	}else{
		struct node *newn = (struct node *) malloc(sizeof(struct node));
		newn->next = NULL;
		newn->character = -1;
		newn->count = (head->count + head->next->count);
		newn->left = head;
		newn->right = head->next;
		return(build_tree(insert_node(head->next->next, newn)));
	}
}

/* pre-order tree traversal to print tree */
void get_codes(struct node *head, char *accl, int len){
	if(head == NULL){
		return;
	}else if(head->character != -1){
		strncpy(ctable[head->character], accl, len);
		free(head);
	}else{
		accl[len] = '0';
		get_codes(head->left, accl, len + 1);
		accl[len] = '1';
		get_codes(head->right, accl, len + 1);
		free(head);
	}
}

/* print codes */
void print_codes(void){
	int i;
	for(i = 0; i < 256; i++){
		if(strncmp(ctable[i], "",1)){
			printf("0x%02x:   %s\n",i, ctable[i]);
		}
	}
}

