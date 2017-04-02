#include <sys/stat.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <libgen.h>
#include "mini_filesystem.h"


//Lower Level File System Calls
int Search_Directory(char* filename) {
  for(int i = 0; i < Superblock.next_free_inode; i++) {
    if(strcmp(filename, Directory_Structure[i].Filename) == 0) {
      return i;
    }
  }
  return -1;
}

int Add_to_Directory(char* filename, int inode_number) {
  if(inode_number >= MAXFILES) {
    printf("Unable to add to directory: Max files reached\n");
    return -1;
  }
  Directory current_directory_entry = Directory_Structure[inode_number -1];
  strcpy(current_directory_entry.Filename, filename);
  current_directory_entry.Inode_Number = inode_number;
  Directory_Structure[inode_number] = current_directory_entry;
  return 0;
}

Inode Inode_Read(int inode_number) {
  if(inode_number < Superblock.next_free_inode) {
    return Inode_List[inode_number];
  }
  printf("Error reading inode: Invalid inode number argument\n");
  Inode temp_inode;
  temp_inode.Inode_Number = -1;
  return temp_inode;
}

int Inode_Write(int inode_number, Inode input_inode) {
  if(inode_number > Superblock.next_free_inode) {
    return -1;
  }
  Inode_List[inode_number] = input_inode;
  return 0;
}

int Block_Read(int block_number, int num_bytes, char* to_read) {
  if(block_number > MAXBLOCKS || block_number < 0) {
    printf("Error reading block: Invalid block number\n");
    return -1;
  }
  if(num_bytes > BLOCKSIZE) {
    printf("Error reading block: Invalid number of bytes\n");
    return -1;
  }
  strncpy(to_read, Disk_Blocks[block_number], num_bytes);
  return num_bytes;
}

int Block_Write(int block_number, int num_bytes, char* to_write) {
  if(block_number > MAXBLOCKS || block_number < 0) {
    printf("Error writing to block: Invalid block number\n");
    return -1;
  }
  if(num_bytes > BLOCKSIZE) {
    printf("Error writing to block: Invalid number of bytes\n");
    return -1;
  }
  Disk_Blocks[block_number] = to_write;
  return sizeof(to_write);
}

Super_block Superblock_Read() {
  return Superblock;
}

int Superblock_Write(Super_block input_superblock) {
  Superblock = input_superblock;
  return 0;
}

//API Calls

int Initialize_Filesystem(char* log_filename) {
  FILE *log_fp;
	log_fp = fopen(log_filename, "w+");
  Count = 0;
  Superblock.next_free_inode = 0;
  Superblock.next_free_block = 0;
  fclose(log_fp);
  return 0;
}

int Create_File(char* path) {
  char* filename;
  if(strcmp(filename = basename(path), ".") == 0) {
    perror("Error getting basename:");
    return -1;
  }
  if(Search_Directory(filename) > 0) {
    printf("File: %s already exists in directory\n", filename);
    return -1;
  }
  Inode temp_inode;
  Super_block temp_superblock;
  temp_superblock = Superblock_Read();
  struct stat st;
  if(stat(path, &st)){
    printf("Error using stat on file %s:\n",filename);
    perror("Error: ");
    return -1;
  }

  temp_inode.Inode_Number = temp_superblock.next_free_inode;
  temp_inode.User_Id = st.st_uid;
  temp_inode.Group_Id = st.st_gid;
  temp_inode.File_Size = 0;
  temp_inode.Start_Block = -1;
  temp_inode.End_Block = -1;
  temp_inode.Flag = 0;
  if(Inode_Write(temp_inode.Inode_Number, temp_inode)) {
    printf("Error writing to Inode List\n");
    return -1;
  }
  if(Add_to_Directory(filename, temp_inode.Inode_Number)){
    printf("Error adding file to directory\n");
    return -1;
  }
  temp_superblock.next_free_inode++;
  if(Superblock_Write(temp_superblock)) {
    printf("Error updating Superblock\n");
    return -1;
  }
  return 0;
}


int Open_File(char* path) {
  char* filename;
  if(strcmp(filename = basename(path), ".") == 0) {
    perror("Error getting basename:");
    return -1;
  }
  int temp_inode_number;
  Inode temp_inode;
  if((temp_inode_number = Search_Directory(filename)) < 0) {
    printf("File: %s not found in directory\n", filename);
    return -1;
  }
  if(Inode_Read(temp_inode_number).Inode_Number < 0) {
    printf("Error: Invalid Inode Number\n");
    return -1;
  }
  temp_inode = Inode_Read(temp_inode_number);
  temp_inode.Flag = 1;
  if(Inode_Write(temp_inode_number, temp_inode)) {
    printf("Error writing to Inode\n");
    return -1;
  }
  return temp_inode_number;
}


int Read_File(int inode_number, int offset, int count, char* to_read) {
  Inode temp_inode;
  temp_inode = Inode_Read(inode_number);
  if(!temp_inode.Flag) {
    printf("File must be open before reading\n");
    return -1;
  }
  if(temp_inode.Inode_Number < 0) {
    printf("Error reading Inode");
    return -1;
  }
  if(temp_inode.File_Size < (offset + count)) {
    printf("Bytes requested larger than file size\n");
    return -1;
  }
  char* result = (char *)malloc(temp_inode.File_Size);
  int byte_count = 100;
  int num_bytes = BLOCKSIZE;
  int current_block = temp_inode.Start_Block + ceil(offset / BLOCKSIZE);
  int end_block = temp_inode.End_Block;
  while (current_block <= end_block){
    char* to_read = malloc(BLOCKSIZE);
    if(Block_Read(current_block++, num_bytes, to_read)){
        printf("Error reading block\n");
        return -1;
    }
    strcat(result, to_read);
  }
  return byte_count;
}


int Write_File(int inode_number, int offset, char* to_write, long num_bytes) {
  Inode temp_inode;
  Super_block temp_superblock;
  temp_inode = Inode_Read(inode_number);
  if(!temp_inode.Flag) {
    printf("File must be open before writing\n");
    return -1;
  }
  if(temp_inode.Inode_Number < 0) {
    printf("Error reading Inode\n");
    return -1;
  }
  temp_superblock = Superblock_Read();
  temp_inode.Start_Block = temp_superblock.next_free_block;
  if(offset > temp_inode.File_Size) {
    printf("Invalid offset\n");
    return -1;
  }
  long write_size = num_bytes;
  int written_count;
  while(write_size > 0) {
    int next_write_size;
    char next_write[BLOCKSIZE];
    temp_superblock = Superblock_Read();
    if(write_size / BLOCKSIZE > 0) {
      next_write_size = BLOCKSIZE;
    }
    else {
      next_write_size = write_size;
    }
    memcpy(next_write, to_write + written_count, next_write_size);
    if(Block_Write(temp_superblock.next_free_block, next_write_size, next_write) < 0) {
      printf("Error writing to block\n");
      return -1;
    }
    written_count += next_write_size;
    write_size -= next_write_size;
    temp_inode.File_Size += next_write_size;
    temp_inode.End_Block = temp_superblock.next_free_block;
    temp_superblock.next_free_block++;

    if(Inode_Write(inode_number, temp_inode)){
      printf("Error writing to Inode\n");
      return -1;
    }
    if(Superblock_Write(temp_superblock)) {
      printf("Error updating Superblock\n");
      return -1;
    }
  }
  return written_count;
}

int Close_File(int inode_number) {
  Inode temp_inode;
  if(Inode_Read(inode_number).Inode_Number < 0) {
    printf("Error: Invalid Inode Number\n");
    return -1;
  }
  temp_inode = Inode_Read(inode_number);
  temp_inode.Flag = 0;
  if(Inode_Write(inode_number, temp_inode)) {
    printf("Error writing to Inode\n");
    return -1;
  }
  return 0;
}
