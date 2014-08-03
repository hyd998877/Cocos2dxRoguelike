//
//  BattleScene.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/22.
//
//

#include "BattleScene.h"

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
    listener->setSwallowTouches(true);
    listener->onTouchBegan = CC_CALLBACK_2(BattleScene::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(BattleScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(BattleScene::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    // 背景配置
    auto bgBack = Sprite::create("bg/Grassland_back.png");
    bgBack->setPosition(Point(winSize / 2));
    this->addChild(bgBack);
    auto bgFront = Sprite::create("bg/Grassland_front.png");
    bgFront->setPosition(Point(winSize / 2));
    this->addChild(bgFront);
    
    // ActorSpriteを配置
    ActorDto actorDto = ActorDto(4);
    auto pActorSprite = ActorSprite::createWithActorDto(actorDto, 1);
    pActorSprite->setPosition(Point(winSize.width * 0.85, winSize.height / 2));
    this->addChild(pActorSprite, 102, 1);
    pActorSprite->runBottomAction();
    
    // 剣設定
    auto weapon = Sprite::create("icon_set/item_768.png");
    weapon->setPositionY(weapon->getContentSize().height);
    weapon->setVisible(false);
    pActorSprite->addChild(weapon, 1, 3);
    
    // 斬撃設定
    auto weaponEffect = Sprite::create("battle/animation_2.png");
//    weaponEffect->setPositionY(weaponEffect->getContentSize().height / 2);
    weaponEffect->setVisible(false);
    weapon->addChild(weaponEffect, 1, 4);
    
    // ActorSpriteを配置
    ActorDto enemyDto = ActorDto(5);
    auto enemySprite = ActorSprite::createWithActorDto(enemyDto, 1);
    enemySprite->setPosition(Point(winSize.width * 0.15, winSize.height / 2));
    this->addChild(enemySprite, 101, 2);
    enemySprite->setFlippedX(true);
    enemySprite->runBottomAction();
    
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
 
    Size winSize = Director::getInstance()->getWinSize();
    
    // 技名表示
    
    ActorSprite* actorSprite = dynamic_cast<ActorSprite*>(getChildByTag(1));
    
    ActorSprite* enemySprite = dynamic_cast<ActorSprite*>(getChildByTag(2));
    
    // 移動前の位置を覚えておく（戻るため）
    Point startPoint = actorSprite->getPosition();
    
    // -------------------
    const auto showAllWeaponCallFunc = CallFunc::create([this, actorSprite]() {
        // 剣表示
        auto weapon = actorSprite->getChildByTag(3);
        weapon->setVisible(true);
        
        // 斬撃エフェクト（右から左）
        auto weaponEffect = dynamic_cast<Sprite*>(weapon->getChildByTag(4));
        weaponEffect->setFlippedX(true);
        weaponEffect->setVisible(true);
    });
    
    const auto hideAllWeaponCallFunc = CallFunc::create([this, actorSprite]() {
        // 剣表示
        auto weapon = actorSprite->getChildByTag(3);
        weapon->setVisible(false);
        
        // 斬撃エフェクト（右から左）
        auto weaponEffect = dynamic_cast<Sprite*>(weapon->getChildByTag(4));
        weaponEffect->setFlippedX(true);
        weaponEffect->setVisible(false);
    });
    
    const auto showWeaponCallFunc = CallFunc::create([this, actorSprite]() {
        auto weapon = dynamic_cast<Sprite*>(actorSprite->getChildByTag(3));
        weapon->setFlippedX(false);
        weapon->setPositionX(0);
        weapon->setVisible(true);
    });
    const auto weaponFlipCallFunc = CallFunc::create([this, actorSprite]() {
        auto weapon = dynamic_cast<Sprite*>(actorSprite->getChildByTag(3));
        weapon->setFlippedX(true);
        weapon->setPositionX(weapon->getContentSize().width * 1.5);
        weapon->setVisible(true);
    });
    
    const auto actorFlipCallFunc = CallFunc::create([this, actorSprite]() {
        actorSprite->setFlippedX(!actorSprite->isFlippedX());
    });
    
    // プレイヤーアニメーション変更（攻撃）
    actorSprite->stopAllActions();
    actorSprite->changeSpriteFrame(3, "right");
    {
        // プレイヤー移動（敵の前）
        MoveTo* move1 = MoveTo::create(0.5f, Point(enemySprite->getPosition().x + enemySprite->getContentSize().width,
                                                   enemySprite->getPosition().y));
        auto seq1 = Sequence::create(move1, showAllWeaponCallFunc, NULL);
    
        // プレイヤーアニメーション変更（攻撃）：前と同じだから変更なし
        // プレイヤー移動（左上）
        MoveTo* move2 = MoveTo::create(0.1f, Point(winSize.width * 0.1, winSize.height * 0.9));
        auto seq2 = Sequence::create(DelayTime::create(0.2f),
                                     hideAllWeaponCallFunc,
                                     move2,
                                     actorFlipCallFunc,
                                     weaponFlipCallFunc,
                                     NULL);
        
        // プレイヤーアニメーション変更（攻撃）：前と同じだから変更なし
        // プレイヤー移動（右下）
        // 斬撃エフェクト（左から右）
        MoveTo* move3 = MoveTo::create(0.1f, Point(winSize.width * 0.3, winSize.height * 0.2));
        
        
        auto seq3_1 = Sequence::create(DelayTime::create(0.2f),
                                     move3,
                                     actorFlipCallFunc,
                                     showWeaponCallFunc,
                                     NULL);
        auto seq3_2 = Sequence::create(DelayTime::create(0.2f),
                                     showAllWeaponCallFunc,
                                     DelayTime::create(0.05f),
                                     hideAllWeaponCallFunc,
                                     NULL);
        auto spawn3 = Spawn::create(seq3_1, seq3_2, NULL);
        
        // プレイヤーアニメーション変更（攻撃）：前と同じだから変更なし
        // プレイヤー移動（右上）
        // 斬撃エフェクト（下から上）
        
        
        actorSprite->runAction(Sequence::create(seq1, seq2, spawn3, NULL));
    }


    
    // プレイヤーアニメーション変更（攻撃）：前と同じだから変更なし
        // プレイヤー移動（左下）
        // 斬撃エフェクト（上から下）
    
    // カットイン
    
    // プレイヤーアニメーション変更（回転攻撃）
        // プレイヤー移動（ちょっと左奥へ）
        // 斬撃エフェクト（クロス）
    
    // 敵アニメーション変更（被弾）
        // 画面フラッシュ
        // ダメージ表示
    
    // プレイヤーアニメーション変更（通常）
        // プレイヤー移動（初期位置）
    
    // 敵アニメーション変更（通常)
    
}
