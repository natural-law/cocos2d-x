#include "HelloWorldScene.h"
#include "AppMacros.h"
#include "math.h"
#include "MyMenu.h"

USING_NS_CC;


Scene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
 
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    auto s = Director::getInstance()->getWinSize();
    
    _cantouch = false;
    
    _baseMap = cocos2d::Sprite::create("basemap.jpg");
    auto baseMapSize = _baseMap->getContentSize();
    
    _baseMap->setScale(0.5 , 0.5);
    
    _baseMap->setPosition(Vec2(visibleSize / 2) + origin);
    addChild(_baseMap, -2);
    
    auto timeMenu = MyMenu::createMenu("press me 2 seconds", Vec2(visibleOrigin.x + 200, 50));
    addChild(timeMenu);
    addChild(timeMenu->label);
    
    
    // position the sprite on the center of the screen
    //sprite->setPosition(Vec2(visibleSize / 2) + origin);

    // add the sprite as a child to this layer
    //this->addChild(sprite);
    //auto mapData = MapData::generate(Vec2(640, 960));
    //cocos2d::Layer* map = cocos2d::Layer::create();
    //auto displayMap = new DisplayMap(map);
    
    //addChild(displayMap->getMap());
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
    listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    _mapLayer = new MapLayer();//MapLayer::create();
    addChild(_mapLayer, -1);

    _rowSize = MAX_ROW_COUNT;
    _colSize = MAX_COLUMN_COUNT;
    _boxsize = BLOCK_SIZE;
    _origin = origin + cocos2d::Vec2((visibleSize.width - _colSize * _boxsize)/2, visibleSize.height - _rowSize * _boxsize - 30);
    _mapLayer->setPosition(_origin);
    
    _player = Sprite::create("grossini.png");
    _player->setAnchorPoint(cocos2d::Vec2(0, 0));
    auto playerSize = _player->getContentSize();
    _player->setScale(_boxsize/playerSize.width, _boxsize/playerSize.height);
    _playerPosIndex = _mapLayer->getMapData()->getBeginPos();
    _playerPosition = _origin + Vec2(_playerPosIndex.columnIdx * _boxsize, _playerPosIndex.rowIdx * _boxsize);
    _player->setPosition(_playerPosition);
    addChild(_player);
    
    _boss = Sprite::create("grossinis_sister1.png");
    _boss->setAnchorPoint(cocos2d::Vec2(0, 0));
    auto bossSize = _boss->getContentSize();
    _boss->setScale(_boxsize/bossSize.width, _boxsize/bossSize.height);
    _bossPosIndex = _mapLayer->getMapData()->getEndPos();
    _boss->setPosition(_origin+Vec2(_bossPosIndex.columnIdx * _boxsize, _bossPosIndex.rowIdx * _boxsize));
    addChild(_boss);
    
    auto color = Color4F::GREEN;
    _grid = cocos2d::DrawNode::create();
    for(int i = 0; i <= _rowSize; i++)
    {
        _grid->drawLine(_origin+Vec2(0, i * _boxsize), _origin+Vec2(_colSize * _boxsize, i * _boxsize), color);
    }
    
    for(int j = 0; j <= _colSize; j++)
    {
        _grid->drawLine(_origin+Vec2(0 + j * _boxsize, 0), _origin+Vec2(0 + j * _boxsize, _rowSize*_boxsize), color);
    }
    
    addChild(_grid, 20);
    
//    runAction( Sequence::create(
//                                /*CallFunc::create( CC_CALLBACK_0(HelloWorld::countDown,this)),*/
//                                DelayTime::create(5.0f),
//                                CallFunc::create( CC_CALLBACK_0(HelloWorld::hideMapLayer,this)),
//                                nullptr)
//              );
    //auto label1 = Label::createWithBMFont("bitmapFontTest2.fnt", "5");
    auto label1 = LabelTTF::create("Power by cocos2d-x", "arial.ttf", 70);
    label1->setAnchorPoint( Vec2(0.5,0.5) );
    label1->setPosition(Vec2(s.width/2, s.height/2));
    addChild(label1, 0, 1);
    auto fade = FadeOut::create(1.0f);
    auto fade_in = fade->reverse();
    auto seq = Sequence::create(fade, fade_in, nullptr);
    auto repeat = RepeatForever::create(seq);
    label1->runAction(repeat);
    _time = 5;
    schedule(CC_CALLBACK_0(HelloWorld::countDown, this), 1, 5, 1, " ");
    
    _mapLayer->showBlock(_playerPosIndex, true);
    _mapLayer->showBlock(_bossPosIndex, true);
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC || CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
    registeKeyEvent();
#endif

    return true;
}

void HelloWorld::countDown()
{
    char string[5] = {0};
    sprintf(string, "%d", _time);
    
    auto label1 = (LabelTTF*) getChildByTag(1);
    label1->setString(string);
    
    if(_time == 0)
    {
        removeChild(label1);
        hideMapLayer();
        _cantouch = true;
    }
    
    _time = _time - 1;
}

void HelloWorld::hideMapLayer()
{
    _mapLayer->hideAllBlocks();
}

void HelloWorld::registeKeyEvent()
{
    auto listener = EventListenerKeyboard::create();
    listener->onKeyReleased = CC_CALLBACK_2(HelloWorld::onKeyReleased, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
    if(_cantouch)
    {
        switch(keyCode)
        {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                playerGoLeft();
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                playerGoRight();
                break;
            case EventKeyboard::KeyCode::KEY_UP_ARROW:
                playerGoUp();
                break;
            case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
                playerGoDown();
                break;
            default:
                break;
        }
    }
}

void HelloWorld::playerGoUp()
{
    Vec2 newPosition(_playerPosition);
    PosIndex newPosIndex(_playerPosIndex.rowIdx, _playerPosIndex.columnIdx);
    newPosition = _playerPosition + Vec2(0, _boxsize);
    newPosIndex.rowIdx = _playerPosIndex.rowIdx + 1;
    newPosIndex.columnIdx = _playerPosIndex.columnIdx;
    updatePlayerPos(newPosition, newPosIndex);
}

void HelloWorld::playerGoDown()
{
    Vec2 newPosition(_playerPosition);
    PosIndex newPosIndex(_playerPosIndex.rowIdx, _playerPosIndex.columnIdx);
    newPosition = _playerPosition + Vec2(0, -_boxsize);
    newPosIndex.rowIdx = _playerPosIndex.rowIdx - 1;
    newPosIndex.columnIdx = _playerPosIndex.columnIdx;
    updatePlayerPos(newPosition, newPosIndex);
}

void HelloWorld::playerGoLeft()
{
    Vec2 newPosition(_playerPosition);
    PosIndex newPosIndex(_playerPosIndex.rowIdx, _playerPosIndex.columnIdx);
    newPosition = _playerPosition + Vec2(-_boxsize, 0);
    newPosIndex.rowIdx = _playerPosIndex.rowIdx;
    newPosIndex.columnIdx = _playerPosIndex.columnIdx - 1;
    updatePlayerPos(newPosition, newPosIndex);
}

void HelloWorld::playerGoRight()
{
    Vec2 newPosition(_playerPosition);
    PosIndex newPosIndex(_playerPosIndex.rowIdx, _playerPosIndex.columnIdx);
    newPosition = _playerPosition + Vec2(_boxsize, 0);
    newPosIndex.rowIdx = _playerPosIndex.rowIdx;
    newPosIndex.columnIdx = _playerPosIndex.columnIdx + 1;
    updatePlayerPos(newPosition, newPosIndex);
}

void HelloWorld::updatePlayerPos(cocos2d::Vec2 newPosition, PosIndex newPosIndex)
{
    if(newPosIndex.rowIdx < 0 || newPosIndex.columnIdx < 0 || newPosIndex.rowIdx >= _rowSize || newPosIndex.columnIdx >= _colSize)
    {
    }
    else if(_mapLayer->isRoadPos(newPosIndex))
    {
        _mapLayer->showBlock(newPosIndex, true);
        _playerPosition = newPosition;
        _playerPosIndex.columnIdx = newPosIndex.columnIdx;
        _playerPosIndex.rowIdx = newPosIndex.rowIdx;
        _player->setPosition(newPosition);
    }
}

bool HelloWorld::onTouchBegan(Touch* touch, Event  *event)
{
    if(_cantouch)
    {
        _touchBegin = touch->getLocation();
    }
    return false;
}

/*void transformPosition(Vec2& origin, float boxsize, Vec2& from, PosIndex& to)
{
    Vec2 diff = from - origin;
    to.rowIdx = diff.y/boxsize;
    to.columnIdx = diff.x/boxsize;
}*/

void HelloWorld::onTouchEnded(Touch* touch, Event  *event)
{
    if(_cantouch)
    {
        _touchEnded = touch->getLocation();
        
        
        if((_touchEnded.x - _touchBegin.x > 50) && (fabs(_touchEnded.y - _touchBegin.y) < 40))
        {
            // right
            playerGoRight();
        }
        else if((_touchEnded.y - _touchBegin.y > 50) && (fabs(_touchEnded.x - _touchBegin.x) < 40))
        {
            // up
            playerGoUp();
        }
        else if((_touchBegin.y - _touchEnded.y > 50) && (fabs(_touchBegin.x - _touchEnded.x) < 40))
        {
            // down
            playerGoDown();
        }
        else if((_touchBegin.x - _touchEnded.x > 50) && (fabs(_touchBegin.y - _touchEnded.y) < 40))
        {
            // left
            playerGoLeft();
        }
    }
}
