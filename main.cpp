#include <iostream>
#include <fstream>
#include <string.h>
#include "docente.h"
#include "buffer.h"

using namespace std;

int main (int argv, char* argc[]){

    //Load the docente text into memory
    character* docenteCSV = createBufferFile("csv/docentes.csv");

    //Remove the first line of the text (it is useless)
    character* currentLine = removeFirstBufferLine(&docenteCSV);
    currentLine = removeFirstBufferLine(&docenteCSV);
    character* idFirst = getNthCommaData(currentLine, 1);
    printBufferFile(idFirst);
    
    //Iterate line by line of "docentes.csv" removing them
    /*
    while(currentLine = removeFirstBufferLine(&docenteCSV)){

      destroyBufferFile(&currentLine);
    }
    */

    destroyBufferFile(&docenteCSV);
    destroyBufferFile(&idFirst);


    return 0;
}
