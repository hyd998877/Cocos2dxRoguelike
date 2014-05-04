/**
 @file ItemInventoryWindowHelper.h
 
 @brief アイテムイベントリウィンドウのヘルパー
 
 @author kyokomi
 @date 2014/04/27
 
 イベントリ表示を楽にしてくれるヘルパークラス
 */
#ifndef __Cocos2dRogueLike__ItemInventoryWindowHelper__
#define __Cocos2dRogueLike__ItemInventoryWindowHelper__

#include "ModalLayer.h"
#include "ItemWindowLayer.h"

#include "IteminventoryDto.h"

/**
 @class ItemInventoryWindowHelper ItemInventoryWindowHelper.h
 
 @brief アイテムイベントリウィンドウのヘルパー
 
 イベントリ表示を楽にしてくれるヘルパークラス
 
 @author kyokomi
 @date 2014/04/27
 */
class ItemInventoryWindowHelper {
public:
    /**
     @brief アイテム選んだときの操作区分
     
     対応するコールバックメソッド呼び出しに利用します。
     */
    enum class ActionType {
        ITEM_DROP,  /// < 捨てる
        ITEM_DIST,  /// < ？
        ITEM_USE,   /// < 使う
        ITEM_EQUIP, /// < 装備/はずす
        ITEM_SALE,  /// < 売る
    };
    
    typedef struct _ActionCallback {
        ActionType _actionType;
        ItemWindowLayer::ItemWindowMenuCallback callback;
    } ActionCallback;
    
    typedef std::list<ItemInventoryDto> ItemInventoryList;
    
    static ModalLayer* create(ItemInventoryDto itemInventoryDto, std::list<ActionCallback> actionCallbackList);
    
    static ModalLayer* create(ItemInventoryList itemInventoryList, std::list<ActionCallback> actionCallbackList);

private:
    static void searchCallbackFire(std::list<ActionCallback> actionCallbackList, cocos2d::Ref* ref, ItemDto itemDto, ActionType fireActionType);
    
    /**
     @brief Menuの要素の長さを合わせて中心を作成
     @param[in] menu メニュー
     @return menuの中心Point
     */
    static cocos2d::Point createMenuCenterPoint(cocos2d::Menu* menu);
    
    constexpr static const float WINDOW_SIZE_SCALE = 0.8;
};
#endif /* defined(__Cocos2dRogueLike__ItemInventoryWindowHelper__) */
