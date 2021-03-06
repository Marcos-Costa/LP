#include "buffer.h"
#include "producao.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

using namespace std;


int baker(docente** docentes, producao** producoes, int* rules, character** orientacoes, character* congressos, character* periodicos, char* curso, const char* regrasNomeArquivo, int anoEntrada ,int anoSaida){
  int sucess = 1;

  if((*docentes) != NULL && (*producoes) != NULL){
    if(rules != NULL && orientacoes != NULL && congressos != NULL && periodicos != NULL){
      dictionary* issnDictionary = NULL;
      docente* currentDocente = NULL;
      char* slashN = new char[2];
      slashN[0] = '\n';
      slashN[1] = '\0';

      //It is here to avoid bug of going non stop through .csv
      int numberOfLinesCongressoCSV = numberOfLinesBufferFile(congressos);
      cout << "======================"<< curso << "=======================" << endl;
      int numDeProducoes = 0;
      int numDeCogressos = 0;
      //Get the first node from the Double Linked List
      currentDocente = (*docentes);
      //Iterate through all docentes
      while(currentDocente != NULL){
        numDeProducoes = 0;
        cout << "Docente: " << currentDocente->name << endl;

        //Store the every single "producao" from the current docente
        producao* allProducao = NULL;

        //It willcurrentProducao = removeProducao(&allProducao) receive the last "producao" whithing the current docente node at the BST
        allProducao = getAllProducoesFromThatDocente(*producoes, currentDocente->id);
        //If the docente has some producao
        if(allProducao != NULL){
          cout << "Producoes: "<< endl;

          //Iterate through all producoes
          producao* currentProducao = NULL;


          while((currentProducao = removeProducao(&allProducao))){
            //If the current producao is a normal publicacao
            if(((!strcmp(currentProducao->type, "ARTIGO-PUBLICADO")) || (!strcmp(currentProducao->type, "ARTIGO-ACEITO-PARA-PUBLICACAO"))) && (currentProducao->year >= anoEntrada) && (currentProducao->year <= anoSaida)){
              int semEstratoQualis = 1;
              cout << currentProducao->year << endl;


              //Iterate through all issn's that match
              character* iterator = periodicos;
              while(iterator = find(iterator, currentProducao->issn)){

                //Check if the type is the same at the rule file
                char* areaAvaliacao = getNthColumnDataFromCur(iterator->prev, 3);

                if(!strcmp(curso,areaAvaliacao) ){
                  //Set docente points
                  semEstratoQualis = 0;
                  char* qualis = clean(getNthColumnDataFromCur(iterator->prev, 4));
                  int pontos = qualisCodePeriodicosToInt(qualis,rules);
                  currentDocente->points[getPosQualisPeriodico(qualis)] += pontos;
                  currentDocente->totalPoints += pontos;
                  cout << "\t" << clean(getNthColumnDataFromCur(iterator->prev, 4)) << " (" << pontos << ")"  << " - " << currentProducao->issn << " - " << currentProducao->type << " - " << currentProducao->title << " - " <<currentProducao->year<< endl;
                  numDeProducoes++;
                  delete[] qualis;
                  break;
                }

                iterator = iterator->next;
              }
              if(semEstratoQualis == 1){
                int pontos = rules[8];
                currentDocente->points[8] += pontos;
                cout << "\t" << "Sem Estrato Qualis" << " (" << pontos << ")"  << " - " << currentProducao->issn << " - " << currentProducao->type << " - " <<currentProducao->title <<" - " << currentProducao->year << endl;
                currentDocente->totalPoints += pontos;
                numDeProducoes++;
              }
            }else if((!strcmp(currentProducao->type, "TRABALHO_EM_EVENTO") )&& (currentProducao->year >= anoEntrada) && (currentProducao->year <= anoSaida)){
              char* localProducao = new char[2048];
              strcpy(localProducao,(currentProducao->local));

              int hasNoQualis = 1;
              character* iteratorSiglaCongresso = congressos;

              //Separate all words whiting the field LOCAL
              char* siglaProducao = strtok(currentProducao->local, " -.,():");

              char* siglaCSV = getNthColumnDataCongresso(iteratorSiglaCongresso,4);


              //Update siglaCSV value
              while(siglaProducao != NULL){
                while(siglaCSV != NULL){
                  char* siglaUpperCase = convertToUpper(siglaProducao);
                  char* congressoCompleteName = strstr(getNthColumnDataCongresso(iteratorSiglaCongresso, 1)," - ");
                  congressoCompleteName++;
                  congressoCompleteName++;
                  congressoCompleteName++;
                  //Check if the name OR sigla match of congresso match with the local
                  //Ponha um strstr aqui tendo o local da producao e o nome do congresso

                  if((!strcmp(siglaUpperCase,siglaCSV))  || (strstr(localProducao,congressoCompleteName) != NULL)){
                    delete[] siglaUpperCase;
                    hasNoQualis = 0;
                    char* qualis = clean(getNthColumnDataCongresso(iteratorSiglaCongresso, 5));
                    int pontos = qualisCodeCongressosToInt(qualis,rules);
                    currentDocente->points[getPosQualisCongresso(qualis)] += pontos;
                    currentDocente->totalPoints += pontos;
                    cout << "\t" << clean(getNthColumnDataCongresso(iteratorSiglaCongresso, 5)) << " (" << pontos << ")"  << " - " << currentProducao->type << " - " <<currentProducao->title <<" - " << currentProducao->year << endl;
                    numDeProducoes++;
                    break;
                  }

                  iteratorSiglaCongresso = find(iteratorSiglaCongresso->next,slashN);
                  siglaCSV = getNthColumnDataCongresso(iteratorSiglaCongresso,4);
                }

                if(hasNoQualis == 0)
                  break;
                iteratorSiglaCongresso = congressos;
                siglaCSV = getNthColumnDataCongresso(iteratorSiglaCongresso,4);

                //Separate all words whiting the field take, the next one
                siglaProducao = strtok(NULL, " -.,():");
              }

              if(hasNoQualis == 1){
                int pontos = rules[17];
                currentDocente->points[17] += pontos;
                cout << "\t" <<"Sem Estrato Qualis" << " (" << pontos << ")"  << " - " << currentProducao->type << " - " <<currentProducao->title <<" - " << currentProducao->year << endl;
                currentDocente->totalPoints += pontos;
                numDeProducoes++;
              }
              strcpy(currentProducao->local, localProducao);
              delete[] localProducao;
            }
            destroyProducao(&currentProducao);
          }
        }

        //Count the number of orientacoes
        char* idDocente = new char[257];
        long sizeIdDocente = sprintf (idDocente,"%ld", currentDocente->id);

        character* txt = createBufferFile(regrasNomeArquivo);

        while(txt != NULL){

          //Go to the start of the line
          char* titleOrientacao = getNthColumnLocalOrientacao(txt,4);
          char* idDoDocenteTmp = getNthColumnLocalOrientacao(txt,1);
          long idDoDocente = stringToLong(getNthColumnLocalOrientacao(txt,1));
          char* typeOrientacao = getNthColumnLocalOrientacao(txt,3);
          char* tmpYear = getNthColumnLocalOrientacao(txt,6);
          int year = stringToInt(tmpYear);

          int pontos = 0;
          if((!strcmp(typeOrientacao,"INICIACAO_CIENTIFICA")) && idDoDocente == currentDocente->id && (year >= anoEntrada) && (year <= anoSaida)){
            pontos = rules[18];
            currentDocente->points[18] += pontos;
            currentDocente->totalPoints += pontos;
            cout << "\t" << typeOrientacao << " (" << pontos << ")"  << " - "  << typeOrientacao << " - " << titleOrientacao <<" - " << year << endl;

          }else if((!strcmp(typeOrientacao,"TRABALHO_DE_CONCLUSAO_DE_CURSO_GRADUACAO")) && idDoDocente == currentDocente->id && (year >= anoEntrada) && (year <= anoSaida)){
            pontos = rules[19];
            currentDocente->points[19] += pontos;
            currentDocente->totalPoints += pontos;
            cout << "\t" << typeOrientacao << " (" << pontos << ")"  << " - "  << typeOrientacao << " - " << titleOrientacao <<" - " << year << endl;

          }else if((!strcmp(typeOrientacao,"Dissertação de mestrado")) && idDoDocente == currentDocente->id && (year >= anoEntrada) && (year <= anoSaida)){
            pontos = rules[20];
            currentDocente->points[20] += pontos;
            currentDocente->totalPoints += pontos;
            cout << "\t" << typeOrientacao << " (" << pontos << ")"  << " - "  << typeOrientacao << " - " << titleOrientacao <<" - " << year << endl;

          }else if((!strcmp(typeOrientacao,"Tese de doutorado")) && idDoDocente == currentDocente->id && (year >= anoEntrada) && (year <= anoSaida)){
            pontos = rules[21];
            currentDocente->points[21] += pontos;
            currentDocente->totalPoints += pontos;

            cout << "\t" << typeOrientacao << " (" << pontos << ")"  << " - "  << typeOrientacao << " - " << titleOrientacao <<" - " << year << endl;
          }
          delete[] titleOrientacao;
          delete[] idDoDocenteTmp;
          delete[] typeOrientacao;
          delete[] tmpYear;
          character* deleteme = removeFirstBufferLine(&txt);
          destroyBufferFile(&deleteme);
        }


        currentDocente = currentDocente->next;
        cout << "==============================================" << endl;
      }

    }
  }
  return sucess;
}

int getPosQualisPeriodico(char* qualisCode){
  int pos = -1;
  if(qualisCode != NULL){
    if(!strcmp(qualisCode,"A1")){
      pos = 0;
    }else if(!strcmp(qualisCode,"A2")){
      pos = 1;
    }else if(!strcmp(qualisCode,"B1")){
      pos = 2;
    }else if(!strcmp(qualisCode,"B2")){
      pos = 3;
    }else if(!strcmp(qualisCode,"B3")){
      pos = 4;
    }else if(!strcmp(qualisCode,"B4")){
      pos = 5;
    }else if(!strcmp(qualisCode,"B5")){
      pos = 6;
    }else if(!strcmp(qualisCode,"C")){
      pos = 7;
    }
    //0 = sem estrato no qualis ??
  }
  return pos;
}

int getPosQualisCongresso(char* qualisCode){
  int pos = -1;
  if(qualisCode != NULL){
    if(!strcmp(qualisCode,"A1")){
      pos = 9;
    }else if(!strcmp(qualisCode,"A2")){
      pos = 10;
    }else if(!strcmp(qualisCode,"B1")){
      pos = 11;
    }else if(!strcmp(qualisCode,"B2")){
      pos = 12;
    }else if(!strcmp(qualisCode,"B3")){
      pos = 13;
    }else if(!strcmp(qualisCode,"B4")){
      pos = 14;
    }else if(!strcmp(qualisCode,"B5")){
      pos = 15;
    }else if(!strcmp(qualisCode,"C")){
      pos = 16;
    }
  }
  return pos;
}

char* getNthColumnLocalOrientacao(character* bufferFile, int collumn){
  char* data = NULL;
  if(bufferFile!= NULL){
    if(collumn > 0 || collumn < 7){

      switch (collumn) {
        case 1:{
          int numberOfChars = 0;
          character* cur = bufferFile;

          //Move to the first letter
          cur = cur->next;

          //Count the number of characters
          while(cur->next != NULL){
            if(cur->data == '"'){
              break;
            }else{
              numberOfChars++;
            }
            cur = cur->next;
          }

          data = new char[numberOfChars+1];

          //Go back to start
          cur = bufferFile;
          //Jump the first quote
          cur = cur->next;

          //Copy the string
          for(int i=0; i < numberOfChars; i++){
            data[i] = cur->data;
            cur = cur->next;
          }
          data[numberOfChars] = '\0';
          break;
        }
        case 2:{
          int numberOfChars = 0;
          character* cur = bufferFile;
          character* start = NULL;

          while(cur->next != NULL){
            if(cur->data == ',' && cur->prev->data == '"'){

              //Move to the first number
              cur = cur->next;
              //Save it's address
              start = cur;
              break;
            }
            cur = cur->next;
          }

          //Count the number of characters
          while(cur->next != NULL){
            if(cur->data == ','){
              break;
            }else{
              numberOfChars++;
            }
            cur = cur->next;
          }

          //alocate it
          data = new char[numberOfChars+1];

          //Go back to the start of the data
          cur = start;

          //Copy the string
          for(int i=0; i < numberOfChars; i++){
            data[i] = cur->data;
            cur = cur->next;
          }
          data[numberOfChars] = '\0';
          break;
        }
        case 3:{
          int numberOfChars = 0;
          character* cur = bufferFile;
          character* start = NULL;

          int numberOfCommas = 0;
          int numberOfQuotes = 0;

          cur = bufferFile;
          start = NULL;

          while(cur->next != NULL){
            if(cur->data == '"'){
              numberOfQuotes++;
            }
            if(cur->data == ','){
              numberOfCommas++;
            }
            if((numberOfCommas == 2) && (numberOfQuotes == 2)){

              //Move to the first character
              cur = cur->next->next;
              start = cur;
              break;
            }
            cur = cur->next;
          }

          //Count the number of characters
          while(cur->next != NULL){
            if(cur->data == '"'){
              break;
            }else{
              numberOfChars++;
            }
            cur = cur->next;
          }

          //alocate it
          data = new char[numberOfChars+1];

          //Go back to the start of the data
          cur = start;

          //Copy the string
          for(int i=0; i < numberOfChars; i++){
            data[i] = cur->data;
            cur = cur->next;
          }
          data[numberOfChars] = '\0';
          break;
        }
        case 4:{
          character* cur = bufferFile;
          character* start = NULL;

          int numberOfChars = 0;
          int numberOfCommas = 0;
          int numberOfQuotes = 0;

          cur = bufferFile;
          start = NULL;

          while(cur->next != NULL){
            if(cur->data == '"'){
              numberOfQuotes++;
            }
            if(cur->data == ','){
              numberOfCommas++;
            }
            if((numberOfCommas == 3) && (numberOfQuotes == 4)){

              //Move to the first character
              cur = cur->next->next;
              break;
            }
            cur = cur->next;
          }

          start = cur;

          //Count the number of characters
          while(cur->next != NULL){
            if(cur->data == '"'){
              break;
            }else{
              numberOfChars++;
            }
            cur = cur->next;
          }

          //alocate it
          data = new char[numberOfChars+1];

          //Go back to the start of the data
          cur = start;

          //Copy the string
          for(int i=0; i < numberOfChars; i++){
            data[i] = cur->data;
            cur = cur->next;
          }
          data[numberOfChars] = '\0';
          break;
        }
        case 5:{

        }
        case 6:{
          character* cur = bufferFile;
          character* start = NULL;

          int numberOfChars = 0;
          int numberOfCommas = 0;
          int numberOfQuotes = 0;

          int numberOfMarks = 0;

          cur = bufferFile;
          start = NULL;

          //Go to the last comma
          while(cur->next != NULL){

            if(cur->data == ',' && cur->prev->data == '"'){
              numberOfMarks++;
            }
            if(numberOfMarks == 4){
            	break;
            }
            cur = cur->next;
          }

          //Go to the first number
          cur = cur->next;

          //save its address
          start = cur;

          //Count the number of characters
          while(cur->next != NULL){
            if(cur->data == '\n'){
              break;
            }else{

              numberOfChars++;
            }

            cur = cur->next;
          }
          numberOfChars--;



          //alocate it
          data = new char[numberOfChars+1];

          //Go back to the start of the data
          cur = start;

          //Copy the string
          for(int i=0; i < numberOfChars; i++){
            data[i] = cur->data;
            cur = cur->next;
          }
          data[numberOfChars] = '\0';
          break;
        }
      }

    }
  }
  return data;
}


char* getNthColumnLocal(character* bufferFile){
  char* string = NULL;
  if(bufferFile != NULL){
    char* slashN = new char[2];
    slashN[0] = '\n';
    slashN[1] = '\0';
    //Go until the end of the string
    character* iterator = bufferFile;
    while(iterator->next != NULL){
      iterator = iterator->next;
    }

    delete[] slashN;
    while(iterator->prev != NULL){
      if(iterator->data == ',' && iterator->prev->data == '\"'){
        //The last letter of the field local
        iterator = iterator->prev->prev;
        break;
      }
      iterator = iterator->prev;
    }

    while(iterator->prev != NULL){
      if(iterator->prev->data == '\"'){
        //Iterator points to first letter
        break;
      }
      iterator = iterator->prev;
    }

    character* start = iterator;
    character* end = NULL;


      int numberOfChars = 0;

      //Move through the line char by char
      while(iterator->next != NULL && iterator->data != '\"'){

        //Count how many character there are
        numberOfChars++;
        iterator = iterator->next;
      }
      end = iterator;

      //Alocate a new string
      string = new char[(numberOfChars+1)];
      if(string != NULL){

        //Load the first character again to move throught the line
        iterator = start;

        //Copy all chars from the buffer file to the new char array
        for(int i=0; i<(numberOfChars); i++){
          string[i] = iterator->data;
          iterator = iterator->next;
        }

        //Put the special character at it's end
        string[numberOfChars] = '\0';

      }
  }
  return string;
}

char* convertToUpper(char* str){
  char* upperCaseStr = NULL;
  if(str != NULL){
    upperCaseStr = new char[strlen(str)+1];
    for(int i=0; i<strlen(str); i++){
      upperCaseStr[i] = toupper(str[i]);
    }
    upperCaseStr[strlen(str)] = '\0';
  }
  return upperCaseStr;
}

char* getNthColumnDataCongresso(character* bufferFile, int position){

  //A buffer line with a copy of the N position data
  char* nthText = NULL;
  if(bufferFile != NULL){
    if(position > 0){

      //The first character of the right data
      character* startPoint = NULL;
      //The last character of the right data
      character* finishPoint = NULL;

      int numberOfCommas = 0;
      character* iterator = bufferFile;
      //Make a backup of the first character address
      character* firstCharAddress = iterator;

      //Count the number of VALID commas int the text
      while(iterator->next != NULL){
        if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){
          numberOfCommas++;
        }
        iterator = iterator->next;
      }

      iterator = firstCharAddress;

      //The required collumn does not exist
      if(position > (numberOfCommas+1)){
        return NULL;
      //The last collumn is bein required
      }else if (position == (numberOfCommas+1)){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == (position-1)){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go foward one char, to ignore read the comma
            iterator = iterator->next;
            while(iterator->next != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->next;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            //int i = (numberOfChars-1);
            int i = 0;

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->next;
              while(iterator->next != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i++;
                }
                iterator = iterator->next;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      //The collumn given is anyone but the last
      }else if((position < (numberOfCommas+1)) && position > 0){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == position){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go backwards one char, to ignore read the comma
            iterator = iterator->prev;
            while(iterator->prev != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->prev;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            int i = (numberOfChars-1);

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->prev;
              while(iterator->prev != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->prev != NULL && iterator->next != NULL && iterator->prev->data == '\"' && iterator->data == ',' && iterator->next->data == '\"'){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i--;
                }
                iterator = iterator->prev;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      }
      }
    }

  return nthText;
}

int numberOfLinesBufferFile(character* congressosCSV){
  int numberOfLines = 0;

  if(congressosCSV!= NULL){
    char* slashN = new char[2];
    slashN[0] = '\n';
    slashN[1] = '\0';


    //Jump the header
    character* iterator = congressosCSV;

    while(iterator = find(iterator->next,slashN)){

      numberOfLines++;
      iterator = iterator->next;
    }

  }
  return (numberOfLines+1);
}

char* clean2(char* string){
  char* cleanedString = NULL;
  if(string != NULL){
    cleanedString = new char[strlen(string)];
    int lastPos = 0;
    for(int i=0; i<strlen(string) && string[i] != '\n';i++){
      cleanedString[i] = string[i];
      lastPos = i;
    }
    cleanedString[lastPos+1] = '\0';
  }
  return cleanedString;
}

char* clean(char* string){
  char* cleanedString = NULL;
  if(string != NULL){
    cleanedString = new char[strlen(string)];
    int lastPos = 0;
    for(int i=0; i<strlen(string) && string[i] != '\n';i++){
      cleanedString[i] = string[i];
      lastPos = i;
    }
    cleanedString[lastPos] = '\0';
  }
  return cleanedString;
}

char* stringToUpperCase(char* string){
  char* upperCaseString = NULL;
  if(string != NULL){
    upperCaseString = new char[strlen(string)+1];
    for(int i=0; i<strlen(string); i++){
      upperCaseString[i] = toupper(string[i]);
    }
    upperCaseString[strlen(string)] = '\0';
  }
  return upperCaseString;
}

char* loadAreaAvaliacao(const char* filePath){
  char* areaDeAvaliacao = NULL;

  if(filePath != NULL){

    //Load the rule file into memory
    character* ruleTxt = createBufferFile(filePath);
    if(ruleTxt != NULL){

        //Remove the header of the file
        character* header = removeFirstBufferLine(&ruleTxt);
        destroyBufferFile(&header);
        //Get the "area de avaliacao"
        character* area = removeFirstBufferLine(&ruleTxt);
        areaDeAvaliacao = bufferStringToString(area);
        //remove the '\n' from the end of the line
        areaDeAvaliacao[strlen(areaDeAvaliacao)-1] = '\0' ;
        destroyBufferFile(&area);
    }
    return areaDeAvaliacao;
  }

  return areaDeAvaliacao;
}

int qualisCodePeriodicosToInt(char* qualisCode ,int* rules){
  int value = 0;
  if(qualisCode != NULL){
    if(rules != NULL){

      if(!strcmp(qualisCode,"A1")){
        value = rules[0];
      }else if(!strcmp(qualisCode,"A2")){
        value = rules[1];
      }else if(!strcmp(qualisCode,"B1")){
        value = rules[2];
      }else if(!strcmp(qualisCode,"B2")){
        value = rules[3];
      }else if(!strcmp(qualisCode,"B3")){
        value = rules[4];
      }else if(!strcmp(qualisCode,"B4")){
        value = rules[5];
      }else if(!strcmp(qualisCode,"B5")){
        value = rules[6];
      }else if(!strcmp(qualisCode,"C")){
        value = rules[7];
      }
      //0 = sem estrato no qualis ??
    }
  }
  return value;
}

int qualisCodeCongressosToInt(char* qualisCode ,int* rules){
  int value = 0;
  if(qualisCode != NULL){
    if(rules != NULL){
      if(!strcmp(qualisCode,"A1")){
        value = rules[9];
      }else if(!strcmp(qualisCode,"A2")){
        value = rules[10];
      }else if(!strcmp(qualisCode,"B1")){
        value = rules[11];
      }else if(!strcmp(qualisCode,"B2")){
        value = rules[12];
      }else if(!strcmp(qualisCode,"B3")){
        value = rules[13];
      }else if(!strcmp(qualisCode,"B4")){
        value = rules[14];
      }else if(!strcmp(qualisCode,"B5")){
        value = rules[15];
      }else if(!strcmp(qualisCode,"C")){
        value = rules[16];
      }
      //0 = sem estrato no qualis ??
    }
  }
  return value;
}

void addDictionaryWord(dictionary** dictionaryToAdd, char* word, int value){
  if((*dictionaryToAdd)!= NULL ){
    if(word != NULL){
      //If the last word was not set yet
      if((*dictionaryToAdd)->last == NULL){
        dictionary* iterator = (*dictionaryToAdd);

        //Go until the last word
        while(iterator->next != NULL){
          iterator = iterator->next;
        }

        //Save its address
        (*dictionaryToAdd)->last = iterator;
      }

      //create a new word and add it
      dictionary* newWord = new dictionary[1];
      if(newWord != NULL){
        newWord->next = NULL;
        newWord->value = value;
        strcpy(newWord->word,word);
        newWord->prev = (*dictionaryToAdd)->last;
        (*dictionaryToAdd)->last->next = newWord;
        (*dictionaryToAdd)->last = newWord;
      }
    }
  }else{
    (*dictionaryToAdd) = createDictionary(word,value);
  }
}

char* getNthColumnDataFromCur(character* bufferFile, int position){
  //A buffer line with a copy of the N position data
  char* nthText = NULL;
  if(bufferFile != NULL){
    if(position > 0){

      //The first character of the right data
      character* startPoint = NULL;
      //The last character of the right data
      character* finishPoint = NULL;

      int numberOfCommas = 0;
      character* iterator = bufferFile;
      //Make a backup of the first character address
      character* firstCharAddress = iterator;

      //Count the number of commas int the text
      while(iterator->next != NULL){
        if(iterator->data == ','){
          numberOfCommas++;
        }
        iterator = iterator->next;
      }

      iterator = firstCharAddress;

      //The required collumn does not exist
      if(position > (numberOfCommas+1)){
        return NULL;
      //The last collumn is bein required
      }else if (position == (numberOfCommas+1)){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->data == ','){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == (position-1)){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go foward one char, to ignore read the comma
            iterator = iterator->next;
            while(iterator->next != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->data == ','){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->next;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            //int i = (numberOfChars-1);
            int i = 0;

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->next;
              while(iterator->next != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->data == ','){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i++;
                }
                iterator = iterator->next;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      //The collumn given is anyone but the last
      }else if((position < (numberOfCommas+1)) && position > 0){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->data == ','){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == position){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go backwards one char, to ignore read the comma
            iterator = iterator->prev;
            while(iterator->prev != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->data == ','){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->prev;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            int i = (numberOfChars-1);

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->prev;
              while(iterator->prev != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->data == ','){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i--;
                }
                iterator = iterator->prev;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      }
      }
    }

  return nthText;
}

void purgeDictionary(dictionary* dictionaryToDestroy){
  if(dictionaryToDestroy != NULL){
    dictionary* current = dictionaryToDestroy;
    //Go until the end of the dictionary
    while(current->next != NULL){
      current = current->next;
    }

    //Go until the first word
    while(current->prev != NULL){
      current = current->prev;
      destroyDictionary(&(current->next));
    }
    //destroy the last one
    destroyDictionary(&(current->next));
  }
}

void destroyDictionary(dictionary** dictionaryToDestroy){
  if(*dictionaryToDestroy != NULL){
    if((*dictionaryToDestroy)->next == NULL){
      delete[] (*dictionaryToDestroy)->word;
      delete[] (*dictionaryToDestroy);
    }
  }
}

dictionary* createDictionary(char* word,int value){
  dictionary* newDictionary = NULL;
  newDictionary = new dictionary[1];

  if(newDictionary != NULL){
    strcpy(newDictionary->word, word);
    newDictionary->next = NULL;
    newDictionary->prev = NULL;
    newDictionary->value = value;
  }

  return newDictionary;
}

dictionary* memoization(dictionary* dictionaryToSearch, char* word){
  dictionary* foundIt = 0;

  if(dictionaryToSearch != NULL){
    if(word != NULL){

      dictionary* currentWord = dictionaryToSearch;

      //Iterate through the dictionary looking for the word
      while(currentWord->next != NULL){

        //If the word was found
        if(!strcmp(currentWord->word,word)){
          foundIt = currentWord;
        }
        currentWord = currentWord->next;
      }

      //If the last word of the dictionary is equal
      if(!strcmp(currentWord->word,word)){
        foundIt = currentWord;
      }
    }
  }
  return foundIt;
}

character* findNth(character* bufferFile, char* string, int nth){
  character* wordFound = NULL;

  if(bufferFile != NULL){
    if(string != NULL){
      int stringSize = strlen(string);
      int foundIt = 0;

      character* iterator = bufferFile;
      while(iterator->next){

        //Make a copy of iterator to no lost the addres of the current character
        character* iteratorCopy = iterator;

        for(int i=0 ; i<stringSize ; i++){

          //If the current char is equal to current character
          if(string[i] != iteratorCopy->data){
            break;
          }

          //Sinalize if some error happened
          if(iteratorCopy->next == NULL){
            break;
          }else{
            iteratorCopy = iteratorCopy->next;
          }

          //If the last char of the string is equal as well
          if(i == (stringSize-1)){
            foundIt++;
          }
        }

        //If the string was found for the nth time
        if(foundIt == nth){

          //Copy the addres of its first character
          wordFound = iterator;
          break;
        }
        iterator = iterator->next;
      }
    }
  }
  return wordFound;
}

int* loadAllRules(const char* filePath,int rulesNum){
  if(rulesNum <= 25){
    int amountOfRules = rulesNum + 1;
    int* rulesTemporary = NULL;
    int* rules = NULL;
    character* rulesTemporaryTXT = createBufferFile(filePath);

    //Create an array of rules values + 1
    rulesTemporary = new int[amountOfRules];

    //Create an array of the rules to return starting from 0
    rules = new int[rulesNum];

    if(rulesTemporary != NULL){

      //Look in each rule whitin the file(start at rule num 1)
      for(int i=1; i<amountOfRules;i++){

        character* iteratorChar = intToBufferString(i);
        character* colon = createChar(':');
        character* qualisBufferString = concatenate(&iteratorChar,&colon);
        char* qualis = bufferStringToString(qualisBufferString);

        //Save its value
        rulesTemporary[i] = getRuleValue(rulesTemporaryTXT,qualis);
        delete[] qualis;
      }
      destroyBufferFile(&rulesTemporaryTXT);

      //Copy and past the values to return
      for(int i=0; i<rulesNum;i++){
        rules[i] = rulesTemporary[i+1];
      }
      delete[] rulesTemporary;
      return rules;
    }
  }
}


character* concatenate(character** string, character** newChar){

  character* sucess = NULL;
  //If the user actually pass a empty string
  if(*string != NULL){

    //Check if the new character was allocated
    if(*newChar != NULL){

      //Check if the last character was not set yet
      if((*string)->last == NULL){

        //Iterate through string until the its end
        character* iterator = (*string);
        while(iterator->next != NULL){
          iterator = iterator->next;
        }

        //Check if the string is not a sub-string
        if((*string)->prev == NULL){

          //Save the address of the last character
          (*string)->last = iterator;
        }else{
          //Save the address of the last character
          character* lastCharacterAddress = iterator;

          //Go back until the given character as parameter
          iterator = (*string);

          //Iterate through string until the its head
          while(iterator->prev != NULL){
            iterator = iterator->prev;
          }

          //Save the address of the last character
          iterator->last = lastCharacterAddress;
        }
      }

      //Make the second string a sub-string
      (*newChar)->last = NULL;
      //Save the adress of the old last character
      (*newChar)->prev = (*string)->last;
      //Make the old last character point to the new one
      (*string)->last->next = (*newChar);
      //Update the new last character
      (*string)->last = (*newChar);

      sucess = (*string);
    }
  }
  return sucess;
}

character* intToBufferString(int integer){
  character* bufferInteger = NULL;
  char* charArray = NULL;

  //All numbers that are representable by int will fit in a 12-char-array without overflow
  charArray = new char[12];
  if(charArray != NULL){
    int bufferRealSize = 0;

    //Will convert the integer given to a char array (charArray) and save its size (bufferRealSize)
    bufferRealSize = sprintf (charArray,"%d", integer);

    if(bufferRealSize > 0){

      //Create the buffer string
      for(int i=0; i < bufferRealSize; i++){
        addChar(&bufferInteger,charArray[i]);
      }
    }
  }
  return bufferInteger;
}

char* bufferStringToString(character* bufferString){
    char* string = NULL;
    if(bufferString != NULL){

      int numberOfChars = 0;

      //Move through the line char by char
      character* iterator = bufferString;
      while(iterator->next != NULL){

        //Count how many character there are
        numberOfChars++;
        iterator = iterator->next;
      }
      //Count the last character
      numberOfChars++;

      //Alocate a new string
      string = new char[(numberOfChars+1)];
      if(string != NULL){

        //Load the first character again to move throught the line
        iterator = bufferString;

        //Copy all chars from the buffer file to the new char array
        for(int i=0; i<(numberOfChars); i++){
          string[i] = iterator->data;
          iterator = iterator->next;
        }

        //Put the special character at it's end
        string[numberOfChars] = '\0';

      }
    }
    return string;
}

int getRuleValue(character* bufferFile, char* qualis){
  int ruleValue = -1;
  if(bufferFile != NULL){
    if(qualis != NULL ){
      if(atoi(qualis)>0){
        int qualisSize = strlen(qualis);
        if(qualisSize > 0){

          character* ruleFirstChar = find(bufferFile,qualis);
          //If the rule actually exist
          if(ruleFirstChar != NULL){

            //Go until its value
            while(ruleFirstChar->next != NULL && ruleFirstChar->data != '\0' && ruleFirstChar->data != ':'){
              ruleFirstChar = ruleFirstChar->next;
            }

            character* firstNumberDigit = NULL;
            //If the current character is double point
            if(ruleFirstChar->data == ':'){
              firstNumberDigit = ruleFirstChar->next;
              char* ruleValueString = NULL;
              ruleValueString = bufferStringToString(firstNumberDigit);
              ruleValue = atoi(ruleValueString);
            }
          }
        }
      }
    }
  }
  return ruleValue;
}

character* find(character* bufferFile, char* string){
  character* wordFound = NULL;

  if(bufferFile != NULL){
    if(string != NULL){
      int stringSize = strlen(string);
      int foundIt = 0;

      character* iterator = bufferFile;
      while(iterator->next){

        //Make a copy of iterator to no lost the addres of the current character
        character* iteratorCopy = iterator;

        for(int i=0 ; i<stringSize ; i++){

          //If the current char is equal to current character
          if(string[i] != iteratorCopy->data){
            break;
          }

          //Sinalize if some error happened
          if(iteratorCopy->next == NULL){
            foundIt = -1;
            break;
          }else{
            iteratorCopy = iteratorCopy->next;
          }

          //If the last char of the string is equal as well
          if(i == (stringSize-1)){
            foundIt = 1;
          }
        }
        //If the string was found
        if(foundIt == 1){

          //Copy the addres of its first character
          wordFound = iterator;
          break;
        }
        iterator = iterator->next;
      }
    }
  }
  return wordFound;
}

character* createChar(char data){
  character* newCharacter = NULL;

  newCharacter = new character[1];

  if(newCharacter != NULL){
    newCharacter->data = data;
    newCharacter->isNumber = -1; //Default for "I dont know"
    newCharacter->next = NULL;
    newCharacter->prev = NULL;
    newCharacter->last = NULL;
  }

  return newCharacter;
}

void destroyChar(character* charToDestroy){

  //If the user actually pass a valid character
  if(charToDestroy != NULL){
    delete[] charToDestroy;
  }
}

void addChar(character** string, char data){

  //If the user actually pass a empty string
  if(*string != NULL){

    //Create a new character
    character* newChar = new character[1];

    //Check if the new character was allocated
    if(newChar != NULL){

      //Check if the last character was not set yet
      if((*string)->last == NULL){

        //Iterate through string until the its end
        character* iterator = (*string);
        while(iterator->next != NULL){
          iterator = iterator->next;
        }

        //Check if the string is not a sub-string
        if((*string)->prev == NULL){

          //Save the address of the last character
          (*string)->last = iterator;
        }else{
          //Save the address of the last character
          character* lastCharacterAddress = iterator;

          //Go back until the given character as parameter
          iterator = (*string);

          //Iterate through string until the its head
          while(iterator->prev != NULL){
            iterator = iterator->prev;
          }

          //Save the address of the last character
          iterator->last = lastCharacterAddress;
        }
      }

      //Insert the new character at the string
      newChar->data = data;
      newChar->next = NULL;
      //Save the adress of the old last character
      newChar->prev = (*string)->last;
      //Make the old last character point to the new one
      (*string)->last->next = newChar;
      //Update the new last character
      (*string)->last = newChar;
    }
  }else{
    character* newString = createChar(data);
    newString->last = newString;
    *string = newString;
  }
}

character* createBufferFile(const char* filePath){

  //Create file a variable to read data from files
  ifstream file;

  //Trys to open the given file
  file.open(filePath);

  //Check if the file was open
  if(!file.is_open()){

    cerr << "Error while trying to open the file \"" << filePath << "\".";

  }else{

    //Create a temporary char to receive a file char by char
    char currentChar;

    //Get the first character in the file
    file >> noskipws >> currentChar;

    //The first character of a Buffer File
    character* bufferFile = createChar(currentChar);

    //Iterate through file, char by char, without skip whitespaces
    while(file >> noskipws >> currentChar){

      //Add each char to buffer file
      addChar(&bufferFile, currentChar);
    }
    file.close();
    return bufferFile;
  }
}

void destroyBufferFile(character** bufferFile){

  //If the user actually pass a valid buffer file
  if(bufferFile != NULL){

    //Get the address of the first character
    character* iterator = *bufferFile;

    //Go to the end of the buffer file
    while(iterator->next != NULL){
        iterator = iterator->next;
    }

    //Go to the start of the buffer
    while(iterator->prev != NULL){

      //Go backwards one character
      iterator = iterator->prev;

      //Desalocate the character that is after the current
      destroyChar(iterator->next);
    }

    //Destroy the first character (the only one remaing)
    destroyChar(iterator);
    *bufferFile = NULL;
  }
}

void printBufferFile(character* bufferFile){
  if(bufferFile != NULL){

    //Get the reference to the first character
    character* iterator = bufferFile;

    //While there is characters on the Buffer File
    while(iterator != NULL){
      //Print its text
      cout << iterator->data;

      //Go to the next character on the Buffer File
      iterator = iterator->next;
    }
  }
}

character* removeFirstBufferLine(character** bufferFile){
  character* iterator = NULL;
  //If the user actually pass a valid buffer file
  if(*bufferFile != NULL){

    //Get the address of the current character
    iterator = *bufferFile;

    //Go until the head of the file (for safety)
    while(iterator->prev != NULL){
      iterator = iterator->prev;
    }

    //Go until the first ocurrence of '\n'
    while(iterator->next != NULL && iterator->data != '\n'){
        iterator = iterator->next;
    }

    //Save the address of the character after the last '\n' if exist
    if(iterator->next != NULL){
      *bufferFile = iterator->next;
    }else{
      *bufferFile = NULL;
    }

    //Remove the reference to new buffer file
    iterator->next = NULL;

    //If the line is not the last one
    if(*bufferFile != NULL){
      //Separete the buffer file from the old line
      (*bufferFile)->prev = NULL;
    }

    //Go back to the head of the line
    while(iterator->prev != NULL){
      iterator = iterator->prev;
    }
  }
  return iterator;
}


char* getNthColumnData(character* bufferFile, int position){
  //A buffer line with a copy of the N position data
  char* nthText = NULL;

  if(bufferFile != NULL){
    if(position > 0){

      //The first character of the right data
      character* startPoint = NULL;
      //The last character of the right data
      character* finishPoint = NULL;

      int numberOfCommas = 0;

      //Go to the head of the bufferfile
      character* iterator = bufferFile;
      while(iterator->prev != NULL){
        iterator = iterator->prev;
      }


      //Make a backup of the first character address
      character* firstCharAddress = iterator;

      //Count the number of commas int the text
      while(iterator->next != NULL){
        if(iterator->data == ','){
          numberOfCommas++;
        }
        iterator = iterator->next;
      }

      iterator = firstCharAddress;

      //The required collumn does not exist
      if(position > (numberOfCommas+1)){

      //The last collumn is bein required
      }else if (position == (numberOfCommas+1)){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->data == ','){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == (position-1)){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go foward one char, to ignore read the comma
            iterator = iterator->next;
            while(iterator->next != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->data == ','){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->next;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            //int i = (numberOfChars-1);
            int i = 0;

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->next;
              while(iterator->next != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->data == ','){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i++;
                }
                iterator = iterator->next;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      //The collumn given is anyone but the last
      }else if((position < (numberOfCommas+1)) && position > 0){
        numberOfCommas = 0;
        //Goes until the given collumn
        while(iterator->next != NULL){
          if(iterator->data == ','){
            numberOfCommas++;
          }
          //Everything behind this comma until another is the wanted data
          if(numberOfCommas == position){

            //Count how many valid characters there's (Quotes are not valid)
            character* commaPosition = iterator;
            int numberOfChars = 0;

            //Go backwards one char, to ignore read the comma
            iterator = iterator->prev;
            while(iterator->prev != NULL){

              //everything before this quotes, belongs to other data
              if(iterator->data == ','){

                break;
              }

              //if the character is diferent of a quote
              if(iterator->data != '\"'){
                numberOfChars++;
              }
              iterator = iterator->prev;
            }
            //Alocate the string to receive each character
            nthText = new char[numberOfChars+1];
            //The extra char is for \0
            nthText[numberOfChars] = '\0';
            //The last valid character
            int i = (numberOfChars-1);

            //If the text was successfully allocated
            if(nthText != NULL){
              iterator = commaPosition->prev;
              while(iterator->prev != NULL){

                //Everything before this quotes, belongs to other data
                if(iterator->data == ','){
                  break;
                }
                //if the character is diferent of a quote
                if(iterator->data != '"'){
                  nthText[i] = iterator->data;
                  i--;
                }
                iterator = iterator->prev;
              }

            }else{
              cerr << "Error: Function: getNthColumnData. Desc: error while allocating nthText." << endl;
            }

            //Make the iterator go back to the position where it was
            iterator = commaPosition;
            //End the function
            break;
          }

          iterator = iterator->next;
        }
      }
      }
    }

  return nthText;
}

long stringToLong(char* numberToConvert){
  return atol(numberToConvert);
}

int stringToInt(char* numberToConvert){
  return atoi(numberToConvert);
}
