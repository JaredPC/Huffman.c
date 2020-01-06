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
#include "hdecode.h"

#define BUFFERSIZE 4096

extern char *buff;
extern char ctable[256][256];
extern uint32_t htable[256];

int main (int argc, char *argv[]){
	int argcheck;
	int fdin, fdout;
	argcheck = checkarg(argc, argv);

	if(argcheck == 1 || argcheck == 2){
		read_from_file2(STDIN_FILENO,STDOUT_FILENO);
	}else if(argcheck == 3){
		if((fdin = open_filere(argv[1])) == 0){
			return -1;
		}
		read_from_file2(fdin,STDOUT_FILENO);
		close(fdin);
	}else if(argcheck == 4){
		if((fdout = open_filewr(argv[2])) == 0){
			return -1;
		}
		read_from_file2(STDIN_FILENO,fdout);
		close(fdout);
	}else if(argcheck == 5){
		if(((fdin = open_filere(argv[1])) == 0) || 
			((fdout = open_filewr(argv[2])) == 0)){
			return -1;
		}
		read_from_file2(fdin,fdout);
		close(fdin);
		close(fdout);
	}else{
		return -1;
	}
	return 1;
}

/* opens a file for writing */
int open_filewr(char *arg){
	int fd = open(arg, O_WRONLY | O_CREAT | O_TRUNC, 0777); 
	if(fd == -1){
		printf("%s: %s\n",arg,strerror(errno));
		return 0;
	}
	return fd;
}

/* opens a file for reading */
int open_filere(char *arg){
	int fd = open(arg, O_RDONLY, 0777); 
	if(fd == -1){
		printf("%s: %s\n",arg,strerror(errno));
		return 0;
	}
	return fd;
}

/* checks for correct number of arguments */
int checkarg (int argc, char *argv[]){
	if(argc > 3){
		printf("usage: hdecode [(infile | - )[outfile]]\n");
		return -1;
	}else if(argc == 1){
		return 1;
	}else if (argc == 2){
		if(strncmp(argv[1],"-",1) == 0){
			return 2;
		}else{
			return 3;
		}
	}else{
		if(strncmp(argv[1],"-",1) == 0){
			return 4;
		}else{
			return 5;
		}
	}
}


/* decodes file in and outputs to file out */
void read_from_file2(int fdin, int fdout){
	uint64_t charcount = 0;
	int i;
	struct node *head;
	read_file_header(fdin);
	for(i = 0; i < 256; i++){
		charcount += htable[i];
	}
	head = create_linked();
	head = build_tree(head);
	decode_file(fdin, fdout, charcount, head);
	free_tree(head);
}

/* builds tree from header*/
void read_file_header(int fdin){
	uint32_t count;
	uint8_t charval;
	uint32_t freq;
	int i;
	read(fdin, &count, sizeof(uint32_t));
	for(i = 0; i < count; i++){
		read(fdin, &charval, sizeof(uint8_t));
		read(fdin, &freq, sizeof(uint32_t));
		htable[charval] = freq;
	}
}

/* decodes file to file out*/
void decode_file(int fdin, int fdout, uint64_t charcount, struct node *head){
	int charlength;
	struct node *curr = head;
	int8_t inchar;
	int8_t bitc;
	int i;
	int flag = 1;

	if(head == NULL){
		;
	}else if((head->left == NULL) && (head->right == NULL)){
		for(i = 0; i < head->count; i++){
			write(fdout, &head->character, sizeof(char));
		}
	}

	if((i = read(fdin, &inchar, 1)) == 0){
		flag = 0;
	}else{
		charlength = 8;
	}
	while(flag){
		while((charcount > 0) && (curr->character == -1) && flag){
			bitc = inchar & 0x80;
			if(bitc == 0){
				curr = curr->left;
			}else{
				curr = curr->right;
			}
			inchar = inchar<<1;
			charlength--;
			if(charlength <= 0){
				if((i = read(fdin, &inchar, sizeof(uint8_t))) == 0){
					flag = 0;
				}else{
					charlength = 8;
				}
			}
		}
		if(charcount > 0){
			write(fdout, &curr->character, sizeof(char));
			charcount--;
			curr = head;
		}else{
			flag = 0;
		}
	}
}


/* pre-order tree traversal to free tree */
void free_tree(struct node *head){
	if(head == NULL){
		return;
	}
	free_tree(head->left);
	free_tree(head->right);
	free(head);

}

