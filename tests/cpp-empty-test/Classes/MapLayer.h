#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__

#include "cocos2d.h"
#include "base/CCVector.h"
#include <vector>

typedef enum
{
    BlockTypeNull = 0,

    BlockTypeRoad,
    BlockTypeWall
} BlockType;

class BlockSprite : public cocos2d::Sprite
{
public:
    BlockSprite(BlockType type);
    virtual ~BlockSprite();
    
    void setType(BlockType type);
    inline BlockType getType() { return _type; }

private:
    BlockType _type;
};

typedef cocos2d::Vector<BlockSprite*> ColumnData;
struct PosIndex
{
    int rowIdx;
    int columnIdx;
    
    PosIndex(int row, int column)
    {
        rowIdx = row;
        columnIdx = column;
    }
};

class MapData
{
public:
    MapData();
    virtual ~MapData();
    
    BlockSprite* getBlockByIdx(const PosIndex & pos);
    
private:
    void generateRoad();
    void randomRoad();
    bool isRoadPos(PosIndex idx);

    std::vector<ColumnData*> _data;
};

class MapLayer : public cocos2d::LayerColor
{
public:
    MapLayer();
    virtual ~MapLayer();

private:
    MapData* _map;
};

#endif // __MAP_LAYER_H__
