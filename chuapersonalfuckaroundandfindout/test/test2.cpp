#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;

int main()
{
	vector<int>	a;
	vector<int>	b;

	int		*test;

	b.push_back(1);
	a = b;
	test = &a[0];

	cout << *(test) << endl;

	// memory will be reallocated
	b.push_back(2);
	// the original tab can say bye bye d
	cout << *(test) << endl;
}