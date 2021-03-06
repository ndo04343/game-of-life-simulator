/*
 * ============================================================================
 *
 *       Filename:  game.h
 *
 *    Description:  Header file of game.c
 *
 *        Created:  08/12/2021
 *       Compiler:  cc
 *
 *         Author:  Heesung Yang
 *   Organization:  Kyoungpook National University
 *
 * ============================================================================
 */
#ifndef GAME_H
#define GAME_H

/*
 * Game constants 
 */
/* macros */
#define INIT_ARR_SIZE 1
#define SEARCH_NO_ELEMENT -1

#define RULE1_CONSTANT 3 /* If dead cell adjacent with '3' alived cells, then the cell is born. */
#define RULE2_CONSTANT1 2 /* If living cell adjacent with '2' living cells, then the cell keeps alive. */
#define RULE2_CONSTANT2 3 /* If living cell adjacent with '3' living cells, then the cell keeps alive. */

#define LOAD_MAP_INIT_CAPACITY 1024

/* direction */
typedef enum {north_east, east, south_east, south, south_west, west, north_west} direction_t;

/*
 * Types
 */
typedef unsigned long long count_t; /* counting variable types */
typedef struct { int row; int col; } coordinate_t; /* coordinate type */
typedef struct { count_t num_of_cells, capacity; coordinate_t *coordinate_arr; } map_t; /* map type */

/*
 * Logics
 */
void freemap(map_t map);
map_t _copy_map(map_t origin);
map_t _get_cell_map_need_to_check(map_t map);
map_t _get_next_map(map_t map);
count_t _get_num_of_adjacent_living_cells(map_t map, coordinate_t pos);
coordinate_t get_onestep_coordinate(coordinate_t pos, direction_t dir);
map_t init_map();
map_t _cell_birth(map_t map, coordinate_t pos);
map_t _cell_death(map_t map, coordinate_t pos);
int _search_coordinate(map_t map, coordinate_t pos, int left, int right);

/*
 * Temporal implemented for DEBUG
 */
void print_map(map_t map);
void save_instance(const char *file_name, map_t map);
void write_map(int fd, map_t map);                      /* IPC Protocol */
map_t load_file(const char *file_name);
map_t read_map(int fd);                                 /* IPC Protocol */

#endif /* end of #ifndef GAME_H */
