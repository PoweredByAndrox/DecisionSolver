#include <iostream>
#include <string>
#include <vector>
#include <iostream>

#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost::algorithm;
int main()
{
	string s = "G:\\DecisionSolver\\Engine\\resource\\textures\\sumka\\ "\
		"sumka.v3_low.exported_Diffuse.birki.png sumka.v3_low.exported_Diffuse.default.png";
	vector<string> res;
	boost::split(res, s, boost::is_any_of("\ "));

	string path = res.front();
	res.erase(res.begin());

	vector<string> Test;
	for (size_t i = 0; i < res.size(); i++)
	{
		Test.push_back(path + res.at(i));
	}

	setlocale(LC_ALL, "RUSSIAN");
	//string s = "C:/Doc1/PBAX.doc";
	bool b = equals(s, "C:/Doc1/pbax2.dll");

	b = contains(s, "PBAX");

	b = equals(s, "C:/Doc1/PBAX.doc");

	getchar();
}
