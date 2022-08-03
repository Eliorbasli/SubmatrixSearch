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

enum ranks{
    MASTER, 
    WORKER
};



int readFromFile(const char* fileName , matrixObject** pictures , int* pictureSize , matrixObject** objects , int* pObjSize , double* matchingValue);

void masterSendDataToSlaves(matrixObject* pictures , int pictureSize , matrixObject* objects , int objectSize , int matchingValue , int numprocs , int* picturePerProces , int* pictureForMaster , int* indexMaster);

void slaveReciveDataFromMaster(int my_rank , int matchingValue , int* pictureSize ,int* pObjSize ,matrixObject** pPictures ,matrixObject** pObjects , int* picturePerProces );

void printMat(int** Mat , int size);

int matrixSearch(matrixObject pPicture , matrixObject pObject , int matchingValue ,matchObject* match); 

int saveToFile(const char* fileName ,matchObject* arr , int size);

void checkAllocation ();

void masterCollectDataFromProces(int numprocs , int picturePerProces , int pictureSize , int my_rank , matchObject* matchArr , int pictureForMaster);


void salveSearchMatch(int my_rank , int pictureSize ,int picturePerProces , int objectSize ,int matchingValue , matrixObject* pictures ,matrixObject* objects );


void masterSearchMatch(int my_rank , int pictureSize ,int picturePerProces , int objectSize ,int matchingValue , matrixObject* pictures ,matrixObject* objects , matchObject* matchArr , int indexMaster);
//void masterSearchSubMatrix(int picturePerProces , int objectSize , matrixObject** pictures , matrixObject** objects , int matchingValue , int my_rank , matchObject* matchArr);

void readMat (FILE* fp , matrixObject* mat);

void freeMemoryMaster(int pictureSize , int objectSize , matrixObject** pictures ,matrixObject* objects , matchObject** matchArr );

void freeMemorySlave(int pictureSize , int objectSize , matrixObject** pictures ,matrixObject* objects );
