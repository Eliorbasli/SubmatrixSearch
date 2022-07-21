#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "function.h"


int readFromFile(const char* fileName , matrixObject** pictures , int* pictureSize , matrixObject** objects , int* pObjSize , int* matchingValue)
{
    int match ;
    int PicturesNumers;
	int PictureID = 0 ;
    int ObjectID = 0 ;
    int size = 0 ; 
    int sizePicture = 0 ; 

    matrixObject* PictureArr;
    matrixObject* objectArr;
    
    FILE *fp;

    fp = fopen(fileName , "r");
    if(fp == 0 ){
        printf("error with open file\n");
        return 0; 
    }

    fscanf(fp , "%d" , &match);
    fscanf(fp , "%d" , &PicturesNumers);
	
	//printf(" match :%d\n ", match );
    
    //------------------read pictures--------------------//
	//printf(" PicturesNumers :%d\n ", PicturesNumers );

    PictureArr = (matrixObject*)malloc(PicturesNumers * sizeof(matrixObject));
    if(!PictureArr){
        printf("error with allocation\n");
        return 0 ; 
    }
    for (int i = 0 ; i < PicturesNumers ; i ++){
		fscanf(fp , "%d" , &PictureID);
        fscanf(fp , "%d" , &sizePicture);        // Read the picture size
        
        PictureArr[i].ID = PictureID;
        PictureArr[i].size = sizePicture;
		
        PictureArr[i].matrixObj = (int**)malloc(sizePicture * sizeof(int*));
        for(int j = 0 ; j < sizePicture ; j ++){
            PictureArr[i].matrixObj[j] = (int*)malloc(sizePicture * sizeof(int));
            for(int g= 0 ; g<sizePicture ; g++){
                fscanf(fp , "%d" , &PictureArr[i].matrixObj[j][g]);
            }
        }   
    }
    *matchingValue = match;
    *pictures = PictureArr;
    *pictureSize = PicturesNumers;

     //------------------read objects--------------------//
    int numberOfObjects = 0 ; 
    int numberObject = 0 ; 
    int objectSize = 0 ; 
   
    fscanf(fp , "%d" , &numberOfObjects);

    objectArr = (matrixObject*)malloc(numberOfObjects * sizeof(matrixObject));
    if(!objectArr){
        printf("error with allocation\n");
        return 0 ; 
    }
       
    for (int i = 0 ; i < numberOfObjects ; i ++)
    {
        fscanf(fp , "%d" , &ObjectID);
        objectArr[i].ID = ObjectID ; 

        fscanf(fp , "%d" , &objectSize);

        objectArr[i].size = objectSize;
        objectArr[i].matrixObj = (int**)malloc((objectSize) * sizeof(int*));
        
        if(!objectArr[i].matrixObj){
           printf("error with allocation\n");
            return 0 ;  
        }

        for(int j = 0 ; j < objectSize ; j ++)
        {
            objectArr[i].matrixObj[j] = (int*)malloc(objectSize * sizeof(int));
            for (int g= 0 ; g < objectSize; g++)
            {
                fscanf(fp , "%d" , &objectArr[i].matrixObj[j][g]);
            }
        
         }
            
    }
    fclose(fp);
   *objects = objectArr; 
   *pObjSize = numberOfObjects;
}


void printMat (int** pMat , int size){
    for(int i = 0 ; i < size ; i ++ )
    {
        for(int j = 0 ; j < size ; j++)
        {
           printf("%d " , pMat[i][j]);
        }
        printf("\n");
    }
}



int matrixSearch(matrixObject pPicture , matrixObject pObject , int matchingValue ,matchObject* match ){

    int num1 = 0;
    int num2 = 0;
    int matchCount = 0; 
    double sumVal = 0 ; 
    matrixObject* objectArr;

    for(int i = 0 ; i < pPicture.size - pObject.size + 1 ; i ++ )
    {
        for(int j = 0 ; j < pPicture.size - pObject.size + 1 ; j ++ )
        {
            for(int k = 0 ; k < pObject.size; k++)
            {
                for(int g = 0 ; g < pObject.size ; g++)
                {
                    num1 = pPicture.matrixObj[i+k][j+g];
                    num2 = pObject.matrixObj[k][g];

                    sumVal = sumVal + fabs((num1 -num2)/num1);
                }
            }
            if(sumVal <= 0 )
            {
                printf("we have matching!!! Picture %d: found Object %d , (%d , %d)\n" , pPicture.ID , pObject.ID , i , j);
                match->Status = 1;
                match->PictureID = pPicture.ID;
                match->ObjectID = pObject.ID;
                match->PositionX = i;
                match->PositionY = j;
                
                return 1;
            }
            else
                sumVal = 0;
        }
    }
    printf("Picture %d No Objects were found\n" , pPicture.ID);
    return 0;
}



int saveToFile(const char* fileName ,matchObject* arr , int size ){
    FILE *fp = fopen(fileName , "w");
    if(fp == 0 ){
        printf("error with open file\n");
        return 0; 
    }
    
    for(int i = 0 ; i < size ; i++)
     {
        if(arr[i].Status == 1)
            fprintf(fp , "Picture %d: found Object %d in Position (%d , %d) \n" , arr[i].PictureID , arr[i].ObjectID ,arr[i].PositionX , arr[i].PositionY);
        else
            fprintf(fp ,"Picture %d No Object were found\n",i );
     }
    return 1;
    
    
}