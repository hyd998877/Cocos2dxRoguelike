//
//  ActorSprite.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/05.
//
//

#ifndef __Cocos2dxSRPGQuest__ActorSprite__
#define __Cocos2dxSRPGQuest__ActorSprite__

#include "cocos2d.h"

#include "MapManager.h"
#include "AppMacros.h"

#include "ActorDto.h"

USING_NS_CC;

class ActorSprite  : public Sprite
{
public:
    
    // コンストラクタ、デストラクタ
    ActorSprite();
    virtual ~ActorSprite();
    
    // 初期化とか
    bool initWithActorDto(ActorDto actorDto);
    bool initWithActorDto(ActorDto actorDto, int typeId);
    void changeSpriteFrame(int typeId);
    void changeSpriteFrame(int typeId, std::string frameName);
    static ActorSprite* createWithActorDto(ActorDto actorDto);
    static ActorSprite* createWithActorDto(ActorDto actorDto, int typeId);
    
    void setActorMapItem(ActorMapItem actorMapItem);
    const ActorMapItem& getActorMapItem() const;
    void moveDone(bool done = true);
    void attackDone(bool done = true);
    void done(bool done = true);
    
    ActorDto* getActorDto();
    
    void runMoveAction(MapIndex moveMapIndex);
    void runBottomAction();
    void runLeftAction();
    void runRightAction();
    void runTopAction();
    
    enum class NovelDirection {
        LEFT,
        CENTER,
        RIGHT
    };
    
    static Sprite* createNovelActorSprite(ActorSprite::NovelDirection novelDirection = ActorSprite::NovelDirection::LEFT);
private:
    ActorMapItem m_actorMapItem;
    ActorDto m_actorDto;
    int m_nowTypeId;
    
    FiniteTimeAction* createBottomActorAnimate();
    FiniteTimeAction* createLeftActorAnimate();
    FiniteTimeAction* createRightActorAnimate();
    FiniteTimeAction* createTopActorAnimate();
    FiniteTimeAction* createActorAnimate(int actorId, std::string frameName);
    FiniteTimeAction* createActorAnimate(int actorId, int typeId, std::string frameName);
};

#endif /* defined(__Cocos2dxSRPGQuest__ActorSprite__) */

