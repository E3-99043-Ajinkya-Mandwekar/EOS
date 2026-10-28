#include<stdio.h>
#include<stdlib.h>
#include<dirent.h>
#include<string.h>

int main(){

char dirpath[256];

printf("Directory path: ");

if(fgets(dirpath, sizeof(dirpath), stdin) == NULL)
{
  printf("ERROR");
  
  return 1;
  
 }
 
 dirpath[strcspn(dirpath, "\n")] = '\0';
 
 
 DIR *dp = opendir(dirpath);
 
 if(dp == NULL)
 {
  perror("ERROR");
  return 2;
 }
 
 struct dirent *ent;
 printf("Contents of directory : '%s' = \n", dirpath);
 
 while((ent = readdir(dp)) != NULL)
 {
  printf("%s\n", ent->d_name);
  }
  
  closedir(dp);
  
  return 0;
  
  }
  
  
 
 
 
