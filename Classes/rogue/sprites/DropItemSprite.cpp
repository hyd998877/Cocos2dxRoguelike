//
//  DropItemSprite.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "DropItemSprite.h"

#include "AppMacros.h"
#include "StringUtils.h"

#include "ItemLogic.h"

USING_NS_CC;

DropItemSprite::DropItemSprite()
{
    
}

DropItemSprite::~DropItemSprite()
{
    
}

bool DropItemSprite::initWithItemDto(const ItemDto& itemDto)
{
    _itemDto = itemDto;
    auto spriteFileName = ItemLogic::createItemImageFileName(_itemDto.getImageResId());
    if (!Sprite::initWithFile(spriteFileName))
    {
        return false;
    }
    
    return true;
}
bool DropItemSprite::initWithTextureItemDto(const ItemDto& itemDto, Texture2D *texture)
{
    _itemDto = itemDto;
    if (!Sprite::initWithTexture(texture))
    {
        return false;
    }
    
    return true;
}

DropItemSprite* DropItemSprite::createWithItemDto(const ItemDto& itemDto)
{
    DropItemSprite *sprite = new DropItemSprite();
    if (sprite && sprite->initWithItemDto(itemDto))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

DropItemSprite* DropItemSprite::createWithTextureItemDto(const ItemDto& itemDto, Texture2D *texture)
{
    DropItemSprite *sprite = new DropItemSprite();
    if (sprite && sprite->initWithTextureItemDto(itemDto, texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}
