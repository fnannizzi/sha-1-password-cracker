#include <unordered_map>

#pragma once

#ifndef DICTIONARY_DEFINED
#define DICTIONARY_DEFINED

struct Entry {
	std::string word; // plain text word
	unsigned char hash[20]; // hashed version of word
	char hex[41]; // hex version of hash
};

namespace dict
{
	class Dictionary 
	{
	private:
		std::unordered_map<std::string, Entry*> dictionary_map;
		std::string dictionary_filename;
		void addEntry(std::string entry_word);

	public:
		Dictionary(int num_buckets);
		~Dictionary();
		bool loadDictionary(std::string filename);
		bool dictionaryloaded();
		bool matchFilenames(std::string filename);
		Entry* lookupEntry(std::string key);
	};


}
 
#endif // DICTIONARY_DEFINED