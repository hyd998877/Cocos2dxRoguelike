//
//  ItemInventoryLayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/05.
//
//

#ifndef __Cocos2dRogueLike__ItemInventoryLayer__
#define __Cocos2dRogueLike__ItemInventoryLayer__

#include "ItemWindowLayer.h"
#include "ModalLayer.h"

class ItemInventoryLayer : public ModalLayer
{
public:
    enum class CloseType {
        UN_CLOSE,
        CLOSE,
    };
    
    typedef std::function<void(void)> CloseCallback;
    typedef std::function<void(ItemInventoryDto::Comparator)> SortCallback;
    
    typedef struct _ActionCallback {
        ItemWindowLayer::ItemWindowMenuType _actionType;
        CloseType _closeType;
        ItemWindowLayer::ItemWindowMenuCallback _callback;
    } ActionCallback;
    
    typedef std::list<ItemInventoryDto> ItemInventoryList;
    
    ItemInventoryLayer();
    virtual ~ItemInventoryLayer();
    virtual bool init(ItemInventoryList itemInventoryList);
    
    static ItemInventoryLayer* create(ItemInventoryDto itemInventoryDto);
    static ItemInventoryLayer* create(ItemInventoryList itemInventoryList);
    
    void initMenuActionCallback(std::list<ActionCallback> actionCallbackList);
    
    void changeTab(const std::string &inventoryName);
    void refreshInventory();
    void refresh(const ItemInventoryList &itemInventoryList);
    void refresh(const ItemInventoryList &itemInventoryList, const ItemInventoryDto &selectInventory);
    
    void setCloseCallback(const CloseCallback &callback) { _closeCallback = callback; }
    void setSortCallback(const SortCallback &callback) { _sortCallback = callback; }
private:
    constexpr static const float WINDOW_SIZE_WIDTH_SCALE = 0.9;
    constexpr static const float WINDOW_SIZE_HEIGHT_SCALE = 0.8;
    
    ItemInventoryList _itemInventoryList;
    ItemInventoryDto _selectInventory;
    
    ItemWindowLayer* _itemWindowLayer;
    cocos2d::Label* _goldTextLabel;
    cocos2d::Map<std::string, cocos2d::Label*> _menuItemMap;
    
    CloseCallback _closeCallback;
    SortCallback _sortCallback;
    
    void initHeader();
    void initDetail();
    void initFooter();
    
    static bool searchCallbackFire(std::list<ActionCallback> actionCallbackList,
                                   cocos2d::Ref* ref,
                                   const ItemDto &itemDto,
                                   ItemWindowLayer::ItemWindowMenuType fireActionType);
    
    /**
     @brief Menuの要素の長さを合わせて中心を作成
     @param[in] menu メニュー
     @return menuの中心Point
     */
    static cocos2d::Point createMenuCenterPoint(cocos2d::Menu* menu);
    
    void close();
};

#endif /* defined(__Cocos2dRogueLike__ItemInventoryLayer__) */
