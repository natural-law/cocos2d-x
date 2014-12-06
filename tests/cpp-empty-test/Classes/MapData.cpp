#include "MapData.h"

USING_NS_CC;

MapData::MapData
: int _row(0)
, int _col(0)
, float _width(0)
, float _height(0)
, _data(nullptr)
{
}

MapData* MapData::generate(const Vec2& sreenSize)
{
    MapData* ret = new(std::nothrow) MapData();
    return ret;
}