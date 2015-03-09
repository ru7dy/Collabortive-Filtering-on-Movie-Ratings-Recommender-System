#include <string>
using namespace std;
class SparseVector
{
private:
	struct node
	{
		int index;
		int value;
		node *next;
		node(int idx, int val, node *nxt = 0): index(idx), value(val), next(nxt)
		{}
	};

	int size;

	// node-pointer to the first nonzero element in the list
	node *firstnon0;
	void clear();
	void copyList(const SparseVector &sv);
	void setNonzeroElem(int idx, int val);
	void removeElem(int idx);
	void checkListOrder() const;

	// helpper function for adding or substracting sparse vectors
	void addSubVector(const SparseVector &sv, bool add);

	// helper function to clean out any elements whose values are 0's
	void removeZeros();

public:
	// constructor
	// one arg constructor
	SparseVector(int s);

	// copy constructor
	SparseVector(const SparseVector &sv);

	// destructor
	~SparseVector();

	// access member function
	int getSize() const;
	int getElem(int idx) const;
	double cosine(const SparseVector &sv, string mode) const;
	double pearson(const SparseVector &sv, string mode) const;
	double jaccard(const SparseVector &sv) const;
	double abs_length(double mean) const;
	
	// mutators
	void setElem(int idx, int val);

	// Overloading the assignment, += operator
	SparseVector& operator=(const SparseVector &sv);
	SparseVector& operator+=(const SparseVector &sv);
	SparseVector& operator-=(const SparseVector &sv);
	// SparseVector& operator*=(const SparseVector &sv)
	// Overloading the +, - operator
	const SparseVector operator+(const SparseVector &sv) const;
	const SparseVector operator-(const SparseVector &sv) const;

	// Overloading == and !=
	bool operator==(const SparseVector &sv) const;
	bool operator!=(const SparseVector &sv) const;

};