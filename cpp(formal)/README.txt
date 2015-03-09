C++ version (Formal):

	Including seven files:
	—SparseVector.hh, SparseVector.cc:
	    Implementation of sparse vector class. 
	    SparseVector.index = movie id / user id,
	    SparseVector.value = rating.
 	    Similarity calculation is a method of this class;
	—kFoldCross.hh, kFoldCross.cc:
	    Implementation of k-fold cross validation class. Using a vector with the same 	    size with original data. Random shuffle it into 5 set.
	—RecoSys.hh, RecoSys.cc:
	    Implementation of recommend system class. Three different CF methods are 		    detailed here.
	—CF.cpp
	    Main function, including reading cvs files, reading from command line and 		    calling the instantiate the RecoSys. Data reading from cvs files are stored in 	    the unordered_map, user id/movie id as the hash, sparse vector in the bucket.

	The cpp program implementing the time calculating function printing out the time 	spent on every 1000 ratings for estimating the program running time and further 	optimization. 

	Every 1000 ratings will cost from 50 sec to 70 sec.
	
	To compile:
		g++ -std=c++0x CF.cc -o CF.o
	To run:
 		./CF.o <para1> <para2> <para3>
	See help:
		./CF.o

Output:
	First line is user id, second line is movie id, third line is predicted rating.
