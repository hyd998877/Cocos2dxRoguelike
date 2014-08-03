//
//  ActorSprite.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/05.
//
//

#include "ActorSprite.h"

ActorSprite::ActorSprite()
{
    
}

ActorSprite::~ActorSprite()
{
    
}

bool ActorSprite::initWithActorDto(ActorDto actorDto)
{
    return initWithActorDto(actorDto, 0);
}

bool ActorSprite::initWithActorDto(ActorDto actorDto, int typeId)
{
    m_actorDto = actorDto;
    m_nowTypeId = typeId;
    
    // ActorのSpriteFrameのplistをキャッシュ
    auto spriteFramePlistName = cocos2d::StringUtils::format("actor/%d/actor_%d_%d.plist", m_actorDto.getPlayerId(), m_actorDto.getPlayerId(), typeId);
    CCLOG("initWithActorDto = %s", spriteFramePlistName.c_str());
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteFramePlistName);
    
    // Spriteを生成
    auto spriteFrameName = cocos2d::StringUtils::format("actor_%d_%d_%s_%d.png", m_actorDto.getPlayerId(), typeId, "bottom", 1);
    if ( !Sprite::initWithSpriteFrameName(spriteFrameName) )
    {
        return false;
    }
    
    return true;
}

void ActorSprite::changeSpriteFrame(int typeId)
{
    changeSpriteFrame(typeId, "bottom");
}

void ActorSprite::changeSpriteFrame(int typeId, std::string frameName)
{
    if (m_nowTypeId != typeId)
    {
        m_nowTypeId = typeId;
        
        // ActorのSpriteFrameのplistをキャッシュ
        auto spriteFramePlistName = cocos2d::StringUtils::format("actor/%d/actor_%d_%d.plist",
                                                        m_actorDto.getPlayerId(), m_actorDto.getPlayerId(), typeId);
        CCLOG("changeSpriteFrame = %s", spriteFramePlistName.c_str());
        if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFramePlistName))
        {
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteFramePlistName);
        }
    }
    auto spriteFrameName = cocos2d::StringUtils::format("actor_%d_%d_%s_%d.png", m_actorDto.getPlayerId(), typeId, frameName.c_str(), 1);
    CCLOG("changeSpriteFrame spriteFrameName = %s", spriteFrameName.c_str());
    auto spriteFrame = Sprite::createWithSpriteFrameName(spriteFrameName);
    this->setTexture(spriteFrame->getTexture());
}

ActorSprite* ActorSprite::createWithActorDto(ActorDto actorDto)
{
    auto *pRet = new ActorSprite();
    if (pRet && pRet->initWithActorDto(actorDto))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

ActorSprite* ActorSprite::createWithActorDto(ActorDto actorDto, int typeId)
{
    auto *pRet = new ActorSprite();
    if (pRet && pRet->initWithActorDto(actorDto, typeId))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

void ActorSprite::setActorMapItem(ActorMapItem actorMapItem)
{
    m_actorMapItem = actorMapItem;
}

const ActorMapItem& ActorSprite::getActorMapItem() const
{
    return m_actorMapItem;
}
void ActorSprite::moveDone(bool done)
{
    m_actorMapItem.moveDone = done;
}
void ActorSprite::attackDone(bool done)
{
    m_actorMapItem.attackDone = done;
}
void ActorSprite::done(bool done)
{
    moveDone(done);
    attackDone(done);
}

ActorDto* ActorSprite::getActorDto()
{
    return &m_actorDto;
}

void ActorSprite::runMoveAction(MapIndex moveMapIndex)
{
    if (moveMapIndex.x == 0 && moveMapIndex.y == 0)
    {
        CCLOG("移動なし");
        return;
    }
    
    this->m_actorMapItem.mapIndex.moveDictType = moveMapIndex.moveDictType;
    
    // キャラの向きを変更
    if (moveMapIndex.moveDictType == MOVE_DOWN)
    {
        this->runBottomAction();
    }
    else if (moveMapIndex.moveDictType == MOVE_LEFT)
    {
        this->runLeftAction();
    }
    else if (moveMapIndex.moveDictType == MOVE_RIGHT)
    {
        this->runRightAction();
    }
    else if (moveMapIndex.moveDictType == MOVE_UP)
    {
        this->runTopAction();
    }
}

void ActorSprite::runBottomAction()
{
    if (this->isRunning() && this->getActionByTag(1))
    {
        return;
    }
    this->stopAllActions();
    auto action = createBottomActorAnimate();
    action->setTag(1);
    this->runAction(action);
}
void ActorSprite::runLeftAction()
{
    if (this->isRunning() && this->getActionByTag(2))
    {
        return;
    }
    this->stopAllActions();
    auto action = createLeftActorAnimate();
    action->setTag(2);
    this->runAction(action);
}
void ActorSprite::runRightAction()
{
    if (this->isRunning() && this->getActionByTag(3))
    {
        return;
    }
    this->stopAllActions();
    auto action = createRightActorAnimate();
    action->setTag(3);
    this->runAction(action);
}
void ActorSprite::runTopAction()
{
    if (this->isRunning() && this->getActionByTag(4))
    {
        return;
    }
    this->stopAllActions();
    auto action = createTopActorAnimate();
    action->setTag(4);
    this->runAction(action);
}

FiniteTimeAction* ActorSprite::createBottomActorAnimate()
{
    return createActorAnimate(m_actorDto.getPlayerId(), "bottom");
}

FiniteTimeAction* ActorSprite::createLeftActorAnimate()
{
    return createActorAnimate(m_actorDto.getPlayerId(), "left");
}

FiniteTimeAction* ActorSprite::createRightActorAnimate()
{
    return createActorAnimate(m_actorDto.getPlayerId(), "right");
}

FiniteTimeAction* ActorSprite::createTopActorAnimate()
{
    return createActorAnimate(m_actorDto.getPlayerId(), "top");
}

FiniteTimeAction* ActorSprite::createActorAnimate(int actorId, std::string frameName)
{
    return createActorAnimate(actorId, m_nowTypeId, frameName);
}

FiniteTimeAction* ActorSprite::createActorAnimate(int actorId, int typeId, std::string frameName)
{
    auto pAnimation = Animation::create();
    auto startSpriteFrameName = cocos2d::StringUtils::format("actor_%d_%d_%s_%d.png", actorId, typeId, frameName.c_str(), 2);
    auto pStartFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(startSpriteFrameName);
    pAnimation->addSpriteFrame(pStartFrame);
    for (int i = 0; i < 3; i++)
    {
        auto spriteFrameName = cocos2d::StringUtils::format("actor_%d_%d_%s_%d.png", actorId, typeId, frameName.c_str(), (i + 1));
        auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
        pAnimation->addSpriteFrame(pFrame);
    }
    pAnimation->setDelayPerUnit(0.3);
    pAnimation->setLoops(-1);
    
    return Animate::create(pAnimation);
}


/////////////
// static

// 立ち絵
Sprite* ActorSprite::createNovelActorSprite(ActorSprite::NovelDirection novelDirection)
{
    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    // キャラ表示
    auto playerSprite = Sprite::create("novel/actor4_novel_s_0.png");
    playerSprite->setScale(resolutionSize.height/playerSprite->getContentSize().height);
    if (novelDirection == ActorSprite::NovelDirection::LEFT) {
        playerSprite->setPosition(Point(winSize.width * 0.15, winSize.height * 0.4));
        playerSprite->setFlippedX(true);
    } else if (novelDirection == ActorSprite::NovelDirection::CENTER) {
        playerSprite->setPosition(Point(winSize.width * 0.55, winSize.height * 0.4));
    } else if (novelDirection == ActorSprite::NovelDirection::RIGHT) {
        playerSprite->setPosition(Point(winSize.width * 0.85, winSize.height * 0.4));
    }
    
    return playerSprite;
}





