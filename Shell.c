#include "blackdos.h"

void processCommand(char*, char*);
void extractCommand(char*, char*);
int strCmp(char*, char*);
void strCpy(char*, char*, int);
void extractSingleArg(char*, char*);
void extractTwoArgs(char*, char*, char*);
int findFirstOf(char*, char);
int findNthOf(char*, char, int);
void zeroOut(char*, int);
int isUpper(char);
void displayFiles(char*);
int strLen(char*);
int sectorsUsed(char*);
int aToI(char*);


void main() {
  char command[128], config[512];
  int fg, bg;

  /*read the configuration values into local variables*/
  interrupt(33,2,config,258,0);

  /*clear the screen*/
  interrupt(33,12,config[0]+1,config[1]+1,0);

  PRINTS("Welcome to BlackDOS\r\n\0");
  PRINTS("NOTE: The help manual broke printing, so it's gone for now.\r\n\0");
  PRINTS("Running programs doesn't play nice since the stop function reboots the shell.\r\n\0");

  while(1) {
    PRINTS("blackdos~(__^>\0");
    SCANS(command);
    PRINTS("\r\n\0");
    /*just manually chop out a carriage return i guess. there's an extra*/
    command[strLen(command) - 1] = 0;
    processCommand(command, config);
  }
}

/* copy a number of characters from one string to another */
void strCpy(char* from, char* to, int bytes) {
  int i = 0;
  while(i < bytes) {
    to[i] = from[i];
    ++i;
  }
  return;
}

void processCommand(char* cmd, char* cfg) {
  char cmdWord[7], arg1[60], arg2[60];
  char directory[512], map[512];
  char comment[100];
  int sectors;
  int c;
  zeroOut(arg1, 60);
  zeroOut(arg2, 60);
  /*longest command is 6 chars + nullterm*/
  /*any command has a maximum of 2 arguments*/
  extractCommand(cmd, cmdWord);

  if(strCmp(cmdWord, "boot\0") == 0) {
    /* boot
       reboot the system*/
    interrupt(33, 11, 0, 0, 0);
  }
  else if(strCmp(cmdWord, "cls\0") == 0) {
    /* cls
       clear the screen*/
    interrupt(33, 12, cfg[0]+1, cfg[1]+1,0);
  }
  else if(strCmp(cmdWord, "copy\0") == 0) {
    /* copy file1 file2
       create file2 and copy all bytes of file1 to file2 */

    char buffer[12288];
    zeroOut(buffer, 12288);

    extractTwoArgs(cmd, arg1, arg2);
    if(isUpper(arg1[0]) || isUpper(arg2[0])) {
      interrupt(33, 15, 2, 0, 0);
    }
    /*read file1 into the buffer*/
    interrupt(33, 3, arg1, buffer, &sectors);

    /*write the contents of the buffer to file2*/
    interrupt(33, 8, arg2, buffer, sectors);
  }
  else if(strCmp(cmdWord, "del\0") == 0) {
    /* del filename
       delete the named file */
    extractSingleArg(cmd, arg1);
    if(isUpper(arg1[0])) {
      interrupt(33, 15, 2, 0, 0);
    }
    interrupt(33, 7, arg1, 0, 0);
  }
  else if(strCmp(cmdWord, "dir\0") == 0) {
    /* dir
       list directory contents */

    interrupt(33, 2, directory, 257, 0, 0);
    interrupt(33, 2, map, 256, 0, 0);

    displayFiles(directory);

  }
  else if(strCmp(cmdWord, "echo\0") == 0) {
    /* echo comment
       display comment on screen followed by single newline
       if no argument, issue new prompt */
    strCpy(cmd + 5, comment, 99);
    comment[99] = 0;
    PRINTS(comment);
    PRINTS("\r\n\0");
  }
  else if(strCmp(cmdWord, "help\0") == 0) {
    PRINTS("Commands:\r\nboot\r\ncls\r\ncopy\r\ndel\r\ndir\r\necho\r\nhelp\r\nlprint\r\nrun\r\nsetenv\r\ntweet\r\ntype\r\n\0");
  /*
    PRINTS("BlackDOS command help:\r\n\0");
    PRINTS("boot\r\n\tReboot the system\r\n\0");
    PRINTS("cls\r\n\tClear the screen\r\n\0");
    PRINTS("copy file1 file2\r\n\tCopy file1 to file2, if possible\r\n\0");
    PRINTS("del filename\r\n\tDelete the named file, if it exists\r\n\0");
    PRINTS("dir\r\n\tList file directory contents\r\n\0");
    PRINTS("echo comment\r\n\tDisplay comment on the screen\r\n\0");
    PRINTS("help\r\n\tDisplay this user manual\r\n\0");
    PRINTS("--PRESS ENTER FOR MORE--\r\n\0");
    SCANS(comment);
    PRINTS("lprint filename\r\n\tPrint the named file to the printer\r\n\0");
    PRINTS("run filename\r\n\tExecute filename\r\n\0");
    PRINTS("setenv fg color\r\n\tChange the foreground color of the console\r\n\0");
    PRINTS("\tValid colors are in the range from 0(black) to 15(bright white)\r\n\0");
    PRINTS("setenv bg color\r\n\tChange the background color of the console\r\n\0");
    PRINTS("\tValid colors are in the range from 0(black) to 7(white)\r\n\0");
    PRINTS("tweet filename\r\n\tCreate named file and fill it with a line of text of at most 140 characters\r\n\0");
    PRINTS("type filename\r\n\tDisplay the contents of the named file\r\n\0");
    */
  }
  else if(strCmp(cmdWord, "lprint\0") == 0) {
    /* lprint filename
       Load filename into mem and print its contents to the printer */
    char buffer[12288];
    extractSingleArg(cmd, arg1);
    interrupt(33, 3, arg1, buffer, &sectors);
    LPRINTS(buffer);
  }
  else if(strCmp(cmdWord, "run\0") == 0) {
    /* run filename
       execute filename at segment 4 */
    extractSingleArg(cmd, arg1);
    interrupt(33, 4, arg1, 4, 0);
  }
  else if(strCmp(cmdWord, "setenv\0") == 0) {
    /* setenv fg color
       and
       setenv bg color
       change the values of the foreground or background color and edit the config */
    extractTwoArgs(cmd, arg1, arg2);

    if(strCmp(arg1, "fg\0") == 0) {
      /*must convert arg2 to integer*/
      c = aToI(arg2);

      if(c < 0 || c > 15)
        PRINTS("Error: Foreground color value must be between 0 and 15\r\n\0");

      else {
        /*update value*/
        cfg[1] = c;
        interrupt(33, 6, cfg, 258, 0);
        /*clear the screen*/
        interrupt(33, 12, cfg[0]+1, cfg[1]+1, 0);
      }
    }

    /*FIXME issues command not valid*/
    else if(strCmp(arg1, "bg\0") == 0) {
      c = aToI(arg2);

      if(c < 0 || c > 7)
        PRINTS("Error: Background color value must be between 0 and 7\r\n\0");

      else {
        /*update value*/
        cfg[0] = c;
        interrupt(33, 6, cfg, 258, 0);
        /*clear the screen*/
        interrupt(33, 12, cfg[0]+1, cfg[1]+1, 0);
      }
    }
    else {
      PRINTS("Error: must set either foreground (fg) or background (bg)\r\n\0");
      PRINTS(arg1);

    }
  }
  else if(strCmp(cmdWord, "tweet\0") == 0) {
    /* tweet filename
       create a text file. prompt the user for a line of text <140 chars,
       store it in a buffer, and write to filename */
    char line[141];
    PRINTS("Enter a line of text less than 140 characters long.\r\n>\0");
    SCANS(line);

    extractSingleArg(cmd, arg1);

    if(isUpper(arg1[0])) {
      interrupt(33, 15, 2, 0, 0);
    }

    interrupt(33, 8, arg1, line, 1);
    PRINTS("\n\r\0");
  }
  else if(strCmp(cmdWord, "type\0") == 0) {
    /* type filename
       load filename into memory and display its content */
    char buffer[12288];
    extractSingleArg(cmd, arg1);

    interrupt(33, 3, arg1, buffer, &sectors);

    PRINTS(buffer);
    PRINTS("\n\0");
  }

  else {
    PRINTS("\nInvalid command.\r\n\0");
  }

}

/*C-style implementation of strcmp ripped from stackoverflow*/
int strCmp(char string1[], char string2[] )
{
  int i = 0;
  for (; ; i++)
  {
    if (string1[i] != string2[i])
    {
      return string1[i] < string2[i] ? -1 : 1;
    }

    if (string1[i] == '\0')
    {
      return 0;
    }
  }
}

/*copies the command word into cmdWord */
void extractCommand(char* cmd, char* cmdWord) {
  /*find the first space or a nullterm*/
  int i = 0;
  while(i < 128) {
    if(cmd[i] == ' ' || cmd[i] == '\0') {
      /*from [0, i-1] is the command*/
      strCpy(cmd, cmdWord, i);
      cmdWord[i] = 0;
      return;
    }
    ++i;
  }
}

/*extracts one argument from a command*/
void extractSingleArg(char* cmd, char* arg) {
  int j = 0;
  int i = findFirstOf(cmd, ' ') + 1;

  while(cmd[i] != ' ' && cmd[i] != 0) {
    arg[j] = cmd[i];
    ++j;
    ++i;
  }
  arg[j] = 0;
}

/*extracts two arguments from a command*/
void extractTwoArgs(char* cmd, char* arg1, char* arg2) {
  char* secondSpace;
  extractSingleArg(cmd, arg1);
  secondSpace = cmd + findNthOf(cmd, ' ', 2);
  extractSingleArg(secondSpace, arg2);
}

/*returns the index of the first occurrence of c in the string,
or -1 if not found */
int findFirstOf(char* str, char c) {
  int i = 0;
  while(str[i]) {
    if(str[i] == c)
      return i;
    ++i;
  }
  return -1;
}

/*returns the index of the Nth occurrence of c in a the string, or -1 if
  not found*/
int findNthOf(char* str, char c, int n) {
  int i = 0;
  int occurrences = 0;
  while(str[i]) {
    if(str[i] == c)
      ++occurrences;
    if(occurrences == n)
      return i;
    ++i;
  }
  return -1;
}

void zeroOut(char* arr, int n) {
  int i = 0;
  while(i < n) {
    arr[i] = 0;
    ++i;
  }
}

int isUpper(char c) {
  return (c <= 'Z' && c >= 'A');
}

void displayFiles(char* directory) {
  char filename[9];
  int fullSectors = 0;
  char* currentEntry = directory;

  while(currentEntry < directory + 512) {

    strCpy(currentEntry, filename, 8);
    filename[8] = 0;

    /* if there is a file */
    if(filename[0] != 0) {

      /*if the file is not a directory file*/
      if(!isUpper(filename[0])) {
        PRINTS(filename);
        PRINTS("\t\0");
        PRINTN(sectorsUsed(currentEntry));
        PRINTS("\n\r\0");
      }
      /* add the sectors regardless of type*/
      fullSectors += sectorsUsed(currentEntry);
    }
    currentEntry += 32;
  }
  PRINTS("\n\rCurrently using \0");
  PRINTN(fullSectors);
  PRINTS(" sectors with \0");
  PRINTN(255 - fullSectors);
  PRINTS(" sectors remaining.\n\r\0");
}

int sectorsUsed(char* entry) {
  int full = 0;
  char* currentSector = entry + 8;

  while(currentSector < entry + 32) {
    if(*currentSector !=  0)
      ++full;
      ++currentSector;
  }

  return full;
}

int strLen(char* str) {
  int i = 0;
  while(str[i]) {
    ++i;
  }
  return i;
}

/*atoi implementation ripped from stackoverflow*/
int aToI(char* str) {
  int result = 0, i = 0;
  while(str[i]) {
    result = result * 10 + str[i] - '0';
    ++i;
  }
  return result;
}
