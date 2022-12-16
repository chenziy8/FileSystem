#pragma once
#include <string>
using namespace std;
#define BLOCKSIZE 512

typedef struct superblock {
	int num_inodes;
	int num_blocks;
	int size_blocks;
}superblock_t;

typedef struct inode {
	int size;
	int first_block;
	char name[8];
}inode_t;

typedef struct disk_block {
	int next_block_num;
	char data[BLOCKSIZE];
}disk_block_t;

void create_fs();
void load_fs();
void write_fs();
void print_fs();
int allocate_file(char name[8]);
void set_filesize(int filenum, int size);
void write_byte(int filenum, int pos, char* data);