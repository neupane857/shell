#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports four arguments
#define maxPid 15
#define History 15

int main()
{

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );
  char history[History][MAX_NUM_ARGUMENTS];
  int historyCount = 0;
  int historyIndex = 0;
  pid_t PID;

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );


    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      token[i] = NULL;
    }
    strcpy(history[historyCount], command_string);
    historyCount++;
    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr = NULL;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;
    if(historyIndex<=14)
    {
      historyIndex++;
    }
    if(historyCount > 14)
    {
      historyCount = 0;
    }

    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    if(command_string[0] == '!')
    {
      char Number = command_string[1]; 
      int commandLineParameter = Number - '0';
      command_string = history[commandLineParameter];
    }
    

    // Now print the tokenized input as a debug check
    // \TODO Remove this for loop and replace with your shell functionality
    if(strcmp("cd", token[0]) == 0) 
    {
      if(chdir(token[1]) == -1 || token[1] != NULL) // this line indicates that if the directory after cd isn't there then it willl print no directory
      {
        printf( "-msh: cd: %s: No such file or directory\n" ,token[1]);
      }
      else 
      {
        chdir(token[1]);
      }  
    }
    else if(strcmp("history", token[0]) == 0) // This part of the code will look for the last 15 commands that was written and will print that
    {     
      for(int i = 0; i<historyIndex; i++)
      {
        printf("%d %s\n", i, history[i]);
      }
    }
    else if((strcmp("exit", token[0])==0) || (strcmp("quit",token[0]) == 0))
    {
      exit(0);
    }
    else
    {
      PID = fork();   
      if(PID == 0)
      {
        int returnExecvp;
        returnExecvp = execvp(token[0], &token[0]);
        if(returnExecvp == -1)
        {
          printf("%s: Command not found.\n", token[0]);
        }
        exit(0);
      }
      else
      {
        int status;
        wait(&status);
      }
    }       
    // Cleanup allocated memory
    for( int i = 0; i < MAX_NUM_ARGUMENTS; i++ )
    {
      if( token[i] != NULL )
      {
        free( token[i] );
      }
    }
    free( head_ptr );
  }
  free( command_string );

  return 0;
  // e2520ca2-76f3-90d6-0242ac120003
}
