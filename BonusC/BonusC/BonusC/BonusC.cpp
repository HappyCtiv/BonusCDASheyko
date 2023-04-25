//Sheyko Zakhar

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>

// Define constants
#define CACHE_SIZE 32
#define BLOCK_SIZE 4
#define NUM_SETS (CACHE_SIZE / BLOCK_SIZE)
#define DIRECT_MAPPED 1
#define TWO_WAY_SET_ASSOCIATIVE 2
#define FOUR_WAY_SET_ASSOCIATIVE 4
#define FULLY_ASSOCIATIVE 0
#define LRU 0
#define RANDOM 1


typedef struct {
    int tag;
    int data[BLOCK_SIZE];
    int valid;
    int lru;
} CacheBlock;

// Declare global variables
CacheBlock cache[CACHE_SIZE];
int hit_count = 0;
int miss_count = 0;

// Function to calculate the set index and tag for a given memory address
void calculate_set_index_and_tag(int address, int* set_index, int* tag)
{
    *set_index = (address / BLOCK_SIZE) % NUM_SETS;
    *tag = address / (BLOCK_SIZE * NUM_SETS);
}

// Function to access the cache and return the index of the cache block containing the tag
int access_cache(int set_index, int tag, int associativity, int replacement_policy)
{
    int start_index = set_index * associativity;
    int end_index = start_index + associativity - 1;
    int hit_count = 0;
    int lru_index = start_index;
    int i;

    // Search for tag in cache
    for (i = start_index; i <= end_index; i++)
    {
        if (cache[i].valid == 1)
        {
            if (cache[i].tag == tag)
            {
                hit_count++;
            }
            // Update LRU index
            if (cache[i].lru > cache[lru_index].lru)
            {
                lru_index = i;
            }
        }
        else
        {
            // Found an empty block
            hit_count++;
            break;
        }
    }

    // Perform replacement if cache miss
    if (hit_count == 0)
    {
        miss_count++;
        if (replacement_policy == LRU)
        {
            // Replace the cache block with the LRU index
            hit_count++;
            cache[lru_index].tag = tag;
            cache[lru_index].valid = 1;
        }
        else
        {
            // Replace a random cache block
            hit_count++;
            int random_index = start_index + (rand() % associativity);
            cache[random_index].tag = tag;
            cache[random_index].valid = 1;
        }
    }
    else
    {
        hit_count--;
        hit_count += associativity;  // Full hit count for fully associative cache
        if (replacement_policy == LRU)
        {
            cache[lru_index].lru = hit_count;
        }
    }

    return hit_count;
}

// Function to read memory address from file and access the cache
void read_file_and_access_cache(const char* filename, int cache_type, int replacement_policy)
{
    int address;
    int set_index;
    int tag;
    
    std::ifstream input_file(filename);

    if (!input_file.is_open()) {
        std::cerr << "Error: could not open file." << std::endl;
        return;
    }

    while (input_file >> address)
    {
        calculate_set_index_and_tag(address, &set_index, &tag);
        if (cache_type == DIRECT_MAPPED)
        {
            hit_count = set_index;
        }
        else
        {
            hit_count = access_cache(set_index, tag, cache_type, replacement_policy);
        }
        cache[hit_count].data[address % BLOCK_SIZE] = address;
    }
    input_file.close();
}

int main()
{
    // Initialize cache
    int i, j;
    for (i = 0; i < CACHE_SIZE; i++)
    {
        cache[i].tag = 0;
        cache[i].valid = 0;
        cache[i].lru = 0;
        for (j = 0; j < BLOCK_SIZE; j++)
        {
            cache[i].data[j] = 0;
        }
    }

    std::ifstream input_file("../traces.txt");
    // Read memory addresses from file and access cache for different cache types and replacement policies
    read_file_and_access_cache("../traces.txt", DIRECT_MAPPED, RANDOM);
    printf("Direct-mapped, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", DIRECT_MAPPED, LRU);
    printf("Direct-mapped, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", TWO_WAY_SET_ASSOCIATIVE, RANDOM);
    printf("2-way set associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", TWO_WAY_SET_ASSOCIATIVE, LRU);
    printf("2-way set associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", FOUR_WAY_SET_ASSOCIATIVE, RANDOM);
    printf("4-way set associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", FOUR_WAY_SET_ASSOCIATIVE, LRU);
    printf("4-way set associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", FULLY_ASSOCIATIVE, RANDOM);

    printf("Fully associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("../traces.txt", FULLY_ASSOCIATIVE, LRU);
    printf("Fully associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    return 0;
}