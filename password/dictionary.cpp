#include "stdafx.h"
#include "sha1.h"
#include "dictionary.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <iostream>

namespace dict
{
	// Constructor sets the number of buckets (defaults to 100000)
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

	Dictionary::~Dictionary()
	{
		// Delete existing dictionary (does nothing if no dictionary loaded)
		dictionary_map.clear();
	}

	bool Dictionary::loadDictionary(std::string filename)
	{
		std::ifstream file(filename);

		// Check to make sure file opened correctly
		if (!file.is_open())
		{
			return false;
		}

		// Delete existing dictionary (does nothing if no dictionary loaded)
		dictionary_map.clear();

		// start timer on loading operation
		LARGE_INTEGER freq, before, after;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&before);

		// Read the passwords from the file line by line and add them to the dictionary
		std::string word;
		while (std::getline(file, word))
		{
			addEntry(word);
		}
		file.close();

		// read timer on loading operation
		QueryPerformanceCounter(&after);
		// determine time elapsed while loading dictionary
		float fElapsed = static_cast<float>(after.QuadPart - before.QuadPart) / freq.QuadPart;
		std::cout << "Loading dictionary from " << filename << " took " << fElapsed << " seconds." << std::endl;
		
		// Save the filename so we can prevent the user from reloading the same dictionary later
		dictionary_filename = filename;

		return true;
	}

	// Returns true if a dictionary (of arbitrary size) was loaded, and false otherwise.
	bool Dictionary::dictionaryloaded()
	{
		if (dictionary_map.size() < 1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	// Returns true if the stored filename matches the new filename (to avoid loading a dictionary that was already loaded)
	bool Dictionary::matchFilenames(std::string filename)
	{
		if (dictionary_filename.compare(filename) == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	void Dictionary::addEntry(std::string entry_word)
	{
		Entry *new_entry = new Entry;
		new_entry->word = entry_word;
		sha1::calc(new_entry->word.c_str(), new_entry->word.length(), new_entry->hash);
		sha1::toHexString(new_entry->hash, new_entry->hex);

		dictionary_map[std::string(new_entry->hex)] = new_entry;
	}

	Entry* Dictionary::lookupEntry(std::string key)
	{
		if (dictionary_map[key])
		{
			return dictionary_map[key];
		}
		else
		{
			return nullptr;
		}
	}
}