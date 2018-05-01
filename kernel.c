/* ACADEMIC INTEGRITY PLEDGE                                              */
/*                                                                        */
/* - I have not used source code obtained from another student nor        */
/*   any other unauthorized source, either modified or unmodified.        */
/*                                                                        */
/* - All source code and documentation used in my program is either       */
/*   my original work or was derived by me from the source code           */
/*   published in the textbook for this course or presented in            */
/*   class.                                                               */
/*                                                                        */
/* - I have not discussed coding details about this project with          */
/*   anyone other than my instructor. I understand that I may discuss     */
/*   the concepts of this program with other students and that another    */
/*   student may help me debug my program so long as neither of us        */
/*   writes anything during the discussion or modifies any computer       */
/*   file during the discussion.                                          */
/*                                                                        */
/* - I have violated neither the spirit nor letter of these restrictions. */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/* Signed: Matthew Britton_____________________ Date:2/7/2018_____        */
/*                                                                        */
/*                                                                        */
/* 3460:4/526 BlackDOS2020 kernel, Version 1.01, Spring 2018.             */

void handleInterrupt21(int,int,int,int);
void printLogo();
void printString(char*, int);
void readInt(int* n);
void writeInt(int x, int d);
void readString(char*);
void error(int);
void writeFile(int, int, int);
void readFile(int, int, int);


void main()
{
  char buffer[512]; int size;
  char file[12288];
  makeInterrupt21();
  /* Step 0 – config file */
  interrupt(33,2,buffer,258,0);  /*read sector*/
  interrupt(33,12,buffer[0]+1,buffer[1]+1,0);   /*clear screen*/
  printLogo();

  interrupt(33, 4, "Shell\0", 2, 0);
  interrupt(33, 0, "Bad or missing command interpreter.\r\n\0", 0, 0);

  while (1) ;
}


/* Print a null-terminated character string.
 * if d == 0, print to the screen; if d == 1, print to the printer. */
void printString(char* c, int d)
{
  int destination;
  if(d == 0) {
    /*Send to the screen*/
    while(*c != '\0') {
    char al = *c;
    char ah = 14;
    int ax = ah * 256 + al;
    interrupt(16, ax, 0, 0, 0);
    ++c;
    }
  }
  else if(d == 1) {
    while(*c != '\0') {
      char al = *c;
      char ah = 14;
      char ax = ah * 256 + al;
      interrupt(23, ax, 0, 0, 0);
      ++c;
    }
  }
   return;
}

void printLogo()
{
   printString("       ___   `._   ____  _            _    _____   ____   _____ \r\n\0",0);
   printString("      /   \\__/__> |  _ \\| |          | |  |  __ \\ / __ \\ / ____|\r\n\0",0);
   printString("     /_  \\  _/    | |_) | | __ _  ___| | _| |  | | |  | | (___ \r\n\0",0);
   printString("    // \\ /./      |  _ <| |/ _` |/ __| |/ / |  | | |  | |\\___ \\ \r\n\0",0);
   printString("   //   \\\\        | |_) | | (_| | (__|   <| |__| | |__| |____) |\r\n\0",0);
   printString("._/'     `\\.      |____/|_|\\__,_|\\___|_|\\_\\_____/ \\____/|_____/\r\n\0",0);
   printString(" BlackDOS2020 v. 1.01, c. 2018. Based on a project by M. Black. \r\n\0",0);
   printString(" Author(s): Matthew Britton.\r\n\r\n\0",0);
}

/* MAKE FUTURE UPDATES HERE */
/* VVVVVVVVVVVVVVVVVVVVVVVV */

/*read a null-terminated string*/
void readString(char* buffer) {
   char c;
   int count, ax, size;
   printString("\n\r\0");

   size = 80;
   count = 0;

   do {
     c = interrupt(22, 0, 0, 0, 0);
     /*Read a non-backspace character into the buffer*/
     if(c != 0x8) {
       buffer[count] = c;
     }

     /*Print the typed character to the screen*/
     ax = 14 * 256 + c;
     interrupt(16, ax, 0, 0, 0);

     /*if the typed character was a backspace, decrement the counter.
       otherwise, increment it*/
     if(c == 0x8) {
       /*the index does not go below 0*/
       if(count - 1 >= 0) {
	 --count;
       }
     }
     else
       ++count;
   } while(count < size && c != 0xD); /*read until a newline*/
   buffer[count] = '\0';
   printString("\r\n\0", 0);  /*go to a new line*/
   return;
}

int mod(int a, int b) {
   int x = a;
   while(x >= b) x = x - b;
   return x;
}

int div(int a, int b) {
   int q = 0;
   while(q * b <= a) q++;
   return (q - 1);
}

/*an integer exponentiation function*/
int intpow(int a, int b) {
  int i, temp;
  if(b == 0) {
    return 1;
  }
  if(b == 1) {
    return a;
  }

  temp = a;
  for(i = 1; i < b; ++i){
    temp *= a;
  }
  return temp;
}

/*read an integer value from the screen into the pointed value*/
void readInt(int* n) {
   char input[10], *c, *end;
   int digits, value, ten_pow;

   readString(input);  /*up to 5 digits and null term*/

   c = input;
   digits = 0;
   /*count how many digits*/
   while(*c != '\0') {
	    ++digits;
	    ++c;
   }

   if(digits > 5) {
     *n = -1;
     return;
   }

   value = 0;  /*initialize the value of the integer to be read to 0*/
   end = input + digits - 1;  /*point to the last digit*/

   while(end >= input) {

     ten_pow = digits - (end - input + 2);
     ten_pow = intpow(10, ten_pow);

      switch(*end) {
	/*add the value of the digit times the appropriate power of 10*/
	case '0':
	  value += 0 * ten_pow;
	  break;
	case '1':
	  value += 1 * ten_pow;
	  break;
	case '2':
	  value += 2 * ten_pow;
	  break;
	case '3':
	  value += 3 * ten_pow;
	  break;
	case '4':
	  value += 4 * ten_pow;
	  break;
	case '5':
	  value += 5 * ten_pow;
	  break;
	case '6':
	  value += 6 * ten_pow;
	  break;
	case '7':
	  value += 7 * ten_pow;
	  break;
	case '8':
	  value += 8 * ten_pow;
	  break;
	case '9':
	  value += 9 * ten_pow;
	  break;
      }
      --end;
   }

   *n = value;
}

/*write an integer to the screen*/
void writeInt(int x, int d) {
  char out[6], digit; /*max 5 digits and a null terminator*/
  int i;

  if(x == 0) {
    out[0] = '0';
    out[1] = '\0';
  }
  else {
    int length = 0;
    for(; x > 0; ++length) {
      /*mod the appropriate power of 10 for the current digit,add offset*/
      out[length] = mod(x, 10) + '0';
      /*lop off a digit*/
      x = div(x, 10);
    }
    out[length] = '\0';

    /*it's backwards... reverse it*/
    for(i = 0; i < div(length, 2); ++i) {
      digit = out[i];
      out[i] = out[length - i - 1];
      out[length - i - 1] = digit;
    }
  }
  printString(out, d);
}

/*read a sector from the disk*/
void readSector(char* buffer, int sector) {
  int ah, al, ax, bx, ch, cl, cx, dh, dl, dx;
  ah = 2; /*read sector*/
  al = 1; /*# sectors to read*/
  ch = div(sector, 36); /*track number*/
  cl = mod(sector, 18) + 1; /*relative sector number*/
  dh = mod(div(sector, 18), 2); /*head number*/
  dl = 0; /*floppy disk*/

  ax = ah * 256 + al;
  cx = ch * 256 + cl;
  dx = dh * 256 + dl;

  interrupt(19, ax, buffer, cx, dx);
}

/*write to a disk sector*/
void writeSector(char* buffer, int sector) {
  int ah, al, ax, bx, ch, cl, cx, dh, dl, dx;
  ah = 3; /*write sector*/
  al = 1; /*# sectors to read*/
  ch = div(sector, 36); /*track number*/
  cl = mod(sector, 18) + 1; /*relative sector number*/
  dh = mod(div(sector, 18), 2); /*head number*/
  dl = 0; /*floppy disk*/

  ax = ah * 256 + al;
  cx = ch * 256 + cl;
  dx = dh * 256 + dl;

  interrupt(19, ax, buffer, cx, dx);
}

/*clear the screen and set the colors*/
void clearScreen(int bx, int cx) {
  int i = 0;
  while(i < 24) {
    printString("\r\n\0", 0);
    ++i;
  }

  interrupt(16, 512, 0, 0, 0);

  if(bx <= 8 && cx <= 16) {
    if(bx > 0 && cx > 0) {
      interrupt(16, 1536, 4096 * (bx - 1) + 256 * (cx - 1), 0, 6223);
    }
  }
}

/*copy a certain number of bytes from one location to another
  oh my GOD why didn't i write this earlier*/
void memCopy(char* from, char* to, int bytes) {
  int i = 0;
  while(i < bytes) {
    to[i] = from[i];
    ++i;
  }
  return;
}

/*filenames are 8 bytes, not null-terminated
  returns 0 if not equal, 1 if equal*/
int compareFilenames(char* seek, char* disk) {
  int i = 0;

  while(seek[i] != '\0') {
    if(i >= 8) /*if the sought filename is longer than 8 bytes, stop looking*/
      return 0;
    if(seek[i] != disk[i]) /*if we encounter unequal chars, stop looking*/
      return 0;
    ++i;
  }
  return 1;
}

/*read the filename in from a directory entry*/
void getFilename(char* entry, char* filename) {
  memCopy(entry, filename, 8);
}

/*find the index of the first free sector in the map*/
int findFreeSector(char* map) {
  int i = 0;
  while(i < 512) {
    if(map[i] == 0) {
      return i;
    }
    ++i;
  }
  error(2);
}

/*set a sector number to be full*/
void setSectorFull(char* map, int index) {
  map[index] = 255;
}

/*set a sector number to be empty*/
void setSectorEmpty(char* map, int index) {
  map[index] = 0;
}

/*read a file with the given name, into the buffer, and store its size*/
void readFile(char* fname, char* buffer, int* size) {
  char directory[512], filename[8];
  char* currentEntry, * endCurrentEntry, * currentSector;
  int filenamesEqual = 0, sectorCount = 0;

  *size = 0;

  /*load in the directory*/
  readSector(directory, 257);

  /*there are 16 directory entries*/
  /*each entry is 32 bytes*/
  /*the first 8 bytes of the entry are the filename*/
  /*the next 24 are sector numbers*/

  /*begin pointing to the first directory entry*/
  currentEntry = directory;

  while(currentEntry < directory + 512) {
    endCurrentEntry = currentEntry + 32;
    /*compare the first 8 bytes of the entry*/
    getFilename(currentEntry, filename);
    filenamesEqual = compareFilenames(fname, filename);

    /*if they match, do something*/
    if(filenamesEqual == 1) {
      /*
        while we're in this directory entry
          read a sector number from the entry
          read that sector number into the buffer
          add 512 to the buffer address
          advance to the next sector
      */
      /*the first sector # in the file is just past the filename (8 bytes)*/
      currentSector = currentEntry + 8;

      while(currentSector < endCurrentEntry) {
        if(*currentSector != 0) {
          readSector(buffer, *currentSector);
          buffer += 512;
          *size += 1;
          ++sectorCount;
        }
        ++currentSector;
      }
      return;
    }
    /*if they don't match, go to the next entry
      the next entry is 32 bytes along the directory*/
    else {
      currentEntry += 32;
    }
  }
  /*if we get down here, the file couldn't be found.*/
  error(0);
  return;
}

/*delete a file with the given name form the disk if it exists*/
void deleteFile(char* name) {
  char directory[512], map[512], testFilename[8];
  char* currentEntry, * endCurrentEntry, * currentSector, * filename;
  int i = 0;

  char deletedFileName[9];

  readSector(directory, 257);
  readSector(map, 256);

  currentEntry = directory;

  while(currentEntry < directory + 512) {
    getFilename(currentEntry, testFilename);
    /* if the file is found...*/
    if(compareFilenames(name, testFilename) == 1) {
      printString("Found file to delete.\r\n\0", 0);

      /*set the first byte to 0*/

      /* a little sanity check*/
      if(mod(currentEntry - directory,  32) != 0)
        error(99);

      currentEntry[0] = 0;

      /*walk thru sector numbers belonging to file
        set every used sector in the directory to 0 in the bytemap*/
      currentSector = currentEntry + 8;

      while(*currentSector != 0 && currentSector < currentEntry + 32) {
        setSectorEmpty(map, *currentSector);
        ++currentSector;
      }

      /*write stuff back to disk*/
      writeSector(directory, 257);
      writeSector(map, 256);

      return;
    }
    else
      currentEntry += 32;
  }
  /*if we get out here, the filename couln't be found*/
  if(currentEntry >= directory + 512)
    error(0);
}

/*write the content of the buffer to a file with the provided name*/
void writeFile(char* name, char* buffer, int numberOfSectors) {
  char directory[512], map[512], testFilename[8], nextChunk[512];
  char* currentEntry, * endCurrentEntry, * freeDirectory, * currentSector;
  int i = 0, j = 0, freeSector;

  char printableFilename[9], printableSectorFilename[9];

  readSector(directory, 257);
  readSector(map, 256);

  currentEntry = directory; /*point to the start of the first entry*/
  endCurrentEntry = currentEntry + 32;  /*point past the end of the entry*/

  while(currentEntry < directory + 512) {
    getFilename(currentEntry, testFilename);

    /*if the filename already exists, error(1)*/
    if(compareFilenames(name, testFilename) == 1)
      error(1);

    /*otherwise, find and note a free directory - first byte is 0*/
    else if(*currentEntry == 0) {
      freeDirectory = currentEntry;
      break;
    }
    currentEntry += 32;
  }
  /*if we get out here and the current entry is the end of the directory,no space*/
  if(currentEntry >= directory + 512)
    error(2);

  /*copy the name to the directory entry*/
  i = 0;
  while(name[i] != '\0') {
    freeDirectory[i] = name[i];
    ++i;
  }
  /*pad any remaining space with 0s*/
  while(i < 8) {
    freeDirectory[i] = 0;
    ++i;
  }

  /*for each sector in the file:
      find a free sector
      set that sector to 255
      add that sector number to the directory entry
      write 512 bytes from the buffer into that sector*/

  i = 0;
  currentSector = freeDirectory + 8;
  endCurrentEntry = freeDirectory + 32;
  while(i < numberOfSectors && currentSector < endCurrentEntry) {
    freeSector = findFreeSector(map); /*find a free sector*/
    setSectorFull(map, freeSector);  /*set that sector to 255*/
    *currentSector = freeSector; /*add that sector number to the dir entry*/
    ++currentSector; /*advance to the next sector in the directory*/
    ++i;

    /*copy the next 512 bytes of the buffer into nextChunk to write*/
    j = 0;
    while(j < 512) {
      nextChunk[j] = *buffer;
      ++j;
      ++buffer;
    }
    writeSector(nextChunk, freeSector);  /*write that chunk*/
  }

  /*fill the remaining bytes of the entry with 0s*/
  while(currentSector < endCurrentEntry) {
    *currentSector = 0;
    ++currentSector;
  }

  /*write the map and directory back to disk*/
  writeSector(directory, 257);
  writeSector(map, 256);

}

void runProgram(char* name, int segment) {
  char buffer[12288];
  int numSectors = 0;
  int baseLocation = 0;
  int offset = 0;

  if(segment < 2 || segment > 9) {
    printString("Invalid sector\r\n\0");
    while(1);
  }

  /* Use readFile() to load the file into a buffer */
  readFile(name, buffer, &numSectors);

  /* Multiply segment by 0x1000 to derive base location of segment */
  baseLocation = segment * 0x1000;

  /* In a loop:
      transfer loaded file from buffer to memory based at the computed
      segment location, starting from offset 0, using putInMemory()*/
  while(offset < 12288) {
    putInMemory(baseLocation, offset, buffer[offset]);
    ++offset;
  }

  /* Call launchProgram which takes the base segment address from (2) */
  launchProgram(baseLocation);
}

void stop() {
  launchProgram(8192);
}

/*print an error message*/
void error(int bx) {
  switch(bx) {
    case 0:
      printString("File not found.\r\n\0");
      break;
    case 1:
      printString("Bad file name.\r\n\0");
      break;
    case 2:
      printString("Disk full.\r\n\0");
      break;
    default:
      printString("General error.\r\n\0");
  }
  stop();
}

/* ^^^^^^^^^^^^^^^^^^^^^^^^ */
/* MAKE FUTURE UPDATES HERE */

void handleInterrupt21(int ax, int bx, int cx, int dx)
{
   /*return;*/
   switch(ax) {
     case 0: printString(bx,cx); break;
     case 1: readString(bx); break;
     case 2: readSector(bx, cx); break;
     case 3: readFile(bx, cx, dx); break;
     case 4: runProgram(bx, cx); break;
     case 5: stop(); break;
     case 6: writeSector(bx, cx); break;
     case 7: deleteFile(bx); break;
     case 8: writeFile(bx, cx, dx); break;
     /*case 9:
     case 10: */
     case 11: interrupt(25, 0, 0, 0, 0); break;
     case 12: clearScreen(bx, cx); break;
     case 13: writeInt(bx, cx); break;
     case 14: readInt(bx); break;
     case 15: error(bx); break;
     default: printString("General BlackDOS error: Unrecognized interrupt.\r\n\0");
   }
}
