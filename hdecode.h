#ifndef HDECODE_H
#define HDECODE_H


int open_filewr(char *arg);
int open_filere(char *arg);
int checkarg (int argc, char *argv[]);
void read_from_file2(int fdin, int fdout);
void read_file_header(int fdin);
void decode_file(int fdin, int fdout, uint64_t charcount, struct node *head);
void free_tree(struct node *head);

#endif
