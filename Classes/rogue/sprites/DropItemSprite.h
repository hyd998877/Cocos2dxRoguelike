//
//  DropItemSprite.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/05.
//
//

#ifndef __Cocos2dxSRPGQuest__DropItemSprite__
#define __Cocos2dxSRPGQuest__DropItemSprite__

#include "cocos2d.h"
#include "MapManager.h"

#include "ItemDto.h"

USING_NS_CC;

class DropItemSprite : public cocos2d::Sprite
{
public:
    
    // コンストラクタ、デストラクタ
    DropItemSprite();
    virtual ~DropItemSprite();
    
    // 初期化とか
    virtual bool initWithItemDto(const ItemDto& itemDto);
    virtual bool initWithTextureItemDto(const ItemDto& itemDto, Texture2D *texture);
    static DropItemSprite* createWithItemDto(const ItemDto& itemDto);
    static DropItemSprite* createWithTextureItemDto(const ItemDto& itemDto, Texture2D *texture);
    
    void setDropMapItem(DropMapItem dropMapItem) { _dropMapItem = dropMapItem; }
    
    const DropMapItem& getDropMapItem() const { return _dropMapItem; }
    const ItemDto& getItemDto() const { return _itemDto; }
    
private:
    ItemDto _itemDto;
    DropMapItem _dropMapItem;
    
};

#endif /* defined(__Cocos2dxSRPGQuest__DropItemSprite__) */
