/**
 @file ItemMixedPageLayer.h
 
 @brief アイテム合成画面
 
 アイテムの合成屋
 
 @author kyokomi
 @date 2014/05/07
 
 */

#ifndef __Cocos2dRogueLike__ItemMixedPageLayer__
#define __Cocos2dRogueLike__ItemMixedPageLayer__

#include "cocos2d.h"

class ItemDto;

/**
 @class ItemMixedPageLayer ItemMixedPageLayer.h
 
 @brief マイページのアイテム合成ページのクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class ItemMixedPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(ItemMixedPageLayer);
    ItemMixedPageLayer();
    virtual ~ItemMixedPageLayer();
    
    virtual bool init();
    
    void setMenuCallback1(const cocos2d::ccMenuCallback& callback) { _menuCallback1 = callback; }
    void setMenuCallback2(const cocos2d::ccMenuCallback& callback) { _menuCallback2 = callback; }
    void setMenuCallback3(const cocos2d::ccMenuCallback& callback) { _menuCallback3 = callback; }
    
    void refreshSelectBase(const ItemDto &itemDto);
    void refreshSelectMaterial(const ItemDto &itemDto);
    void mixedItemAfter();
private:
    void refreshSelectItem(cocos2d::Sprite* materialSprite, cocos2d::MenuItem* menuItem, const ItemDto &itemDto);
    
private:
    cocos2d::ccMenuCallback _menuCallback1;
    cocos2d::ccMenuCallback _menuCallback2;
    cocos2d::ccMenuCallback _menuCallback3;
    
    cocos2d::Sprite* _baseSprite;
    cocos2d::Sprite* _materialSprite;
    cocos2d::MenuItem* _baseMenuItemLabel;
    cocos2d::MenuItem* _materialMenuItemLabel;
};

#endif /* defined(__Cocos2dRogueLike__ItemMixedPageLayer__) */
