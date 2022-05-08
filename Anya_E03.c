#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#define SEGMENT 1024 

static const char *WibuLogPath = "/home/soreta/Documents/Wibu.log";
static const char *directoryPath = "/home/soreta/Downloads";
char prefix[8] = "Animeku_";

void logging1(const char* kind, const char* old, char* new) {
	
	FILE * logFile = fopen(WibuLogPath, "a");
	if (logFile == NULL) printf("Error : errno = '%s'.\n",strerror(errno));
	else fprintf(logFile, "RENAME %s %s --> %s\n",kind, old, new);
    fclose(logFile);
}

char atbash[26]={"ZYXWVUTSRQPONMLKJIHGFEDCBA"};
char alphabetup[26]={"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};
char alphabetlow[26]={"abcdefghijklmnopqrstuvwxyz"};
char rot13[26]={"nopqrstuvwxyzabcdefghijklm"};

char bash (char alphabet){
    for(int i = 0 ; i < 26 ; i++) if (alphabet == alphabetup[i]) return atbash[i];
    return '.';
}
char rot (char alphabet){
    for(int i = 0 ; i < 26 ; i++) if (alphabet == alphabetlow[i]) return rot13[i];    
    return '.';
}
void encode1(char* strEnc1,const char *path) { //encrypt ANimeku
	char oldPath[SEGMENT];
	char newPath[SEGMENT];
	strcpy(oldPath,"");
	strcpy(newPath,"");
	strcpy(oldPath,path);
	strcat(oldPath,"/");
	strcat(oldPath,strEnc1);
	if(strcmp(strEnc1, ".") == 0 || strcmp(strEnc1, "..") == 0)
        return;
    int strLength = strlen(strEnc1);
    int LastDot=-1;
    for (int i = 0 ; i < strLength; i++){
        if(strEnc1[i]=='.')LastDot=i;
    }
    for(int i = 0; i < strLength; i++) {
		if(strEnc1[i] == '/') 
            continue;
		if(strEnc1[i]=='.' && i==LastDot)
            break;
        
		if(strEnc1[i]>='A'&&strEnc1[i]<='Z')
            strEnc1[i] = 'Z'+'A'-strEnc1[i];
        if(strEnc1[i]>='a'&&strEnc1[i]<='z')
            strEnc1[i] = 'a'+ (strEnc1[i] - 'a' + 13) %26 ;
    }
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,strEnc1);
	logging1("terenkripsi",oldPath, newPath);
}

void decode1(char * strDec1,const char *path){ //decrypt Animeku_
	char oldPath[SEGMENT];
	char newPath[SEGMENT];
	strcpy(oldPath,"");
	strcpy(newPath,"");
	strcpy(oldPath,path);
	strcat(oldPath,"/");
	strcat(oldPath,strDec1);
	if(strcmp(strDec1, ".") == 0 || strcmp(strDec1, "..") == 0 || strstr(strDec1, "/") == NULL) 
        return;
    
    int strLength = strlen(strDec1), s=0;
	for(int i = strLength; i >= 0; i--){
		if(strDec1[i]=='/')break;

		if(strDec1[i]=='.'){//nyari titik terakhir
			strLength = i;
			break;
		}
	}
	for(int i = 0; i < strLength; i++){
		if(strDec1[i]== '/'){
			s = i+1;
			break;
		}
	}
    for(int i = s; i < strLength; i++) {
		if(strDec1[i] =='/'){
            continue;
        }
        if(strDec1[i]>='A'&&strDec1[i]<='Z'){
            strDec1[i] = 'Z'+'A'-strDec1[i];
        }
        if(strDec1[i]>='a'&&strDec1[i]<='z'){
            strDec1[i] = 'a' + (strDec1[i] - 'a' + 13) % 26 ;
        }
	}
	strcpy(newPath,path);
	strcat(newPath,"/");
	strcat(newPath,strDec1);
	logging1("terdecode",oldPath, newPath);
}

// --------soal no 3------------
// nama setelah uppercase + tambah extension biner
char newSpecial[1024];
// string hasil fungsi toString
char stringFromInt[1024];

// fungsi int->string
void toString(int x){
	int len = 0;
	len++;
	int temp = x;
	while(temp>10){
	    temp /= 10;
	    len++;
	}
	temp = len;
	// fodder
	for(int i=0; i<len; i++)stringFromInt[i] = 'a';
	while(temp--){
	    if(x)stringFromInt[temp] = '0' + (x%10);
	    else stringFromInt[temp] = '0' + x;
	    x/=10;
	}
	stringFromInt[len] = '\0';
}

// cek file diawali "nam_do-saq_"
bool checkSp(char fileName[]){
  int fileNameLen = strlen(fileName);
  if(fileNameLen >= 11){
    char checkNDS[1024];
    for(int i = 0; i < 11; i++)checkNDS[i] = fileName[i];
    checkNDS[11] = '\0';
    if(strcmp(checkNDS, "nam_do-saq_")==0)return true;
  }
  return false;
}
double pow (double x, double y){
	double ret = 1;
	for(int i = 0 ; i < y ; i++) ret *= x;
	return ret;
}
// ganti nama dir spesial (uppercase + tambah extension biner)
void renameExt(char fileName[]){
  // find extension
  int lastDot = -1;
  for(int i = 0; i < strlen(fileName); i++)if(fileName[i] == '.')lastDot = i;
  
  char noExt[1024], theExt[1024];
  strcpy(newSpecial,"");
  strcpy(noExt,"");
  strcpy(theExt,"");
  
  // no extension
  if(lastDot == -1)lastDot = strlen(fileName);
  
  // split
  for(int i = 0; i < lastDot; i++)noExt[i] = fileName[i];
  noExt[lastDot] = '\0';
  
  if(lastDot != strlen(fileName)){
    int ctr = 0;
    for(int i = lastDot; i < strlen(fileName); i++){
      theExt[ctr] = fileName[i];
      ctr++;
    }
    theExt[ctr]='\0';
  }
  
  char upperCased[1024];
  strcpy(upperCased,"");
  
  for(int i = 0; i < strlen(noExt); i++){
    if(noExt[i] >= 'a' && noExt[i] <= 'z') {
         upperCased[i] = noExt[i] -32;
    }
    else upperCased[i] = noExt[i];
  }
  upperCased[strlen(noExt)] = '\0';
  
  int diff = 0, multi = 0;
  
  for(int i = strlen(upperCased)-1; i >= 0; i--){
    if(noExt[i]!=upperCased[i])diff += pow(2, multi);
    multi++;
  }
  
  toString(diff);
  
  char dotz[2];
  dotz[0]='.';
  dotz[1]='\0';
  
  strcpy(newSpecial,upperCased);
  strcat(newSpecial,theExt);
  strcat(newSpecial,dotz);
  strcat(newSpecial,stringFromInt);
}
//---------------------------------------------------------//
//Get file attributes
static  int  xmp_getattr(const char *path, struct stat *stbuf){
	char * strToEnc1 = strstr(path, prefix);
	
	if(strToEnc1 != NULL){
		decode1(strToEnc1,path);
    }

	char newPath[1000];
	int result;
	sprintf(newPath,"%s%s", directoryPath, path);
	result = lstat(newPath, stbuf);
	if (result == -1)
		return -errno;
	return 0;
}
//Read directory
static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){ 
	char * strToEnc1 = strstr(path, prefix);
    
	if(strToEnc1 != NULL) {
        decode1(strToEnc1,path);
    }

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path = directoryPath;
		sprintf(newPath, "%s", path);
	} else 
        sprintf(newPath, "%s%s", directoryPath, path);

	int result = 0;
	struct dirent *dir;
	DIR *dp;
	(void) fi;
	(void) offset;
	dp = opendir(newPath);
	if (dp == NULL) return -errno;
	
	//buat loop yang ada di dalam directory
	while ((dir = readdir(dp)) != NULL) { 
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = dir->d_ino;
		st.st_mode = dir->d_type << 12;
		if(strToEnc1 != NULL){
			//encode yang ada di dalam directory sekarang
			encode1(dir->d_name,path); 
        }
		
		result = (filler(buf, dir->d_name, &st, 0));
		if(result!=0) break;
	}

	closedir(dp);
	return 0;
}
//Create a directory
static int xmp_mkdir(const char *path, mode_t mode){ 

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=directoryPath;
		sprintf(newPath,"%s",path);
	}
	else 
        sprintf(newPath, "%s%s",directoryPath,path);

	int result = mkdir(newPath, mode);
    char str[100];
	sprintf(str, "MKDIR::%s", path);

	char * folderPath = strstr(path, prefix);
    
	if(folderPath != NULL) {
        logging1("terenkripsi",newPath, newPath);
    }
	
	printf("%s\n",path);
	printf("%s\n",newPath);
	if (result == -1)
		return -errno;

	return 0;
}
//Create a file node
static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path = directoryPath;
		sprintf(newPath,"%s",path);
	} 
    else 
        sprintf(newPath, "%s%s",directoryPath,path);
	
    int result;

	if (S_ISREG(mode)) {
		result = open(newPath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (result >= 0)
			result = close(result);
	} 
	else if (S_ISFIFO(mode)) {
		result = mkfifo(newPath, mode);
	}
	else {
		result = mknod(newPath, mode, rdev);
	}
    char str[100];
	sprintf(str, "CREATE::%s", path);
	
	if (result == -1){
			return -errno;
	}

	return 0;
}
//Remove a file
static int xmp_unlink(const char *path) { 
	char * strToEnc1 = strstr(path, prefix);
	
	if(strToEnc1 != NULL){
        decode1(strToEnc1,path); //buat ngebalikin biar bisa dibaca di document
    }

	char newPath[1000];
	if(strcmp(path,"/") == 0){
		path=directoryPath;
		sprintf(newPath,"%s",path);
	} 
    else {
    	 sprintf(newPath, "%s%s",directoryPath,path);
	}
       
    char str[100];
	sprintf(str, "REMOVE::%s", path);
	
	char * folderPath = strstr(path, prefix);
    


	int result;
	result = unlink(newPath);
	
	if (result == -1)
		return -errno;

	return 0;
}
//Remove a directory
static int xmp_rmdir(const char *path) {
	char * strToEnc1 = strstr(path, prefix);
	if(strToEnc1 != NULL){
        decode1(strToEnc1,path); //buat ngebalikin biar bisa dibaca di document
    }
	
	char newPath[1000];
	sprintf(newPath, "%s%s",directoryPath,path);
    char str[100];
	sprintf(str, "RMDIR::%s", path);
	int result;
	result = rmdir(newPath);
	
	if (result == -1){
		return -errno;
	}
	
	return 0;
}
//Rename
static int xmp_rename(const char *source, const char *dest) {

	char fileSource[1000], fileDest[1000];
	sprintf(fileSource, "%s%s", directoryPath, source);
	sprintf(fileDest, "%s%s", directoryPath, dest);

    char str[100];
	sprintf(str, "RENAME::%s::%s", source, dest);
	logging1("terdecode",fileSource, fileDest);
	int result;
	result = rename(fileSource, fileDest);
	
	if (result == -1){
			return -errno;
	}

	return 0;
}
//File open operation
static int xmp_open(const char *path, struct fuse_file_info *fi){ 

	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	int result;
	result = open(newPath, fi->flags);
	
	if (result == -1){
		return -errno;
	}
		
	close(result);
	return 0;
}
//Read data from open file
static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){ 
	
	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	int fd;
	int result;

	(void) fi;
	fd = open(newPath, O_RDONLY);
	if (fd == -1) {
		return -errno;
	}
	
	result = pread(fd, buf, size, offset);
	
	if (result == -1) {
		result = -errno;
	}
		
	close(fd);
	return result;
}
//write data to an open file
static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) { 
	
	char newPath[1000];
	sprintf(newPath, "%s%s", directoryPath, path);
	
    int fd;
	int result;
	(void) fi;
	fd = open(newPath, O_WRONLY);
	
	if (fd == -1){
		return -errno;
	}
		
    char str[100];
	sprintf(str, "WRITE::%s", path);
	result = pwrite(fd, buf, size, offset);
	
	if (result == -1){
		result = -errno;
	}
	
	close(fd);
	return result;
}
static struct fuse_operations xmp_oper = {

	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.open = xmp_open,
	.write = xmp_write,

};

int  main(int  argc, char *argv[]){
	
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);

}