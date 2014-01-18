//
//  DropItemSprite.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#include "DropItemSprite.h"

using namespace cocos2d;

DropItemSprite::DropItemSprite()
{
    
}

DropItemSprite::~DropItemSprite()
{
    
}

bool DropItemSprite::initWithDropItemDto(DropItemDto dropItemDto)
{
    m_dropItemDto = dropItemDto;
    
    auto spriteFileName = DropItemSprite::createItemImageFileName(m_dropItemDto.imageResId);
    if (!Sprite::initWithFile(spriteFileName))
    {
        return false;
    }
    
    return true;
}
bool DropItemSprite::initWithTextureDropItemDto(DropItemDto dropItemDto, Texture2D *texture)
{
    m_dropItemDto = dropItemDto;

    if (!Sprite::initWithTexture(texture))
    {
        return false;
    }
    
    return true;
}

DropItemSprite* DropItemSprite::createWithDropItemDto(DropItemDto pDropItemDto)
{
    DropItemSprite *sprite = new DropItemSprite();
    if (sprite && sprite->initWithDropItemDto(pDropItemDto))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}

DropItemSprite* DropItemSprite::createWithTextureDropItemDto(DropItemDto dropItemDto, Texture2D *texture)
{
    DropItemSprite *sprite = new DropItemSprite();
    if (sprite && sprite->initWithTextureDropItemDto(dropItemDto, texture))
    {
        sprite->autorelease();
        return sprite;
    }
    CC_SAFE_DELETE(sprite);
    return nullptr;
}


void DropItemSprite::setDropMapItem(DropMapItem dropMapItem)
{
    m_dropMapItem = dropMapItem;
}

DropMapItem* DropItemSprite::getDropMapItem()
{
    return &m_dropMapItem;
}

DropItemSprite::DropItemDto* DropItemSprite::getDropItemDto()
{
    return &m_dropItemDto;
}
