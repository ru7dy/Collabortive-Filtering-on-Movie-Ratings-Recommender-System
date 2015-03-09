#include "RecoSys.hh"
#include <numeric>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <iostream>


using namespace std;

RecoSys::RecoSys(string _mode, const data_t &um, const data_t &usr, const data_t &mov, const data_t &pred, bool val)
{
	mode = _mode;
	validation = val;
	usr_info_set.resize(usr.size());
	mov_info_set.resize(mov.size());
	pred_set.resize(pred.size());
	   // cout << "I;m here" << endl;
	copy(usr.begin(),usr.end(),usr_info_set.begin());
	copy(mov.begin(), mov.end(), mov_info_set.begin());
	copy(pred.begin(),pred.end(),pred_set.begin());

	if (mode == "User")
	{
		for (unsigned n = 0; n < um.size(); n++)
	    {
	          if (util_mat.count(um[n][0]) == 1)
	          {
	                // user id exsits
	                util_mat[um[n][0]]->setElem(um[n][1], um[n][2]);
	          }
	          else
	          {
	                // user id is new
	                // SparseVector movie
	                util_mat[um[n][0]] = new SparseVector(1);
	                util_mat[um[n][0]]->setElem(um[n][1], um[n][2]);
	          }
	    }

	}
	else if (mode == "Item")
	{
		for (unsigned n = 0; n < um.size(); n++)
	    {
	          if (util_mat.count(um[n][1]) == 1)
	          {
	                // Movie id exsits
	          		// cout << um[n][1] << endl;
	                util_mat[um[n][1]]->setElem(um[n][0], um[n][2]);
	          }
	          else
	          {
	                // user id is new
	                // SparseVector movie
	                util_mat[um[n][1]] = new SparseVector(1);
	                util_mat[um[n][1]]->setElem(um[n][0], um[n][2]);
	          }
	    }
	}
	else
	{
		// combination 
		for (unsigned n = 0; n < um.size(); n++)
	    {
	          if (util_mat.count(um[n][0]) == 1)
	          {
	                // user id exsits
	                util_mat[um[n][0]]->setElem(um[n][1], um[n][2]);
	          }
	          else
	          {
	                // user id is new
	                // SparseVector movie
	                util_mat[um[n][0]] = new SparseVector(1);
	                util_mat[um[n][0]]->setElem(um[n][1], um[n][2]);
	          }
	    }

		for (unsigned n = 0; n < usr_info_set.size(); n++)
	    {

	          
	                // user id is new
	                // SparseVector movie
	                usr_info_mat[usr_info_set[n][0]] = new int[3];
	                // sexuality
	                if (usr_info_set[n][1] == (int)('M'))
	                	usr_info_mat[usr_info_set[n][0]][0] = 1;
	                else
	                	usr_info_mat[usr_info_set[n][0]][0] = 0;
	                // age
	                usr_info_mat[usr_info_set[n][0]][1] = usr_info_set[n][2];
	                // occupation
	                usr_info_mat[usr_info_set[n][0]][2] = usr_info_set[n][3];	          
	    }

	}

}

RecoSys::~RecoSys()
{
	for (unsigned j = 0; j < usr_info_set.size(); j++)
	{

		delete util_mat[usr_info_set[j][0]];

		if (mode == "CombineMethod")
			delete[] usr_info_mat[usr_info_set[j][0]];
	}

}



void RecoSys::UserBased(string sim_mode)
{
	int count = 0;
	double error = 0;
    clock_t start, finish;
    double duration;
    start = clock();
    ofstream output_file("UserBased_Result_cpp.csv");
	for (unsigned i = 0; i < pred_set.size(); i++)
    {
    	double ratings_sim = 0.0;
    	double sim = 0.0;
    	double sum_sim = 0.0;
    	double cur_rating = 0.0;
        count++;
        // cout << " i am here" << endl;
        	
	        for (unsigned j = 0; j < usr_info_set.size(); j++)
	        {
	        	if (util_mat.count(usr_info_set[j][0]) == 0)
		        {
		        	continue;
				}
				else
				{
		          if (util_mat[usr_info_set[j][0]]->getElem(pred_set[i][1]) == 0)
		          {
		              // do nothing
		          }
		          else
		          {		       
		          	  if (sim_mode == "cosine")
		          	  {
		          	  	if (util_mat.count(pred_set[i][0]))
		              	  	sim = util_mat[usr_info_set[j][0]]->cosine(*util_mat[pred_set[i][0]], mode);
		              	else
		              		sim = 0.0;
		              }
		              else if (sim_mode == "pearson")
		              {
		              	 if (util_mat.count(pred_set[i][0]))
		              	  	sim = util_mat[usr_info_set[j][0]]->pearson(*util_mat[pred_set[i][0]], mode);
		              	 else
		              		sim = 0.0;
		              }
		              else if (sim_mode == "jaccard")
		              {
		              	 if (util_mat.count(pred_set[i][0]))
		              	  	sim = util_mat[usr_info_set[j][0]]->jaccard(*util_mat[pred_set[i][0]]);
		              	 else
		              		sim = 0.0;
		              }
		              else
		              	  cerr << "You have typed a WRONG similarity type!" << endl;
		              if (sim > 0)
		              {
		              	  sum_sim += sim;
		              	  ratings_sim += sim * util_mat[usr_info_set[j][0]]->getElem(pred_set[i][1]);
		              }
		          }
		    }
		}
	   	if (sum_sim > 0)
	   		cur_rating = ratings_sim / sum_sim;
	   	else 
	   		cur_rating = 3.0;
	   	if (!validation)
	   	{	   		
	       	output_file << pred_set[i][0] << "," <<  pred_set[i][1] << "," << cur_rating << endl;
	   	}
	   	else
	   	{	   		
	   		error += pow(cur_rating - pred_set[i][2], 2.0);
	   	}
	       if (count % 1000 == 0)
	       {
	             finish = clock();
	             duration = (double) (finish - start) / CLOCKS_PER_SEC;
	             cout << "1000 ratings used " << duration << " sec" << endl;
	             start = clock();
	       }
    }    
    if (validation)
		rmse = sqrt(error / pred_set.size());
}

void RecoSys::ItemBased(string sim_mode)
{
	int count = 0;
    double error = 0.0;
    clock_t start, finish;
    double duration;
    start = clock();
    ofstream output_file("ItemBased_Result_cpp.csv");

	for (unsigned i = 0; i < pred_set.size(); i++)
    {
    	float ratings_sim = 0.0;
    	float sim = 0.0;
    	float sum_sim = 0.0;
    	float sum_sim_test = 0.0;
    	float cur_rating = 0.0;
        count++;
        for (unsigned j = 0; j < mov_info_set.size(); j++)
        {
        	  if (util_mat.count(mov_info_set[j][0]) == 0)
        	  {
        	  		continue;
        	  }
        	  else
        	  {
		          if (util_mat[mov_info_set[j][0]]->getElem(pred_set[i][0]) == 0)
		          {
		          	  // do nothing
		          }
		          else
		          {
		              if (sim_mode == "cosine")
		              {
		              	 if (util_mat.count(pred_set[i][1]))
		              	  	sim = util_mat[mov_info_set[j][0]]->cosine(*util_mat[pred_set[i][1]], mode);
		              	 else 
		              	 	sim = 0.0;
		              }
		              else if (sim_mode == "pearson")
		              {
		              	  if (util_mat.count(pred_set[i][1]))
 		              	  	 sim = util_mat[mov_info_set[j][0]]->pearson(*util_mat[pred_set[i][1]], mode);
 		              	  else 
 		              	   	 sim = 0.0;	
		              	}
		              else if (sim_mode == "jaccard")
		               { 
		               	   if (util_mat.count(pred_set[i][1]))
		              	  	  sim = util_mat[mov_info_set[j][0]]->jaccard(*util_mat[pred_set[i][1]]);
		              	   else
		              	   	  sim = 0.0;
		              	}
		              else
		              {
		              		cerr << "You have typed a WRONG similarity type!" << endl;
		              }
		              if (sim > 0)
		              {
		              	  sum_sim += sim;
		              	  ratings_sim += sim * util_mat[mov_info_set[j][0]]->getElem(pred_set[i][0]);
		              }
		        }
		    }
	    }
	     if (sum_sim > 0)
	    		cur_rating = ratings_sim / sum_sim;
	     else 
	    		cur_rating = 3.0;

	     if (!validation)
	     {
	        	output_file << pred_set[i][0] << "," <<  pred_set[i][1] << "," << cur_rating << endl;
	     }
	     else
	     {
	    		error += pow(cur_rating - pred_set[i][2], 2.0);
	     }


	     if (count % 1000 == 0)
	     {
	              finish = clock();
	              duration = (double) (finish - start) / CLOCKS_PER_SEC;
	              cout << "1000 ratings used " << duration << " sec" << endl;
	              start = clock();
	     }
    }
    if (validation)
		rmse = sqrt(error / pred_set.size());
}

void RecoSys::CombineMethod(string sim_mode)
{
	int count = 0;
	double error = 0;
    clock_t start, finish;
    double duration;
    start = clock();
    ofstream output_file("Combine_Result_cpp.csv");
	for (unsigned i = 0; i < pred_set.size(); i++)
    {
    	double ratings_sim = 0.0;
    	double sim = 0.0;
    	double sum_sim = 0.0;
    	double cur_rating = 0.0;
        count++;      	
        for (unsigned j = 0; j < usr_info_set.size(); j++)
        {
        	if (util_mat.count(usr_info_set[j][0]) == 0)
	        {
	        	continue;
			}
			else
			{  
	          if (util_mat[usr_info_set[j][0]]->getElem(pred_set[i][1]) == 0)
	          {
	              // do nothing
	          }
	          else
	          {		       
	          	  if (sim_mode == "cosine")
	          	  {
	          	  	if (util_mat.count(pred_set[i][0]))
	              	  	sim = util_mat[usr_info_set[j][0]]->cosine(*util_mat[pred_set[i][0]], mode);
	              	else
	              		sim = 0.0;
	              	// sexuality comparison
	              	if (usr_info_mat[usr_info_set[j][0]][0] == usr_info_mat[pred_set[i][0]][0])
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// occupation comparison
	              	if (usr_info_mat[usr_info_set[j][0]][2] == usr_info_mat[pred_set[i][0]][2]) 
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// age comparison
	              	int diff = 0;
	              	diff = abs(usr_info_mat[usr_info_set[j][0]][1] - usr_info_mat[pred_set[i][0]][1]);
	              	sim -= (double)(diff / 10) / 10000.0;
	              }
	              else if (sim_mode == "pearson")
	              {
	              	 if (util_mat.count(pred_set[i][0]))
	              	  	sim = util_mat[usr_info_set[j][0]]->pearson(*util_mat[pred_set[i][0]], mode);
	              	 else
	              		sim = 0.0;
	              	// sexuality comparison
	              	if (usr_info_mat[usr_info_set[j][0]][0] == usr_info_mat[pred_set[i][0]][0])
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// occupation comparison
	              	if (usr_info_mat[usr_info_set[j][0]][2] == usr_info_mat[pred_set[i][0]][2]) 
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// age comparison
	              	int diff = 0;
	              	diff = abs(usr_info_mat[usr_info_set[j][0]][1] - usr_info_mat[pred_set[i][0]][1]);
	              	sim -= (double)(diff / 10) / 10000.0;	          
	              }
	              else if (sim_mode == "jaccard")
	              {
	              	 if (util_mat.count(pred_set[i][0]))
	              	  	sim = util_mat[usr_info_set[j][0]]->jaccard(*util_mat[pred_set[i][0]]);
	              	 else
	              		sim = 0.0;
	              	// sexuality comparison
	              	if (usr_info_mat[usr_info_set[j][0]][0] == usr_info_mat[pred_set[i][0]][0])
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// occupation comparison
	              	if (usr_info_mat[usr_info_set[j][0]][2] == usr_info_mat[pred_set[i][0]][2]) 
	              		sim += 0.0001;
	              	else
	              		sim -= 0.0001;
	              	// age comparison
	              	int diff = 0;
	              	diff = abs(usr_info_mat[usr_info_set[j][0]][1] - usr_info_mat[pred_set[i][0]][1]);
	              	sim -= (double)(diff / 10) / 10000.0;	              	
	              }
	              else
	              	  cerr << "You have typed a WRONG similarity type!" << endl;
	              if (sim > 0)
	              {
	              	  sum_sim += sim;
	              	  ratings_sim += sim * util_mat[usr_info_set[j][0]]->getElem(pred_set[i][1]);
	              }
	          }
	    	}
		}
	   	if (sum_sim > 0)
	   		cur_rating = ratings_sim / sum_sim;
	   	else 
	   		cur_rating = 3.0;
	   	if (!validation)
	   	{	   		
	       	output_file << pred_set[i][0] << "," <<  pred_set[i][1] << "," << cur_rating << endl;
	   	}
	   	else
	   	{	   		
	   		error += pow(cur_rating - pred_set[i][2], 2.0);
	   	}
	       if (count % 1000 == 0)
	       {
	             finish = clock();
	             duration = (double) (finish - start) / CLOCKS_PER_SEC;
	             cout << "1000 ratings used " << duration << " sec" << endl;
	             start = clock();
	       }
    }    
    if (validation)
		rmse = sqrt(error / pred_set.size());
}

void RecoSys::Predict(string sim_mode)
{

	if (mode == "User")
	{
		this->UserBased(sim_mode);	
	}
	else if (mode == "Item")
		this->ItemBased(sim_mode);
	else if (mode == "CombineMethod")
		this->CombineMethod(sim_mode);
	else
		cerr << "You have typed a WRONG mode" << endl;
}

double RecoSys::getRMSE()
{
	return rmse;
}





