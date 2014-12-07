//
//  MyMenu.cpp
//  cocos2d_tests
//
//  Created by minggo on 12/7/14.
//
//

#include "MyMenu.h"

using namespace cocos2d;

void MyMenu::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event)
{
    Menu::onTouchEnded(touch, event);
    
    struct timeval time;
    gettimeofday(&time, nullptr);
    
    int diff = (time.tv_sec * 1000 + time.tv_usec / 1000) - _duration;
    
    // duration is 2 sends, and error is less than 200 ms
    if (abs(diff - 2000) < 200)
    {
        log("hit 2 seconds");
    }
    else
    {
        label->setVisible(true);
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
    label = Label::createWithSystemFont("less or longer than 2 seconds", "system", 20);
    label->setPosition(Vec2(500, 50));
    label->retain();
    label->setVisible(false);
    
    auto function = [this](float dt) { this->label->setVisible(false); };
    //schedule(const ccSchedulerFunc& callback, void *target, float interval, bool paused, const std::string& key);
    Director::getInstance()->getScheduler()->schedule(function, this, 5, false, "show label");
    
    _duration = 0;
}

