#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "MapLayer.h"

class HelloWorld : public cocos2d::Layer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* scene();
    
    void cleanMapLayer();
    void resetMapLayer();
    
    void registeKeyEvent();
    void onKeyReleased(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event  *event);
    void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event  *event);

    void countDown();
    void hideMapLayer();
    
    void playerGoUp();
    void playerGoDown();
    void playerGoLeft();
    void playerGoRight();
    void updatePlayerPos(cocos2d::Vec2 newPosition, PosIndex newPosIndex);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
    
public:
    MapLayer* _mapLayer;
    cocos2d::Sprite* _player;
    PosIndex _playerPosIndex;
    cocos2d::Vec2 _playerPosition;
    cocos2d::Sprite* _boss;
    PosIndex _bossPosIndex;
    int _rowSize, _colSize;
    float _boxsize;
    cocos2d::Vec2 _origin;
    
    cocos2d::Vec2 _touchBegin;
    cocos2d::Vec2 _touchEnded;
    
    cocos2d::Sprite* _baseMap;
    
    cocos2d::DrawNode* _grid;
    
    int _time;
    bool _cantouch;
};

#endif // __HELLOWORLD_SCENE_H__
