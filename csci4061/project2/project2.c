#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<errno.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<linkedlist.h>

int convert_to_JPG(char *filename, char *inputDir, char *outputDir) {
	char fileIn[80];
	char fileOut[80];
	int ret;
  sprintf(fileIn, "%s/%s", inputDir, filename);
	char *basename;
	basename = filename;
	//Strip extension
	basename[strlen(basename) - 4] = 0;
	//Format output directory
	sprintf(fileOut, "%s%s", filename, ".jpg");
	sprintf(fileOut, "%s/%s", outputDir, fileOut);
	char *cmd[] = { "convert", inputDir, fileOut, (char *)0 };
	//printf("\n%s\n", inputDir);
	ret = execv("/usr/bin/convert", cmd);
	if(ret == -1) {
		perror("Problem converting file to jpg");
	}
	return 0;
}

int main (int argc, char *argv[]) {
	//Incorrect Usage
	if(argc != 4) {
		perror("Invalid usage \nUsage: parallel_convert convert_count output_dir input_dir");
		exit(1);
	}
	//Get args
	int convertCount = atoi(argv[1]);
	char *inputDir = argv[2];
	char *outputDir = argv[3];
	if(convertCount < 1 || convertCount > 10) {
		perror("Convert count must be between 1 and 10");
		exit(0);
	}

	//Check if output directory exitsts
	struct stat s = {0};
	if (stat(outputDir, &s)){
		mkdir(outputDir, 0700);
		printf("Output directory created\n");
	}


	//Get all files, save in linked list
	node_t *head = NULL;
	head = malloc(sizeof(node_t));
	// head->filename = "head";
	// head->next = NULL;

	//create log and non-mage text files;
	FILE *logFp, *nonimageFp;
	char logFile[80];
	char nonimageFile[80];
	sprintf(logFile, "%s%s", outputDir, "/log.txt");
	sprintf(nonimageFile, "%s%s", outputDir, "/nonImage.txt");
	printf("Log file created: %s\n", logFile);
	printf("Non-image file created: %s\n",nonimageFile);
	logFp = fopen(logFile, "w+");
	nonimageFp = fopen(nonimageFile, "w+");


	//Check input directory
	DIR *dp;
  struct dirent *ep;
  dp = opendir (inputDir);
  if (dp != NULL){
      while ((ep = readdir (dp))){
				if (stat(ep->d_name, &s)) {
						push_node(head, ep->d_name);
					}
			}
			(void) closedir (dp);
  }
  else {
    perror ("Couldn't open the directory");
		exit(0);
	}


	print_list(head);
	//printf("Removed Node: %s\n", remove_node(head, "Albert_Einstein.gif"));
	//print_list(head);

	//Create pipes for communicating to parent node
	int pd[convertCount + 1];
	char *pipedFilename;

	//Forked Process
	int childCount = convertCount;
	pid_t childpid;
	while(!isEmpty(head)) {
		if(childCount <= 0) {
			wait();
			childCount++;
		}
		else {
			childCount--;
			childpid = fork();
			if (childpid == 0) {
				break;
			}
			if(((long)getpid() % 2 == 0) && !((long)getpid() % 3 == 0)){
				//png
				if (!(stat(outputDir, &s))){

					//Thumbnail command
					//convert "$file" -resize 200x200 "$thumbnail_name.jpg"
					//Used for finding size
					//identify $file

				}
			}
			else if(((long)getpid() % 3 == 0) && !((long)getpid() % 2 == 0)){
				//bmp
			}
			else if(((long)getpid() % 2 == 0) && ((long)getpid() % 3 == 0)){
				//gif
			}
			else {
				//nonimage
			}
			//printf("%s converted to jpg by process with id %d",filename, childPid);
		}
	}
	printf("\n");
	//fclose(logFp);
	//fclose(nonimageFp);


}
