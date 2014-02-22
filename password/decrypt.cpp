#include "decrypt.h"
#include "sha1.h"
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

	bool Decrypt::decrypt(std::string filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			return false;
		}

		// start timer on processing operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		std::string hashed_password;
		int line_index = 0;
		while (std::getline(file, hashed_password))
		{
			addEntry(hashed_password, line_index);
			line_index++;
		}
		file.close();
		
		bruteForce();
		writeToFile();

		// read timer on loading operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while cracking passwords
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;
		std::cout << "Processing passwords from " << filename << " took " << fElapsed << " seconds." << std::endl;

		return true;
	}

	void Decrypt::addEntry(std::string hashed_password, int line_index)
	{
		DecryptionEntry *decryptionEntry = new DecryptionEntry;
		decryptionEntry->entry_number = line_index;
		decryptionEntry->hash = hashed_password;

		Entry *dictionaryEntry = dictionary->lookupEntry(hashed_password);

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
		// start timer on brute force operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		for_each(brute_force_list.begin(), brute_force_list.end(), [this](DecryptionEntry *entry){decryptWordByBruteForce(entry); });

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
		for (int word_length = 1; word_length < 5; word_length++)
		{
			 test_word = constructBase36Word(word_length);
			 do
			 {
				 converted_word = convertFromBase36ToChar(test_word);
				 sha1::calc(converted_word, strlen(converted_word), hash);
				 sha1::toHexString(hash, hex_str);

				 if (entry->hash.compare(std::string(hex_str)) == 0)
				 {
					 entry->word = std::string(converted_word);
					 dictionary_lookup_map[entry->entry_number] = entry;
					 decrypted = true;
				 }
				 delete[] converted_word;
				 //delete[] hash;

			 } while (!incrementBase36Word(test_word) && !decrypted);


			 delete[] test_word->value;
			 delete test_word;

			 if (decrypted)
			 {
				 break;
			 }
		}

		if (!decrypted)
		{
			entry->word = "????";
			dictionary_lookup_map[entry->entry_number] = entry;
		}
	}

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

Base36Word* constructBase36Word(int length)
{
	Base36Word *word = new Base36Word;
	word->length = length;
	word->value = new int[length];
	for (int index = 0; index < length; index++)
	{
		word->value[index] = 0;
	}
	
	return word;
}

char* convertFromBase36ToChar(Base36Word *word)
{
	char* converted_word = new char[word->length + 1];
	for (int index = 0; index < word->length; index++)
	{
		if ((word->value[index] >= 0) && (word->value[index] < 26))
		{
			converted_word[index] = static_cast<char>(word->value[index] + 97);
		}
		else if ((word->value[index] > 25) && (word->value[index] < 36))
		{
			converted_word[index] = static_cast<char>(word->value[index] + 22);
		}
	}

	converted_word[word->length] = '\0';
	return converted_word;
}

bool incrementBase36Word(Base36Word *word)
{
	bool rolled_over = false;
	int word_length = word->length;
	for (int index = (word_length - 1); index >= 0; index--)
	{
		if (word->value[index] < 35)
		{
			word->value[index]++;
			break;
		}
		else
		{
			// need to carry a 1
			word->value[index] = 0;
			if (index > 0)
			{
				if (word->value[index - 1] < 35)
				{
					word->value[index - 1]++;
					break;
				}
				// need to carry again
				else
				{
					word->value[index - 1] = 0;
					if (index > 1)
					{
						if (word->value[index - 2] < 35)
						{
							word->value[index - 2]++;
							break;
						}
						// need to carry again
						else
						{
							word->value[index - 2] = 0;
							if (index > 2)
							{
								if (word->value[index - 3] < 35)
								{
									word->value[index - 3]++;
									break;
								}
								// rolling over
								else
								{
									rolled_over = true;
								}
							}
							else
							{
								rolled_over = true;
							}
						}
					}
					else
					{
						rolled_over = true;
					}
				}
			}
			else
			{
				rolled_over = true;
			}
		}
	}
	return rolled_over;
}