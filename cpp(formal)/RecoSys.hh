#include <string>
#include <unordered_map>
#include "SparseVector.cc"

using namespace std;

typedef vector <int> record_t;
typedef vector <record_t> data_t;

class RecoSys{

private:
	string mode;
	string sim;
	bool validation;
	double rmse;
	vector<double> pred_score;
	data_t usr_info_set;
	data_t mov_info_set;
	data_t pred_set;
	unordered_map<int, SparseVector*> util_mat;
	unordered_map<int, int*> usr_info_mat;

public:
	RecoSys(string mode, const data_t &um, const data_t &usr, const data_t &mov, const data_t &pred, bool val);
	~RecoSys();
	double CrossValidation();
	void Predict(string sim_mode);
	void UserBased(string sim_mode);
	void ItemBased(string sim_mode);
	void CombineMethod(string sim_mode);
	double getRMSE();

};