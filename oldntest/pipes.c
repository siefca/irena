#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main (int argc, char *argv[])
{
  char buf[256];
  int pip_r[2];
  int pip_w[2];
  size_t z;
  pid_t x;

  if ((pipe(pip_r))+(pipe(pip_w)) < 0)
  {
    printf ("aaaa\n");
    exit (1);
  }
  
  x = fork();
  
  if (x == -1) 
    {
	printf("bbb\n");
	exit(2);
    }
  if (!x)
    {
	printf ("hello, i am child\n");
      close(pip_r[0]); close(pip_w[1]);
	printf ("after close\n");
      if ((dup2(pip_w[0], 0)) == -1 || (dup2(pip_r[1], 1)) == -1)
	{
	    printf ("child - dup2\n"); 
	    perror("dup2");
	    exit (3);
	}
	
        if ((execlp("./test.sh", "TEST", NULL)) == -1)
	{
	    fprintf (stderr, "child - execlp\n");
	    perror("execlp");
	    exit (5); 
	}
	else
	{
	    fprintf(stderr, "execlp chujnia\n");
	    perror("aaa");
	    exit (1);
	
	}
    
      exit (0);
    }
  else
    {
      close(pip_r[1]); close(pip_w[0]);
      
      while ((z=read(pip_r[0], buf, 255)) > 0)
      {
	    buf[z] = 0;
	    fprintf(stderr, "was: %s", buf);
      }

        if (!z)
	{
	    printf("EOF from child!\n");
	    exit(0);
	}
	else if (z == -1)
	{
	    perror("read");
	    exit(10);
	}

      fprintf (stderr, "chld has exited\n");
      perror ("dupa");
      sleep (2);
    }

  exit(0);
}

