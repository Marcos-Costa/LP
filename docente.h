#ifndef DOCENTE_H
  #define DOCENTE_H

  typedef struct _docente{
    //Basic info given on the file
    long id;
    char* name;

    //Info used during the processing
    int totalPoints;
    int* points;
    //Double linked list struct atributes
    struct _docente* next;
    struct _docente* prev;
    struct _docente* last;


  }docente;
  
  docente* orderDocentes(docente* docentes);
  docente* split(docente* docentes);
  docente* merge(docente*first, docente*second);

  docente* createDocente(long id, char* name);
  void destroyDocente(docente* docenteToDestroy);

  void addDocente(docente** docenteList, long id, char* name);
  void destroyAllDocentes(docente** docentes);

  docente* loadAllDocentes(const char* filePath);
#endif
