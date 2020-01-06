#ifndef HTABLE_H
#define HTABLE_H

struct node
{
	struct node *next;
	struct node *left;
	struct node *right;
	int32_t character;
	int32_t count;
};

int open_file(char *argv[]);
int read_from_file(char *argv[], int fd);
int read_and_place(char *argv[]);
struct node *create_linked(void);
struct node *insert_node(struct node *head, struct node *nextn);
struct node *build_tree(struct node *head);
void get_codes(struct node *head, char *accl, int len);
void print_codes(void);

#endif
