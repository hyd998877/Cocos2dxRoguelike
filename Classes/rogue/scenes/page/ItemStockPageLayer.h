/**
 @file ItemStockPageLayer.h
 
 @brief 倉庫画面
 
 倉庫のアイテムと所持アイテムの交換。
 アイテムを捨てたり売ったり整理できます。
 
 @author kyokomi
 @date 2014/05/07
 
 */

#ifndef __Cocos2dRogueLike__ItemStockPageLayer__
#define __Cocos2dRogueLike__ItemStockPageLayer__

#include "cocos2d.h"

#include "ItemInventoryDto.h"

class ItemInventoryLayer;

/**
 @class ItemStockPageLayer ItemStockPageLayer.h
 
 @brief マイページの倉庫ページ用Layerクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class ItemStockPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(ItemStockPageLayer);
    ItemStockPageLayer();
    virtual ~ItemStockPageLayer();
    
    virtual bool init();
    
    void setMenuCallback1(const cocos2d::ccMenuCallback& callback) { _menuCallback1 = callback; }
    void setMenuCallback2(const cocos2d::ccMenuCallback& callback) { _menuCallback2 = callback; }
private:
    cocos2d::ccMenuCallback _menuCallback1;
    cocos2d::ccMenuCallback _menuCallback2;
};

#endif /* defined(__Cocos2dRogueLike__ItemStockPageLayer__) */
