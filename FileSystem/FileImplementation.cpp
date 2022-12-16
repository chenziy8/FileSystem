#pragma once
#include "FileHeader.h"
#include <vector>

superblock_t sb;
vector<inode_t> inodes;
vector<disk_block_t> dbs;
//create file system

void create_fs() {
	sb.num_inodes = 10;
	sb.num_blocks = 100;
	sb.size_blocks = sizeof(disk_block_t);

	int i;
	inode_t inode_temp;
	inodes.assign(sb.num_inodes, inode_temp);
	for (i = 0; i < sb.num_inodes; i++) {
		inodes[i].size = -1;
		inodes[i].first_block = -1;
		strcpy_s(inodes[i].name, "aaaaaaa");
	}

	int j;
	disk_block_t disk_temp;
	dbs.assign(sb.size_blocks, disk_temp);
	for (j = 0; j < sb.size_blocks; j++) {
		dbs[j].next_block_num = -1;
	}
}

void write_fs() {
	FILE* file;
	if (fopen_s(&file, "fs_data", "w+") == 0) {
		fwrite(&sb, sizeof(superblock_t), 1, file);
		//write indoes
		for (int i = 0; i < sb.num_inodes;i++) {
			fwrite(&(inodes[i]), sizeof(inode_t), 1, file);
		}
		//write disk blocks
		for (int i = 0; i < sb.num_blocks; i++) {
			fwrite(&(dbs[i]), sizeof(disk_block_t), 1, file);
		}
		fclose(file);
	}
	else {
		printf("The file was not opened\n");
	}
}

void load_fs() {
	FILE* file;
	if (fopen_s(&file, "fs_data", "r") == 0) {
		fread(&sb, sizeof(superblock_t), 1, file);
		for (int i = 0; i < sb.num_inodes;i++) {
			fread(&(inodes[i]), sizeof(inode_t), 1, file);
		}
		for (int i = 0; i < sb.num_blocks;i++) {
			fread(&(dbs[i]), sizeof(disk_block), 1, file);
		}
		fclose(file);
	}
	else {
		printf("The file was not opened\n");
	}
}

void print_fs() {
	printf("Superblock info\n");
	printf("\tnum of inodes: %d\n", sb.num_inodes);
	printf("\tnum of blocks: %d\n", sb.num_blocks);
	printf("\tsize of blocks: %d\n", sb.size_blocks);
	
	printf("inodes\n");	
	for (int i = 0; i < sb.num_inodes; i++) {
		printf("\tsize: %d block: %d name: %s\n", inodes[i].size, inodes[i].first_block,inodes[i].name);
	}
	for (int i = 0; i < sb.num_blocks; i++) {
		printf("\tblock num: %d next block: %d\n", i, dbs[i].next_block_num);
	}
}

int find_empty_inode() {
	for (int i = 0;i < sb.num_inodes;++i) {
		if (inodes[i].first_block == -1) {
			return i;
		}
	}
	return -1;
	//no empty inode
}


int find_empty_block() {
	for (int i = 0;i < sb.num_blocks; ++i) {
		if (dbs[i].next_block_num == -1) {
			return i;
		}
	}
	return -1;
	//no empty block
}

void shorten_file(int bn) {
	int nn = dbs[bn].next_block_num;
	if (nn>= 0) {
		shorten_file(nn);
	}
	dbs[bn].next_block_num = -1;
}

int allocate_file(char name[8]) {
	//find an empty inode
	int inode_id = find_empty_inode();
	//claim a disk block
	int block_id = find_empty_block();
	//claim it
	inodes[inode_id].first_block = block_id;
	dbs[block_id].next_block_num = -2;
	strcpy_s(inodes[inode_id].name, name);
	//return the file descriptor
	return inode_id;
}

void set_filesize(int filenum, int size) {
	int temp = size + BLOCKSIZE - 1;
	int num = temp / BLOCKSIZE;
	int bn = inodes[filenum].first_block;
	num--;
	while (num > 0) {
		int next_num = dbs[bn].next_block_num;
		if (next_num == -2) {
			int empty = find_empty_block();
			dbs[bn].next_block_num = empty;
			dbs[empty].next_block_num = -2;
		}
		bn = dbs[bn].next_block_num;
		num--;
	}
	shorten_file(bn);
	dbs[bn].next_block_num = -2;
}

int get_block_num(int file, int offset) {
	int togo = offset;
	int bn = inodes[file].first_block;
	while (togo > 0) {
		bn = dbs[bn].next_block_num;
		togo--;
	}
	return bn;
}

void write_byte(int filenum, int pos, char* data) {
	int relative_block = pos / BLOCKSIZE;
	int bn = get_block_num(filenum, relative_block);
	int offset = pos % BLOCKSIZE;
	dbs[bn].data[offset] = *data;
}
