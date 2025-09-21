#include "transaction.h"
#include "gamestate.h"
#include "cglm/ivec2.h"
#include "tilemap.h"

ivec2 directions[] = {{0, -1},{0, 1},{-1, 0},{1, 0}};

struct TxBatch transactionlist_init()
{
    struct TxBatch list;
    list.transactions = NULL;
    list.count = 0;
    list.capacity = 0;
    return list;
}

void transactionlist_deinit(struct TxBatch *list)
{
    if(list->capacity)
    {
        free(list->transactions);
    }
}

void transactionlist_append(struct TxBatch *list, struct Transaction transaction)
{
    if(list->capacity <= list->count) 
    {
        if(list->capacity <= 0)
        {
            list->capacity = 1;
        }
        else
        {
            list->capacity *= 2;
        }
        struct Transaction *new_list = malloc(sizeof(struct Transaction)*list->capacity);
        for(int i = 0; i < list->count; ++i)
        {
            new_list[i] = list->transactions[i];
        }
        free(list->transactions);
        list->transactions = new_list;
    }
    list->transactions[list->count++] = transaction;
}

int transactionlist_is_empty(struct TxBatch *transactions)
{
    return !transactions->count;
}

struct Transaction create_transaction_from_move(struct GameState *gamestate, struct MoveRequest move)
{
    struct Transaction tr;
    tr.moves[0] = move;
    tr.moves_count = 1;
    int cur_move_index = 0; 
    ivec2 dir;
    glm_ivec2_sub(move.to, move.from, dir);
    while(cur_move_index < tr.moves_count)
    {
        struct MoveRequest cur_move = tr.moves[cur_move_index];
        for(int i = 0; i < gamestate->entity_count; ++i)
        {
            struct Entity *other = gamestate->entities+i; 
            if(!glm_ivec2_eqv(other->position, cur_move.to)) continue;
            if(other->solidity == SOLIDITY_STATIC)
            {
                tr.moves_count = 0;            
                return tr;
            }
            if(other->solidity == SOLIDITY_MOVABLE)
            {
                struct MoveRequest other_move;
                other_move.entity = i;
                glm_ivec2_copy(other->position, other_move.from);
                glm_ivec2_add(other_move.from, dir, other_move.to);
                tr.moves[tr.moves_count++] = other_move;
            }
        }
        ++cur_move_index;
    }
    return tr;
}

struct Transaction create_transaction_from_action(struct GameState *gamestate, enum ActionType type, int target_index)
{
    struct Entity *target = gamestate->entities+target_index;
    struct Transaction transaction;
    int dir_index = (int)(type - ACTION_UP);
    if(target->solidity == SOLIDITY_MOVABLE)
    {
        struct MoveRequest move;
        move.entity = target_index;
        glm_ivec2_copy(target->position, move.from);
        glm_ivec2_add(move.from, directions[dir_index], move.to);
        transaction = create_transaction_from_move(gamestate, move);
    }
    else
    {
        transaction.moves_count = 0;
    }
    return transaction;
}

void batch_remove_tilmap_collision(struct TxBatch *batch, struct GameState *gamestate,  struct TileMap *tilemap)
{
    for(int i = batch->count-1; i >= 0 ; --i)
    {
        struct Transaction *tr = batch->transactions+i;
        int valid = 1;
        for(int j = 0; j < tr->moves_count; ++j)
        {
            struct MoveRequest move = tr->moves[j];
            struct Entity *ent = gamestate->entities+move.entity;
            if(ent->type == ENTITY_PLAYER && is_door_at(gamestate, move.to) && gamestate->is_door_opened)
            {
                continue;
            }
            if(is_tilemap_solid_at(tilemap, move.to))
            {
                valid = 0;
                break;
            }
        }
        if(valid) continue;
        printf("Transaction reduced for entity %d.\n", batch->transactions[i].moves[0].entity);
        batch->transactions[i] = batch->transactions[--batch->count];
    }
}


