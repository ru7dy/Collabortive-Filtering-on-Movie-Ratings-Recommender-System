#include "SparseVector.hh"
#include <cassert>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace::std;

SparseVector::SparseVector(int s)
{
	assert(s > 0);
	size = s;
	firstnon0 = 0;
}

// Helper function to delete sparse vector
void SparseVector::clear()
{
	node* cur = firstnon0;
	while (cur != 0)
	{
		// Get waht is next, before deleting cur
		node *nxt = cur->next;
		delete cur;
		cur = nxt;
	}
	firstnon0 = 0;
	size = 0;
}

//Helper function to copu sparse vector
void SparseVector::copyList(const SparseVector &sv)
{
	size = sv.getSize();
	//Get a pointer to other list's first node
	node *otherCur = sv.firstnon0;
	// Use a prev and cur to create the copy
	node *prev = 0;
	while (otherCur != 0)
	{
		node *cur = new node(otherCur->index, otherCur->value);
		// cur is the first node in the copy
		if (prev == 0)
			firstnon0 = cur;
		// cur is not the first
		else
			prev->next = cur;
		prev = cur;
		otherCur = otherCur->next;

	}
	if (sv.firstnon0 == 0)
		firstnon0 = 0;
}

// helper method to set a nonzero value for a node with specified index
void SparseVector::setNonzeroElem(int idx, int val)
{
	assert(val != 0);
	node *cur = firstnon0;
	node *prev = 0;

	if (firstnon0 != 0)
	{
		// go through the sparse vector
		while (cur->index < idx && cur->next != 0)
		{
			prev = cur;
			cur = cur->next;
		}

		// if we find the index
		if (cur->index ==idx )
			cur->value = val;

		// if we can't find the index and are at the beginning of the list
		else if (firstnon0->index > idx)
		{
			node *newnode = new node(idx, val, firstnon0);
			firstnon0 = newnode;
			size++;
		}

		// if we can't find the index and are at the end of the list
		else if (cur->index < idx && cur->next == 0)
		{
			node *newnode = new node(idx, val);
			cur->next = newnode;
			size++;
		}

		// if we can't find the index and are at the middle of the list
		else if (cur->index > idx)
		{
			node *newnode = new node(idx, val, cur);
			prev->next = newnode;
			size++;
		}
	}
	// first node is 0
	else  
		firstnon0 = new node(idx, val);
}

// helper function to delete a node from the list
void SparseVector::removeElem(int idx)
{
	node *cur = firstnon0;
	node *prev = 0;

	// go through the sparse vector
	while (cur->next != 0 && cur->index < idx)
	{
		prev = cur;
		cur = cur->next;
	}

	// if we find the index at the beginning of the list
	if (cur->index == idx && prev == 0)
	{
		firstnon0 = cur->next;
		delete cur;
		size--;
	}

	// if we find the index at the end of the list
	else if (cur->index == idx && cur->next == 0)
	{
		prev->next = 0;
		delete cur;
		size--;
	}

	// if we find the index in the middle of the list
	else if (cur->index == idx && cur != 0 )
	{
		prev->next = cur->next;
		delete cur;
		size--;
	}

	// if we do not find the index, do nothing
}

// Copy constructor
SparseVector::SparseVector(const SparseVector &sv)
{
	this->copyList(sv);
}

// Destructor
SparseVector::~SparseVector()
{
	this->clear();
}

// Get the number of elements in Sparse Vector
int SparseVector::getSize() const
{
	return size;
}

int SparseVector::getElem(int idx) const
{
	node *cur = firstnon0;
	if (cur != 0)
	{
		while (cur->index < idx && cur->next != 0)
		{
			cur = cur->next;
		}

		// if we find the desired index, return its value
		if (cur->index == idx)
		{
			return cur->value;
		}
	}
	return 0;
}

void SparseVector::setElem(int idx, int val)
{
	if (val != 0)
	{
		setNonzeroElem(idx, val);
	}
	else
	{
		removeElem(idx);
	}
}

void SparseVector::checkListOrder() const
{
	node *test = firstnon0;
	int idx = -1;
	while (test != 0)
	{
		if (idx < test->index)
		{
			idx = test->index;
		}
		test = test->next;
	}
}
double SparseVector::abs_length(double mean) const
{
	if (mean == -1)
		mean = 0;
	double sum = 0;
	node *cur = firstnon0;
	// if (mode == "User")
	while (cur != 0)
	{		
		sum += (cur->value - mean) * (cur->value - mean);
		cur = cur->next;
		// cout << " while " << endl;
	}
	return sqrt(sum);
}

double SparseVector::cosine(const SparseVector &sv, string mode) const
{
	double mean = -1.0;
	double numerator = 0.0;
	double denominator = 0.0;
	node *cur_1 = firstnon0;
	node *cur_2 = sv.firstnon0;
	// one vector is 0, similarity is 0
	if (cur_1 == 0 || cur_2 == 0)
		return 0.0;
	while (cur_1 != 0 && cur_2 != 0)
	{
		// neither is 0
		if (cur_1->index == cur_2->index)
		{
			numerator += (cur_1->value * cur_2->value);
			cur_1 = cur_1->next;
			cur_2 = cur_2->next;
		}
		// not align, move the slower one
		else if (cur_1->index < cur_2->index)
		{
			cur_1 = cur_1->next;
		}
		// not align move the slower one
		else
		{
			cur_2 = cur_2->next;
		}
	}

	double length_1 = this->abs_length(mean);
	double length_2 = sv.abs_length(mean);

	denominator = length_1 * length_2;
	if (denominator == 0.0)
	{
		if (mode == "User" || mode == "CombineMethod")
		{
			return 0.0;
		}
		if (mode == "Item")
		{
			if (length_1 == 0.0)
				return 1.0;
			else 
				return 1.0; 
		}
	}

	return numerator / denominator;

}

double SparseVector::pearson(const SparseVector &sv, string mode) const
{
	double numerator = 0.0;
	double denominator = 0.0;
	double mean_1 = 0.0;
	double sum_1 = 0.0;
	double mean_2 = 0.0;
	double sum_2 = 0.0;
	node *cur_1 = firstnon0;
	node *cur_2 = sv.firstnon0;
	// cout << "first at pearson" << endl;
	if (cur_1 == 0 || cur_2 == 0)
		return 0.0;

	while (cur_1 != 0)
	{
		sum_1 += cur_1->value;
		cur_1 = cur_1->next;
	}
	// cout << size << endl;
	// cout << sv.size<< endl;
	mean_1 = sum_1 / ((double)size);
	while(cur_2 != 0 )
	{
		sum_2 += cur_2->value;
		cur_2 = cur_2->next;
	}
	mean_2 = sum_2 / ((double)sv.size);

	cur_1 = firstnon0;
	cur_2 = sv.firstnon0;
	while (cur_1 != 0 && cur_2 != 0)
	{
		// neither is 0
		if (cur_1->index == cur_2->index)
		{
			numerator += (cur_1->value - mean_1) * (cur_2->value - mean_2);
			cur_1 = cur_1->next;
			cur_2 = cur_2->next;
		}
		// not align, move the slower one
		else if (cur_1->index < cur_2->index)
		{
			cur_1 = cur_1->next;
		}
		// not align move the slower one
		else
		{
			cur_2 = cur_2->next;
		}
	}
	double length_1 = this->abs_length(mean_1);
	double length_2 = sv.abs_length(mean_2);

	denominator = length_1 * length_2;
	// cout << "l1" << length_1 << endl;
	// cout << "l2" << length_2 << endl;


	if (denominator == 0.0)
	{
		if (mode == "User" || mode == "CombineMethod")
		{
			return 0.0;
		}
		if (mode == "Item")
		{
			if (length_1 == 0.0)
				return 1.0;
			else 
				return 1.0; 
		}
	}
	// if (denominator == 0.0)
	// 	return 0.0;
	return numerator / denominator;

}

double SparseVector::jaccard(const SparseVector &sv) const
{
	double numerator = 0.0;
	double denominator = 0.0;
	node *cur_1 = firstnon0;
	node *cur_2 = sv.firstnon0;
	while (cur_1 != 0 && cur_2 != 0)
	{
		// neither is 0
		if (cur_1->index == cur_2->index)
		{
			denominator += 1;
			numerator += 1;
			cur_1 = cur_1->next;
			cur_2 = cur_2->next;
		}
		// not align, move the slower one
		else if (cur_1->index < cur_2->index)
		{
			denominator += 1;
			cur_1 = cur_1->next;
		}
		// not align move the slower one
		else
		{
			denominator += 1;
			cur_2 = cur_2->next;
		}
	}

	return numerator / denominator;

}
// Overloading assignment operator
SparseVector& SparseVector::operator= (const SparseVector &sv)
{
	if (this != &sv)
	{
		// Delete the sparse vector
		this->clear();

		// make a copy
		this->copyList(sv);
	}
	return *this;
}


// Overloading the sparse vector == operator































