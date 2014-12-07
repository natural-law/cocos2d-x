#ifndef __MAP_LAYER_H__
#define __MAP_LAYER_H__

#include "cocos2d.h"
#include "base/CCVector.h"
#include <vector>

#define MAX_ROW_COUNT     10
#define MAX_COLUMN_COUNT  10

#define BLOCK_SIZE        50

#define RANDOM_ROAD_PERCENT     25

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

    inline void setShownForever(bool showForever) { _bShownForever = showForever; }
    inline bool isShownForever() { return _bShownForever; }

private:
    BlockType _type;
    bool _bShownForever;
};

typedef cocos2d::Vector<BlockSprite*> ColumnData;
struct PosIndex
{
    int rowIdx;
    int columnIdx;
    
    PosIndex():rowIdx(0), columnIdx(0) {}
    
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
    bool isRoadPos(PosIndex idx);
    
    PosIndex& getBeginPos();
    PosIndex& getEndPos();

private:
    void generateRoad();
    void randomRoad();

    std::vector<ColumnData*> _data;
    
    PosIndex _beginPos;
    PosIndex _endPos;
};

class MapLayer : public cocos2d::LayerColor
{
public:
    MapLayer();
    virtual ~MapLayer();

    void hideAllBlocks();
    void showBlock(PosIndex pos, bool bShowForever = false);
    bool isRoadPos(PosIndex idx);
    
    MapData* getMapData();

private:
    MapData* _map;
};

#endif // __MAP_LAYER_H__
