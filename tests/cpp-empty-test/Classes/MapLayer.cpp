#include "MapLayer.h"

#define MAX_ROW_COUNT     10
#define MAX_COLUMN_COUNT  10

#define BLOCK_SIZE        40
#define POS_INDEX_SIZE    2

#define RANDOM_ROAD_PERCENT     25

static int road1[][POS_INDEX_SIZE] = {
    8, 0,
    8, 1,
    9, 1,
    9, 2,
    9, 3,
    8, 3,
    7, 3,
    7, 2,
    6, 2,
    5, 2,
    5, 3,
    5, 4,
    6, 4,
    6, 5,
    6, 6,
    5, 6,
    4, 6,
    3, 6,
    3, 5,
    2, 5,
    1, 5,
    1, 6,
    1, 7,
    1, 8,
    2, 8,
    3, 8,
    4, 8,
    4, 9
};

using namespace cocos2d;

/////////////////////////////////
//
// BlockSprite
//
/////////////////////////////////
BlockSprite::BlockSprite(BlockType type)
{
    Sprite::init();
    this->setType(type);
    this->setAnchorPoint(Vec2(0,0));
}

BlockSprite::~BlockSprite()
{
    
}

void BlockSprite::setType(BlockType type)
{
    switch (type) {
        case BlockTypeNull:
            break;
        case BlockTypeRoad:
            this->setTexture("block_road.jpg");
            break;
        case BlockTypeWall:
            this->setTexture("block_wall.jpg");
            break;
        default:
            break;
    }
    
    _type = type;
}

/////////////////////////////////
//
// Map
//
/////////////////////////////////
MapData::MapData()
{
    for (int i = 0; i < MAX_ROW_COUNT; i++) {
        ColumnData* column = new Vector<BlockSprite*>();
        _data.push_back(column);
        for (int j = 0; j < MAX_COLUMN_COUNT; j++) {
            BlockSprite* block = new BlockSprite(BlockTypeWall);
            column->pushBack(block);
        }
    }

    this->generateRoad();
    this->randomRoad();
}

MapData::~MapData()
{
    for (int i = 0; i < MAX_ROW_COUNT; i++) {
        ColumnData* column = _data.at(i);
        for (int j = 0; j < MAX_COLUMN_COUNT; j++) {
            BlockSprite* block = this->getBlockByIdx(PosIndex(i, j));
            block->release();
        }
        CC_SAFE_DELETE(column);
    }
}

void MapData::generateRoad()
{
    int roadStepCount = sizeof(road1) / POS_INDEX_SIZE / sizeof(int);
    for (int i = 0; i < roadStepCount; i++)
    {
        int rowIdx = road1[i][0];
        int columnIdx = road1[i][1];
        PosIndex pos(rowIdx, columnIdx);
        BlockSprite* block = this->getBlockByIdx(pos);
        block->setType(BlockTypeRoad);
    }
}

void MapData::randomRoad()
{
    for (int i = 0; i < MAX_ROW_COUNT; i++) {
        for (int j = 0; j < MAX_COLUMN_COUNT; j++) {
            PosIndex curPos(i, j);
            if (this->isRoadPos(curPos))
            {
                continue;
            }
            
            int randValue = RandomHelper::random_int(1, 100);
            if (randValue <= RANDOM_ROAD_PERCENT)
            {
                auto block = this->getBlockByIdx(curPos);
                block->setType(BlockTypeRoad);
            }
        }
    }
}

bool MapData::isRoadPos(PosIndex idx)
{
    auto block = this->getBlockByIdx(idx);
    if (block != nullptr && block->getType() == BlockTypeRoad)
    {
        return true;
    }
    else
    {
        return false;
    }
}

BlockSprite* MapData::getBlockByIdx(const PosIndex & pos)
{
    ColumnData* col = _data.at(pos.columnIdx);
    BlockSprite* block = col->at(pos.rowIdx);
    
    return block;
}

/////////////////////////////////
//
// MapLayer
//
/////////////////////////////////
MapLayer::MapLayer()
{
    LayerColor::initWithColor(Color4B(255, 0, 0, 0));
    _map = new MapData();
    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        for (int j = 0; j < MAX_COLUMN_COUNT; j++)
        {
            BlockSprite* block = _map->getBlockByIdx(PosIndex(i, j));
            auto blockPos = Vec2(j * BLOCK_SIZE, i * BLOCK_SIZE);
            block->setPosition(blockPos);
            this->addChild(block);
        }
    }
}

void MapLayer::hideAllBlocks()
{
    for (int i = 0; i < MAX_ROW_COUNT; i++)
    {
        for (int j = 0; j < MAX_COLUMN_COUNT; j++)
        {
            BlockSprite* block = _map->getBlockByIdx(PosIndex(i, j));
            block->setVisible(false);
        }
    }
}

void MapLayer::showBlock(PosIndex pos)
{
    auto block = _map->getBlockByIdx(pos);
    block->setVisible(true);
}

bool MapLayer::isRoadPos(PosIndex idx)
{
    return _map->isRoadPos(idx);
}

MapLayer::~MapLayer()
{
    CC_SAFE_DELETE(_map);
}
