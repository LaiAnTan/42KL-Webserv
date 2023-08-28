#include <iostream>
#include <map>

using std::map;
using std::pair;

class B
{

};

class A
{
	private:
		int x;
	public:
		A()
		{
			x = 10;
		}

		A(A &other)
		{
			this->x = other.x;
		}

		int	getx()
		{
			return (x);
		}
};

class BigStuff
{
	private:
		map<int ,A> stuff;

	public:
		void	addstuff(A stufftoadd)
		{
			pair<int, A>	tem(1, stufftoadd);
			stuff.insert(tem);
		};

		A	&getstuff()
		{
			return (stuff[1]);
		}
};

int main()
{
	BigStuff	contain;

	A			stuff;

	contain.addstuff(stuff);
	std::cout << contain.getstuff().getx() << std::endl;
}