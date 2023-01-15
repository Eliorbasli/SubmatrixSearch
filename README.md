# SubmatrixSearch
Parallel implementaion of Submatrix Search

Final project Course 10324, Parallel and Distributed Computation

The program get matchingValue (double) and some pictures and objects from input file,
Master(rank 0 ) know to divide jobs for other process and finally the master recive result from all process,
if there are match in picture (there is object in picture ).

in this program i used in : 
-OpenMp
-Mpi


the problem: 
Picture(N) and Object(N) – are square matrices of integers with N rows and N columns. Each member of the matrix represents a “color”. The range of possible colors is [1, 100].
Position(I, J) defines a coordinates of the upper left corner of the Object into Picture. 
For each pair of overlapping members p and o of the Picture and Object we will calculate a relative difference
				diff =  abs((p – o)/p)
The total difference is defined as a sum of all relative differences for all overlapping members for given Position(I, J) of the Object into Picture. We will call it Matching(I, J).
For example, for the Picture and Object from the Fig.1 the matching at Position(0,0) is equal
Matching(0,0) = abs((10-5)/10) + abs((5-14)/5) + abs((67-9)/67) + abs((23-20)/23) + abs((6-56)/6) +
abs((5-2)/5) + abs((12-6)/12) + abs((10-10)/10) + abs((20-3)/20)

![image](https://user-images.githubusercontent.com/45131527/212557705-4031a446-b2ae-44e2-9440-8f8ef909883e.png)

![image](https://user-images.githubusercontent.com/45131527/212557715-ed9270ef-0f0f-41c3-9d3b-c325324d7c88.png)

This project deals with a “recognition” if there is a Position(I,J) of the Object into Picture with a Matchin(I,J) less than the given value.

Input data and Output Result of the project

The input file contains in the first line a Matching Value. Next line contains a number of Pictures in the file. For all pictures the first line defines its ID, next line contains the picture size, followed by picture elements. The same order is for objects as well. 




