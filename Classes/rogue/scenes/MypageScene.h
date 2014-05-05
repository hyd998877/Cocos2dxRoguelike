//
//  MypageScene.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/23.
//
//

#ifndef __Cocos2dRogueLike__MypageScene__
#define __Cocos2dRogueLike__MypageScene__

#include "cocos2d.h"
#include "AppMacros.h"

#include "ItemInventoryDto.h"

NS_ROGUE_BEGIN

class MypageScene : public cocos2d::Layer
{
public:
    enum GlobalMenuTags {
        TOP = 1,
        QUEST,
        STOCK,
        NOVEL,
        MIXED,
        TEST
    };
    
    static cocos2d::Scene* scene();
    MypageScene();
    virtual ~MypageScene();
    
    virtual bool init();
private:
    void initQuestSave();
    
    void initMyPage();
    void initItemStockPage();
    void initMixedPage();

    void initGlobalMenu(int selectMenuTag);
    void initHeaderTitle(const std::string titleText);
    cocos2d::Menu* createGlobalMenu();

    void showItemStockWindow();
    void showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback);
    
    bool mixedItem();
    
    CREATE_FUNC(MypageScene);
    
    cocos2d::Layer* _baseLayer;
    // 倉庫用
    ItemInventoryDto _itemInventory;
    ItemInventoryDto _itemInventoryStock;
    
    // 合成用
    ItemDto _baseItemDto;
    ItemDto _materialItemDto;
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__MypageScene__) */
