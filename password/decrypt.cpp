#include "decrypt.h"
#include "sha1.h"
#include <ppl.h>
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>
#include <algorithm>

struct Base36Word
{
	int length;
	int* value;
};

Base36Word* constructBase36Word(int length);
char* convertFromBase36ToChar(Base36Word* word);
bool incrementBase36Word(Base36Word *word);

namespace dcry
{
	Decrypt::Decrypt(dict::Dictionary *dict)
	{
		dictionary = dict;
	}

	Decrypt::~Decrypt()
	{
		delete dictionary;
		dictionary_lookup_map.clear();
		brute_force_list.clear();
	}

	bool Decrypt::decrypt(std::string filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			return false;
		}

		// Clear old dictionary map and list of brute forced passwords
		dictionary_lookup_map.clear();
		brute_force_list.clear();

		// start timer on processing operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		std::string hashed_password;
		int line_index = 0;

		// Read file line by line and store passwords
		while (std::getline(file, hashed_password))
		{
			addEntry(hashed_password, line_index);
			line_index++;
		}
		file.close();
		// read timer on loading operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while cracking passwords
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;
		std::cout << "Loading and doing dictionary lookup on passwords from " << filename << " took " << fElapsed << " seconds." << std::endl;

		// Run brute-force matching on remaining passwords
		bruteForce();

		// Write the solved passwords to pass_solved.txt
		writeToFile();

		// read timer on loading operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while cracking passwords
		fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;
		std::cout << "Cracking all passwords from " << filename << " took " << fElapsed << " seconds." << std::endl;

		return true;
	}

	void Decrypt::addEntry(std::string hashed_password, int line_index)
	{
		// Create a new decryption entry to store the hashed password information
		DecryptionEntry *decryptionEntry = new DecryptionEntry;
		decryptionEntry->entry_number = line_index;
		decryptionEntry->hash = hashed_password;

		// Attempt a dictionary lookup of the hashed password
		Entry *dictionaryEntry = dictionary->lookupEntry(hashed_password);

		// If the lookup succeeds, store the password in the dictionary_lookup_map, otherwise store it in a list to be brute-forced later
		if (dictionaryEntry)
		{
			decryptionEntry->word = dictionaryEntry->word;
			dictionary_lookup_map[line_index] = decryptionEntry;	
		}
		else
		{
			brute_force_list.push_back(decryptionEntry);
		}

	}

	void Decrypt::bruteForce()
	{
		std::cout << "Beginning brute force matching on remaining passwords." << std::endl;
		// start timer on brute force operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		concurrency::parallel_for_each(brute_force_list.begin(), brute_force_list.end(), [this](DecryptionEntry *entry){decryptWordByBruteForce(entry); });

		// read timer on brute force operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while cracking passwords
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;
		std::cout << "Brute forcing passwords took " << fElapsed << " seconds." << std::endl;
	}

	void Decrypt::decryptWordByBruteForce(DecryptionEntry *entry)
	{
		bool decrypted = false;
		Base36Word *test_word;
		char *converted_word;
		unsigned char hash[20];
		char hex_str[41];

		// For passwords of length 1-4, run the brute-force matching algorithm
		for (int word_length = 1; word_length < 5; word_length++)
		{
			// Create a test word (an array of word_length ints)
			 test_word = constructBase36Word(word_length);

			 // While we can continue incrementing the test word, and we don't succeed in decrypting, attempt to match the word
			 do
			 {
				 // Converts the integer array to a char array
				 converted_word = convertFromBase36ToChar(test_word);
				 
				 // Calculate the sha1 hash of the test word
				 sha1::calc(converted_word, strlen(converted_word), hash);
				 sha1::toHexString(hash, hex_str);

				 // Score! We cracked the password
				 if (entry->hash.compare(std::string(hex_str)) == 0)
				 {
					 // so we just store it in the dictionary
					 entry->word = std::string(converted_word);
					 dictionary_lookup_map[entry->entry_number] = entry;
					 decrypted = true;
				 }
				 delete[] converted_word;

			 } while (!incrementBase36Word(test_word) && !decrypted);


			 delete[] test_word->value;
			 delete test_word;

			 // Need to jump out of the outer loop if we succeeded in decrypting the password
			 if (decrypted)
			 {
				 break;
			 }
		}

		// Some passwords may not be decrypted, so just write ???? in place of the decrypted password
		if (!decrypted)
		{
			entry->word = "????";
			dictionary_lookup_map[entry->entry_number] = entry;
		}
	}

	// Writes the cracked passwords to a file
	void Decrypt::writeToFile()
	{
		std::ofstream output_file("pass_solved.txt");
		int size = static_cast<int>(dictionary_lookup_map.size());
		for (int index = 0; index < size; index++)
		{
			output_file << dictionary_lookup_map[index]->hash << "," << dictionary_lookup_map[index]->word << "\n";
		}
		
		output_file.close();
	}
}

// Constructs a new word in base 36 to aid in cracking the passwords
Base36Word* constructBase36Word(int length)
{
	Base36Word *word = new Base36Word;
	word->length = length;
	word->value = new int[length];

	// want to initialize all values in the array to 0
	for (int index = 0; index < length; index++)
	{
		word->value[index] = 0;
	}
	
	return word;
}

// Converts an array of integers in base 36 to a char array
char* convertFromBase36ToChar(Base36Word *word)
{
	char* converted_word = new char[word->length + 1];
	for (int index = 0; index < word->length; index++)
	{
		// if the number corresponds to a lowercase letter
		if ((word->value[index] >= 0) && (word->value[index] < 26))
		{
			converted_word[index] = static_cast<char>(word->value[index] + 97);
		}
		// if the number corresponds to a number
		else if ((word->value[index] > 25) && (word->value[index] < 36))
		{
			converted_word[index] = static_cast<char>(word->value[index] + 22);
		}
	}

	// Add a null terminator (not necessary except when printing out these values while debugging)
	converted_word[word->length] = '\0';
	return converted_word;
}

bool incrementBase36Word(Base36Word *word)
{
	bool rolled_over = false;
	bool carry = false;
	int word_length = word->length;
	for (int index = (word_length - 1); index >= 0; index--)
	{
		if (carry)
		{
			if (word->value[index] < 35)
			{
				word->value[index]++;
				carry = false;
				break;
			}
			else
			{
				word->value[index] = 0;
			}
		}
		else
		{
			if (word->value[index] < 35)
			{
				word->value[index]++;
				break;
			}
			else
			{
				word->value[index] = 0;
				carry = true;
			}
		}
	}

	// If we need to carry after looping over all the indexes, the number has rolled over back to 0
	if (carry)
	{
		rolled_over = true;
	}

	return rolled_over;
}

// Test function for base 36 counter
void testBase36Counter()
{
	bool decrypted = false;
	Base36Word *test_word;
	char *converted_word;

	//for (int word_length = 1; word_length < 5; word_length++)
	{
		// Create a test word (an array of word_length ints)
		//test_word = constructBase36Word(word_length);
		test_word = constructBase36Word(4);
		test_word->value[0] = 35;
		test_word->value[1] = 35;
		test_word->value[2] = 35;
		test_word->value[3] = 0;

		// While we can continue incrementing the test word, and we don't succeed in decrypting, attempt to match the word
		do
		{
			// Converts the integer array to a char array
			converted_word = convertFromBase36ToChar(test_word);
			std::cout << converted_word << std::endl;
			delete[] converted_word;

		} while (!incrementBase36Word(test_word) && !decrypted);


		delete[] test_word->value;
		delete test_word;

	}
}