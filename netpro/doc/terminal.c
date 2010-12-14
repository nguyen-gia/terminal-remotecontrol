#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
void parse(char *line, char *argv[])
{
  int argc;
  argc = 0;
  while(*line)
    {
      argv[argc++] = line;
      while(*line && *line>' ') line++;
      while(*line && *line<=' ')
	*line++ = '\0';
    }
  argv[argc]= NULL;
}

void execute(char *argv[])
{
  int pid,status;
  pid = fork();
  if(pid<0)
    {
      printf("fork error\n");
      exit(1);
    }
  if(pid == 0)
    {
      execve(argv[0],argv,NULL);
      printf("%s error\n", argv[0]);
      exit(1);
    }
  while(wait(&status) != pid);
}
int main()
{
  char line[512];
  char *argv[16];
  while(1)
    {
      printf("$");
      fgets(line,512,stdin);
      parse(line,argv);
      execute(argv);
    }
}
