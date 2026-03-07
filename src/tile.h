#ifndef TILE_H
#define TILE_H

// Includes
#include <stddef.h>
#include <stdbool.h>
#include <gtk/gtk.h>

typedef void (tileUpdate_t) (void* widget);
typedef struct
{
    tileUpdate_t* update;
    GtkWidget* widget;

} tileVmt_t;

typedef struct
{
    bool mine;
    size_t adjacentMineCount;

} tileConfig_t;

typedef struct
{
    tileVmt_t vmt;
    tileConfig_t config;

    bool visible;
    GtkWidget* tile;

} tile_t;

tile_t* tileInit (tileConfig_t* config);

#endif // TILE_H