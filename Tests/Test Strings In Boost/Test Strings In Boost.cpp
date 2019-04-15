#include <iostream>
#include <string>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;
int main()
{
	setlocale(LC_ALL, "RUSSIAN");
	string s = "C:/Doc1/PBAX.doc";
	bool b = equals(s, "C:/Doc1/pbax2.dll");

	b = contains(s, "PBAX");

	b = equals(s, "C:/Doc1/PBAX.doc");

	getchar();
}
