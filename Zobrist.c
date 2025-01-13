#include "Zobrist.h"
#include <stdlib.h>
#include <time.h>
#include "mt19937.h"
#define HASH_SIZE (1 << 20)

static HashEntry* hash_table;
 uint64 zobrist_table[GRID_SIZE][GRID_SIZE][3];
static uint64 current_hash;

void init_zobrist(void) {
    srand((unsigned int)time(NULL));
    mt_initialize((uint64)time(0));
    hash_table = (HashEntry*)malloc(HASH_SIZE * sizeof(HashEntry));
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            for (int k = 0; k < 3; ++k) {
                zobrist_table[i][j][k] = mt_rand();
            }
        }
    }
}

void free_zobrist(void) {
    free(hash_table);
}

void update_hash(int row, int col, int piece) {
    current_hash ^= zobrist_table[row][col][piece];
}

bool probe_hash(int depth, int* value) {
    int index = current_hash % HASH_SIZE;
    HashEntry* entry = &hash_table[index];
    if (entry->hash == current_hash && entry->valid[depth]) {
        *value = entry->values[depth];
        return true;
    }
    return false;
}

void store_hash(int depth, int value) {
    int index = current_hash % HASH_SIZE;
    HashEntry* entry = &hash_table[index];
    entry->hash = current_hash;
    entry->values[depth] = value;
    entry->valid[depth] = true;
}