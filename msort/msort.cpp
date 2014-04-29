#include "msort.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <iomanip>

// returns true if LHS has fewer factors than RHS
struct FactorComp {
	bool operator()(const int lhs, const int rhs)
	{
		int lhs_facts = 0;
		int rhs_facts = 0;

		for (int i = 1; i <= lhs; ++i) {
			if (lhs % i == 0) {
				++lhs_facts;
			}
		}
		for (int i = 1; i <= rhs; ++i) {
			if (rhs % i == 0) {
				++rhs_facts;
			}
		}

		return lhs_facts <= rhs_facts;
	}
};

// returns true if LHS has fewer digits than RHS
struct DigitComp {
	bool operator()(const int lhs, const int rhs)
	{
		int lhs_digits = 0;
		int rhs_digits = 0;

		for (int i = 1; lhs / i > 0; i *= 10) { ++lhs_digits; }
		for (int i = 1; rhs / i > 0; i *= 10) { ++rhs_digits; }

		return lhs_digits <= rhs_digits;
	}
};

void print(const std::list<int> & in)
{
	for (std::list<int>::const_iterator i = in.begin();
	     i != in.end();
	     ++i) {
		std::cout << std::setw(3) << *i;
	}

	std::cout << std::endl;
}

int main()
{
	std::list<int> in;

	srand(time(0));

	for (size_t i = 0; i < 20; ++i) {
		in.push_back(rand() % 20);
	}

	std::cout << "Unsorted:" << std::endl;
	print(in);

	FactorComp comp1;
	merge_sort(in, comp1);

	std::cout << "Sorted by number of factors:" << std::endl;
	print(in);

	DigitComp comp2;
	merge_sort(in, comp2);

	std::cout << "Sorted by number of digits:" << std::endl;
	print(in);

	return 0;
}
