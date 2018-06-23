#include "set.h"
#include "test.h"

int main()
{
	test<std::string>();
	test<int>();
	set<int> c;
	c.insert(1);
	c.insert(2);
	c.insert(3);
	c.insert(4);
	auto it1 = c.begin();
	auto it2 = it1++;
	std::swap(it1, it2);
	std::cout << (it1 != it2) << '\n';
	c = c;
	set<int> d;
	swap(c, d);

	set<int> myset;
	int myints[] = {21,64,17,78,49};
	for (auto i : myints){
		myset.insert(i);
	}


	std::cout << "myset contains:";
	for (auto rit = myset.rbegin(); rit != myset.rend(); ++rit)
	std::cout << ' ' << *rit;

	std::cout << '\n';
    return 0;       
}