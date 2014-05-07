#ifndef MSORT_H
#define MSORT_H

#include <cstddef> // size_t
/*
A list is most appropriate for merge sort, since the only access
happens at the front, the back, and when we iterate through the
entire list.
A deque would be next best, and vector would be the least optimal.
If we decided to use a vector, we would need to change the pop_front()
calls to e.g. erase(0) calls.
*/
#include <list>

template <class T, class C>
std::list<T> sort(const std::list<T> & in, C comp)
{
	// base case: the list is already sorted
	if (in.size() <= 1) {
		return in;
	}
	
	// the left half (or half - 1 if size is odd) of the list
	std::list<T> l;
	// the right half (or half + 1)
	std::list<T> r;

	// we iterate along the list until halfway/h.w. + 1
	// we need to keep track of which "index" we're at
	size_t c = 0;
	for (typename std::list<T>::const_iterator i = in.begin();
	     i != in.end();
	     ++i) {
		// push all items to left of center to left half
		if (c < in.size() / 2) {
			l.push_back(*i);
		}
		else {
			// push items at center or right to right half
			r.push_back(*i);
		}

		++c;
	}

	// the algorithm recursively calls here
	// depthwise splits list until it hits a 1 or 0-size list
	l = sort(l, comp);
	r = sort(r, comp);

	// after left and right have returned, we want to merge them together
	std::list<T> out;

	// while either list has items left to be merged
	while (l.size() > 0 || r.size() > 0) {
		// both lists still have items to merge
		if (l.size() > 0 && r.size() > 0) {
			// compare left and right
			// move greater item to output list
			if (comp(l.front(), r.front())) {
				out.push_back(l.front());
				l.pop_front();
			}
			else {
				out.push_back(r.front());
				r.pop_front();
			}
		}
		// only one or the other has items
		else if (l.size() > 0) {
			out.push_back(l.front());
			l.pop_front();
		}
		else {
			out.push_back(r.front());
			r.pop_front();
		}
	}

	// return the merged list
	return out;
}

template <class T, class C>
void merge_sort(std::list<T> & in, C comp)
{
	// replace the unsorted list with
	// the result of the merge sort
	in = sort(in, comp);
}

#endif
