// Header
#include "tile.h"

static void update (void* widget)
{
    tile_t* tile = widget;

    if (tile->visible && tile->config.mine)
        gtk_button_set_label (GTK_BUTTON (tile->tile), "!");

    else if (tile->visible && tile->config.adjacentMineCount == 0)
        gtk_button_set_label (GTK_BUTTON (tile->tile), "");

    else if (tile->visible)
    {
        char* adjacentMineCount = malloc (sizeof (char*));
        if (adjacentMineCount == NULL)
            return;

        sprintf (adjacentMineCount, "%zu", tile->config.adjacentMineCount);
        gtk_button_set_label (GTK_BUTTON (tile->tile), adjacentMineCount);
        free (adjacentMineCount);
    }
}

tile_t* tileInit (tileConfig_t* config)
{
    tile_t* tile = malloc (sizeof (tile_t));
    if (tile == NULL)
        return NULL;

    *tile = (tile_t)
    {
        .vmt =
        {
            .update = update,
            .widget = gtk_grid_new ()
        },
        .visible = false,
        .config = *config,
        .tile = gtk_button_new_with_label ("-")
    };

    return tile;
}