#ifndef __MAP_DATA_H__
#define __MAP_DATA_H__

#include "cocos2d.h"

enum class ObjectType
{
    GREEN,
    RED,
    TREE,
    GRASS,
    Boss,
    Player
};

class MapData
{
public:
    //static MapData* generate(const cocos2d::Vec2& sreenSize);
    
private:
    int _row;
    int _col;
    float _width;
    float _height;
    
    ObjectType** _data;
};

#endif // __MAP_DATA_H__
