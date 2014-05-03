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

USING_NS_CC;

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
    CCLOG("initWithDropItemDto = %s", spriteFileName.c_str());
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

std::string DropItemSprite::dropItemToString(const DropItemDto& drop_item)
{
    return cocos2d::StringUtils::format("%ld,%d,%d,%d,%s,%d,%d",
                               drop_item.objectId,
                               drop_item.itemId,
                               drop_item.itemType,
                               drop_item.imageResId,
                               drop_item.name.c_str(),
                               drop_item.isEquip,
                               drop_item.param
                               );
}

DropItemSprite::DropItemDto DropItemSprite::createDropItemDto(std::string data_string)
{
    std::vector<std::string> data_string_array = GameCore::StringUtils::split(std::move(data_string));
    int index = 0;
    
    if (data_string_array.size() != 7) {
        return {0l, 0, MUseItem::ItemType::NONE, 0, "", false};
    }
    
    DropItemSprite::DropItemDto res_dto;
    res_dto.objectId   = atol(data_string_array[index].c_str()); index++;
    res_dto.itemId     = atoi(data_string_array[index].c_str()); index++;
    res_dto.itemType   = static_cast<MUseItem::ItemType>(atoi(data_string_array[index].c_str())); index++;
    res_dto.imageResId = atoi(data_string_array[index].c_str()); index++;
    res_dto.name       = data_string_array[index];               index++;
    res_dto.isEquip    = static_cast<bool>(atoi(data_string_array[index].c_str())); index++;
    res_dto.param      = atoi(data_string_array[index].c_str()); index++;
    return res_dto;
}


