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
    auto spriteFramePlistName = StringUtils::format("actor/%d/actor_%d_%d.plist", m_actorDto.playerId, m_actorDto.playerId, typeId);
    CCLOG("initWithActorDto = %s", spriteFramePlistName.c_str());
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteFramePlistName);
    
    // Spriteを生成
    auto spriteFrameName = StringUtils::format("actor_%d_%d_%s_%d.jpg", m_actorDto.playerId, typeId, "bottom", 1);
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
        auto spriteFramePlistName = StringUtils::format("actor/%d/actor_%d_%d.plist",
                                                        m_actorDto.playerId, m_actorDto.playerId, typeId);
        CCLOG("changeSpriteFrame = %s", spriteFramePlistName.c_str());
        if (!SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFramePlistName))
        {
            SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteFramePlistName);
        }
    }
    auto spriteFrameName = StringUtils::format("actor_%d_%d_%s_%d.jpg", m_actorDto.playerId, typeId, frameName.c_str(), 1);
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

ActorMapItem* ActorSprite::getActorMapItem()
{
    return &m_actorMapItem;
}

ActorSprite::ActorDto* ActorSprite::getActorDto()
{
    return &m_actorDto;
}

void ActorSprite::equipWeapon(long objectId, int weaponId)
{
    // TODO: マスターデータの仕組みが必要
}

void ActorSprite::equipReleaseWeapon()
{
    m_actorDto.equip.weaponObjectId = 0;
    m_actorDto.equip.weaponId       = 0;
    m_actorDto.equip.weaponImgResId = 0;
    m_actorDto.equip.weaponName     = "";
    m_actorDto.equip.weaponStr      = 0;
}

void ActorSprite::equipAccessory(long objectId, int accessoryId)
{
    // TODO: マスターデータの仕組みが必要
}

void ActorSprite::equipReleaseAccessory()
{
    m_actorDto.equip.accessoryObjectId = 0;
    m_actorDto.equip.accessoryId       = 0;
    m_actorDto.equip.accessoryImgResId = 0;
    m_actorDto.equip.accessoryName     = "";
    m_actorDto.equip.accessoryDef      = 0;
}

void ActorSprite::runMoveAction(MapIndex moveMapIndex)
{
    if (moveMapIndex.x == 0 && moveMapIndex.y == 0)
    {
        CCLOG("移動なし");
        return;
    }
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
    return createActorAnimate(m_actorDto.playerId, "bottom");
}

FiniteTimeAction* ActorSprite::createLeftActorAnimate()
{
    return createActorAnimate(m_actorDto.playerId, "left");
}

FiniteTimeAction* ActorSprite::createRightActorAnimate()
{
    return createActorAnimate(m_actorDto.playerId, "right");
}

FiniteTimeAction* ActorSprite::createTopActorAnimate()
{
    return createActorAnimate(m_actorDto.playerId, "top");
}

FiniteTimeAction* ActorSprite::createActorAnimate(int actorId, std::string frameName)
{
    return createActorAnimate(actorId, m_nowTypeId, frameName);
}

FiniteTimeAction* ActorSprite::createActorAnimate(int actorId, int typeId, std::string frameName)
{
    auto pAnimation = Animation::create();
    auto startSpriteFrameName = StringUtils::format("actor_%d_%d_%s_%d.jpg", actorId, typeId, frameName.c_str(), 2);
    auto pStartFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(startSpriteFrameName);
    pAnimation->addSpriteFrame(pStartFrame);
    for (int i = 0; i < 3; i++)
    {
        auto spriteFrameName = StringUtils::format("actor_%d_%d_%s_%d.jpg", actorId, typeId, frameName.c_str(), (i + 1));
        auto pFrame = SpriteFrameCache::getInstance()->getSpriteFrameByName(spriteFrameName);
        pAnimation->addSpriteFrame(pFrame);
    }
    pAnimation->setDelayPerUnit(0.3);
    pAnimation->setLoops(-1);
    
    return Animate::create(pAnimation);
}

// static

ActorSprite::ActorEquipDto ActorSprite::createEquipDto()
{
    ActorSprite::ActorEquipDto equipDto;
    equipDto.weaponObjectId    = 0;
    equipDto.weaponId          = 0;
    equipDto.weaponName        = "";
    equipDto.weaponImgResId    = 0;
    equipDto.weaponStr         = 0;
    equipDto.accessoryId       = 0;
    equipDto.accessoryObjectId = 0;
    equipDto.accessoryName     = "";
    equipDto.accessoryImgResId = 0;
    equipDto.accessoryDef      = 0;
    return equipDto;
}

ActorSprite::ActorEquipDto ActorSprite::createEquipDto(std::string data_string)
{
    std::vector<std::string> data_string_array = StringUtil::split(std::move(data_string));
    int index = 0;

    if (data_string_array.size() != 10) {
        return createEquipDto();
    }
    
    ActorSprite::ActorEquipDto equipDto;
    
    equipDto.weaponObjectId    = atoi(data_string_array[index].c_str()); index++;
    equipDto.weaponId          = atoi(data_string_array[index].c_str()); index++;
    equipDto.weaponName        = data_string_array[index];               index++;
    equipDto.weaponImgResId    = atoi(data_string_array[index].c_str()); index++;
    equipDto.weaponStr         = atoi(data_string_array[index].c_str()); index++;

    equipDto.accessoryObjectId = atoi(data_string_array[index].c_str()); index++;
    equipDto.accessoryId       = atoi(data_string_array[index].c_str()); index++;
    equipDto.accessoryName     = data_string_array[index];               index++;
    equipDto.accessoryImgResId = atoi(data_string_array[index].c_str()); index++;
    equipDto.accessoryDef      = atoi(data_string_array[index].c_str()); index++;
    return equipDto;
}

ActorSprite::ActorDto ActorSprite::createDto()
{
    ActorSprite::ActorDto actorDto;
    /** プレイヤーを一意に識別するID. */
    actorDto.playerId = 0;
    /** キャライメージのID. */
    actorDto.imageResId = 0;
    /** 顔画像のデフォルトID. */
    actorDto.faceImgId = 0;
    /** キャラ名. */
    actorDto.name = "";
    /** 攻撃力. */
    actorDto.attackPoint = 0;
    /** 防御力. */
    actorDto.defencePoint = 0;
    /** レベル. */
    actorDto.lv = 0;
    /** 経験値. */
    actorDto.exp = 0;
    /** 次のレベルまでの経験値 */
    actorDto.nextExp = 0;
    /** HP. */
    actorDto.hitPoint = 0;
    /** HP最大値. */
    actorDto.hitPointLimit = 0;
    /** MP. */
    actorDto.magicPoint = 0;
    /** MP最大値. */
    actorDto.magicPointLimit = 0;
    /** 移動力(ステータス表示用). */
    actorDto.movePoint = 0;
    /** 攻撃範囲(ステータス表示用). */
    actorDto.attackRange = 0;
    /** 装備. */
    actorDto.equip = ActorSprite::createEquipDto();
    return actorDto;
}

ActorSprite::ActorDto ActorSprite::createActorDto(std::string data_string)
{
    std::vector<std::string> data_string_array = StringUtil::split(std::move(data_string));
    int index = 0;
    
    if (data_string_array.size() != 15) {
        return createDto();
    }
    
    ActorSprite::ActorDto actor_dto;
    
    actor_dto.playerId        = atoi(data_string_array[index].c_str()); index++;
    actor_dto.imageResId      = atoi(data_string_array[index].c_str()); index++;
    actor_dto.faceImgId       = atoi(data_string_array[index].c_str()); index++;
    actor_dto.name            = data_string_array[index];            index++;
    actor_dto.attackPoint     = atoi(data_string_array[index].c_str()); index++;
    actor_dto.defencePoint    = atoi(data_string_array[index].c_str()); index++;
    actor_dto.lv              = atoi(data_string_array[index].c_str()); index++;
    actor_dto.exp             = atoi(data_string_array[index].c_str()); index++;
    actor_dto.nextExp         = atoi(data_string_array[index].c_str()); index++;
    actor_dto.hitPoint        = atoi(data_string_array[index].c_str()); index++;
    actor_dto.hitPointLimit   = atoi(data_string_array[index].c_str()); index++;
    actor_dto.magicPoint      = atoi(data_string_array[index].c_str()); index++;
    actor_dto.magicPointLimit = atoi(data_string_array[index].c_str()); index++;
    actor_dto.movePoint       = atoi(data_string_array[index].c_str()); index++;
    actor_dto.attackRange     = atoi(data_string_array[index].c_str()); index++;
    return actor_dto;
}
