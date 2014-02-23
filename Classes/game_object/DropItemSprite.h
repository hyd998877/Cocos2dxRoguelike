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
#include "MUseItem.h"

USING_NS_CC;

class DropItemSprite : public cocos2d::Sprite
{
public:
    
    typedef struct _DropItemDto {
        /** すべてのアイテムで一意な値 */
        long objectId;
        /** アイテムを一意に識別するID. */
        int itemId;
        
        // 以下は、itemIdから引けるはず？
        /** アイテムタイプ */
        MUseItem::ItemType itemType;
        /** アイテムイメージのID. */
        int imageResId;
        /** アイテム名. */
        std::string name;
        
        // status
        bool isEquip;
        
        // 装備優先 武器優先
        static bool const compare_dropItem_equip(const _DropItemDto& o1, const _DropItemDto& o2)
        {
            if (o1.isEquip && !o2.isEquip)
            {
                return true;
            }
            else if (o1.isEquip && o2.isEquip && o1.itemType == MUseItem::ItemType::EQUIP_WEAPON)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
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
