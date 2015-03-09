#include "kFoldCross.hh"
#include <cassert>

template<class In>
Kfold<In>::Kfold(int _k, In _beg, In _end):
		beg(_beg), end(_end), K(_k) 
{
			assert(K > 0);
			int foldNo = 0;
			for (In i = beg; i != end; i++)
			{
				whichFoldToGo.push_back(++foldNo);
				if (foldNo == K)
					foldNo = 0;
			}
			random_shuffle(whichFoldToGo.begin(), whichFoldToGo.end());
}

template<class In>
template<class Out>
void Kfold<In>::getFold(int foldNo, Out training, Out testing) 
{
	int k = 0;
	In i = beg;
	while (i != end) {
		if (whichFoldToGo[k++] == foldNo)
			*testing++ = *i++;
		else
			*training++ = *i++;
	}
}