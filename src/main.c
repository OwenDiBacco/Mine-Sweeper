// Headers
#include "tile.h"
#include "stopwatch.h"

// Includes
#include <stdio.h>
#include <gtk/gtk.h>

typedef struct
{
    size_t rows;
    size_t cols;

    size_t tileCount;

    GtkWidget* grid;
    tile_t*** tiles;

    stopwatch_t* stopwatch;
    guint timeout;

} board_t;

static void freeBoard (GtkWidget* widget, gpointer data)
{
    board_t* board = data;

    for (size_t row = 0; row < board->rows; ++row)
        for (size_t col = 0; col < board->cols; ++col)
            free (board->tiles[row][col]);

    for (size_t row = 0; row < board->rows; ++row)
        free (board->tiles[row]);
    free (board->tiles);

    free (board->stopwatch);
    free (board);

    g_source_remove (board->timeout);
}

gboolean updateStopwatch (gpointer data)
{
    stopwatch_t* stopwatch = data;
    if (stopwatch == NULL)
        return false;

    if (stopwatch->incrementing)
        stopwatch->vmt.update (stopwatch);

    return true;
}

static void updateTile (GtkWidget* widget, gpointer data)
{
    (void) widget;

    tile_t* tile = data;
    tile->visible = true;
}

static void updateBoard (GtkWidget* widget, gpointer data)
{
    (void) widget;
    board_t* board = data;

    size_t tileCount = 0;

    if (board->stopwatch->incrementing)
    {
        for (size_t row = 0; row < board->rows; ++row)
        {
            for (size_t col = 0; col < board->cols; ++col)
            {

                // Reveals hidden clear tiles that are adjacent to visible clear tiles
                if (board->tiles[row][col]->config.adjacentMineCount == 0 && board->tiles[row][col]->visible)
                    for (ssize_t r = row - 1; r <= row + 1; ++r)
                        for (ssize_t c = col - 1; c <= col + 1; ++c)
                            if ((r >= 0 && r < board->rows) && (c >= 0 && c < board->cols))
                                if (board->tiles[r][c]->config.adjacentMineCount == 0)
                                    board->tiles[r][c]->visible = true;

                board->tiles[row][col]->vmt.update (board->tiles[row][col]);

                // Checks for Loss
                if (board->tiles[row][col]->visible && board->tiles[row][col]->config.mine)
                    board->stopwatch->incrementing = false;

                else if (board->tiles[row][col]->visible)
                    ++tileCount;
            }
        }

        // Checks for Win
        if (board->tileCount == tileCount)
            // TODO: save high score to S3 Bucket
            board->stopwatch->incrementing = false;
    }
}

size_t boardInit (GtkWidget* window, board_t* board)
{
    srand (time (NULL));

    board->grid = gtk_grid_new ();
    gtk_window_set_child (GTK_WINDOW (window), board->grid);

    // Creates the Stopwatch

    board->stopwatch = stopwatchInit (&(stopwatchConfig_t)
    {
        .width = 25,
        .height = 50
    });

    gtk_grid_attach (GTK_GRID (board->grid), board->stopwatch->stopwatch, 4, 0, 1, 1);

    // Creates the Board

    board->rows = 9;
    board->cols = 9;

    board->tileCount = board->rows * board->cols;

    // Allocates Dynamic 2d Array
    board->tiles = malloc (board->rows * sizeof (tile_t **));
    if (board->tiles == NULL)
        return -1;

    for (size_t row = 0; row < board->rows; ++row)
    {
        board->tiles[row] = malloc (board->cols * sizeof (tile_t*));
        if (board->tiles[row] == NULL)
            return -1;
    }

    // Populates the Board with Mines / Tiles
    for (size_t row = 0; row < board->rows; ++row) {
        for (size_t col = 0; col < board->cols; ++col) {

            bool mine = rand () % 8;
            if (!mine) --board->tileCount;

            board->tiles[row][col] = tileInit (&(tileConfig_t)
            {
                .mine = !mine,
                .adjacentMineCount = 0
            });

            g_signal_connect (board->tiles[row][col]->tile, "clicked", G_CALLBACK (updateTile), board->tiles[row][col]);
            g_signal_connect (board->tiles[row][col]->tile, "clicked", G_CALLBACK (updateBoard), board);

            gtk_grid_attach (GTK_GRID (board->grid), board->tiles[row][col]->tile, col, row + 1, 1, 1);
        }
    }

    // Sets the Adjacent Mine Count of each Tile
    for (size_t row = 0; row < board->rows; ++row)
        for (size_t col = 0; col < board->cols; ++col)
            if (board->tiles[row][col]->config.mine)
                for (ssize_t r = row - 1; r <= row + 1; ++r)
                    for (ssize_t c = col - 1; c <= col + 1; ++c)
                        if ((r >= 0 && r < board->rows) && (c >= 0 && c < board->cols))
                            board->tiles[r][c]->config.adjacentMineCount += 1;

    return 0;
}

static void activate (GtkApplication *app, gpointer user_data)
{
    GtkWidget* window;
    GtkWidget* button;

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Mine-Sweeper");
    gtk_window_set_default_size (GTK_WINDOW (window), 425, 320);

    board_t* board = malloc (sizeof (board_t));
    if (board == NULL)
        return;

    boardInit (window, board);
    gtk_window_present (GTK_WINDOW (window));

    board->timeout = g_timeout_add (16, updateStopwatch, board->stopwatch);
    g_signal_connect (window, "destroy", G_CALLBACK (freeBoard), board);
}

int main (int argc, char** argv)
{
    GtkApplication* app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}