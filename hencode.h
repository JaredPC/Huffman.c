#ifndef HENCODE_H
#define HENCODE_H

int checkarg (int argc, char *argv[]);
void get_huff_codes(char *argv[]);
void write_to_file(int fdin, int fdout);
int open_filewr(char *argv[]);
void write_header(int fdout);
void write_encodedfile(int fdin, int fdout);
int read_from_file2(int fd);
unsigned char *get_huf_string(unsigned int ch);
int open_filere(char *arg);

#endif
