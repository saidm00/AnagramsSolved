#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "hash_map.h"

typedef struct freq_array
{
	uint32_t data[26];
} freq_array_t;

void diff_freq_array(freq_array_t *dst_freq, const freq_array_t *f1, const freq_array_t *f2)
{
	for (uint32_t i = 0; i < 26; i++)
		dst_freq->data[i] = f2->data[i] - f1->data[i];
}

void build_freq_table(freq_array_t *freq_table, const char *s, size_t len)
{
	freq_array_t freq = {0};

	for (size_t i=0; i<len; ++i)
	{
		++freq.data[s[i]-97];
		memcpy(freq_table[i].data, &freq, sizeof(freq_array_t));
	}
}

void print_substring(const char *p1, const char *p2)
{
	size_t len = p2 - p1 + 1;
	char *substring = (char *)malloc(len + 1);

	memcpy(substring, p1, len);
	substring[len] = '\0';

	printf("%s\n", substring);

	free(substring);
}




int main(int argc, char *argv[])
{
	const char *s = "ifailuhkqq";
	size_t len = strlen(s);
	size_t freq_table_size = len * sizeof(freq_array_t);
	freq_array_t *freq_table = (freq_array_t *)malloc(freq_table_size);
	memset(freq_table, 0, freq_table_size);

	build_freq_table(freq_table, s, len);

/*
#ifndef _NDEBUG
	for (size_t i=0; i<len; ++i)
	{
		printf("%lu (%c): [", i, s[i]);
		for (size_t j=0; j<26; ++j) printf("%u ", freq_table[i].data[j] );
		printf("]\n");
	}
#endif
*/
	
	// Substring table

	size_t substring_count = len * (len + 1) / 2;
#if defined(_NDEBUG) && 0
	printf("Substring count is %lu\n", substring_count);
#endif
	hash_map_t substrings;
	initialize_hash_map(&substrings, substring_count, sizeof(freq_array_t), sizeof(unsigned int));


	// Check for substrings
	for (size_t i1 = 0; i1 < len; ++i1)
	{
		for (size_t i2 = i1; i2 < len; ++i2)
		{
			freq_array_t   f;	// freq array(substr)
			freq_array_t *f1;
			freq_array_t *f2 = freq_table + i2;
			
			if (i1 >= 1)
			{
				f1 = freq_table + (i1-1);
				diff_freq_array(&f, f1, f2); // Difference between freq array at i1 and i2
			}
			else
				memcpy(&f, f2, sizeof(freq_array_t));

#if defined(_NDEBUG) && 0
			print_substring(&s[i1], &s[i2]);
#endif

			hash_key_t key;
			key.data = &f.data[0];
			key.len = sizeof(freq_array_t);

			if (hash_map_has(&substrings, &key))
			{
				puts("FOUND A PAIR");
			
				/* Increment occurrence for this substring */
				unsigned int value = *(unsigned int *)hash_map_get(&substrings, &key) + 1;
				hash_map_set(&substrings, &key, &value);
			}
			else
			{
				unsigned int value = 1;
				hash_map_set(&substrings, &key, &value); // :)
			}

			// puts("====================");
		}
	}

	size_t numPairs = 0;

	for (size_t i = 0; i < substrings.elemCount; ++i)
	{
		_Bool isUsed = hash_map_has_by_index(&substrings, i);
		if (isUsed)
		{
			const void *ptr = hash_map_get_by_index(&substrings, i);
			unsigned int value = *(unsigned int *)ptr;
			printf("%lu: %u\n", i, value);

			numPairs += ( value * (value - 1) ) / 2;
		}
		else
		{
			printf("%lu: UNUSED\n", i);
		}
		
	}

	puts("=========================");
	printf("Number of pairs: %lu\n", numPairs);
	puts("=========================");

	destroy_hash_map(&substrings);
	free(freq_table);

	return EXIT_SUCCESS;
}

/*
Yeah it's writing a 3
S = abba
substrings
						V	  	V(V-1)/2
	a	a				2	  		1
	b b				2	  		1
	ab ba			2	  		1
	bb				1	  		0
	abb bba		2	  		1
	abba			1	  		0
						^   sum=4
						|
		we should have these in hashmap as values
		
It does find 4 pairs, but writes into one that already has a 2. Yes



*/