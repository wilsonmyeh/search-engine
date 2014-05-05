#include <iostream>
 #include <vector>
 
 using namespace std;
 
 struct AlphaStrComp {
 bool operator() (const string& lhs, const string& rhs) 
    { 
    	if(lhs<=rhs)
    	 return true;
        else
        	return false;
    }
 };
 
    //compares page ranks using integers
   struct IntComp {
     bool operator() (const int lhs, const int rhs) 
    { 
      if(lhs>rhs)
    	 return true;
        else
        	return false;
    }
   };
 
 template <class T, class Comparator>
 bool DoCompare(const T& obj1, const T& obj2, Comparator comp)
 {
  	return comp(obj1, obj2);  // calls comp.operator()(s1,s2);
 }
 
 template <class T, class Comparator>
 void mergesort(vector<T> & mylist, Comparator comp) 
 { 
 	//takes in the list, start, end and comparator object
 	msort(mylist, 0, mylist.size(), comp); 
 }
 
 template <class T, class Comparator>
 void msort(vector<T> & mylist, int start, int end,Comparator comp) 
 { 
 // base case 
 	if(end == start) 
 	return; 
 	else 
 	{
 	// recursive calls 
 	int mid = (start + end)/2; 
 	//breaks up the first half in the list
 	msort(mylist, start, mid, comp);
 	//breaks up the second half in the list
 	msort(mylist, mid, end, comp); 
 	// merges the two lists into a sorted one
 	merge(mylist, start, mid, end, comp); 
 	}
 }
 
 template <class T, class Comparator>
 void merge(vector<T>& mylist, int start, int mid, int end, Comparator comp) 
 { 
 	vector<T> temp;  //temporary vector to store the broken up parts of the list while we sort it
 //
 	int fromLeft = start;
 	int fromMid = mid; 
 
 	//adds the smallest element among the elements compared to a temporary vector
 	for(int k = 0; k < (end - start); k++) 
 	{ 
 		if (fromMid >= end || (fromLeft < mid && DoCompare(mylist[fromLeft], mylist[fromMid], comp) ))
 		{
 			temp.push_back(mylist[fromLeft]);  //
 			fromLeft++;              
 		} 
 		else
 		{
 			temp.push_back(mylist[fromMid]);
 			fromMid++;
 		}
 	}
 
 	for(int k=0;k<temp.size();k++)
 
 //copies elements from temporary vector into original list
 	for(int k = start; k < end; k++)
 	{
 		mylist[k] = temp[k-start];
 	} 
 
 }