// ACADEMIC INTEGRITY PLEDGE
//
// - I have not used source code obtained from another student nor
//   any other unauthorized source, either modified or unmodified.
//
// - All source code and documentation used in my program is either
//   my original work or was derived by me from the source code
//   published in the textbook for this course or presented in
//   class.
//
// - I have not discussed coding details about this project with
//   anyone other than my instructor. I understand that I may discuss
//   the concepts of this program with other students and that another
//   student may help me debug my program so long as neither of us
//   writes anything during the discussion or modifies any computer
//   file during the discussion.
//
// - I have violated neither the spirit nor letter of these restrictions.
//
//
//
// Signed: Jaret Varn Date: 10/11/20

// 3460:426 Lab 1 - Basic C shell rev. 9/10/2020

/* Basic shell */

/*
 * This is a very minimal shell. It finds an executable in the
 * PATH, then loads it and executes it (using execv). Since
 * it uses "." (dot) as a separator, it cannot handle file
 * names like "minishell.h"
 *
 * The focus on this exercise is to use fork, PATH variables,
 * and execv. 
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define MAX_ARGS		64
#define MAX_ARG_LEN		16
#define MAX_LINE_LEN	80
#define WHITESPACE		" ,\t\n"

struct command_t {
   char *name;
   int argc;
   char *argv[MAX_ARGS];
};

int foxPid, fStatus, chPid;

/* Function prototypes */
int parseCommand(char *, struct command_t *);
void printPrompt();
void readCommand(char *);
int manPage();
int getCmd(struct command_t *);

int main(int argc, char *argv[]) {
   int pid;
   int status;
   char cmdLine[MAX_LINE_LEN];
   struct command_t command;

   while (1) {
      printPrompt();
      /* Read the command line and parse it */
      readCommand(cmdLine);
      parseCommand(cmdLine, &command);
      command.argv[command.argc] = NULL;
	  
      /* Create a child process to execute the command */
      if (*command.name != 0) // Check to make sure there actually is a command
      {
         if ((pid = fork()) == 0) {
            /* Child executing command */
            execvp(command.name, command.argv);
            perror(command.name); return -1;
         }
         wait(&status); 
      }
      /* Wait for the child to terminate */
      
   }

   /* Shell termination */
   printf("\n\n shell: Terminating successfully\n");
   return 0;
}

/* End basic shell */

int manPage() // Function that prints out the manual for the shell
{
   const int MANLENGTH = 128;
   char* text[MANLENGTH];

   text[0] = "\t\tBasic C shell written for OS\n";
   text[1] = "\nThis shell executes basic commands that are listed below.\n";
   text[2] = "Commands are executed by first typing the command, and a space before any additional arguments for the command.\n";
   text[3] = "\n\t\tList of available commands:\n";

   text[4] = "Name\n";
   text[5] = "\tC\n";
   text[6] = "Arguments\n";
   text[7] = "\tfile1 file2\n";
   text[8] = "Description\n";
   text[9] = "\tCopies all bytes of file1 to file2 without deleting file1\n\n";

   text[10] = "Name\n";
   text[11] = "\tD\n";
   text[12] = "Arguments\n";
   text[13] = "\tfile\n";
   text[14] = "Description\n";
   text[15] = "\tDelete the named file\n\n";

   text[16] = "Name\n";
   text[17] = "\tE\n";
   text[18] = "Arguments\n";
   text[19] = "\tcomment\n";
   text[20] = "Description\n";
   text[21] = "\tDisplays comment on screen followed by a new line\n\n";

   text[22] = "Name\n";
   text[23] = "\tH\n";
   text[24] = "Description\n";
   text[25] = "\tDisplays this manual\n\n";

   text[26] = "Name\n";
   text[27] = "\tL\n";
   text[28] = "Description\n";
   text[29] = "\tDisplays the current directory and then lists directory contents in long form\n\n";

   text[30] = "Name\n";
   text[31] = "\tM\n";
   text[32] = "Arguments\n";
   text[33] = "\tfile\n";
   text[34] = "Description\n";
   text[35] = "\tCreates a named text file by launching nano\n\n";

   text[36] = "Name\n";
   text[37] = "\tP\n";
   text[38] = "Arguments\n";
   text[39] = "\tfile\n";
   text[40] = "Description\n";
   text[41] = "\tDisplays the contents of the named file\n\n";

   text[42] = "Name\n";
   text[43] = "\tQ\n";
   text[44] = "Description\n";
   text[45] = "\tQuits the shell\n\n";

   text[46] = "Name\n";
   text[47] = "\tS\n";
   text[48] = "Description\n";
   text[49] = "\tSurf the web by launching a browser as a background process\n\n";

   text[50] = "Name\n";
   text[51] = "\tW\n";
   text[52] = "Description\n";
   text[53] = "\tClear the screen\n\n";

   text[54] = "Name\n";
   text[55] = "\tX\n";
   text[56] = "Arguments\n";
   text[57] = "\tprogram\n";
   text[58] = "Description\n";
   text[59] = "\tExecutes the named program\n\n";

   int i=0;
   for (i; i <= 59; i++)
   {
      printf("%s", text[i]);
   }
   return 0;
}

/* Change command name from user given name */
int getCmd(struct command_t *cmd) // This function converts the user inputs into linux commands
{
   if (!strcmp(cmd->name, "C")) 
   {
      cmd->name = "cp"; // Copy file1 to file2
   }
   else if (!strcmp(cmd->name, "D"))
   {
      cmd->name = "rm"; // remove file1
   }
   else if (!strcmp(cmd->name, "E"))
   {
      if (cmd->argv && cmd->argv[2] != 0) // Check to make sure 'comment' argument isn't empty
      {
         cmd->name = "echo";
      }
      else
      {
         cmd->name = "\0";
      }
      
   }
   else if (!strcmp(cmd->name, "H"))
   {
      manPage(); // Print out the manual page
      cmd->name = "\0";
   }
   else if (!strcmp(cmd->name, "L"))
   {
      int status;
      int pid = fork();
      if (pid == 0) // Create a new process and execute the 'pwd' program 
      {
         execl("/bin/pwd", "pwd", (char *) NULL);
         perror(cmd->name); return -1;
      }
      wait(&status); // Wait for child process to complete

      printf("\n");

      pid = fork();
      if (pid == 0) // Create a new process and execute the 'ls' command with additional argument '-l'
      {
         execl("/bin/ls", "ls", "-l", (char *) NULL);
         perror(cmd->name); return -1;
      }
      wait(&status); // Wait for child process
      cmd->name = "\0"; // Everything is done here so just 'pass' the command from executing
   }
   else if (!strcmp(cmd->name, "M"))
   {
      cmd->name = "nano"; // Open the nano text editor
   }
   else if (!strcmp(cmd->name, "P"))
   {
      cmd->name = "cat"; // Display contents of a file
   }
   else if (!strcmp(cmd->name, "Q"))
   { 
      exit(0);
   }
   else if (!strcmp(cmd->name, "S"))
   {
      foxPid = fork();
      if (foxPid > 0)
      {
         chPid = waitpid(foxPid, &fStatus, WNOHANG);
      }
      else
      {
         execl("/bin/firefox", "firefox", (char *) NULL);
         perror(cmd->name); return -1;
      }
      if (chPid == foxPid) // I could not get killing extra firefox applications working, but it can allow multiple instances of firefix running in the background. 
      {
         printf("Detecting firefox termination. \n");
         kill(foxPid, SIGTERM); foxPid = 0;
      }
      cmd->name = "\0"; 
   }
   else if (!strcmp(cmd->name, "W"))
   {
      cmd->name = "clear"; // Clear display
   }
   else if (!strcmp(cmd->name, "X"))
   {
      cmd->name = cmd->argv[1]; // Load the program name into cmd->name
      int i = 1;
      for (i; cmd->argv[i + 1]; i++) // Go through the arguments and shift them over to accomidate the program as the first argument
      {
         cmd->argv[i] = cmd->argv[i + 1];
      }
   }
}

/* Parse Command function */

/* Determine command name and construct the parameter list.
 * This function will build argv[] and set the argc value.
 * argc is the number of "tokens" or words on the command line
 * argv[] is an array of strings (pointers to char *). The last
 * element in argv[] must be NULL. As we scan the command line
 * from the left, the first token goes in argv[0], the second in
 * argv[1], and so on. Each time we add a token to argv[],
 * we increment argc.
 */
int parseCommand(char *cLine, struct command_t *cmd) {
   int argc;
   char **clPtr;
   /* Initialization */
   clPtr = &cLine;	/* cLine is the command line */
   argc = 0;
   cmd->argv[argc] = (char *) malloc(MAX_ARG_LEN);
   /* Fill argv[] */
   while ((cmd->argv[argc] = strsep(clPtr, WHITESPACE)) != NULL) {
      cmd->argv[++argc] = (char *) malloc(MAX_ARG_LEN);
   }

   /* Set the command name and argc */
   cmd->argc = argc-1;
   cmd->name = (char *) malloc(sizeof(cmd->argv[0]));
   strcpy(cmd->name, cmd->argv[0]);
   getCmd(cmd);
   return 1;
}

/* End parseCommand function */

/* Print prompt and read command functions - Nutt pp. 79-80 */

void printPrompt() {
   /* Build the prompt string to have the machine name,
    * current directory, or other desired information
    */
   char* promptString = "linux jav76|>";
   printf("%s ", promptString);
}

void readCommand(char *buffer) {
   /* This code uses any set of I/O functions, such as those in
    * the stdio library to read the entire command line into
    * the buffer. This implementation is greatly simplified,
    * but it does the job.
    */
   fgets(buffer, 80, stdin);
}

/* End printPrompt and readCommand */