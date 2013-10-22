#include<stdio.h>
#include<readline/readline.h>
#include<string.h>
#include <unistd.h>

void stringProcess(char command[1024], char args[30][1024])
{ 
  char input[1024];
  int count = 0;
  int i = 0;
  int newWordCount = 0;

  //empties all args so they no longer hold the values of the last user command
  int j = 0;
  for(j = 0; j < sizeof args; j++)
    {
      memset(args[j], 0, sizeof args[j]);
    }
  //gets new user command
  fgets(input,sizeof input,stdin);
  //fills args with the command split up into "words"
  char *newLineChar = "\n";
  for (i = 0; strcmp(&input[i], newLineChar) != 0; i++)
    {
	if (input[i] == ' ')
	  {
	    //sanatise(args[newWordCount])
	    count = 0;
	    newWordCount++;
	  }
	else if(newWordCount == 0)
	  {
	    command[count] = input[i];
	    count++;
	  }
	else
	  {
	    args[newWordCount-1][count] = input[i];
	    count++;
	  }
      
    }


  //printf("in method: %s\n",args[0]);
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa


int commandExecutor(char command[1024], char args[30][1024])
{

  //**case cd
  if(strcmp(command, "cd") == 0)
    {
      //code for cd input
      if (chdir(args[0]) == -1)
	{
	  //failed
	  printf("Directory not found: /%s\n", args[1]);
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
      printf("Command not found: %s\n", command);
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
      char args[30][1024];
      char command[1024];

      //prints the tosh prompt
      printf("[%s:tosh]:)",path);
      
      //takes in command from user and puts each word of it into an array
      stringProcess(command,args);
      
     //if user types exit or q the program will stop
      if (strcmp(command,"exit") == 0 || strcmp(args[0],"q\n") == 0)
	{
	  exit = 1;
	  continue;//leave the rest of the code and exit basically
	}
      else
	{
	  commandExecutor(command,args);
	}
    }
  return 0;
}
