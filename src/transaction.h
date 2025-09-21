#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "cglm/types.h"

struct GameState;
struct TileMap;
enum ActionType;

struct MoveRequest
{
    int entity;
    ivec2 from;
    ivec2 to;
};

struct Transaction
{
    struct MoveRequest moves[20];
    int moves_count;
};

struct TxBatch
{
    struct Transaction *transactions;
    int count;
    int capacity;
};

struct TxBatch transactionlist_init();

void transactionlist_deinit(struct TxBatch *list);

void transactionlist_append(struct TxBatch *list, struct Transaction transaction);

int transactionlist_is_empty(struct TxBatch *transactions);

struct Transaction create_transaction_from_move(struct GameState *gamestate, struct MoveRequest move);

struct Transaction create_transaction_from_action(struct GameState *gamestate, enum ActionType type, int target_index);

void batch_remove_tilmap_collision(struct TxBatch *batch, struct GameState *gamestate,  struct TileMap *tilemap);

#endif // TRANSACTION_H
