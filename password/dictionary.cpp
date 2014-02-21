#include "stdafx.h"
#include "dictionary.h"

namespace dict
{
	Dictionary::Dictionary(int num_buckets)
	{
		if (num_buckets > 0)
		{
			dictionary_map.rehash(num_buckets);
		}
		else 
		{
			dictionary_map.rehash(100000);
		}
	}

	bool Dictionary::loadDictionary(std::string filename)
	{
		std::ifstream file(filename);
		if (!file.is_open())
		{
			return false;
		}

		// start timer on loading operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		std::string word;
		while (std::getline(file, word))
		{
			addEntry(word);
		}

		// read timer on loading operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while loading dictionary
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;

		return true;
	}

	void Dictionary::addEntry(std::string entry_word)
	{
		Entry new_entry;
		new_entry.word = entry_word;
		sha1::calc(new_entry.word.c_str(), new_entry.word.length(), new_entry.hash);
		sha1::toHexString(new_entry.hash, new_entry.hex);

		dictionary_map[entry_word] = &new_entry;
	}
}