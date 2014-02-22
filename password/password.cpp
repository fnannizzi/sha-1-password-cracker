// password.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "sha1.h"
#include "dictionary.h"
#include "decrypt.h"
#include <string>
#include <iostream>

void menu();
void basicHashing();
void loadDictionary(dict::Dictionary *dictionary);
void decrypt(dict::Dictionary *dictionary);
void testBruteForce(dict::Dictionary *dictionary);

int _tmain(int argc, _TCHAR* argv[])
{
	//dict::Dictionary *dictionary = new dict::Dictionary(100000);
	//testBruteForce(dictionary);
	menu();
	return 0;
}


void menu(){
	dict::Dictionary *dictionary = new dict::Dictionary(100000);

	std::string input = "";
	while (input == "")
	{
		std::cout << "Select an option: " << std::endl;
		std::cout << "  1. Basic Hashing " << std::endl;
		std::cout << "  2. Load Dictionary " << std::endl;
		std::cout << "  3. Decrypt " << std::endl;
		std::cout << "  4. Exit " << std::endl;

		if (!std::getline(std::cin, input))
		{
			std::cout << "Invalid input. Please enter a number between 1-4 to select an option." << std::endl;
			input = "";
		}
		else if (input.compare("1") == 0)
		{
			std::cout << "Option 1 selected. " << std::endl;
			basicHashing();
			input = "";
		}
		else if (input.compare("2") == 0)
		{
			std::cout << "Option 2 selected. " << std::endl;
			loadDictionary(dictionary);
			input = "";
		}
		else if (input.compare("3") == 0)
		{
			std::cout << "Option 3 selected. " << std::endl;
			decrypt(dictionary);
			input = "";
		}
		else if (input.compare("4") == 0)
		{
			std::cout << "Option 4 selected. Exiting... " << std::endl;
			return;
		}
		else
		{
			std::cout << "Invalid input. Please enter a number between 1-4 to select an option." << std::endl;
			input = "";
		}
	}
}

void basicHashing()
{
	std::string input = "";
	while (input == "")
	{
		std::cout << "Select an option: " << std::endl;
		std::cout << "  1. Hash a password " << std::endl;
		std::cout << "  2. Return to main menu " << std::endl;

		if (!std::getline(std::cin, input))
		{
			std::cout << "Invalid input. Please enter a number between 1-2 to select an option." << std::endl;
			input = "";
		}
		else if (input.compare("1") == 0)
		{
			std::cout << "Option 1 selected. Enter a sample password to be hashed:" << std::endl;
			std::string password = "";
			unsigned char hash[20];
			char hex_str[41];

			if (!std::getline(std::cin, password))
			{
				std::cout << "Please enter a valid password. Restarting..." << std::endl;
			}
			else if (password.compare("") == 0)
			{
				std::cout << "Please enter a valid password. Restarting..." << std::endl;
			}
			else 
			{
				sha1::calc(password.c_str(), password.length(), hash);
				sha1::toHexString(hash, hex_str);
				std::cout << "Hashed: " << hex_str << std::endl;
			}
			input = "";
			
		}
		else if (input.compare("2") == 0)
		{
			std::cout << "Option 2 selected. Returning to main menu... " << std::endl;
			return;
		}
		else
		{
			std::cout << "Invalid input. Please enter a number between 1-2 to select an option." << std::endl;
			input = "";
		}
	}
}

void loadDictionary(dict::Dictionary *dictionary)
{
	std::string input = "";
	while (input == "")
	{
		std::cout << "Select an option: " << std::endl;
		std::cout << "  1. Load default dictionary " << std::endl;
		std::cout << "  2. Load custom dictionary " << std::endl;
		std::cout << "  3. Return to main menu " << std::endl;

		if (!std::getline(std::cin, input))
		{
			std::cout << "Invalid input. Please enter a number between 1-3 to select an option." << std::endl;
			input = "";
		}
		else if (input.compare("1") == 0)
		{
			std::cout << "Option 1 selected. Loading default dictionary d8.txt." << std::endl;
			if (dictionary->loadDictionary("d8.txt"))
			{
				std::cout << "Dictionary from d8.txt loaded." << std::endl;
			}
			else
			{
				std::cout << "Invalid dictionary filename. Restarting... " << std::endl;
			}
			input = "";

		}
		else if (input.compare("2") == 0)
		{
			std::cout << "Option 2 selected. Please enter the filename of the custom dictionary:" << std::endl;
			std::string filename = "";

			if (!std::getline(std::cin, filename))
			{
				std::cout << "Please enter a valid filename. Restarting..." << std::endl;
			}
			else if (filename.compare("") == 0)
			{
				std::cout << "Please enter a valid filename. Restarting..." << std::endl;
			}
			else
			{
				std::cout << "Loading dictionary... " << std::endl;
				if (dictionary->loadDictionary(filename))
				{
					std::cout << "Dictionary from " << filename << " loaded." << std::endl;
				}
				else
				{
					std::cout << "Invalid dictionary filename. Restarting... " << std::endl;
				}
			}
			input = "";

		}
		else if (input.compare("3") == 0)
		{
			std::cout << "Option 3 selected. Returning to main menu... " << std::endl;
			return;
		}
		else
		{
			std::cout << "Invalid input. Please enter a number between 1-3 to select an option." << std::endl;
			input = "";
		}
	}
}

void decrypt(dict::Dictionary *dictionary)
{
	dcry::Decrypt *decrypter = new dcry::Decrypt(dictionary);

	std::string input = "";
	while (input == "")
	{
		std::cout << "Select an option: " << std::endl;
		std::cout << "  1. Use default password file " << std::endl;
		std::cout << "  2. Use custom password file " << std::endl;
		std::cout << "  3. Return to main menu " << std::endl;

		if (!std::getline(std::cin, input))
		{
			std::cout << "Invalid input. Please enter a number between 1-3 to select an option." << std::endl;
			input = "";
		}
		else if (input.compare("1") == 0)
		{
			std::cout << "Option 1 selected. Loading default password file pass.txt." << std::endl;
			if (decrypter->decrypt("pass.txt"))
			{
				std::cout << "Decrypting from pass.txt completed." << std::endl;
			}
			else
			{
				std::cout << "Invalid password filename. Restarting... " << std::endl;
			}
			input = "";

		}
		else if (input.compare("2") == 0)
		{
			std::cout << "Option 2 selected. Please enter the filename of the custom password file:" << std::endl;
			std::string filename = "";

			if (!std::getline(std::cin, filename))
			{
				std::cout << "Please enter a valid filename. Restarting..." << std::endl;
			}
			else if (filename.compare("") == 0)
			{
				std::cout << "Please enter a valid filename. Restarting..." << std::endl;
			}
			else
			{
				std::cout << "Loading password file... " << std::endl;
				if (decrypter->decrypt(filename))
				{
					std::cout << "Decrypting from " << filename << " completed." << std::endl;
				}
				else
				{
					std::cout << "Invalid password filename. Restarting... " << std::endl;
				}
			}
			input = "";

		}
		else if (input.compare("3") == 0)
		{
			std::cout << "Option 3 selected. Returning to main menu... " << std::endl;
			return;
		}
		else
		{
			std::cout << "Invalid input. Please enter a a number between 1-3 to select an option." << std::endl;
			input = "";
		}
	}
}

void testBruteForce(dict::Dictionary *dictionary)
{
	bool didItWork = dictionary->loadDictionary("d8.txt");
	dcry::Decrypt *decrypter = new dcry::Decrypt(dictionary);
	didItWork = decrypter->decrypt("pass_test.txt");

}