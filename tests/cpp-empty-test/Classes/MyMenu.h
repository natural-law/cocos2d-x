//
//  MyMenu.h
//  cocos2d_tests
//
//  Created by minggo on 12/7/14.
//
//

#include "cocos2d.h"

#ifndef __cocos2d_tests__MyMenu__
#define __cocos2d_tests__MyMenu__

class MyMenu : public cocos2d::Menu
{
public:
    
    static MyMenu* createMenu(const std::string& title, const cocos2d::Vec2& pos);
    CREATE_FUNC(MyMenu);
    
    MyMenu();
    
    virtual void onTouchEnded(cocos2d::Touch* touch, cocos2d::Event* event) override;
    virtual bool onTouchBegan(cocos2d::Touch* touch, cocos2d::Event* event) override;
    
    int _duration;
    
    cocos2d::Label *label;
};

#endif /* defined(__cocos2d_tests__MyMenu__) */
