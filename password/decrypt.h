#include "dictionary.h"
#include <map>
#include <list>

#pragma once

#ifndef DECRYPT_DEFINED
#define DECRYPT_DEFINED

struct DecryptionEntry {
	int entry_number; // the index of the password in the file
	std::string word; // plain text word
	std::string hash; // hex version of hash
};

namespace dcry
{
	class Decrypt
	{
	private:
		dict::Dictionary *dictionary;
		std::map<int, DecryptionEntry*> dictionary_lookup_map;
		std::list<DecryptionEntry*> brute_force_list;

	public:
		Decrypt(dict::Dictionary *dict);
		~Decrypt();
		bool decrypt(std::string filename);
		void addEntry(std::string hashed_password, int line_index);
		void bruteForce();
		void decryptWordByBruteForce(DecryptionEntry* entry);
		void writeToFile();
	};


}

void testBase36Counter();

#endif // DECRYPT_DEFINED