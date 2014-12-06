#ifndef __DISPLAY_MAP_H__
#define __DISPLAY_MAP_H__

#include "cocos2d.h"
//#include "MapData.h"

enum class ShowType
{
    ALL,
    PART
};

struct Range
{
    
};

class MapData;

class DisplayMap
{
public:
    DisplayMap(cocos2d::Layer* map);
    
    void show(ShowType showType, Range range, float time);
    
    cocos2d::Layer* getMap();
    
private:
    cocos2d::Layer* _map;
};

#endif // __DISPLAY_MAP_H__
