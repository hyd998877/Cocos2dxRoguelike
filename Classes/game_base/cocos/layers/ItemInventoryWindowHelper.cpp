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
    
    std::list<ItemWindowLayer::ItemWindowMenuType> menuTypeList;
    for (auto actionCallback : actionCallbackList) {
        menuTypeList.push_back(actionCallback._actionType);
    }
    itemWindowLayer->initCreateMenu(menuTypeList);
    
    // 1件目デフォルト
    ItemInventoryDto selectInventory  = (*itemInventoryList.begin());
    std::string selectTabName = selectInventory.getName();
    
    // お金表示
    // ラベル
    std::string goldText = GameCore::StringUtils::format("所持金 %10ld G", selectInventory.getGold());
    auto goldTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), goldText);
    goldTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    goldTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    // 枠
    auto goldLayerWaku = CommonWindowUtil::createWindowWaku(Size(contentSize.width/2, goldTextLabel->getContentSize().height + goldTextLabel->getSystemFontSize()/2) + Size(0, 2));
    // ベースレイヤー
    auto goldLayer = Layer::create();
    goldLayer->setContentSize(Size(contentSize.width/2, goldLayerWaku->getContentSize().height));
    goldLayer->setPosition(Point(contentSize.width + 2 - goldLayerWaku->getContentSize().width, contentSize.height));
    
    // ラベル
    goldTextLabel->setPosition(Point(goldLayer->getContentSize().width /2, goldLayer->getContentSize().height / 2));
    
    goldLayer->addChild(goldTextLabel);
    goldLayer->addChild(goldLayerWaku);
    itemWindowLayer->addChild(goldLayer);
    
    ///////////////////////////////////////
    // ヘッダー
    cocos2d::Vector<MenuItem*> menuItemArray;
    
    // タブ作成
    for (auto inventory : itemInventoryList) {
        auto tabMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), inventory.createTabName()), Size(12, 4), [inventory, itemWindowLayer, goldTextLabel](Ref* ref) {
            CCLOG("TabName = %s", inventory.getName().c_str());
            MenuItemUtil::touchItemRefreshColor(ref, Color3B::GREEN);
            
            std::string goldText = GameCore::StringUtils::format("所持金 %10ld G", inventory.getGold());
            goldTextLabel->setString(goldText);
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
    itemWindowLayer->setItemMenuCallback([modalLayer, actionCallbackList](ItemWindowLayer::ItemWindowMenuType menuType, Ref* ref, const ItemDto &itemDto) {
        CCLOG("RogueScene::itemDropMenuCallback");
        if (searchCallbackFire(actionCallbackList, ref, itemDto, menuType)) {
            modalLayer->setVisible(false);
            modalLayer->removeAllChildrenWithCleanup(true);
        }
    });
    
    ///////////////////////////////////////
    // フッター

    // 並び替えボタン
    auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "並び替え"), Size(12, 4), [itemWindowLayer](Ref* ref) {
        // 並び替え
        itemWindowLayer->sortWeaponWithAccessory();
        itemWindowLayer->reloadItemList();
    });
    
    // 閉じるボタン
    auto clone_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "とじる"), Size(12, 4), [modalLayer](Ref* ref) {
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    ///////////////////////////
    // Menu
    auto menu = Menu::create(sort_menu_item_label, clone_menu_item_label, NULL);
    Point menuCenterPoint = createMenuCenterPoint(menu);
    menu->setPosition(Point(contentSize.width - menuCenterPoint.x,
                            (clone_menu_item_label->getContentSize().height * -1) + menuCenterPoint.y));
    menu->alignItemsHorizontallyWithPadding(0);
    
    itemWindowLayer->addChild(menu);
    modalLayer->addChild(itemWindowLayer);
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer, modalLayer));
    
    return modalLayer;
}

bool ItemInventoryWindowHelper::searchCallbackFire(std::list<ActionCallback> actionCallbackList, Ref* ref, ItemDto itemDto, ItemWindowLayer::ItemWindowMenuType fireActionType)
{
    auto it = std::find_if(actionCallbackList.begin(), actionCallbackList.end(), [fireActionType](ActionCallback actionCallback) -> bool {
        if (actionCallback._actionType == fireActionType) {
            return true;
        }
        return false;
    });
    (*it)._callback(fireActionType, ref, itemDto);
    
    if ((*it)._closeType == ItemInventoryWindowHelper::CloseType::CLOSE) {
        return true;
    }
    return false;
}

Point ItemInventoryWindowHelper::createMenuCenterPoint(Menu* menu)
{
    Size menuItemSize = Size::ZERO;
    if (menu && menu->getChildrenCount() > 0) {
        for (auto node : menu->getChildren()) {
            menuItemSize = menuItemSize + node->getContentSize();
        }
        // 横用
        return Point(menuItemSize.width / 2,
                     menuItemSize.height / menu->getChildrenCount() / 2);
    } else {
        return Point::ZERO;
    }
}

