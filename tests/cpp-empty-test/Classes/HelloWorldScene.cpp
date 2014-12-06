#include "HelloWorldScene.h"
#include "DisplayMap.h"
#include "AppMacros.h"
#include "math.h"
#include "MapLayer.h"

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
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    CCLOG("visiblesize: %f, %f", visibleSize.width, visibleSize.height);
    CCLOG("originsize: %f, %f", origin.x, origin.y);
    

    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        CC_CALLBACK_1(HelloWorld::menuCloseCallback,this));
    
    closeItem->setPosition(origin + Vec2(visibleSize) - Vec2(closeItem->getContentSize() / 2));

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
    
    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    auto label = LabelTTF::create("Hello World", "Arial", TITLE_FONT_SIZE);
    
    // position the label on the center of the screen
    label->setPosition(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - label->getContentSize().height);

    // add the label as a child to this layer
    this->addChild(label, 1);

    // add "HelloWorld" splash screen"
    //auto sprite = Sprite::create("HelloWorld.png");

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

    _rowSize = 10;
    _colSize = 10;
    _boxsize = 30;
    _origin = cocos2d::Vec2(80, 10);
    _player = Sprite::create("grossini.png");
    _player->setAnchorPoint(cocos2d::Vec2(1, 0));
    auto playerSize = _player->getContentSize();
    _player->setScale(_boxsize/playerSize.width, _boxsize/playerSize.height);
    _player->setPosition(_origin+Vec2(_boxsize, 0));
    _boss = Sprite::create("grossinis_sister1.png");
    _boss->setAnchorPoint(cocos2d::Vec2(1, 0));
    _boss->setPosition(_origin+Vec2(_colSize*_boxsize, (_rowSize-1)*_boxsize));
    auto bossSize = _boss->getContentSize();
    _boss->setScale(_boxsize/bossSize.width, _boxsize/bossSize.height);
    
    _baseMap = cocos2d::Sprite::create("basemap.jpg");
    auto baseMapSize = _baseMap->getContentSize();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_MAC)
    _baseMap->setScale(0.5 , 0.5);
#endif
    _baseMap->setPosition(Vec2(visibleSize / 2) + origin);
    addChild(_baseMap);
    
    addChild(_player);
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
    
    addChild(_grid);
    
    auto maplayer = new MapLayer();//MapLayer::create();
    addChild(maplayer, 10);
    maplayer->setPosition(_origin);
    
    
    return true;
}

bool HelloWorld::onTouchBegan(Touch* touch, Event  *event)
{
    _touchBegin = touch->getLocation();
    return true;
}

//transform Touch position to fit box
/*cocos2d::Vec2& transformTouchLocation(cocos2d::Vec2& location)
{
    return location;
}*/

void HelloWorld::onTouchEnded(Touch* touch, Event  *event)
{
    _touchEnded = touch->getLocation();
    
    /*if((fabs(location.x - _player->getPosition().x) <= _boxsize*5) && (fabs(location.y - _player->getPosition().y) <= _boxsize*5))
    {
        if(1 == 1)
        {
            _player->setPosition(location);
        }
        else
        {
            
        }
    }*/
    auto position = _player->getPosition();
    if((_touchEnded.x - _touchBegin.x > 30) && (fabs(_touchEnded.y - _touchBegin.y) < 25))
    {
        _player->setPosition(position + Vec2(_boxsize, 0));
    }
    else if((_touchEnded.y - _touchBegin.y > 30) && (fabs(_touchEnded.x - _touchBegin.x) < 25))
    {
        _player->setPosition(position + Vec2(0, _boxsize));
    }
    else if((_touchBegin.y - _touchEnded.y > 30) && (fabs(_touchBegin.x - _touchEnded.x) < 25))
    {
        _player->setPosition(position + Vec2(0, -_boxsize));
    }
    else if((_touchBegin.x - _touchEnded.x > 30) && (fabs(_touchBegin.y - _touchEnded.y) < 25))
    {
        _player->setPosition(position + Vec2(-_boxsize, 0));
    }
}

void HelloWorld::menuCloseCallback(Ref* sender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif

    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}
