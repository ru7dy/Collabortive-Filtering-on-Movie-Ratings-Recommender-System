// #include "SparseVector.cc"
#include <vector>
#include <algorithm>
#include <iterator>
// #include <boost/foreach.hpp>

using namespace std;

#define foreach BOOST_FOREACH

template<class In>
class Kfold {
public:
	Kfold(int k, In _beg, In _end);
	template<class Out>
	void getFold(int foldNo, Out traning, Out testing);

private:
	In beg;
	In end;
	int K;
	vector<int> whichFoldToGo;
};