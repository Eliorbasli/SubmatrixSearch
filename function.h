#pragma once


#define NUM = 3 

typedef struct 
{
    int ID;
    int size;
    int** matrixObj ; 
}matrixObject;

typedef struct 
{
    int Status;
    int PictureID;
    int ObjectID;
    int PositionX;
    int PositionY; 
    
}matchObject;




int readFromFile(const char* fileName , matrixObject** pictures , int* pictureSize , matrixObject** objects , int* pObjSize , int* matchingValue);

void printMat(int** Mat , int size);

int matrixSearch(matrixObject pPicture , matrixObject pObject , int matchingValue ,matchObject* match); 

int saveToFile(const char* fileName ,matchObject* arr , int size);