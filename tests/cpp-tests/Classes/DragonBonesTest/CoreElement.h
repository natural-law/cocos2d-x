#ifndef __CORE_ELEMENT_H__
#define __CORE_ELEMENT_H__

#include "cocos2d.h"
#include "dragonBones/cocos2dx/CCDragonBonesHeaders.h"

class CoreElementBullet
{
private:
    float _speedX;
    float _speedY;

    dragonBones::Armature* _armature;
    dragonBones::CCArmatureDisplay* _armatureDisplay;
    dragonBones::Armature* _effect;

public:
    CoreElementBullet(const std::string& armatureName, const std::string& effectArmatureName, float radian, float speed, const cocos2d::Vec2& position);
    ~CoreElementBullet();

    bool update();
};

class Mecha
{
public:
    static const std::string NORMAL_ANIMATION_GROUP;
    static const std::string AIM_ANIMATION_GROUP;
    static const std::string ATTACK_ANIMATION_GROUP;
    static const float JUMP_SPEED;
    static const float NORMALIZE_MOVE_SPEED;
    static const float MAX_MOVE_SPEED_FRONT;
    static const float MAX_MOVE_SPEED_BACK;
    static const char* WEAPON_R_LIST[6];
    static const char* WEAPON_L_LIST[5];

private:
    bool _isJumpingA;
    bool _isJumpingB;
    bool _isSquating;
    bool _isAttackingA;
    bool _isAttackingB;
    unsigned _weaponRIndex;
    unsigned _weaponLIndex;
    int _faceDir;
    int _aimDir;
    int _moveDir;
    float _aimRadian;
    float _speedX;
    float _speedY;
    dragonBones::Armature* _armature;
    dragonBones::CCArmatureDisplay* _armatureDisplay;
    dragonBones::Armature* _weaponR;
    dragonBones::Armature* _weaponL;
    dragonBones::AnimationState* _aimState;
    dragonBones::AnimationState* _walkState;
    dragonBones::AnimationState* _attackState;
    cocos2d::Vec2 _target;

public:
    Mecha();
    ~Mecha();

    void update();
    void move(int dir);
    void jump();
    void squat(bool isSquating);
    void attack(bool isAttacking);
    void switchWeaponR();
    void switchWeaponL();
    void aim(const cocos2d::Vec2& target);

private:
    void _animationEventHandler(cocos2d::EventCustom* event);
    void _frameEventHandler(cocos2d::EventCustom* event);
    void _fire(const cocos2d::Vec2& firePoint);
    void _updateAnimation();
    void _updatePosition();
    void _updateAim();
    void _updateAttack();
};

class CoreElementGame : public cocos2d::LayerColor
{
public:
    virtual ~CoreElementGame();
    static cocos2d::Scene* createScene();

    virtual bool init() override;
    virtual void onExit() override;

    CREATE_FUNC(CoreElementGame);

public:
    static const int GROUND;
    static const float G;
    static CoreElementGame* instance;

public:
    dragonBones::CCFactory factory;

    void addBullet(CoreElementBullet* bullet);

private:
    bool _left;
    bool _right;
    Mecha* _player;
    std::vector<CoreElementBullet*> _bullets;

    void _enterFrameHandler(float passedTime);
    void _keyBoardPressedHandler(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void _keyBoardReleasedHandler(cocos2d::EventKeyboard::KeyCode keyCode, cocos2d::Event* event);
    void _mouseDownHandler(cocos2d::EventMouse* event);
    void _mouseUpHandler(cocos2d::EventMouse* event);
    void _mouseMovedHandler(cocos2d::EventMouse* event);
    void _updateMove(int dir) const;
};

#endif // __CORE_ELEMENT_H__