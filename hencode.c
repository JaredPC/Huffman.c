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
#include "hencode.h"

#define BUFFERSIZE 4096
extern char *buff;
extern unsigned char ctable[256][256];
extern uint32_t htable[256];

int main (int argc, char *argv[]){
	int argcheck;

	argcheck = checkarg(argc, argv);
	if(argcheck == 2){
		get_huff_codes(argv);
		write_to_file(open_filere(argv[1]), STDOUT_FILENO);
	}else if(argcheck == 3){
		int fpin;
		int fpout;
		get_huff_codes(argv);
		fpin = open_filere(argv[1]);
		fpout = open_filewr(argv);
		write_to_file(fpin, fpout);
		close(fpin);
		close(fpout);
	}else{
		return -1;
	}
	return 1;
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
	if(argc == 2){
		return 2;
	}else if(argc == 3){
		return 3;
	}else{
		printf("usage: hencode infile [ outfile ]\n");
		return -1;
	}
}

/* counts frequency, builds tree, gets huffcodes */
void get_huff_codes(char *argv[]){
	char accl[256];
	struct node *head;

	buff = (char *) calloc(BUFFERSIZE+1, sizeof(char));

	read_and_place(argv); /* From htable*/

	head = create_linked();

	head = build_tree(head);
	get_codes(head, accl, 0);
}

/* takes a file and writes the output huff code*/
void write_to_file(int fdin, int fdout){
	write_header(fdout);
	write_encodedfile(fdin,fdout);
}

/* opens a file for writing */
int open_filewr(char *argv[]){
	int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0777); 
	if(fd == -1){
		printf("%s: %s\n",argv[1],strerror(errno));
		return 0;
	}
	return fd;
}

void write_header(int fdout){
	uint32_t count = 0;
	int i;
	uint8_t j;
	uint32_t k;
	for(i = 0; i < 256; i++){
		if(htable[i] != 0){
			count++;
		}
	}
	write(fdout, &count, sizeof(count));
	for(i = 0; i < 256; i++){
		if(htable[i] != 0){
			j = (uint8_t)i;
			k = htable[i];

			write(fdout, &j, sizeof(uint8_t));
			write(fdout, &k, sizeof(uint32_t));
		}
	}
}

void write_encodedfile(int fdin, int fdout){
	uint8_t outchar = 0x00;
	char singlebit;
	unsigned char *code;
	int charlength;
	int charlength2;
	int codelength = 0;
	int outcharlen = 0;
	unsigned char i;

	while((charlength = read_from_file2(fdin)) > 0){
		charlength2 = charlength;
		while(charlength > 0){
			i = (unsigned char) buff[charlength2 - charlength];
			code = get_huf_string(i);
			while((singlebit = *(code + codelength)) != '\0'){
				outchar = outchar<<1;
				if(singlebit == '1'){
					outchar = outchar | 0x01;
				}
				outcharlen++;
				if(outcharlen == 8){
					write(fdout, &outchar, sizeof(uint8_t));
					outcharlen = 0;
				}
				codelength++;
			}
			codelength = 0;
			charlength--;
		}
	}
	if(outcharlen != 0){
		outchar = outchar<<(8 - outcharlen);
		write(fdout, &outchar, sizeof(uint8_t));
	}
	free(buff);
}

/* read new page into buffer */
int read_from_file2(int fd){
	int i = read(fd, buff, BUFFERSIZE);
	if(i == -1){
		perror("read failed");
		return -1;
	}
	return i;
}

/* Takes in a int representing a character and returns the huffstring*/
unsigned char *get_huf_string(unsigned int ch){
	return ctable[ch];
}


