//
//  ItemInventoryLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/05.
//
//

#include "ItemInventoryLayer.h"
#include "AppMacros.h"
#include "CommonWindowUtil.h"

#include "MenuItemUtil.h"

USING_NS_CC;

ItemInventoryLayer::ItemInventoryLayer()
: _itemInventoryList()
, _selectInventory()
, _itemWindowLayer(nullptr)
, _goldTextLabel(nullptr)
, _menuItemMap()
, _closeCallback(nullptr)
{
}
ItemInventoryLayer::~ItemInventoryLayer()
{
    _closeCallback = nullptr;
}

ItemInventoryLayer* ItemInventoryLayer::create(ItemInventoryDto itemInventoryDto)
{
    return create(std::list<ItemInventoryDto>({itemInventoryDto}));
}

ItemInventoryLayer* ItemInventoryLayer::create(ItemInventoryList itemInventoryList)
{
    ItemInventoryLayer *pRet = new ItemInventoryLayer();
    if (pRet && pRet->init(itemInventoryList)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool ItemInventoryLayer::init(ItemInventoryList itemInventoryList) {
    if (!ModalLayer::init(cocos2d::Color3B::BLACK, 192)) {
        return false;
    }
    this->_itemInventoryList = itemInventoryList;
    
    // 1件目デフォルト
    this->_selectInventory  = *(this->_itemInventoryList.cbegin());
    
    this->initHeader();
    this->initDetail();
    this->initFooter();
    
    return true;
}

void ItemInventoryLayer::initHeader()
{
    Size contentSize = Director::getInstance()->getWinSize() * ItemInventoryLayer::WINDOW_SIZE_SCALE;
    
    // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
    this->_itemWindowLayer = ItemWindowLayer::createWithContentSize(contentSize);
    // アニメーション不要
#if 0
    this->_itemWindowLayer->setScale(0.1f);
    this->_itemWindowLayer->runAction(ScaleTo::create(0.2f, 1.0f));
#endif
    
    std::string selectTabName = this->_selectInventory.getName();
    
    // お金表示
    // ラベル
    std::string goldText = GameCore::StringUtils::format("お金 %10ld G", this->_selectInventory.getGold());
    this->_goldTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), goldText);
    this->_goldTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    this->_goldTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    // 枠
    auto goldLayerWaku = CommonWindowUtil::createWindowWaku(Size(contentSize.width/2, this->_goldTextLabel->getContentSize().height + this->_goldTextLabel->getSystemFontSize()/2) + Size(0, 2));
    // ベースレイヤー
    auto goldLayer = LayerColor::create(Color4B::BLACK);
    goldLayer->setContentSize(Size(contentSize.width/2, goldLayerWaku->getContentSize().height));
    goldLayer->setPosition(Point(contentSize.width + 2 - goldLayerWaku->getContentSize().width, contentSize.height));
    
    // ラベル
    this->_goldTextLabel->setPosition(Point(goldLayer->getContentSize().width /2, goldLayer->getContentSize().height / 2));
    
    goldLayer->addChild(this->_goldTextLabel);
    goldLayer->addChild(goldLayerWaku);
    this->_itemWindowLayer->addChild(goldLayer);
    
    ///////////////////////////////////////
    // ヘッダー
    cocos2d::Vector<MenuItem*> menuItemArray;
    
    // タブ作成
    for (auto inventory : this->_itemInventoryList) {
        std::string inventoryName = inventory.getName();
        auto label = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), inventory.createTabName());
        this->_menuItemMap.insert(inventoryName, label);
        
        auto tabMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(label, Size(12, 4), [this, inventoryName](Ref* ref) {
            CCLOG("TabName = %s", inventoryName.c_str());
            MenuItemUtil::touchItemRefreshColor(ref, Color3B::GREEN);
            this->changeTab(inventoryName);
        });
        if (selectTabName == inventory.getName()) {
            tabMenuItemLabel->setColor(Color3B::GREEN);
        }
        menuItemArray.pushBack(tabMenuItemLabel);
    }
    
    auto headerMenu = Menu::createWithArray(menuItemArray);
    headerMenu->alignItemsHorizontallyWithPadding(2);
    Point menuItemHeaderPoint = Point::ZERO;
    if (headerMenu->getChildrenCount() > 0) {
        Size menuItemSize = headerMenu->getChildren().at(0)->getContentSize();
        menuItemHeaderPoint = Point(headerMenu->getChildrenCount() * menuItemSize.width / 2, this->_itemWindowLayer->getContentSize().height + menuItemSize.height/2);
    }
    headerMenu->setPosition(menuItemHeaderPoint);
    this->_itemWindowLayer->addChild(headerMenu);
}

void ItemInventoryLayer::initDetail()
{
    Size contentSize = Director::getInstance()->getWinSize() * ItemInventoryLayer::WINDOW_SIZE_SCALE;
    
    ////////////////////////////
    // 内容
    this->_itemWindowLayer->setItemInventory(this->_selectInventory);
    this->_itemWindowLayer->reloadItemList();
    
    this->_itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(this->_itemWindowLayer->getContentSize(), contentSize));
}

void ItemInventoryLayer::initFooter()
{
    Size contentSize = Director::getInstance()->getWinSize() * ItemInventoryLayer::WINDOW_SIZE_SCALE;
    ///////////////////////////////////////
    // フッター
    
    // 並び替えボタン
    auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "並び替え"), Size(12, 4), [this](Ref* ref) {
        // 並び替え
        this->_itemWindowLayer->sortWeaponWithAccessory();
        this->_itemWindowLayer->reloadItemList();
        if (this->_sortCallback) {
            this->_sortCallback(ItemDto::compare_dropItem_weapon_with_accessory);
        }
    });
    
    // 閉じるボタン
    auto clone_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "とじる"), Size(12, 4), [this](Ref* ref) {
        this->close();
    });
    
    ///////////////////////////
    // Menu
    auto menu = Menu::create(sort_menu_item_label, clone_menu_item_label, NULL);
    Point menuCenterPoint = createMenuCenterPoint(menu);
    menu->setPosition(Point(contentSize.width - menuCenterPoint.x,
                            (clone_menu_item_label->getContentSize().height * -1) + menuCenterPoint.y));
    menu->alignItemsHorizontallyWithPadding(0);
    
    this->_itemWindowLayer->addChild(menu);
    this->_itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(this->_itemWindowLayer, this));
    this->addChild(this->_itemWindowLayer);
}

void ItemInventoryLayer::initMenuActionCallback(std::list<ActionCallback> actionCallbackList)
{
    std::list<ItemWindowLayer::ItemWindowMenuType> menuTypeList;
    for (auto actionCallback : actionCallbackList) {
        menuTypeList.push_back(actionCallback._actionType);
    }
    this->_itemWindowLayer->initCreateMenu(menuTypeList);
    
    this->_itemWindowLayer->setItemMenuCallback([this, actionCallbackList](ItemWindowLayer::ItemWindowMenuType menuType, Ref* ref, const ItemDto &itemDto) {
        if (ItemInventoryLayer::searchCallbackFire(actionCallbackList, ref, itemDto, menuType)) {
            this->close();
        }
    });
}

void ItemInventoryLayer::changeTab(const std::string &inventoryName)
{
    for (auto selectInventory : this->_itemInventoryList) {
        this->_menuItemMap.at(selectInventory.getName())->setString(selectInventory.createTabName());
        if (selectInventory.getName() == inventoryName) {
            this->_selectInventory = selectInventory;
        }
    }
    this->refreshInventory();
}

void ItemInventoryLayer::refreshInventory()
{
    std::string goldText = GameCore::StringUtils::format("お金 %10ld G", this->_selectInventory.getGold());
    this->_goldTextLabel->setString(goldText);
    this->_itemWindowLayer->setItemInventory(this->_selectInventory);
    this->_itemWindowLayer->reloadItemList();
}

void ItemInventoryLayer::refresh(const ItemInventoryList &itemInventoryList)
{
    this->_itemInventoryList = itemInventoryList;
    this->changeTab(this->_selectInventory.getName());
}

void ItemInventoryLayer::refresh(const ItemInventoryList &itemInventoryList, const ItemInventoryDto &selectInventory)
{
    this->_itemInventoryList = itemInventoryList;
    this->_selectInventory = selectInventory;
    this->changeTab(this->_selectInventory.getName());
}

bool ItemInventoryLayer::searchCallbackFire(std::list<ActionCallback> actionCallbackList,
                                                   Ref* ref,
                                                   const ItemDto &itemDto,
                                                   ItemWindowLayer::ItemWindowMenuType fireActionType)
{
    auto it = std::find_if(actionCallbackList.begin(), actionCallbackList.end(), [fireActionType](ActionCallback actionCallback) -> bool {
        if (actionCallback._actionType == fireActionType) {
            return true;
        }
        return false;
    });
    (*it)._callback(fireActionType, ref, itemDto);
    
    if ((*it)._closeType == ItemInventoryLayer::CloseType::CLOSE) {
        return true;
    }
    return false;
}

Point ItemInventoryLayer::createMenuCenterPoint(Menu* menu)
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

void ItemInventoryLayer::close()
{
    this->setVisible(false);
    this->removeAllChildrenWithCleanup(true);
    if (this->_closeCallback) {
        this->_closeCallback();
    }
}

