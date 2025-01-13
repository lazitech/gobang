#include <stdint.h>
#include <stdbool.h>

#define GRID_SIZE 15
#define MAX_DEPTH 10

typedef struct {
    uint64_t hash;
    int values[MAX_DEPTH];
    bool valid[MAX_DEPTH];
} HashEntry;

void init_zobrist(void);
void free_zobrist(void);
void update_hash(int row, int col, int piece);
bool probe_hash(int depth, int* value);
void store_hash(int depth, int value);