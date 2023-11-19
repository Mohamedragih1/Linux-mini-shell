 
/*
 * CS354: Shell project
 *
 * Template file.
 * You will need to add more code here to execute the command table.
 *
 * NOTE: You are responsible for fixing any bugs this code may have!
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include "y.tab.h"
#include <time.h>

#include "command.h"

// to handle ctrl+c
void handleControl(int signal)
{
	printf("\n");
}

SimpleCommand::SimpleCommand()
{
	// Creat available space for 5 arguments
	_numberOfAvailableArguments = 5;
	_numberOfArguments = 0;
	_arguments = (char **) malloc( _numberOfAvailableArguments * sizeof( char * ) );
}

void
SimpleCommand::insertArgument( char * argument )
{
	if ( _numberOfAvailableArguments == _numberOfArguments  + 1 ) {
		// Double the available space
		_numberOfAvailableArguments *= 2;
		_arguments = (char **) realloc( _arguments,
				  _numberOfAvailableArguments * sizeof( char * ) );
	}
	
	_arguments[ _numberOfArguments ] = argument;

	// Add NULL argument at the end
	_arguments[ _numberOfArguments + 1] = NULL;
	
	_numberOfArguments++;
}

Command::Command()
{
	// Create available space for one simple command
	_numberOfAvailableSimpleCommands = 1;
	_simpleCommands = (SimpleCommand **)
		malloc( _numberOfSimpleCommands * sizeof( SimpleCommand * ) );

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append=0;
}

void
Command::insertSimpleCommand( SimpleCommand * simpleCommand )
{
	if ( _numberOfAvailableSimpleCommands == _numberOfSimpleCommands ) {
		_numberOfAvailableSimpleCommands *= 2;
		_simpleCommands = (SimpleCommand **) realloc( _simpleCommands,
			 _numberOfAvailableSimpleCommands * sizeof( SimpleCommand * ) );
	}
	
	_simpleCommands[ _numberOfSimpleCommands ] = simpleCommand;
	_numberOfSimpleCommands++;
}

void
Command:: clear()
{
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		for ( int j = 0; j < _simpleCommands[ i ]->_numberOfArguments; j ++ ) {
			free ( _simpleCommands[ i ]->_arguments[ j ] );
		}
		
		free ( _simpleCommands[ i ]->_arguments );
		free ( _simpleCommands[ i ] );
	}

	if ( _outFile ) {
		free( _outFile );
	}

	if ( _inputFile ) {
		free( _inputFile );
	}

	if ( _errFile ) {
		free( _errFile );
	}

	_numberOfSimpleCommands = 0;
	_outFile = 0;
	_inputFile = 0;
	_errFile = 0;
	_background = 0;
	_append=0;
}

void
Command::print()
{
	printf("\n\n");
	printf("              COMMAND TABLE                \n");
	printf("\n");
	printf("  #   Simple Commands\n");
	printf("  --- ----------------------------------------------------------\n");
	
	for ( int i = 0; i < _numberOfSimpleCommands; i++ ) {
		printf("  %-3d ", i );
		for ( int j = 0; j < _simpleCommands[i]->_numberOfArguments; j++ ) {
			printf("\"%s\" \t", _simpleCommands[i]->_arguments[ j ] );
		}
		printf("\n");
	}

	printf( "\n\n" );
	printf( "  Output       Input        Error        Background\n" );
	printf( "  ------------ ------------ ------------ ------------\n" );
	printf( "  %-12s %-12s %-12s %-12s\n", _outFile?_outFile:"default",
		_inputFile?_inputFile:"default", _errFile?_errFile:"default",
		_background?"YES":"NO");
	printf( "\n\n" );
	
}

void handleChildTermination(int signal) {
	//the file make append 
   FILE *logFile = fopen("log.txt", "a");
   if (logFile != NULL) {
        time_t currentTime;
        struct tm *tmInfo;
        char timeString[100];
        time(&currentTime);
        tmInfo = localtime(&currentTime);
        strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", tmInfo);
       fprintf(logFile, "%s - Child process terminated\n", timeString);
       fclose(logFile);
   }
}

void
Command::execute()
{
	// Don't do anything if there are no simple commands
	if ( _numberOfSimpleCommands == 0 ) {
		prompt();
		return;
	}

	// Print contents of Command data structure
	print();

	// Add execution here
	
	int defaultin =dup(0);
	int defaultout =dup(1);
	int defaulterr =dup(2);
	
	
	
	// For every simple command fork a new process
	int outfd, infd;
	if (_outFile != 0)
	{	if(_append !=0)
			outfd = open(_outFile,O_WRONLY | O_APPEND | O_CREAT,0777);	
		else 
			outfd = creat(_outFile,0777);
		if(outfd < 0)
		{
		perror("file not created");
		//exit(2);
		}

	}
	if(_inputFile !=0)
	{
		infd= open(_inputFile,O_RDONLY,0777);
		if(infd < 0)
		{
		perror("file not exist");
		//exit(2);
		}

	}
	
	int fdpipe[_numberOfSimpleCommands][2];
	for( int i=0;i < _numberOfSimpleCommands ; i++)
	{
		if (strcmp(_simpleCommands[i]->_arguments[0], "cd") == 0) {
			   char *dir = NULL;
			   if (_simpleCommands[i]->_numberOfArguments > 1) {
			       dir = _simpleCommands[i]->_arguments[1];
			   } else {
			       dir = getenv("HOME");
			   }
			   if (chdir(dir) != 0) {
			       perror("chdir failed");
			   }
       		}
	       else
	       {
			pipe(fdpipe[i]);
			if(i ==0)
			{	if (_inputFile)
				{
				dup2(infd , 0);
				close(infd);
				}
				else
					dup2(defaultin , 0);
				
			}
			else 
			{
				dup2(fdpipe[i-1][0],0);
				close(fdpipe[i-1][0]);
			}
			if (i == _numberOfSimpleCommands-1)
			{
			 	if(_outFile)
			 	{
			 		dup2(outfd , 1);
					//close(outfd);
			 	}
			 	else 
			 		dup2(defaultout , 1);
			}
			else
			{
				dup2(fdpipe[i][1],1);
				close(fdpipe[i][1]);
			}
			
			int pid =fork();
			if (pid==-1)
			{
			
				perror(" ls: fork\n");
				exit(2);
			
			}
			
			if(pid == 0 )
			{	
				// child
				close(fdpipe[i][0]);
				close(fdpipe[i][1]);
				close(defaultin);
				close(defaultout);
				close(defaulterr);
				execvp( _simpleCommands[i]->_arguments[0] , &_simpleCommands[i]->_arguments[0] );
				perror("Error in fork\n");
				exit(2);
			}
			// parent
			signal(SIGCHLD, handleChildTermination);
			//check for background (&)
			if (_background==0)
				waitpid(pid , 0 , 0 );
		}
		
	}
	dup2(defaultin , 0);
	dup2(defaultout , 1);
	dup2(defaulterr , 2);
	
	close(defaultin);
	close(defaultout);
	close(defaulterr);
	

	// Setup i/o redirection
	// and call exec

	// Clear to prepare for next command
	clear();
	
	// Print new prompt
	prompt();
}


// Shell implementation

void
Command::prompt()
{
	printf("myshell>");
	fflush(stdout);
}

Command Command::_currentCommand;
SimpleCommand * Command::_currentSimpleCommand;



int yyparse(void);

int 
main()
{	
	signal(SIGINT,handleControl);
	Command::_currentCommand.prompt();
	yyparse();
	return 0;
}

