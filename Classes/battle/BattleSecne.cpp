//
//  BattleSecne.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/22.
//
//

#include "BattleSecne.h"

#include "ActorSprite.h"

BattleScene::BattleScene()
{
    
}

BattleScene::~BattleScene()
{
    
}

Scene* BattleScene::scene()
{
    Scene *scene = Scene::create();
    BattleScene *layer = BattleScene::create();
    scene->addChild(layer);
    return scene;
}

bool BattleScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }
    Size winSize = Director::getInstance()->getWinSize();
    
    // TouchEvent settings
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    // Spriteを配置
    ActorSprite::ActorDto actorDto = ActorSprite::createDto();
    actorDto.playerId = 4;
    auto pActorSprite = ActorSprite::createWithActorDto(actorDto, 1);
    pActorSprite->setPosition(Point(winSize / 2));
    this->addChild(pActorSprite, 1, 1);
    
    pActorSprite->runBottomAction();
    
    // actorにタッチイベントを設定
    auto actorListener = EventListenerTouchOneByOne::create();
    actorListener->onTouchBegan = [pActorSprite](Touch* touch, Event* event) -> bool {
        CCLOG("%s : %s(%d)", "ActorSprite", "onTouchBegan", __LINE__);
        return true;
    };
    actorListener->onTouchMoved = [pActorSprite](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "ActorSprite", "onTouchMoved", __LINE__);
    };
    actorListener->onTouchEnded = [pActorSprite](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "ActorSprite", "onTouchEnded", __LINE__);
    };
    pActorSprite->getEventDispatcher()->addEventListenerWithSceneGraphPriority(actorListener, pActorSprite);
    
    // 武器をプレイヤーにadd
    auto pWeaponSprite = Sprite::create("icon_set/item_768.png");
    pActorSprite->addChild(pWeaponSprite);
    // 武器にタッチイベントを設定
    auto weaponListener = EventListenerTouchOneByOne::create();
    weaponListener->onTouchBegan = [pWeaponSprite](Touch* touch, Event* event) -> bool {
        CCLOG("%s : %s(%d)", "WeaponSprite", "onTouchBegan", __LINE__);
        return true;
    };
    weaponListener->onTouchMoved = [pWeaponSprite](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "WeaponSprite", "onTouchMoved", __LINE__);
    };
    weaponListener->onTouchEnded = [pWeaponSprite](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "WeaponSprite", "onTouchEnded", __LINE__);
    };
    pWeaponSprite->getEventDispatcher()->addEventListenerWithSceneGraphPriority(weaponListener, pWeaponSprite);
    
    // 青の半透明レイヤーを追加
    auto pLayer = LayerColor::create(Color4B::BLUE);
    pLayer->setOpacity(128);
    pLayer->setContentSize(winSize);
    this->addChild(pLayer, 2, 2);
    // 武器にタッチイベントを設定
    auto layerListener = EventListenerTouchOneByOne::create();
    layerListener->onTouchBegan = [pLayer](Touch* touch, Event* event) -> bool {
        CCLOG("%s : %s(%d)", "LayerColor", "onTouchBegan", __LINE__);
        return false;
    };
    layerListener->onTouchMoved = [pLayer](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "LayerColor", "onTouchMoved", __LINE__);
    };
    layerListener->onTouchEnded = [pLayer](Touch* touch, Event* event) {
        CCLOG("%s : %s(%d)", "LayerColor", "onTouchEnded", __LINE__);
    };
    pLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(layerListener, pLayer);
    
    return true;
}


bool BattleScene::onTouchBegan(Touch *touch, Event *unused_event)
{
    CCLOG("%s : %s(%d)", "BattleScene", __FUNCTION__, __LINE__);
    
    return true;
}

void BattleScene::onTouchMoved(Touch *touch, Event *unused_event)
{
    CCLOG("%s : %s(%d)", "BattleScene", __FUNCTION__, __LINE__);
    
}

void BattleScene::onTouchEnded(Touch *touch, Event *unused_event)
{
    CCLOG("%s : %s(%d)", "BattleScene", __FUNCTION__, __LINE__);
    
}
