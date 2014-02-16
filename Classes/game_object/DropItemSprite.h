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

USING_NS_CC;

class DropItemSprite : public cocos2d::Sprite
{
public:
    enum ItemType {
        NONE               = 0,
        EQUIP_WEAPON       = 1,
        EQUIP_ACCESSORY    = 2,
        HP_RECOVER_VALUE   = 100,
        HP_RECOVER_PERCENT = 101,
        MP_RECOVER_VALUE   = 200,
        MP_RECOVER_PERCENT = 201,
    };
    
    typedef struct _DropItemDto {
        /** すべてのアイテムで一意な値 */
        long objectId;
        /** アイテムを一意に識別するID. */
        int itemId;
        
        // 以下は、itemIdから引けるはず？
        /** アイテムタイプ */
        ItemType itemType;
        /** アイテムイメージのID. */
        int imageResId;
        /** アイテム名. */
        std::string name;
        
        // status
        bool isEquip;
        
    } DropItemDto;
    
    static std::string createItemImageFileName(int itemResId)
    {
        // TODO: ほんとうはMImageDao使うんだが。。。とりあえず10000引く
        return cocos2d::StringUtils::format("icon_set/item_%d.png", itemResId % 10000);
    }
    
    // コンストラクタ、デストラクタ
    DropItemSprite();
    ~DropItemSprite();
    
    // 初期化とか
    virtual bool initWithDropItemDto(DropItemDto dropItemDto);
    virtual bool initWithTextureDropItemDto(DropItemDto dropItemDto, Texture2D *texture);
    static DropItemSprite* createWithDropItemDto(DropItemDto dropItemDto);
    static DropItemSprite* createWithTextureDropItemDto(DropItemDto dropItemDto, Texture2D *texture);
    
    void setDropMapItem(DropMapItem dropMapItem);
    DropMapItem* getDropMapItem();
    DropItemDto* getDropItemDto();
    
private:
    DropItemDto m_dropItemDto;
    DropMapItem m_dropMapItem;
    
};

#endif /* defined(__Cocos2dxSRPGQuest__DropItemSprite__) */
