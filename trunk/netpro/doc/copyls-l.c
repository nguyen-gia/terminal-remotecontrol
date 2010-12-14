#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int main(int argc, char *argv[])
{
  FILE *fp;
  char line[128];
  char path[128];
  char *check;
  if(chdir(argv[2])!=0)
    {
      printf("Loi me no roi\n");
      return 1;
    }
  if((check=getwd(path))==NULL)
    {
      printf("Loi tiep:(\n");
      return 1;
    }
  strcat(path,"/Test");
  puts(path);
  if(mkdir(path)!=0)
    {
      printf("Loi, ko mkdir dc, shit\n");
      return 1;
    }
  if((fp=popen(argv[1],"r"))==NULL)
    {
      printf("loi\n");
      return 1;
    }
  while(fgets(line,sizeof(line),fp)!=NULL)
    {
      printf("%s",line);
    }

  return 0;
}
