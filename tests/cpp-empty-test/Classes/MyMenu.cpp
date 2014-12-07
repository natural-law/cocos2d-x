//
//  MyMenu.cpp
//  cocos2d_tests
//
//  Created by minggo on 12/7/14.
//
//

#include "MyMenu.h"
#include "HelloWorldScene.h"
#include "MapLayer.h"

using namespace cocos2d;

void MyMenu::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Menu::onTouchEnded(touch, event);
    
    struct timeval time;
    gettimeofday(&time, nullptr);
    
    int diff = (time.tv_sec * 1000 + time.tv_usec / 1000) - _duration;
    
    // duration is 2 sends, and error is less than 200 ms
    if (abs(diff - 2000) < 400)
    {
        log("hit 2 seconds");
    
        HelloWorld* parent = static_cast<HelloWorld*>(getParent());
        int column = parent->_playerPosIndex.columnIdx;
        int row = parent->_playerPosIndex.rowIdx;
        showAroundPlayer(column, row);
    }
    else
    {
        char str[20];
        sprintf(str, "%f s", (float)diff / 1000);
        label->setString(str);
        label->runAction(Sequence::create(Show::create(), DelayTime::create(2), Hide::create(), nullptr));
    }
}

bool MyMenu::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event)
{
    bool result = Menu::onTouchBegan(touch, event);
    
    struct timeval time;
    gettimeofday(&time, nullptr);
    _duration = time.tv_sec * 1000 + time.tv_usec / 1000;
    
    return result;
}

MyMenu* MyMenu::createMenu(const std::string &title, const cocos2d::Vec2& pos)
{
    auto menu = MyMenu::create();
    
    auto menuItemFont = MenuItemFont::create(title, nullptr);
    menu->addChild(menuItemFont);
    menu->setPosition(pos);
    
    return menu;
}

MyMenu::MyMenu()
{
    label = Label::createWithSystemFont("", "system", 20);
    label->setPosition(Vec2(500, 50));
    label->retain();
    label->setVisible(false);
    
    _duration = 0;
}

void MyMenu::showAroundPlayer(int column, int row)
{
    
    
    auto showFunc = [this, column, row](){
        auto upPos = PosIndex(row+1, column);
        auto leftUpPos = PosIndex(row+1, column-1);
        auto rightUpPos = PosIndex(row+1, column+1);
        
        auto leftPos = PosIndex(row, column-1);
        auto rightPos = PosIndex(row, column+1);
        
        auto bottomPos = PosIndex(row -1, column);
        auto bottomLeftPos = PosIndex(row-1, column-1);
        auto bottomRightPos = PosIndex(row-1, column+1);
        
        HelloWorld* parent = static_cast<HelloWorld*>(getParent());
        parent->_mapLayer->showBlock(upPos);
        parent->_mapLayer->showBlock(leftUpPos);
        parent->_mapLayer->showBlock(rightUpPos);
        parent->_mapLayer->showBlock(leftPos);
        parent->_mapLayer->showBlock(rightPos);
        parent->_mapLayer->showBlock(bottomPos);
        parent->_mapLayer->showBlock(bottomLeftPos);
        parent->_mapLayer->showBlock(bottomRightPos);
        
    };
    
    auto hideFunc = [this]() {
        HelloWorld* parent = static_cast<HelloWorld*>(getParent());
        parent->_mapLayer->hideAllBlocks();
    };
    
    auto show = CallFunc::create(showFunc);
    auto hide = CallFunc::create(hideFunc);
    auto sequence = Sequence::create(show, DelayTime::create(2), hide, nullptr);
    runAction(sequence);
}

