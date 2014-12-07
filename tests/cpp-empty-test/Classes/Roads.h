#ifndef __ROADS_H__
#define __ROADS_H__

#include "MapLayer.h"

#define POS_INDEX_SIZE 2

static std::vector<PosIndex> road1 = {
    PosIndex(8, 0),  PosIndex(8, 1),  PosIndex(9, 1),  PosIndex(9, 2),  PosIndex(9, 3),
    PosIndex(8, 3),  PosIndex(7, 3),  PosIndex(7, 2),  PosIndex(6, 2),  PosIndex(5, 2),
    PosIndex(5, 3),  PosIndex(5, 4),  PosIndex(6, 4),  PosIndex(6, 5),  PosIndex(6, 6),
    PosIndex(5, 6),  PosIndex(4, 6),  PosIndex(3, 6),  PosIndex(3, 5),  PosIndex(2, 5),
    PosIndex(1, 5),  PosIndex(1, 6),  PosIndex(1, 7),  PosIndex(1, 8),  PosIndex(2, 8),
    PosIndex(3, 8),  PosIndex(4, 8),  PosIndex(4, 9)
};

static std::vector<PosIndex> road2 = {
    PosIndex(2, 0),  PosIndex(2, 1),  PosIndex(1, 1),  PosIndex(1, 2),  PosIndex(1, 3),
    PosIndex(2, 3),  PosIndex(3, 3),  PosIndex(3, 2),  PosIndex(4, 2),  PosIndex(4, 1),
    PosIndex(5, 1),  PosIndex(6, 1),  PosIndex(6, 2),  PosIndex(6, 3),  PosIndex(7, 3),
    PosIndex(7, 4),  PosIndex(8, 4),  PosIndex(8, 5),  PosIndex(8, 6),  PosIndex(7, 6),
    PosIndex(6, 6),  PosIndex(6, 7),  PosIndex(6, 8),  PosIndex(7, 8),  PosIndex(7, 9)
};

static std::vector<PosIndex> road3 = {
    PosIndex(5, 0),  PosIndex(5, 1),  PosIndex(6, 1),  PosIndex(7, 1),  PosIndex(7, 2),
    PosIndex(7, 3),  PosIndex(6, 3),  PosIndex(5, 3),  PosIndex(4, 3),  PosIndex(3, 3),
    PosIndex(3, 2),  PosIndex(3, 1),  PosIndex(2, 1),  PosIndex(1, 1),  PosIndex(1, 2),
    PosIndex(1, 3),  PosIndex(1, 4),  PosIndex(1, 5),  PosIndex(2, 5),  PosIndex(3, 5),
    PosIndex(4, 5),  PosIndex(5, 5),  PosIndex(5, 6),  PosIndex(5, 7),  PosIndex(4, 7),
    PosIndex(3, 7),  PosIndex(2, 7),  PosIndex(2, 8),  PosIndex(2, 9)
};

static std::vector<PosIndex> road4 = {
    PosIndex(3, 0),  PosIndex(4, 0),  PosIndex(5, 0),  PosIndex(6, 0),  PosIndex(6, 1),
    PosIndex(6, 2),  PosIndex(5, 2),  PosIndex(4, 2),  PosIndex(3, 2),  PosIndex(3, 3),
    PosIndex(2, 3),  PosIndex(1, 3),  PosIndex(1, 4),  PosIndex(1, 5),  PosIndex(2, 5),
    PosIndex(3, 5),  PosIndex(4, 5),  PosIndex(5, 5),  PosIndex(6, 5),  PosIndex(6, 6),
    PosIndex(7, 6),  PosIndex(8, 6),  PosIndex(8, 7),  PosIndex(8, 8),  PosIndex(8, 9),
    PosIndex(7, 9),  PosIndex(6, 9),  PosIndex(5, 9)
};

#endif // __ROADS_H__
