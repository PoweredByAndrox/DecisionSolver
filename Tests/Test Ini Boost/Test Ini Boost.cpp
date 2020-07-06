#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace std;
void save(string filename)
{
	using boost::property_tree::ptree;
	ptree pt;

	for (size_t i = 0; i <= 10; i++)
	{
		pt.add<string>("a1." + to_string(i), to_string(i));
	}

	write_ini(filename, pt);
}

int main()
{
	save("settings.ini");
    std::cout << "Hello World!\n";
	system("pause");
	return 0;
}
