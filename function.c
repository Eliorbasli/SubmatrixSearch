#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "function.h"
#include <mpi.h>
#include <omp.h>

int readFromFile(const char* fileName , matrixObject** pictures , int* pictureSize , matrixObject** objects , int* pObjSize , double* matchingValue )
{
    double match ;
    int PicturesNumers;
    matrixObject* PictureArr;
    matrixObject* objectArr;
    FILE *fp;

    fp = fopen(fileName , "r");
    if(!fp ){
        checkAllocation();
    }

    fscanf(fp , "%le" , &match);
    fscanf(fp , "%d" , &PicturesNumers);
    
    *matchingValue = match;
    *pictureSize = PicturesNumers;
 //------------------read pictures--------------------//

    PictureArr = (matrixObject*)malloc(PicturesNumers * sizeof(matrixObject));
    if(!PictureArr)
        checkAllocation(); 
    

    for (int i = 0 ; i < PicturesNumers ; i ++)
    {    
        readMat (fp , &PictureArr[i]);    
    }

    *pictures = PictureArr;
   
  //------------------read objects--------------------//
    int numberOfObjects = 0 ; 
    
    fscanf(fp , "%d" , &numberOfObjects);

    objectArr = (matrixObject*)malloc(numberOfObjects * sizeof(matrixObject));
    if(!objectArr)
        checkAllocation(); 
    
    for (int i = 0 ; i < numberOfObjects ; i ++)
    {
        readMat (fp , &objectArr[i]);   
    }
    fclose(fp);

    printf("from file \"%s\" => matchValue %le , pictureNumbers %d , objectsNumber %d\n", fileName , match , PicturesNumers , numberOfObjects);
   *objects = objectArr; 
   *pObjSize = numberOfObjects;
 
}

//this function get array of pictures , objects , and there size , 
//it know to divide the picture to right process , is use dynamic divide
void masterSendDataToSlaves(matrixObject* pictures , int pictureSize , matrixObject* objects , int objectSize , int matchingValue , int numprocs , int* picturePerProces , int* pictureForMaster  , int* indexMaster)
{
    int pictureToSend = 0 ; 
    int msaterPicture = 0 ; 
    int index ;
    int processRun;
    if(numprocs < pictureSize){
        pictureToSend = pictureSize / numprocs;
        processRun = numprocs;
    }
    else{
        pictureToSend =1 ; 
        processRun = pictureSize;
    }    
    *picturePerProces = pictureToSend;

   // printf("pictureToSend %d , picturetomaster %d\n" , pictureToSend , msaterPicture);
    printf("threre are %d pictures , each process get %d\n" , pictureSize , pictureToSend);

    // master sent for all process the number of pictures.
    MPI_Bcast(&pictureToSend , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    index = 0 ; 
    for(int i = 1 ; i < numprocs ; i++ )
    {
       for(int j = 0 ; j < pictureToSend ; j++)
       {
            MPI_Send(&pictures[index].ID ,1  , MPI_INT , i , 0 , MPI_COMM_WORLD);
            MPI_Send(&pictures[index].size ,1  , MPI_INT , i , 0 , MPI_COMM_WORLD);        
            printf("picture %d sent to proces %d \n", index ,i );
    
        for(int k = 0 ; k < pictures[index].size ; k++)
        {
            for(int g = 0 ; g < pictures[index].size ; g++)
            {
                MPI_Send(&pictures[index].matrixObj[k][g] ,1  , MPI_INT , i , 0 , MPI_COMM_WORLD);
            }	
        }        
        index++;
       }
   
    }

    if(index != pictureSize)
    {
        msaterPicture = pictureSize - index; 
    }
    *indexMaster = index;
    *pictureForMaster = msaterPicture;

    printf("master get %d picture ,start from %d\n\n" , msaterPicture , index);
///////////////////////////////////////////////
    // master sent for all process number of objects and matchingValue by brodCast
    MPI_Bcast(&objectSize , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
    MPI_Bcast(&matchingValue , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

    for(int i = 0 ; i < objectSize ; i ++)
    {
        MPI_Bcast(&objects[i].ID , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
        MPI_Bcast(&objects[i].size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);	
        
       // printf("from master objectID %d , objectssize %d\n" , objects[i].ID , objects[i].size);

        for(int j = 0 ; j < objects[i].size ; j++)
        {
            for(int g = 0 ; g < objects[i].size ; g++)
            {
                MPI_Bcast(&objects[i].matrixObj[j][g] , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
            }
        }
    }
    
}

void slaveReciveDataFromMaster(int my_rank , int matchingValue , int* pictureSize , int* pObjSize ,matrixObject** pPictures ,matrixObject** pObjects , int* picturePerProces){
        matrixObject* pictures;
        matrixObject* objects;
        matchObject match;
        int isMatch;
        int objectSize = 0 ;
        int temp;
        int pictureToRecive;
        MPI_Bcast(&pictureToRecive , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
        
        *picturePerProces = pictureToRecive;
        pictures = (matrixObject*)malloc(pictureToRecive*sizeof(matrixObject));
         if(!pictures)
             checkAllocation();

        for(int i = 0 ; i < pictureToRecive ; i ++){
            MPI_Recv(&pictures[i].ID , 1 , MPI_INT , 0 , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            MPI_Recv(&pictures[i].size , 1 , MPI_INT , 0 , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
           // printf("\n-------from rank %d , picture number Recive %d picuter id---\n" , my_rank , pictures[i].ID);
		    
		    int** mat = (int**)malloc((pictures[i].size)*sizeof(int*));
            if(!mat)
                checkAllocation();
            for(int j = 0 ; j < pictures[i].size ; j ++)
            {
                mat[j] = (int*)malloc(pictures[i].size * sizeof(int));
                if(!mat[j])
                    checkAllocation();
                for(int g = 0 ; g < pictures[i].size ; g++)
                {
                    MPI_Recv(&temp , 1 , MPI_INT , 0 , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    mat[j][g] = temp;          
                }
            }
            pictures[i].matrixObj =mat;
        }

		MPI_Bcast(&objectSize , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
		MPI_Bcast(&matchingValue , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

		objects = (matrixObject*)malloc(objectSize * sizeof(matrixObject));
		if(!objects)
			printf("error with allocatioin\n");

		// get objects from Master by brodCast 
		for(int i = 0 ; i < objectSize ; i ++){
			MPI_Bcast(&objects[i].ID , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
			MPI_Bcast(&objects[i].size , 1 , MPI_INT , 0 , MPI_COMM_WORLD);

			objects[i].matrixObj= (int**)malloc((objects[i].size) * sizeof(int*)); 
			if(!objects[i].matrixObj)
				printf("error with allocatioin\n");
			
			for(int j = 0 ; j < objects[i].size ; j ++)
			{
				objects[i].matrixObj[j] = (int*)malloc((objects[i].size  * sizeof(int)));
				for(int g = 0 ; g < objects[i].size ; g++ )
				{
					MPI_Bcast(&objects[i].matrixObj[j][g] , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
				}
			}

		 }

        *pPictures = pictures;
        *pictureSize =pictureToRecive;
        *pObjSize = objectSize; 
        *pObjects = objects;       
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
    int isMatch1 = 0 ;
    double sumVal = 0 ; 
    matrixObject* objectArr;
    int i , j ,g ,k ;
    
    #pragma omp parallel private (i ,j  , num1 , num2 ,sumVal)
    {
        if(isMatch1 == 0)
        {
        #pragma omp for
        for( i = 0 ; i < pPicture.size - pObject.size + 1 ; i ++ )
        {
            for( j = 0 ; j < pPicture.size - pObject.size + 1 ; j ++ )
            {
                for(int k = 0 ; k < pObject.size; k++)
                {
                    for(int  g = 0 ; g < pObject.size ; g++)
                    {
                        num1 = pPicture.matrixObj[i+k][j+g];
                        num2 = pObject.matrixObj[k][g];
                
                        sumVal = sumVal + fabs((double)(num1 -num2)/num1);

                        // if sunVal is bigger from matchingValue actully we not need to continue 
                        //with sum the other objets
                        if(sumVal > matchingValue)
                        {
                            g = pObject.size;
                            k = pObject.size; 
                        }
                    }
                }
            #pragma omp critical
            {
                if(sumVal <= matchingValue && isMatch1 != 1)
                {
                    // if we get match, we want to save some information about picture, object and position
                    match->Status = 1;
                    match->PictureID = pPicture.ID;
                    match->ObjectID = pObject.ID;
                    match->PositionX = i;
                    match->PositionY = j;
                    
                    // jump to end eqauls to break
                    i = pPicture.size - pObject.size +1;
                    j = pPicture.size - pObject.size+1 ;

                    isMatch1 = 1; 
                    printf("pictureID %d match with objectID %d in  x : %d , y %d \nstop search match for picture number %d\n"  , match->PictureID , match->ObjectID , match->PositionX , match->PositionY , match->PictureID);  
                }else
                    sumVal = 0;
                    }
            }
        }
        }

    }
    //printf("#Picture %d No Objects %d were found\n" , pPicture.ID , pObject.ID);
    return isMatch1;
}


//this function save to txt file, the status about all pictures from input file
int saveToFile(const char* fileName ,matchObject* arr , int size ){
    FILE *fp = fopen(fileName , "w");
    if(fp == 0 ){
        checkAllocation();
    }
    
    for(int i = 0 ; i < size ; i++)
     {
        if(arr[i].Status == 1)
            fprintf(fp , "Picture %d: found Object %d in Position (%d , %d) \n" , arr[i].PictureID , arr[i].ObjectID ,arr[i].PositionX , arr[i].PositionY);
        else
            fprintf(fp ,"Picture %d No Object were found\n",arr[i].PictureID );
     }
     fclose(fp);
    return 1;
     
}

void checkAllocation (){
    fprintf(stderr,"Allocation error\n");  
}




//Master wait to all process, end the search and wait to recive from them the result.
void masterCollectDataFromProces(int numprocs , int picturePerProces , int pictureSize , int my_rank , matchObject* matchArr , int pictureForMaster)
{
    int index = 0  ; 
    int even; 

    if(pictureForMaster % 2 == 0 )
        even = 0;
    else 
        even =1 ;
    if(!matchArr)
        checkAllocation();
        
    for(int i = 1 ; i <  numprocs ; i ++)
    {
        for(int j = 0 ; j < picturePerProces ; j++)
        {
            if(index < pictureSize - pictureForMaster - even)
            {
                //printf("\twait for rank %d \t index  %d....\n" , i , index);        
                MPI_Recv(&matchArr[index].PictureID , 1 , MPI_INT , i , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&matchArr[index].ObjectID , 1 , MPI_INT , i , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&matchArr[index].PositionX , 1 , MPI_INT , i , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&matchArr[index].PositionY , 1 , MPI_INT , i , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                MPI_Recv(&matchArr[index].Status , 1 , MPI_INT , i , 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                //printf("\tRecived from rank %d, picture %d , object %d , x: %d , y : %d \n", i , matchArr[index].PictureID ,matchArr[index].ObjectID , matchArr[index].PositionX , matchArr[index].PositionY  );
                index  ++ ; 
            }
        }
    }

}

//Slave search match and sent the result to Master
void salveSearchMatch(int my_rank , int pictureSize ,int picturePerProces , int objectSize ,int matchingValue , matrixObject* pictures ,matrixObject* objects )
{

    if(my_rank <= pictureSize)
    {
        int isMatch;
        matchObject match1;
        for(int i = 0 ; i < picturePerProces ; i ++)
        {
        //    printf("rank %d , picture %d \n ",my_rank , pictures[i].ID );       
            isMatch = 0 ;
            if(isMatch == 0 )
            {
                for(int j = 0 ; j< objectSize ; j++)
                {
                    if(isMatch == 0 && matrixSearch(pictures[i] , objects[j] , matchingValue , &match1))
                    {
                        //printf("\n\tthere is a mathc!! from rank %d , picture %d , object %d x %d y %d\n" , my_rank ,pictures[i].ID, match1.ObjectID , match1.PositionX , match1.PositionY);
                        isMatch = 1 ; 
                    }
                }
            }
            printf("rank %d sent the result to Master ,pictureID : %d ,  x : %d y : %d\n" , my_rank ,match1.PictureID ,  match1.PositionX , match1.PositionY );
            MPI_Send(&match1.PictureID , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
            MPI_Send(&match1.ObjectID , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
            MPI_Send(&match1.PositionX , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
            MPI_Send(&match1.PositionY , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
            MPI_Send(&match1.Status , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD);
        }
    }
}

//master search match and save the result to matchArray and not sent to somebody
void masterSearchMatch(int my_rank , int pictureSize ,int picturePerProces , int objectSize ,int matchingValue , matrixObject* pictures ,matrixObject* objects , matchObject* matchArr , int indexMaster){
    int isMatch;
    matchObject match;
    for(int i = indexMaster ; i < pictureSize ; i ++)
        {
           
		    isMatch = 0 ;		    
            if(isMatch == 0 )
		    {
                for(int j = 0 ; j< objectSize ; j++)
                {
                    if(isMatch == 0 && matrixSearch(pictures[i] , objects[j] , matchingValue , &match))
                    {
                    
                        isMatch = 1 ; 
                       matchArr[i] = match;
                    }
                }
		    }
            if(isMatch == 0 ){
                matchArr[i].PositionX = 0 ;
				matchArr[i].PictureID = pictures[i].ID;
				matchArr[i].Status = 0 ; 
            }
        }
}

// this function used for read picture and object from file
// picture and object is same thing
void readMat (FILE* fp , matrixObject* mat)
{
    int sizeOfMat;
    int matID;
    int  g ,j;
    fscanf(fp , "%d" , &matID);
    fscanf(fp , "%d" , &sizeOfMat);        // Read the picture size
    mat->ID = matID;
    mat->size = sizeOfMat;    
    mat->matrixObj = (int**)malloc(sizeOfMat * sizeof(int*));

    for(j = 0 ; j < sizeOfMat ; j ++){
        mat->matrixObj[j] = (int*)malloc(sizeOfMat * sizeof(int));
        for( g= 0 ; g<sizeOfMat ; g++){
            fscanf(fp , "%d" , &mat->matrixObj[j][g]);
        }
    }   
}

void freeMemoryMaster(int pictureSize , int objectSize , matrixObject** pictures ,matrixObject* objects , matchObject** matchArr ){
    if(!matchArr )
        printf("dasdasdas\n");
    for(int i = 0 ; i < pictureSize ; i ++)
    {
        for(int j = 0 ; j < pictures[i]->size ; j ++){
            free(pictures[i]->matrixObj[j]);
        }
        free(pictures[i]);
    }

    for(int i = 0 ; i < objectSize ; i ++)
    {
         for(int j = 0 ; j < objects[i].size  ; j ++){
            free(objects[i].matrixObj[j]);
         }
    }
    
    

}

void freeMemorySlave(int pictureSize , int objectSize , matrixObject** pictures ,matrixObject* objects ){
    for(int i = 0 ; i < pictureSize ; i ++ ){
        for(int j = 0 ; j < pictures[i]->size ; j ++){
            free(pictures[i]->matrixObj[j]);
        }
        free(pictures[i]);
        
    }

    for(int i = 0 ; i < objectSize ; i ++)
    {
         for(int j = 0 ; j < objects[i].size  ; j ++){
            free(objects[i].matrixObj[j]);
         }
    }
}
