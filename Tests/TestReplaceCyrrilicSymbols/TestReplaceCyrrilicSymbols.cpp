#include <iostream>
#include <string>
#include <vector>

#include <boost/algorithm/string.hpp>
using namespace std;

vector<string> Rus = {
"а", "б", "в", "г", "д", "е", "ё", "ж", "з", "и", "й",
"к", "л", "м", "н", "о", "п", "р", "с", "т", "у", "ф", "х",
"ц", "ч", "ш", "щ", "ъ", "ы", "ь", "э", "ю", "я"
};

vector<string> Eng = {
"a", "b", "v", "g", "d", "e", "yo", "zh", "z", "i", "j", "k",
"l", "m", "n", "o", "p", "r", "s", "t", "u", "f", "h",
"c", "ch", "sh", "shh", "'", "y", "._", "e-", "yu", "ya"
};

using namespace boost::algorithm;
int main()
{

	setlocale(LC_ALL, "Russian");
	string In = "first_level — копия (3).xml",
		Result = In;

	// Encrypt
	for (size_t i = 0; i < Rus.size(); i++)
	{
		size_t Pos = string::npos;
		if ((Pos = Result.find(Rus.at(i))) != string::npos) // Find Needed Symbol To Replace It (Use The Current Symbol "it was mentioned upper")
			Result.replace(Pos, Eng.at(i).length(), Eng.at(i));
	}

	std::cout << "After Encrypting" << std::endl;
	std::cout << "In: " << In << "\nOut: " << Result << std::endl;

	// Decrypt (Work Is Wrong!!! Need To Think It Over And Refactoring)
	//for (size_t i = 0; i < Eng.size(); i++)
	//{
	//	size_t Pos = string::npos;
	//	if ((Pos = Result.find(Eng.at(i))) != string::npos) // Find Needed Symbol To Replace It (Use The Current Symbol "it was mentioned upper")
	//		Result.replace(Pos, Rus.at(i).length(), Rus.at(i));
	//}

	//std::cout << "After Decrypting" << std::endl;
	//std::cout << "In: " << In << "\nOut: " << Result << std::endl;
}
