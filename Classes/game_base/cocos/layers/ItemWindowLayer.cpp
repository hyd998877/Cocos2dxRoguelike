//
//  ItemWindowLayer.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//
#include "AppMacros.h"
#include "MRogueMap.h"
#include "CommonWindowUtil.h"

#include "ItemWindowLayer.h"
#include "TableViewTestLayer.h"
#include "ItemDto.h"

#include "ItemLogic.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"
#include "MUseItemDao.h"

USING_NS_CC;

#define ITEM_LAYER_NAME_DEFAULT "左のリストを選択すると\nここにアイテム名が\n表示されます"
#define ITEM_LAYER_DETAIL_DEFAULT "左のリストを選択すると\nここにアイテムの説明が\n表示されます"
#define ITEM_LAYER_PARAM_DEFAULT "ぱらめーたが表示されます"

ItemWindowLayer::ItemWindowLayer()
: _itemInventoryDto()
, show_item_detail_idx_(-1)
, _itemMenuCallback(nullptr)
{
}

ItemWindowLayer::~ItemWindowLayer()
{
}

ItemWindowLayer* ItemWindowLayer::createWithContentSize(Size contentSize) {
    ItemWindowLayer *pRet = new ItemWindowLayer();
    if (pRet && pRet->initWithContentSize(contentSize)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

#pragma mark
#pragma mark 初期処理

bool ItemWindowLayer::initWithContentSize(Size contentSize) {
    if (!LayerColor::init()) {
        return false;
    }
    setContentSize(contentSize);
    
    int padding = 2;

    std::list<ItemDto> itemList = this->_itemInventoryDto.getItemList();
    
    // アイテム一覧レイヤー（左側）
    std::list<TableViewTestLayer::TableLayout> itemNameList;
    for (auto itemDto : itemList) {
        TableViewTestLayer::TableLayout layout = {cocos2d::StringUtils::format("item_%d.png", itemDto.getImageResId()), itemDto.getName()};
        
        itemNameList.push_back(layout);
    }
    
    auto pItemListLayer = TableViewTestLayer::createWithTextArray(itemNameList,
                                                                  Size(contentSize.width / 2, contentSize.height));
    pItemListLayer->setColor(Color3B::BLACK);
    pItemListLayer->setOpacity(0);
    pItemListLayer->setPosition(Point(contentSize.width / 2 - pItemListLayer->getContentSize().width - padding,
                                      contentSize.height / 2 - pItemListLayer->getContentSize().height / 2));
    pItemListLayer->setCallback([this, itemList](Ref *pObject, long touchedIdx) {
        // 行選択時
        CCLOG(" touched idx = %ld", touchedIdx);
        // 同じ行連打は無視
        if (show_item_detail_idx_ == touchedIdx) {
            return;
        }
        // 表示indexを更新
        this->show_item_detail_idx_ = touchedIdx;
        
        auto itemDto = this->findItem(this->show_item_detail_idx_);
        this->setItemDetail(itemDto);
    });
    auto pItemItemListWaku = CommonWindowUtil::createWindowWaku(pItemListLayer);
    pItemListLayer->addChild(pItemItemListWaku);
    
    pItemListLayer->setTag(ItemWindowLayer::ItemTableLayerTag);
    this->addChild(pItemListLayer);
    
    // アイテム詳細レイヤー（右側）
    auto pItemDetailLayer = LayerColor::create();
    pItemDetailLayer->setColor(Color3B::BLACK);
    pItemDetailLayer->setOpacity(0);
    pItemDetailLayer->setContentSize(Size(contentSize.width / 2, contentSize.height));
    pItemDetailLayer->setPosition(Point(contentSize.width / 2 + padding, 0));
    pItemDetailLayer->setTag(ItemDetailLayerTag);
    
    // アイテム名
    auto pItemNameTitleLabel = createDetailTitleLabel(pItemDetailLayer, "なまえ", 1.00f);
    pItemDetailLayer->addChild(pItemNameTitleLabel);
    // アイテム名 内容
    auto pItemNameLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_NAME_DEFAULT, 0.80f);
    pItemNameLabel->setTag(ItemWindowLayer::ItemNameTag);
    pItemDetailLayer->addChild(pItemNameLabel);
    
    // アイテム説明
    auto pItemDetailTitleLabel = createDetailTitleLabel(pItemDetailLayer, "せつめい", 0.70f);
    pItemDetailLayer->addChild(pItemDetailTitleLabel);
    // アイテム説明 内容
    auto pItemDetailLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_DETAIL_DEFAULT, 0.50f);
    pItemDetailLabel->setTag(ItemWindowLayer::ItemDetailTag);
    pItemDetailLayer->addChild(pItemDetailLabel);
    
    // アイテムパラメータ
    auto paramTitleLabel = createDetailTitleLabel(pItemDetailLayer, "つよさ", 0.40f);
    pItemDetailLayer->addChild(paramTitleLabel);
    // アイテムパラメータ 内容
    auto paramTextLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_PARAM_DEFAULT, 0.25f);
    paramTextLabel->setTag(ItemWindowLayer::ItemParamTag);
    pItemDetailLayer->addChild(paramTextLabel);
    
    // 枠線
    auto pItemDetailWaku = CommonWindowUtil::createWindowWaku(pItemDetailLayer);
    pItemDetailWaku->setPreferredSize(pItemDetailLayer->getContentSize());
    pItemDetailWaku->setPosition(Point(pItemDetailWaku->getContentSize().width / 2, pItemDetailWaku->getContentSize().height / 2));
    pItemDetailLayer->addChild(pItemDetailWaku);
    this->addChild(pItemDetailLayer);
    
    return true;
}

// pointProportion （height * 0.5とかのやつ）
Label* ItemWindowLayer::createDetailTitleLabel(const Node* base, std::string text, float heightPointProportion) {
    auto titleLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), text);
    titleLabel->setColor(Color3B::GRAY);
    titleLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    titleLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    titleLabel->setPosition(Point(titleLabel->getContentSize().width / 2 + titleLabel->getSystemFontSize() / 2,
                                  base->getContentSize().height * heightPointProportion - titleLabel->getContentSize().height / 2 - titleLabel->getSystemFontSize() / 2));
    return titleLabel;
}

// pointProportion （height * 0.5とかのやつ）
Label* ItemWindowLayer::createDetailTextLabel(const Node* base, std::string text, float heightPointProportion) {
    auto textLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), text);
    textLabel->setColor(Color3B::WHITE);
    textLabel->setPosition(Point(base->getContentSize().width / 2, base->getContentSize().height * heightPointProportion));
    return textLabel;
}

void ItemWindowLayer::initCreateMenu(std::list<ItemWindowLayer::ItemWindowMenuType> menuTypeList)
{
    cocos2d::Vector<MenuItem*> menuItemArray;
    
    const Size WAKU_PADDING = Size(8, 4);
    // -----------------------------
    // メニューボタン
    for (auto menuType : menuTypeList) {
        
        std::string labelText = getMenuItemKeyText(menuType);
        auto label = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), labelText);
        auto menuItem = CommonWindowUtil::createMenuItemLabelWaku(label, WAKU_PADDING, [this, menuType](Ref *pSeneder) {
            CCLOG("item drop menu pressed");
            if (show_item_detail_idx_ < 0) {
                return;
            }
            if (this->_itemMenuCallback) {
                auto itemDto = this->findItem(show_item_detail_idx_);
                this->_itemMenuCallback(menuType, pSeneder, itemDto);
            }
        });
        menuItem->setVisible(false);
        menuItem->setEnabled(false);
        this->_menuItemMap.insert(menuType, menuItem);
        menuItemArray.pushBack(menuItem);
    }
    
    this->_menu = Menu::createWithArray(menuItemArray);
    // 平均サイズ
    float height = 0.0f;
    for (auto node : this->_menu->getChildren()) {
        height += node->getContentSize().height;
    }
    this->_menu->setPosition(Point(0, height / this->_menu->getChildrenCount()));
    this->_menu->alignItemsHorizontallyWithPadding(4);
    
    auto pItemDetailLayer = this->getChildByTag(ItemWindowLayer::ItemDetailLayerTag);
    // メニュー
    this->_menu->setPositionX(pItemDetailLayer->getContentSize().width / 2);
    pItemDetailLayer->addChild(this->_menu);
}

#pragma mark
#pragma mark itemList関連

ItemDto ItemWindowLayer::findItem(long itemListIndex) const
{
    auto itemList = this->_itemInventoryDto.getItemList();
    if (itemListIndex >= 0 && itemList.size() > itemListIndex) {
        auto it = itemList.begin();
        std::advance(it, itemListIndex);
        auto dropItemDto = static_cast<ItemDto>(*it);
        return dropItemDto;
    }
    return ItemDto();
}

void ItemWindowLayer::reloadItemList() {
    // 選択をリセット
    show_item_detail_idx_ = -1;
    
    auto pItemTabelLayer = static_cast<TableViewTestLayer*>(getChildByTag(ItemWindowLayer::ItemTableLayerTag));
    if (pItemTabelLayer) {
        // 装備ソート
        this->_itemInventoryDto.sortItemList();
        auto itemList = this->_itemInventoryDto.getItemList();
        
        std::list<TableViewTestLayer::TableLayout> itemNameList;
        
        for (auto itemDto : itemList) {
            if (itemDto.getObjectId() <= 0) {
                continue;
            }
            TableViewTestLayer::TableLayout layout = {
                ItemLogic::createItemImageFileName(itemDto.getImageResId()),
                itemDto.createItemName(),
                itemDto.isEquip() ? Color3B::YELLOW : Color3B::WHITE // 装備中は黄文字
            };
            itemNameList.push_back(layout);
        }
        pItemTabelLayer->makeItemList(itemNameList);
        
        auto item_count_label = static_cast<Label*>(this->getChildByTag(ItemWindowLayer::ItemCountLabelTag));
        if (item_count_label) {
            item_count_label->setString(cocos2d::StringUtils::format("所持数 %d/%d", (int)itemList.size(), RogueGameConfig::USE_ITEM_MAX));
            item_count_label->setPosition(Point(item_count_label->getContentSize().width / 2, item_count_label->getContentSize().height / 2 + this->getContentSize().height));
            if (itemList.size() >= RogueGameConfig::USE_ITEM_MAX) {
                item_count_label->setColor(Color3B::RED);
            } else {
                item_count_label->setColor(Color3B::WHITE);
            }
        }
    }
    setItemDetail(show_item_detail_idx_);
}

void ItemWindowLayer::sortWeaponWithAccessory()
{
    this->_itemInventoryDto.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
}

#pragma mark
#pragma mark privateメソッド

// アイテム選択時の処理
void ItemWindowLayer::setItemDetail(long itemListIndex) {
    ItemDto itemDto = findItem(itemListIndex);
    setItemDetail(itemDto);
}

// アイテム選択時の処理
// アイテム名とか説明とかパラメータを右側に表示する
void ItemWindowLayer::setItemDetail(const ItemDto& itemDto) {
    auto pItemDetailLayer = this->getChildByTag(ItemWindowLayer::ItemDetailLayerTag);
    if (pItemDetailLayer) {
        // name
        auto pItemNameLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemNameTag));
        if (pItemNameLabel) {
            if (itemDto.getName().empty()) {
                pItemNameLabel->setString(ITEM_LAYER_NAME_DEFAULT);
            } else {
                // TODO: とりあえず装備中で文言かえる
                std::string itemNameText = itemDto.createItemName();
                if (itemDto.isEquip()) {
                    pItemNameLabel->setString(cocos2d::StringUtils::format("%s (装備中)", itemNameText.c_str()));
                } else {
                    pItemNameLabel->setString(itemNameText);
                }
            }
        }
        
        // detail
        auto pItemDetailLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemDetailTag));
        if (pItemDetailLabel) {
            if (itemDto.getName().empty()) {
                pItemDetailLabel->setString(ITEM_LAYER_DETAIL_DEFAULT);
            } else {
                if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON) {
                    MWeapon weapon = MWeaponDao::getInstance()->selectById(itemDto.getItemId());
                    pItemDetailLabel->setString(weapon.getWeaponDetail());
                } else if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    MAccessory accessory = MAccessoryDao::getInstance()->selectById(itemDto.getItemId());
                    pItemDetailLabel->setString(accessory.getAccessoryDetail());
                } else if (itemDto.getItemType() != MUseItem::ItemType::NONE) {
                    MUseItem useItem = MUseItemDao::getInstance()->selectById(itemDto.getItemId());
                    pItemDetailLabel->setString(useItem.getUseItemDetail());
                }
            }
        }
        
        // param
        auto pItemParamlLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemParamTag));
        if (pItemParamlLabel) {
            if (itemDto.getName().empty()) {
                pItemParamlLabel->setString(ITEM_LAYER_PARAM_DEFAULT);
            } else {
                if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON) {
                    MWeapon weapon = MWeaponDao::getInstance()->selectById(itemDto.getItemId());
                    int totalParam = weapon.getAttackPoint() + itemDto.getParam();
                    pItemParamlLabel->setString(cocos2d::StringUtils::format("攻撃力: %3d", totalParam));
                } else if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    MAccessory accessory = MAccessoryDao::getInstance()->selectById(itemDto.getItemId());
                    int totalParam = accessory.getDefensePoint() + itemDto.getParam();
                    pItemParamlLabel->setString(cocos2d::StringUtils::format("防御力: %3d", totalParam));
                } else if (itemDto.getItemType() != MUseItem::ItemType::NONE) {
                    MUseItem useItem = MUseItemDao::getInstance()->selectById(itemDto.getItemId());
                    pItemParamlLabel->setString(cocos2d::StringUtils::format("効果: %3d", useItem.getUseItemParam()));
                }
            }
        }
        
        // menu
        if (this->_menu) {
            for (auto menuItemKey : this->_menuItemMap.keys()) {
                // 未指定
                if (itemDto.getItemType() == MUseItem::ItemType::NONE) {
                    this->_menuItemMap.at(menuItemKey)->setVisible(false);
                    this->_menuItemMap.at(menuItemKey)->setEnabled(false);
                    continue;
                }
                
                this->_menuItemMap.at(menuItemKey)->setVisible(true);
                this->_menuItemMap.at(menuItemKey)->setEnabled(true);
                
                if (itemDto.getItemType() == MUseItem::ItemType::EQUIP_WEAPON ||
                           itemDto.getItemType() == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    if (menuItemKey == ItemWindowMenuType::ITEM_USE) {
                        std::string text = itemDto.isEquip() ? text = "はずす" :  text = "そうび";
                        setMenuItemTest(menuItemKey, text);
                    } else if (menuItemKey == ItemWindowMenuType::ITEM_EQUIP) {
                        // MenuItemLabelのsetStringを行うとsetContentSizeされてwaku分ずれるので補正
                        std::string text = itemDto.isEquip() ? text = "はずす" :  text = "そうび";
                        setMenuItemTest(menuItemKey, text);
                    }
                } else {
                    if (menuItemKey == ItemWindowMenuType::ITEM_USE) {
                        std::string text = getMenuItemKeyText(ItemWindowMenuType::ITEM_USE);
                        setMenuItemTest(menuItemKey, text);
                    } else if (menuItemKey == ItemWindowMenuType::ITEM_EQUIP) {
                        this->_menuItemMap.at(menuItemKey)->setVisible(false);
                        this->_menuItemMap.at(menuItemKey)->setEnabled(false);
                    }
                }
            }
        }
    }
}

void ItemWindowLayer::setMenuItemTest(int menuItemKey, const std::string& text)
{
    auto node = dynamic_cast<MenuItemLabel*>(this->_menuItemMap.at(menuItemKey));
    CCLOG("begore setString %f %f", node->getPosition().x, node->getPosition().y);
    Size beforeSize = node->getContentSize();
    node->setString(text);
    node->setContentSize(beforeSize);
}

std::string ItemWindowLayer::getMenuItemKeyText(ItemWindowMenuType menuType)
{
    std::string labelText;
    switch (menuType) {
        case ITEM_DROP:
            labelText = "すてる";
            break;
        case ITEM_CHANGE:
            labelText = "交　換";
            break;
        case ITEM_SALE:
            labelText = "売　る";
            break;
        case ITEM_STOCK:
            labelText = "出・入";
            break;
        case ITEM_USE:
            labelText = "つかう";
            break;
        case ITEM_THROW:
            labelText = "投げる";
            break;
        case ITEM_EQUIP:
            labelText = "そうび"; // はずす
            break;
        case ITEM_MIXED:
            labelText = "えらぶ";
        default:
            break;
    }
    return labelText;
}

