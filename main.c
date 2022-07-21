#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <mpi.h>
#include "function.h"
#define FILE_NAME "input.txt"
#define FILE_SAVE "output33.txt"

int main(int argc, char* argv[])
{
	int  my_rank; /* rank of process */
	int  p;       /* number of processes */
	int source;   /* rank of sender */
	int dest;     /* rank of receiver */
	int tag=0;    /* tag for messages */

	matrixObject* pictures;
	matrixObject* objects;
	matchObject isMatch;
	matchObject* matchArr;
	int pictureSize = 0;
	int objectSize = 0;
	int matchingValue = 0 ; 
	int matchCount = 0 ; 
	double t1, t2;

	/* start up MPI */
	MPI_Init(&argc, &argv);
	
	/* find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 
	
	/* find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &p); 


	t1 = omp_get_wtime();
	readFromFile(FILE_NAME , &pictures ,&pictureSize ,  &objects , &objectSize , &matchingValue);

	matchArr = (matchObject*)malloc((pictureSize)* sizeof(matchObject));
	for(int i = 0 ; i < pictureSize ; i++){
		for(int j = 0 ; j< objectSize ; j++)
			if(matrixSearch(pictures[i] , objects[j] , matchingValue , &isMatch)){
				matchArr = (matchObject*)realloc(matchArr , (matchCount+1)*sizeof(matchObject));
				matchArr[i] = isMatch;
				break;
			}else{
				matchArr[i].Status = 0;
			}
	}

	for(int i = 0 ; i < matchCount ; i++){
		printf("i = %d , %d %d %d %d\n" ,i , matchArr[i].PictureID , matchArr[i].ObjectID , matchArr[i].PositionX, matchArr[i].PositionY);
	}

	t2 = omp_get_wtime();
	printf("It's take: %f sec\n",  (t2 - t1));

	saveToFile(FILE_SAVE ,matchArr ,pictureSize);	

	MPI_Finalize(); 
}


