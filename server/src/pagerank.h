#ifndef PAGERANK_H
#define PAGERANK_H

#include <set>

#define RAND_RESTART 0.15

struct Node {
	WebPage* obj;
	double rank;

	Node() {
		obj = nullptr;
		rank = 0;
	}
};

void pageRank(std::list<WebPage*> & searchResults)
{
	std::set<WebPage*> counter;

	for (auto & i : searchResults) {
		counter.insert(i);

		for (auto j : i->allIncomingLinks()) {
			counter.insert(j);
		}
		for (auto j : i->allOutgoingLinks()) {
			counter.insert(j);
		}
	}

	for (auto & i : counter) {
		i->setRank(0);
	}

	Node** matrix = new Node*[counter.size()];

	for (size_t i = 0; i < counter.size(); ++i) {
		matrix[i] = new Node[counter.size()];
	}

	size_t i_counter = 0;
	for (auto & i : counter) {
		if (i->allOutgoingLinks().size() == 0) {
			for (size_t j = 0; j < counter.size(); ++j) {
				matrix[i_counter][j].rank = 1.0 / counter.size();
			}
		}
		else {
			size_t j_counter = 0;
			for (auto & j : counter) {
				matrix[i_counter][j_counter].obj = j;
	
				bool edge_exists = false;
				for (auto & k : j->allIncomingLinks()) {
					if (k == i) {
						edge_exists = true;
					}
				}
				for (auto & k : j->allOutgoingLinks()) {
					if (k == i) {
						edge_exists = true;
					}
				}
	
				if (edge_exists) {
					matrix[i_counter][j_counter].rank = RAND_RESTART / counter.size()
					                                  + (1.0 - RAND_RESTART) / j->allOutgoingLinks().size();
				}
				else {
					matrix[i_counter][j_counter].rank = RAND_RESTART / counter.size();
				}
	
				++j_counter;
			}
		}
	
		++i_counter;
	}

	double* multipliers = new double[counter.size()];

	for (size_t i = 0; i < counter.size(); ++i) {
		multipliers[i] = 1.0 / counter.size();
	}

	for (size_t i = 0; i < 30; ++i) {
		for (size_t j = 0;j < counter.size();++j) {
			double rank = 0;

			for (size_t k = 0; k < counter.size(); ++k) {
				rank += matrix[j][k].rank;
			}

			multipliers[j] *= rank;
		}
	}

	i_counter = 0;
	for (auto & i : counter) {
		i->setRank(multipliers[i_counter]);

		++i_counter;
	}

	delete[] multipliers;

	for (size_t i = 0; i < counter.size(); ++i) {
		delete[] matrix[i];
	}

	delete[] matrix;
}

#endif
