/* CSCI 4061 Assignment 3
   Student ID: 4558809 &
   Student Name: Matthew Manthey & Payton Giesen
   test.c
*/
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mini_filesystem.h"

void write_to_filesystem(char* path) {
  int inode_number;
  Create_File(path);
  inode_number = Open_File(path);
  FILE *fileptr;
  char *buffer;
  long filelen;
  if((fileptr = fopen(path, "rb")) == NULL) {
    printf("Error opening file at path: %s\n",path);
  }
  fseek(fileptr, 0, SEEK_END);
  filelen = ftell(fileptr);
  rewind(fileptr);
  buffer = malloc((filelen+1)*sizeof(char)); // Enough memory for file + \0
  size_t bytes_read;
  bytes_read = fread(buffer, 1, filelen, fileptr); // Read in the entire file
  fclose(fileptr); // Close the file
  Write_File(inode_number, 0, buffer, (long) bytes_read);
  Close_File(inode_number);
}

//Recursive function for finding all files within directory/subdirectories
void get_files(char* directory) {
  DIR *dp;
	struct dirent *ep;
	dp = opendir (directory);
	if (dp != NULL){
			while ((ep = readdir (dp))) {
        if (ep->d_name[0] != '.') {
          if (ep->d_type == DT_DIR) {
             char path[1024];
             snprintf(path, sizeof(path)-1, "%s/%s", directory, ep->d_name);
             get_files(path);
          }
          else{
            char path[1024];
            snprintf(path, sizeof(path)-1, "%s/%s", directory, ep->d_name);
            write_to_filesystem(path);
          }
        }
			}
			(void) closedir (dp);
	}
	else {
		printf("Couldn't open the directory");
		exit(1);
	}
}


/* Test Helper Interface */
//void write_into_filesystem(char* input_directory, char *log_filename);
void write_into_filesystem(char* input_directory, char *log_filename) {
  if(Initialize_Filesystem(log_filename)) {
  	printf("Error initializing filesystem\n");
  }
	get_files(input_directory);
}

void make_filesystem_summary(char* filename) {
  int i = 0;
  //printf("%d\n",Inode_List[1].Inode_Number);
  while(i < Superblock.next_free_inode) {
    printf("Filename: %s\nInode Number: %d\nSize: %d\n",Directory_Structure[i].Filename,Inode_List[i].Inode_Number,Inode_List[i].File_Size);
    i++;
  }
}

void read_images_from_filesystem_and_write_to_output_directory(char* output_directory) {
  
}
void generate_html_file(char* filename) {

}

/* Main function */
int main(int argc, char* argv[]){
  // Command line arguments: <executable-name> <input_dir> <output_dir> <log_filename>
  if(argc != 4) {
		perror("Invalid usage \nUsage: test input_dir output_dir log_filename");
    exit(1);
  }
	//Get args
  char *input_dir = argv[1];
  char *output_dir = argv[2];
  char *log_name = argv[3];
	write_into_filesystem(input_dir, log_name);
  make_filesystem_summary("Testing");


  return 0;
}
