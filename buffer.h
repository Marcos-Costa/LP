#ifndef BUFFER_H
  #define BUFFER_H

  typedef struct _character{
    //The actual text whitin
    char data;
    //Flag to tell if the char is int
    int isNumber;

    //Character next to cur
    struct _character* next;
    //Character previous to cur
    struct _character* prev;
    //Last character of this buffer
    struct _character* last;
  }character;

  int baker(docente** docentes, producao** producoes, int* rules, character* orientacoes, character* congressos, character* periodicos);

  character* createChar(char data);
  void destroyChar(character* charToDestroy);
  void addChar(character** string, char data);

  /*
  * A file buffer is a entire file loaded on
  * memory using the structure "character"
  * witch is a double linked list of char.
  */
  character* createBufferFile(const char* fileName);
  void destroyBufferFile(character** bufferFile);
  void printBufferFile(character* bufferFile);
  character* removeFirstBufferLine(character** bufferFile);
  character* find(character* bufferFile, char* string);
  character* concatenate(character** destination, character** stringToConcatenate);

  char* bufferStringToString(character* string);
  character* intToBufferString(int integer);

  //Function to use whith .csv files
  char* getNthColumnData(character* bufferFile, int position);

  int* loadAllRules(const char* filePath,int rulesNum);

  //Function to use with the "rules files"
  int getRuleValue(character* bufferFile,char* qualis);

  long stringToLong(char* numberToConvert);
  int stringToInt(char* numberToConvert);
#endif
