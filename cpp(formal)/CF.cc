#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <time.h>
#include <cassert>
#include <string>
#include <stdlib.h>     /* atoi */
#include "RecoSys.cc"
#include "kFoldCross.cc"

using namespace std;

typedef vector <int> record_t;
typedef vector <record_t> data_t;


istream& operator >> ( istream& ins, record_t& record )
{
    
    record.clear();
    string line;
    getline( ins, line );

    stringstream ss( line );
    string field;
    while (getline( ss, field, ',' ))
    {
          // for each field we wish to convert it to a int
          stringstream fs( field );
          int f = 0;  // (default value is 0)
          fs >> f;

          // add the newly-converted field to the end of the record
          record.push_back( f );
    }

  return ins;
}

istream& operator >> ( istream& ins, data_t& data )
{
    data.clear();

    record_t record;
    while (ins >> record)
    {
        data.push_back( record );
    }

    return ins;  
}

int main(int argc, char* argv[])
{
    //
    if (argc < 4)
    {
        // Print the usage 
        cerr << "------------------------------------------------------------------\n";
        cerr << "To use this program, please do as following instruction.\n";
        cerr << "Supporting 3 collaborative filtering methods,\n";
        cerr << "\twith 3 similarity calculating crierion.\n";
        cerr << "Cross Validation or Predicting:\n";
        cerr << "\tparameter 0 -> Predicting\n";
        cerr << "\tparameter 1 -> Cross Validation\n";
        cerr << "CF methods:\n";
        cerr << "\tparameter 1 -> User-based CF\n";
        cerr << "\tparameter 2 -> Item-based CF\n";
        cerr << "\tparameter 3 -> Combining Method CF\n";
        cerr << "Similarity crierion:\n";
        cerr << "\tparameter 1 -> Cosine similarity\n";
        cerr << "\tparameter 2 -> Pearson similarity\n";
        cerr << "\tparameter 3 -> Jaccard similarity\n";
        cerr << "Example:\n";
        cerr << "\t'./CF.cpp 0 1 2' means we are gonna predict,\n";
        cerr << "\t\t\t\tusing User-based CF with Pearson similarity\n";
        cerr << "------------------------------------------------------------------\n";
        return 1;
    }
    int tmp1, tmp2, tmp3;
    tmp1 = atoi(argv[1]);
    tmp2 = atoi(argv[2]);
    tmp3 = atoi(argv[3]);


    clock_t start, finish;
    double duration;
    start = clock();

    data_t data;

    ifstream infile( "ratings.csv" );
    infile >> data;

    if (!infile.eof())
    {
        cout << "Input File Wrong!\n";
        return 1;
    }

    infile.close();


    cout << "Your CSV file contains " << data.size() << " records.\n";

  
    data_t all_user_id;
    ifstream infile_user_id("users.csv");
    infile_user_id >> all_user_id;
    if (!infile_user_id.eof())
    {
          cout << "Input File Wrong!\n";
          return 1;
    }

    infile_user_id.close();


    data_t all_movie_id;
    ifstream infile_movie_id("movies.csv");
    infile_movie_id >> all_movie_id;
    if (!infile_movie_id.eof())
    {
          cout << "Input File Wrong!\n";
          return 1;
    }

    infile_movie_id.close();

    data_t toBeRated;
    ifstream infile_toBeRated("toBeRated.csv");


    infile_toBeRated >> toBeRated;

    if (!infile_toBeRated.eof())
    {
          cout << "Input File Wrong!\n";
          return 1;
    }

    infile_toBeRated.close();


    string mode;
    string sim;
    bool RMSE = false;

    switch(tmp1) {
      case 0:
          RMSE = false;
          break;
      case 1:
          RMSE = true;
          break;
      default:
          cerr << "Wrong input for RMSE\n";
          break;
    }

    switch(tmp2) {
      case 1:
          mode = "User";
          break;
      case 2:
          mode = "Item";
          break;
      case 3:
          mode = "CombineMethod";
          break;
      default:
          cerr << "Wrong input for mode\n";
          break;
    }

    switch(tmp3) {
      case 1:
          sim = "cosine";
          break;
      case 2:
          sim = "pearson";
          break;
      case 3:
          sim = "jaccard";
          break;
      default:
          cerr << "Wrong input for similarity\n";
          break;
    }

    double Total_RMSE = 0.0;
    if (RMSE)
    {
      const int folds = 5;
      Kfold<data_t::const_iterator> kf(folds, toBeRated.begin(), toBeRated.end());

      data_t test, train;

      for (int i = 0; i != folds; i++)
      {

        kf.getFold(i+1, back_inserter(train), back_inserter(test));

        RecoSys myReco(mode, train, all_user_id, all_movie_id, test, RMSE);
        myReco.Predict(sim);

        Total_RMSE += myReco.getRMSE();
        cout << myReco.getRMSE() << endl;
        
      }
      Total_RMSE = Total_RMSE / 5.0;

      cout << "RMSE is " << Total_RMSE << endl;
    }
    else
    {
       RecoSys myReco(mode, data, all_user_id, all_movie_id, toBeRated, RMSE);
       myReco.Predict(sim);
    }

    finish = clock();
    duration = (double) (finish - start) / CLOCKS_PER_SEC;
    cout << "Total Time is " << duration << endl;

    
    return 0;
}
