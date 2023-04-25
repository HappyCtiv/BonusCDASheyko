//Sheyko Zakhar

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

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
    int hit_index = -1;
    int lru_index = start_index;
    int i;

    // Search for tag in cache
    for (i = start_index; i <= end_index; i++)
    {
        if (cache[i].valid == 1)
        {
            if (cache[i].tag == tag)
            {
                hit_index = i;
                break;
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
            hit_index = i;
            break;
        }
    }

    // Perform replacement if cache miss
    if (hit_index == -1)
    {
        miss_count++;
        if (replacement_policy == LRU)
        {
            hit_index = lru_index;
        }
        else
        {
            hit_index = start_index + (rand() % associativity);
        }
        cache[hit_index].tag = tag;
        cache[hit_index].valid = 1;
    }
    else
    {
        hit_count++;
        if (replacement_policy == LRU)
        {
            cache[hit_index].lru = hit_count;
        }
    }

    return hit_index;
}

// Function to read memory address from file and access the cache
void read_file_and_access_cache(const char* filename, int cache_type, int replacement_policy)
{
    int address;
    int set_index;
    int tag;
    int hit_index;
    FILE* fp;

    // Open file
    errno_t err = fopen_s(&fp, filename, "r");
    if (fp == NULL)
    {
        printf("Error: could not open file.\n");
        return;
    }

    // Read memory addresses from file and access the cache
    while (fscanf_s(fp, "%d", &address) != EOF)
    {
        calculate_set_index_and_tag(address, &set_index, &tag);
        if (cache_type == DIRECT_MAPPED)
        {
            hit_index = set_index;
        }
        else
        {
            hit_index = access_cache(set_index, tag, cache_type, replacement_policy);
        }
    }
    fclose(fp);
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

    std::ifstream myfile; myfile.open("traces.txt");
    // Read memory addresses from file and access cache for different cache types and replacement policies
    read_file_and_access_cache("traces.txt", DIRECT_MAPPED, RANDOM);
    printf("Direct-mapped, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", DIRECT_MAPPED, LRU);
    printf("Direct-mapped, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", TWO_WAY_SET_ASSOCIATIVE, RANDOM);
    printf("2-way set associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", TWO_WAY_SET_ASSOCIATIVE, LRU);
    printf("2-way set associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", FOUR_WAY_SET_ASSOCIATIVE, RANDOM);
    printf("4-way set associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", FOUR_WAY_SET_ASSOCIATIVE, LRU);
    printf("4-way set associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", FULLY_ASSOCIATIVE, RANDOM);

    printf("Fully associative, Random replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    hit_count = 0;
    miss_count = 0;
    read_file_and_access_cache("traces.txt", FULLY_ASSOCIATIVE, LRU);
    printf("Fully associative, LRU replacement:\n");
    printf("Hits: %d\n", hit_count);
    printf("Misses: %d\n", miss_count);
    printf("Hit rate: %.2f%%\n", (float)hit_count / (hit_count + miss_count) * 100);

    return 0;
}