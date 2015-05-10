#include<stdio.h>
#include<readline/readline.h>
#include<string.h>
#include <unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
//I think our loops to malloc and free are not going to the end of the array because sizeof doesnt work in this case
//if you type a space before ls it breaks! 

void stringProcess(char *command,  char *args[50], int *backgrounding)
{ 
  char input[1024];
  char word[1024];
  int count = 0;
  int i = 0;
  int newWordCount = 0;
  *backgrounding = -1;
  

  //empties all args so they no longer hold the values of the last user command
  int j = 0;
 /*for(j = 0; j < 1024; j++)
    {
      
      free(args[j]);
      }*/

  //re alocates memory 
  j = 0;
  for(j = 0; j < 50; j++)
    {

      args[j] = malloc(sizeof args);
      memset(args[j],0,sizeof args);
      
    }
  // printf("size of args = %lu", sizeof args);
  //memset(args,0,sizeof args);
  //gets new user command
  fgets(input,sizeof input,stdin);
  //fills args with the command split up into "words"
  char *newLineChar = "\n";
  for (i = 0; strcmp(&input[i-1], newLineChar) != 0; i++)
    {
      if (input[i] == ' ' || input[i] == '\0' || input[i] == *newLineChar)
	{
	  
	  word[count] = '\0';
	  if(strcmp(word, "") != 0)
	    {
	      //printf("word = %s\n",word);
	      //args[newWordCount] = word;
	      strcpy(args[newWordCount], word);
	    
	      count = 0;
	    
	    
	      if(newWordCount == 0)
		{
		  //*command = *word;
		  strcpy(command, word);
		}
	      
	      newWordCount++;
	      memset(word,'\0',sizeof word);
	    }
	  if(input[i] == '\n' || input[i] == '\0')
	    {
	      args[newWordCount] = NULL;
	    }
	}
      else if(input[i] == '&')
	{
	  *backgrounding = 1;
	  continue;
	}
      else
	{
	  word[count] = input[i];
	  count++;
	}
      
    }
  //args[newWordCount+1] = NULL;


  //printf("method must be <%s> \nIn method: %s\n",command,args[0]);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int commandExecutor(char *command,  char *args[50], int *backgrounding)
{
  //printf("comEx %d\n",*backgrounding);
  //printf("In comEx\n");
  int status = 0;
  //IO REDIRECTION*****************************************************
  int IOIndex = 0;
  int IOBool = 0;
  int PID = -1;
  for (IOIndex = 0; args[IOIndex] != NULL; IOIndex++)
    {
      if(strcmp(args[IOIndex],"1>") == 0 || strcmp(args[IOIndex],"2>") == 0 || strcmp(args[IOIndex],"<") == 0)
	{
	  IOBool = 1;
	  if (PID != 0 && (PID = fork()) < 0)
	    {
	      printf("ERROR: fork of child process failed.\n");
	      exit(1);
	    }
	  //If am parent break out of loop once IO Redirect is found
	  if(PID > 0)
	    {
	      break;
	    }
	  else if(PID == 0 && strcmp(args[IOIndex],"1>") == 0)
	    {
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT, 0666);
	      assert(FID>=0);
	      close(1);
	      dup(FID);
	      close(FID);
	      args[IOIndex] = NULL;
	      continue;
	    }
	  else if(PID == 0 && strcmp(args[IOIndex],"2>") == 0)
	    {
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT, 0666);
	      assert(FID>=0);
	      close(2);
	      dup(FID);
	      close(FID);
	      args[IOIndex] = NULL;
	      continue;
	    }
	  else if(PID == 0 && strcmp(args[IOIndex],"<") == 0)
	    {
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT, 0666);
	      assert(FID>=0);
	      close(0);
	      dup(FID);
	      close(FID);
	      args[IOIndex] = NULL;
	      continue;
	    }
	}
    }
  //printf("Exec in IORedirect\n");
  if(IOBool == 1)
    {
      if (PID == 0)
	{
	  if( execvp(*args,args) < 0)
	    {
	      printf("ERROR: execvp failed in IO section\n");
	      exit(1);
	    }

	}
      else if (PID != 0)
	{
	  if (*backgrounding == -1)
	    {
	      wait(&status);
	    }
	  //printf("about to return\n");
	  return 0;
	}
    }


  //IOREDIRECTION END*********************************************************************
 

  //PIPES!!!*****************************************************************************
  int pipeIdx = -1;
  for (IOIndex = 0; args[IOIndex] != NULL; IOIndex++)
    {
      if(strcmp(args[IOIndex],"|") == 0)
	{
	  args[IOIndex] = NULL;
	  pipeIdx = IOIndex + 1; 
	}      
    }
  if(pipeIdx != -1)
    {

      int dArr[2];
      int PID = -1;
      int PID2 = -1;
      pipe(dArr);
      if((PID = fork())< 0 )
	{
	  printf("ERROR: Fork failed");
	  exit(1);
	}
      else if(PID == 0)
	{
	  if((PID2 = fork()) < 0)
	    {
	      printf("ERROR: Fork failed");
	      exit(1);
	    }
	  else if(PID2 == 0)//right arg
	    {
	      close(dArr[1]);
	      dup2(dArr[0], STDIN_FILENO);
	      //close(dArr[0]);

	      //printf("about to execvp the youngest\n");
	      if(execvp(args[pipeIdx], &args[pipeIdx]) < 0)
		{
		  printf("command not found: %s\n", args[pipeIdx]);
		  exit(1);
		}
	      //execvp(*args, args);
	    }
	  //if middle person
	  else//left arg
	    {
	      close(dArr[0]);
	      dup2(dArr[1], STDOUT_FILENO);
	      printf("%d\n", STDOUT_FILENO);
	      //close(dArr[1]);


	      //printf("Waiting for youngest\n");
	      //waitpid(PID2, &status2, 0);
	      //printf("finished wiating for youngest\n");
	      //printf("about to execvp the middle\n");
	      if(execvp(*args, args) < 0)
		{
		  printf("command not found: %s\n", *args);
		  exit(1);
		}
	      //execvp(args[pipeIdx], &args[pipeIdx]);
	    }
	}
      //if you're the big mama
      else
	{
	  if (*backgrounding == -1)
	    {
	      //printf("waiting for middle \n");
	      waitpid(PID2, &status, 0);
	    }
	  //printf("finished waiting for middle");
	  return 0;
	}
    }
  



  //PIPES END****************************************************************************
  //printf("Command is >%s<\n",command);
    //**case cd
  if(strcmp(command, "cd") == 0)
    {
      //code for cd input
      if (chdir(args[1]) == -1)
	{
	  //failed
	  printf("Directory not found: %s\n", args[1]);
	}
    }
  //**case pwd
  else if(strcmp(command, "pwd") == 0)
    {
      char p[1024];
      //pwd code
      getcwd(p,sizeof(p));
      printf("%s\n",p);
    }
  //defualt bad input
  else
    {
      int child_id = fork();
      if (child_id == -1)
	{
	  //failed fork
	  printf("Process not executed. Please terminate some processes and try again.\n");
	}
      else if(child_id == 0)
	{

	  if(execvp(command, args) < 0)
	    {
	      //printf("ERROR could not execvp\n");
	      printf("Command not found: %s\n", command);
	      exit(1);
	    }
	}
      else
	{
	  //printf("parent waiting\n");
	  //this is the parent... wait()
	  //  while(wait(&status) != child_id)
	  // ;
	  if(*backgrounding == -1)
	    {
	      wait(&status);
	    }
	}
    }
  return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int main(void) 
{  
  char path[1024] = "/";
  //lets user know tosh has started
  printf("Starting Tosh...\n");

  //0 means keep running 1 means stop
  int exit = 0;

  while (exit != 1)//the beating heart of tosh
    {
      //get the current working directory
      getcwd(path, sizeof(path));
      //initilize the array of strings which holds the users command 
      char *args[50];
      char command[1024] = "";
      int backgrounding = -1;
      //prints the tosh prompt
      printf("[%s]tosh$ ",path);
      
      //takes in command from user and puts each word of it into an array
      stringProcess(command,args, &backgrounding);
      
      //if user types exit or q the program will stop
      if(args[0] == NULL)

	{
	  continue;
	}
      else if (strcmp(command,"exit") == 0 || strcmp(args[0],"q\n") == 0)
	{
	  exit = 1;
	  continue;//leave the rest of the code and exit basically
	}

      else
	{
	  commandExecutor(command,args,&backgrounding);
	}
    }
  return 0;
}
/*
  if & is present store new PID array, and run prog. Thgen during every iteration of the loop
  check the prog array PID status for any terminated progs and waith those that have terminated.


 
  operjn the file
  close the stream thats not used
  dup(fileID)
  close(FileID)

  execvp

if ((PID = fork()) < 0)
  {
    printf("ERROR: fork of child process failed.\n");
    exit(1);
  }
 else if (PID == 0)
   {
     //redirect
     int FID = open("temp.txt", O_WRONLY | O_CREAT,0666);
     assert(FID>=0);
     close(1);
     dup(FID);
     close(FID);
     if( execvp(*args,args)<0)
       {
	 printf("ERROR: execvp failed\n");
	 exit(1);
       }
   }
*/
