#include "RestartLayer.h"

using namespace cocos2d;

RestartLayer::RestartLayer()
: _tipMsg("")
, _tipLabel(nullptr)
, _itemRestart(nullptr)
{
    
}

void RestartLayer::onEnter()
{
    LayerColor::onEnter();
    LayerColor::initWithColor(Color4B(0, 0, 0, 128));
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto visibleOrigin = Director::getInstance()->getVisibleOrigin();

    _tipLabel = Label::createWithSystemFont(_tipMsg, "system", 20);
    _tipLabel->setPosition(Vec2(visibleOrigin.x + visibleSize.width/2, visibleOrigin.y + visibleSize.height / 2 + 50));
    addChild(_tipLabel);
    
    _itemRestart = MenuItemFont::create("Retry", CC_CALLBACK_1(RestartLayer::onRetry, this));
    _itemRestart->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2 - 50));
    auto _menu = Menu::create(_itemRestart, NULL);
    _menu->setPosition(Vec2(0, 0));
    addChild(_menu);
    
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->onTouchBegan = CC_CALLBACK_2(RestartLayer::onTouchBegan, this);
    touchListener->onTouchEnded = CC_CALLBACK_2(RestartLayer::onTouchEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
}

void RestartLayer::setTipLabel(const std::string & tipMsg)
{
    _tipMsg = tipMsg;
    if (_tipLabel != nullptr) {
        _tipLabel->setString(_tipMsg);
    }
}

bool RestartLayer::onTouchBegan(cocos2d::Touch* touch, cocos2d::Event  *event)
{
    return true;
}

void RestartLayer::onTouchEnded(cocos2d::Touch* touch, cocos2d::Event  *event)
{
    
}

void RestartLayer::onRetry(Ref* sender)
{
    CCLOG("retry clicked");
    this->removeFromParentAndCleanup(true);
}
