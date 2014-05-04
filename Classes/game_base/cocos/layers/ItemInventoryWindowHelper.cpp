//
//  ItemInventoryWindowHelper.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#include "ItemInventoryWindowHelper.h"
#include "AppMacros.h"
#include "CommonWindowUtil.h"

#include "MenuItemUtil.h"

USING_NS_CC;

ModalLayer* ItemInventoryWindowHelper::create(ItemInventoryDto itemInventoryDto, std::list<ActionCallback> actionCallbackList)
{
    return create(std::list<ItemInventoryDto>({itemInventoryDto}), actionCallbackList);
}

ModalLayer* ItemInventoryWindowHelper::create(ItemInventoryList itemInventoryList, std::list<ActionCallback> actionCallbackList)
{
    Size contentSize = Director::getInstance()->getWinSize() * ItemInventoryWindowHelper::WINDOW_SIZE_SCALE;
    
    auto modalLayer = ModalLayer::create();
    // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
    auto itemWindowLayer = ItemWindowLayer::createWithContentSize(contentSize);
    
    ///////////////////////////////////////
    // ヘッダー
    cocos2d::Vector<MenuItem*> menuItemArray;
    
    // 1件目デフォルト
    ItemInventoryDto selectInventory  = (*itemInventoryList.begin());
    std::string selectTabName = selectInventory.getName();

    // タブ作成
    for (auto inventory : itemInventoryList) {
        auto tabMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), inventory.createTabName()), Size(12, 4), [inventory, itemWindowLayer](Ref* ref) {
            CCLOG("TabName = %s", inventory.getName().c_str());
            MenuItemUtil::touchItemRefreshColor(ref, Color3B::GREEN);
            
            itemWindowLayer->setItemInventory(inventory);
            itemWindowLayer->reloadItemList();
        });
        if (selectTabName == inventory.getName()) {
            tabMenuItemLabel->setColor(Color3B::GREEN);
        }
        menuItemArray.pushBack(tabMenuItemLabel);
    }
    
    auto headerMenu = Menu::createWithArray(menuItemArray);
    headerMenu->alignItemsHorizontallyWithPadding(0);
    Point menuItemHeaderPoint = Point::ZERO;
    if (headerMenu->getChildrenCount() > 0) {
        Size menuItemSize = headerMenu->getChildren().at(0)->getContentSize();
        menuItemHeaderPoint = Point(headerMenu->getChildrenCount() * menuItemSize.width / 2, itemWindowLayer->getContentSize().height + menuItemSize.height/2);
    }
    headerMenu->setPosition(menuItemHeaderPoint);
    itemWindowLayer->addChild(headerMenu);
    
    ////////////////////////////
    // 内容
    itemWindowLayer->setItemInventory(selectInventory);
    itemWindowLayer->reloadItemList();
    
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer->getContentSize(), contentSize));
    itemWindowLayer->setItemDropMenuCallback([modalLayer, actionCallbackList](Ref* ref, ItemDto itemDto) {
        CCLOG("RogueScene::itemDropMenuCallback");
        searchCallbackFire(actionCallbackList, ref, itemDto, ActionType::ITEM_DROP);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    itemWindowLayer->setItemUseMenuCallback([modalLayer, actionCallbackList](Ref* ref, ItemDto itemDto) {
        CCLOG("RogueScene::itemUseMenuCallback");
        searchCallbackFire(actionCallbackList, ref, itemDto, ActionType::ITEM_USE);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    itemWindowLayer->setItemEquipMenuCallback([modalLayer, actionCallbackList](Ref* ref, ItemDto itemDto) {
        CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", itemDto.getItemType());
        searchCallbackFire(actionCallbackList, ref, itemDto, ActionType::ITEM_EQUIP);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    ///////////////////////////////////////
    // フッター

    // 並び替えボタン
    auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "並び替え"), Size(12, 4), [itemWindowLayer](Ref* ref) {
        // 並び替え
        itemWindowLayer->sortWeaponWithAccessory();
        itemWindowLayer->reloadItemList();
    });
    sort_menu_item_label->setPosition(Point(itemWindowLayer->getContentSize().width, itemWindowLayer->getContentSize().height + sort_menu_item_label->getContentSize().height / 2));

    
    // 閉じるボタン
    auto clone_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "とじる"), Size(12, 4), [modalLayer](Ref* ref) {
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    clone_menu_item_label->setPosition(Point(contentSize.width, clone_menu_item_label->getPositionY() - clone_menu_item_label->getContentSize().height / 2));
    
    ///////////////////////////
    // Menu
    auto menu = Menu::create(sort_menu_item_label, clone_menu_item_label, NULL);
    menu->setPosition(Point::ZERO);
    
    itemWindowLayer->addChild(menu);
    
    modalLayer->addChild(itemWindowLayer);
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer, modalLayer));
    
    return modalLayer;
}

void ItemInventoryWindowHelper::searchCallbackFire(std::list<ActionCallback> actionCallbackList, Ref* ref, ItemDto itemDto, ActionType fireActionType)
{
    auto it = std::find_if(actionCallbackList.begin(), actionCallbackList.end(), [fireActionType](ActionCallback actionCallback) -> bool {
        if (actionCallback._actionType == fireActionType) {
            return true;
        }
        return false;
    });
    (*it).callback(ref, itemDto);
}
