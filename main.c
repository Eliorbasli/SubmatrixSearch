#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
#include <time.h>
#include "function.h"
#define FILE_NAME "input __ N lines with N members.txt"
#define FILE_SAVE "output.txt"

	// struct MatchObject{
	// 	    int Status;
    // 	int PictureID;
    // 	int ObjectID;
    // 	int PositionX;
    // 	int PositionY; 
	// };

int main(int argc, char* argv[])
{
	int  my_rank; /* rank of process */
	int  numprocs;       /* number of processes */
	int source;   /* rank of sender */
	int dest;     /* rank of receiver */
	int tag=0;    /* tag for messages */

	matrixObject* pictures;
	matrixObject picture;
	matrixObject* objects;
	matchObject match;
	matchObject* matchArr;
	int pictureSize = 0;
	int objectSize = 0;
	int pictureForMaster;
	int picturePerProces;

	double matchingValue = 0 ; 
	int matchCount = 0 ; 
	double t1, t2;
	int isMatch;
	int x;
	int indexMaster;

	/* start up MPI */
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs); 

	t1 = omp_get_wtime();


	//----------------------------------- Master -----------------------------------
	/* Master is process number 0 , Master need read from file , create allocation for pictures and objects and resultMatch ,
	 sent to Slave some picture and all objets, after master recive from all salves the result and 
	finnaly print the reulst to console and to file */
	if(my_rank == MASTER ){
		readFromFile(FILE_NAME , &pictures ,&pictureSize ,  &objects , &objectSize , &matchingValue );

		masterSendDataToSlaves( pictures , pictureSize , objects , objectSize , matchingValue , numprocs , &picturePerProces , &pictureForMaster , &indexMaster);
		
		matchArr = (matchObject*)malloc((pictureSize )* sizeof(matchObject));
		if(!matchArr)
			printf("error with allocation\n");

		masterSearchMatch(my_rank ,pictureSize , picturePerProces , objectSize , matchingValue ,  pictures , objects , matchArr , indexMaster );
		
		// master get from all process the reuslt of match of picture with object
		masterCollectDataFromProces(numprocs , picturePerProces , pictureSize , my_rank , matchArr , pictureForMaster);

		saveToFile(FILE_SAVE ,matchArr ,pictureSize);	

		printf("\nwrite to file \"%s\" the result is sucsseced \n" , FILE_SAVE);

		t2 = omp_get_wtime();
		printf("\n\tIt's take: %f sec\n",  (t2 - t1));

		freeMemoryMaster( pictureSize , objectSize , &pictures , objects , &matchArr );
	}

	//----------------------------------- Slave -----------------------------------
	//Salve is all procss that rank not eqauls to 0 , Slave need get from Master picture and all objects ,
	// run slaveSrarchMatch function and sent the result to Master, finnally free all memory allocation
	if(my_rank != MASTER ){

		slaveReciveDataFromMaster(my_rank , matchingValue , &pictureSize ,&objectSize ,&pictures , &objects , &picturePerProces );
	
		salveSearchMatch(my_rank ,pictureSize , picturePerProces , objectSize , matchingValue , pictures ,objects );

		freeMemorySlave(pictureSize , objectSize , &pictures , objects );

	}
	
	MPI_Finalize(); 
}
