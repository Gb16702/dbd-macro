#include <iostream>
#include <windows.h>
#include "key/key_script.hpp"

int main() {
	SetConsoleOutputCP(1252);
	
	std::cout << "Cr�� par Panopt�s" << std::endl;

	std::cout << "Appuie sur END pour configurer le raccourci" << std::endl;
	
	key_script();

	return 0;
}
