      /* //printf("%d", IOIndex);
      
      // if 1>
      if (strcmp(args[IOIndex],"1>") == 0 && args[IOIndex + 1] != NULL)
	{
	  args[IOIndex] = NULL;
	  printf("%s is the values of args\n", args[IOIndex]);
	  IOBool = 1;
	  //short circuit if not parent
	  if (PID != 0 && (PID = fork()) < 0)
	    {
	      if(PID != 0)
		{
		  printf("ERROR: fork of child process failed.\n");
		}
	      exit(1);
	    }
	  else if (PID == 0)
	    {
	      printf("im a child\n");
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT, 0666);
	      assert(FID>=0);
	      close(1);
	      dup(FID);
	      close(FID);
	      continue;
	    }
	}
      // if 2>
      else if (strcmp(args[IOIndex],"2>") == 0 && args[IOIndex + 1] != NULL)
	{
	  args[IOIndex] = NULL;
	  IOBool = 1;
	  if (PID != 0 && (PID = fork()) < 0)
	    {
	      if(PID != 0)
		{
		  printf("ERROR: fork of child process failed.\n");
		}
	      exit(1);
	    }
	  else if (PID == 0)
	    {
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT,0666);
	      assert(FID>=0);
	      close(2);
	      dup(FID);
	      close(FID);

	      continue;	      
	    }
	}
      // if <
      else if (strcmp(args[IOIndex],"<")  == 0 && args[IOIndex + 1] != NULL)
	{
	  args[IOIndex] = NULL;
	  IOBool = 1;
	  if (PID != 0 && (PID = fork()) < 0)
	    {
	      if(PID != 0)
		{
		  printf("ERROR: fork of child process failed.\n");
		}
	      exit(1);
	    }
	  else if (PID == 0)
	    {
	      //redirect
	      int FID = open(args[IOIndex + 1], O_WRONLY | O_CREAT,0666);
	      assert(FID>=0);
	      close(3);
	      dup(FID);
	      close(FID);

	      break;
	    }
	    }
    }
  if(IOBool > 0)
    {
      //printf("Exec in IORedirect\n");
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
	  wait(&status);
	  printf("about to return\n");
	  return 0;
	}
	}*/
