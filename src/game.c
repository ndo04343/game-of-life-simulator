/*
 * ============================================================================
 *
 *       Filename:  game.c
 *
 *    Description:  Main logic of gol
 *
 *        Created:  08/12/2021
 *       Compiler:  cc
 *
 *         Author:  Heesung Yang
 *   Organization:  Kyoungpook National University
 *
 * ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>

#include "dynamic-array.h"
#include "game.h"

/*
 * game constants
 */
/* unit direction coordinates */
static const coordinate_t direction[8] = {
    {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}, {1, 0}
};

/*
 * Free map
 */
void freemap(map_t map)
{
    /* If capacity is zero, it is empty map. */
    map.num_of_cells = map.capacity = 0;
    free(map.coordinate_arr);
}

/*
 * Copy map
 * return copied_map(map_t)
 * It contains new reference to heap memory which has same values of origin map.
 */
map_t
_copy_map(map_t origin)
{
    int i;
    map_t copied_map = init_map();

    /* copy metadatas */
    copied_map.num_of_cells = origin.num_of_cells;
    copied_map.capacity = origin.capacity;

    /* new allocation */
    copied_map.coordinate_arr = create_arr(origin.capacity, sizeof(*(copied_map.coordinate_arr)));

    /* copy elements one by one */
    for (i = 0 ;i < origin.num_of_cells; i ++)
        copied_map.coordinate_arr[i] = origin.coordinate_arr[i];

    return copied_map;
}

/*
 * Get cell map to check to move to the next step
 */
map_t 
_get_cell_map_need_to_check(map_t map)
{
    int i, j;
    map_t cellmap = init_map();

    for (i = 0; i < map.num_of_cells;i ++) {
        cellmap = _cell_birth(
            cellmap,
            map.coordinate_arr[i]);
        for (j = (int)north_east; j < 8; j ++)
            cellmap = _cell_birth(
                cellmap,
                get_onestep_coordinate(
                    map.coordinate_arr[i],
                    j));
    }

    return cellmap;
}

/*
 * get next map
 */
map_t 
_get_next_map(map_t map)
{
    int i, searched_index, num_of_adjacent_living_cells;
    coordinate_t check_pos;
    map_t need_to_check_cellmap, new_map;

    /* Initialize */
    need_to_check_cellmap = _get_cell_map_need_to_check(map);
    new_map = _copy_map(map);

    /* one by one check */
    for (i = 0; i < need_to_check_cellmap.num_of_cells;i ++) {
        check_pos = need_to_check_cellmap.coordinate_arr[i];
        searched_index = _search_coordinate(map, check_pos, 0, map.num_of_cells - 1);
        num_of_adjacent_living_cells = _get_num_of_adjacent_living_cells(map, check_pos);

        if ((SEARCH_NO_ELEMENT == searched_index)
            && (_get_num_of_adjacent_living_cells(map, check_pos) == RULE1_CONSTANT))
                /* Dead cell with 3 adjacent alived cells case */
                new_map = _cell_birth(new_map, check_pos);
        else if ((SEARCH_NO_ELEMENT != searched_index)
                  && !(num_of_adjacent_living_cells == RULE2_CONSTANT1 || RULE2_CONSTANT2))
                /* Living cell with not 2 or 3 adjacent living cells case */
                new_map = _cell_death(new_map, check_pos);
    }

    /* clean */
    freemap(map);
    freemap(need_to_check_cellmap);

    return new_map;
}

/*
 * Get number of adjacent living cells
 */
count_t 
_get_num_of_adjacent_living_cells(map_t map, coordinate_t pos)
{
    int i;
    count_t num_of_adjacent_living_cells = 0;

    /* counting from north east direction to north with clockwise */
    for (i = (int)north_east; i < 8; i ++)
        if (_search_coordinate(map, get_onestep_coordinate(pos, i), 0, map.num_of_cells - 1) != SEARCH_NO_ELEMENT) 
            num_of_adjacent_living_cells ++;

    return num_of_adjacent_living_cells;
}


/*
 * Get coordinate moved one step from origin coordinate
 */
coordinate_t 
get_onestep_coordinate(coordinate_t pos, direction_t dir)
{
    coordinate_t moved;

    moved.row = pos.row + direction[dir].row;
    moved.col = pos.col + direction[dir].col;

    return moved;
}

/*
 * Initialize map
 */
map_t
init_map()
{
    map_t new_map = {
        0, 
        1, 
        (coordinate_t *)create_arr(INIT_ARR_SIZE, sizeof(coordinate_t))};
    return new_map;
}

/*
 * Birth cell function with O(N)
 */
map_t
_cell_birth(map_t map, coordinate_t pos)
{
    int i, target_pos;

    /* 
     * empty map case 
     * Since capacity == 1 and num_of_cells == 0,
     * just put in cell to first element.
     */
    if (map.num_of_cells == 0) {
        /*
         * Just put in one cell
         */
        map.coordinate_arr[0] = pos;
        map.num_of_cells ++;
        return map;
    }

    /* 
     * Already alived cell case 
     * Since map already living cell at the pos,
     * we don't need to move anything.
     */
    if (_search_coordinate(map, pos, 0, map.num_of_cells - 1) != SEARCH_NO_ELEMENT)
        return map;

    /* If we need, array doubling. */
    if (map.num_of_cells >= map.capacity) {
        map.coordinate_arr = arr_doubling(map.coordinate_arr, map.capacity, sizeof(*(map.coordinate_arr)));
        map.capacity *= 2;
    }

    /*
     * general case (no duplicated cell position) 
     * insert position to sparse matrix form map.
     */
    for (target_pos = 0; target_pos < map.num_of_cells; target_pos++) {
        if (pos.row == map.coordinate_arr[target_pos].row) {
            if (pos.col < map.coordinate_arr[target_pos].col) 
                break;
        } else if (pos.row < map.coordinate_arr[target_pos].row) 
            break;
    }
    /* array shifting */
    for (i = map.num_of_cells; i > target_pos; i --)
        map.coordinate_arr[i] = map.coordinate_arr[i - 1];
    /* put in cell to target position */
    map.coordinate_arr[target_pos] = pos;
        
    /* Update metadata of map and return */
    map.num_of_cells ++;
    return map;
}

/*
 * Killing cell function with O(N)
 */
map_t
_cell_death(map_t map, coordinate_t pos)
{
    int i, target_pos;

    /* 
     * Already dead cell case
     * We don't need to move anything.
     */
    if ((target_pos = _search_coordinate(map, pos, 0, map.num_of_cells - 1)) == SEARCH_NO_ELEMENT)
        return map;

    /*
     * general case (just killing living cell)
     */
    /* array shifting */
    for (i = target_pos; i < map.num_of_cells - 1 ; i ++)
        map.coordinate_arr[i] = map.coordinate_arr[i + 1];
        
    /* Update metadata of map and return */
    map.num_of_cells --;

    /* 
     * If we need, we have to reduce array size.
     */
    if (map.num_of_cells*2 <= map.capacity && map.capacity != 1) {
        map.coordinate_arr = arr_cut_in_half(map.coordinate_arr, map.capacity, sizeof(*(map.coordinate_arr)));
        map.capacity /= 2;
    }

    return map;
}

/*
 * Implemented with recursive binary search algorithm < O(logN) >.
 * This binary search algorithm aims to sparse matrix search.
 */
int
_search_coordinate(map_t map, coordinate_t pos, int left, int right) 
{
    int mid = (left + right)/2;

    /* Binary search terminal condition */
    if (left > right) return SEARCH_NO_ELEMENT;

    /* comparing by row */
    if (map.coordinate_arr[mid].row < pos.row) 
        return _search_coordinate(map, pos, mid + 1, right);
    else if (map.coordinate_arr[mid].row == pos.row) {
        /* comparing by col */
        if (map.coordinate_arr[mid].col < pos.col) 
            return _search_coordinate(map, pos, left, mid - 1);
        else if (map.coordinate_arr[mid].col == pos.col)
            return mid; /* Found out case */
        else /* map.coordinate_arr[mid].col > pos.col */
            return _search_coordinate(map, pos, mid + 1, right);
    }
    /* map.coordinate_arr[mid].row > pos.row */
    return _search_coordinate(map, pos, left, mid - 1);
}

/*
 * Temporal implemented for DEBUG
 */
void
print_map(map_t map)
{
    int i;

    puts("map info :");
    fprintf(
        stdout, 
        "map.num_of_cells == %lld\n", 
        map.num_of_cells);
    fprintf(
        stdout, 
        "map.capacity == %lld\n", 
        map.capacity);
    fprintf(
        stdout,
        "map.coorinate_arr == 0x%p\n", 
        map.coordinate_arr);

    for (i = 0 ;i < map.num_of_cells; i ++) {
        printf("(%d, %d)\t", 
            map.coordinate_arr[i].row, 
            map.coordinate_arr[i].col);
        if (!((i + 1)%4)) putchar('\n');
    }
}