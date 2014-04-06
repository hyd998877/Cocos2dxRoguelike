//
//  ItemWindowLayer.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//
#include "AppMacros.h"
#include "CommonWindowUtil.h"

#include "ItemWindowLayer.h"
#include "TableViewTestLayer.h"
#include "DropItemSprite.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"
#include "MUseItemDao.h"

USING_NS_CC;

#define ITEM_LAYER_NAME_DEFAULT "左のリストを選択すると\nここにアイテム名が\n表示されます"
#define ITEM_LAYER_DETAIL_DEFAULT "左のリストを選択すると\nここにアイテムの説明が\n表示されます"
#define ITEM_LAYER_PARAM_DEFAULT "ぱらめーたが表示されます"

ItemWindowLayer::ItemWindowLayer()
:item_dto_list_(std::list<DropItemSprite::DropItemDto>()),
show_item_detail_idx_(-1),
item_drop_menu_callback_(nullptr),
item_use_menu_callback_(nullptr)
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

    // アイテム一覧レイヤー（左側）
    std::list<TableViewTestLayer::TableLayout> itemNameList;
    for (DropItemSprite::DropItemDto dropItem : item_dto_list_) {
        TableViewTestLayer::TableLayout layout = {StringUtils::format("item_%d.png", dropItem.imageResId), dropItem.name};
        
        itemNameList.push_back(layout);
    }
    
    auto pItemListLayer = TableViewTestLayer::createWithTextArray(itemNameList,
                                                                  Size(contentSize.width / 2, contentSize.height));
    pItemListLayer->setColor(Color3B::BLACK);
    pItemListLayer->setOpacity(128);
    pItemListLayer->setPosition(Point(contentSize.width / 2 - pItemListLayer->getContentSize().width - padding,
                                      contentSize.height / 2 - pItemListLayer->getContentSize().height / 2));
    pItemListLayer->setCallback([this](Ref *pObject, long touchedIdx) {
        // 行選択時
        CCLOG(" touched idx = %ld", touchedIdx);
        // 同じ行連打は無視
        if (show_item_detail_idx_ == touchedIdx) {
            return;
        }
        // touched DropItemDto
        auto it = item_dto_list_.begin();
        std::advance(it, touchedIdx);
        auto dropItemDto = (DropItemSprite::DropItemDto) *it;
        
        this->setItemDetail(&dropItemDto);
        
        // 表示indexを更新
        show_item_detail_idx_ = touchedIdx;
    });
    auto pItemItemListWaku = CommonWindowUtil::createWindowWaku(pItemListLayer);
    pItemListLayer->addChild(pItemItemListWaku);
    
    pItemListLayer->setTag(ItemWindowLayer::ItemTableLayerTag);
    this->addChild(pItemListLayer);
    
    // アイテム詳細レイヤー（右側）
    auto pItemDetailLayer = LayerColor::create();
    pItemDetailLayer->setColor(Color3B::BLACK);
    pItemDetailLayer->setOpacity(128);
    pItemDetailLayer->setContentSize(Size(contentSize.width / 2, contentSize.height));
    pItemDetailLayer->setPosition(Point(contentSize.width / 2 + padding, 0));
    pItemDetailLayer->setTag(ItemDetailLayerTag);
    
    const int title_font_size = 20;
    const int detail_font_size = 20;
    // アイテム名
    auto pItemNameTitleLabel = createDetailTitleLabel(pItemDetailLayer, "なまえ", title_font_size, 1.00f);
    pItemDetailLayer->addChild(pItemNameTitleLabel);
    // アイテム名 内容
    auto pItemNameLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_NAME_DEFAULT, detail_font_size, 0.80f);
    pItemNameLabel->setTag(ItemWindowLayer::ItemNameTag);
    pItemDetailLayer->addChild(pItemNameLabel);
    
    // アイテム説明
    auto pItemDetailTitleLabel = createDetailTitleLabel(pItemDetailLayer, "せつめい", title_font_size, 0.70f);
    pItemDetailLayer->addChild(pItemDetailTitleLabel);
    // アイテム説明 内容
    auto pItemDetailLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_DETAIL_DEFAULT, detail_font_size, 0.50f);
    pItemDetailLabel->setTag(ItemWindowLayer::ItemDetailTag);
    pItemDetailLayer->addChild(pItemDetailLabel);
    
    // アイテムパラメータ
    auto paramTitleLabel = createDetailTitleLabel(pItemDetailLayer, "つよさ", title_font_size, 0.40f);
    pItemDetailLayer->addChild(paramTitleLabel);
    // アイテムパラメータ 内容
    auto paramTextLabel = createDetailTextLabel(pItemDetailLayer, ITEM_LAYER_PARAM_DEFAULT, detail_font_size, 0.25f);
    paramTextLabel->setTag(ItemWindowLayer::ItemParamTag);
    pItemDetailLayer->addChild(paramTextLabel);
    
    // メニュー
    auto pMenu = initCreateMenu();
    pMenu->setPositionX(pItemDetailLayer->getContentSize().width / 2);
    pItemDetailLayer->addChild(pMenu);
    
    // 枠線
    auto pItemDetailWaku = CommonWindowUtil::createWindowWaku(pItemDetailLayer);
    pItemDetailWaku->setPreferredSize(pItemDetailLayer->getContentSize());
    pItemDetailWaku->setPosition(Point(pItemDetailWaku->getContentSize().width / 2, pItemDetailWaku->getContentSize().height / 2));
    pItemDetailLayer->addChild(pItemDetailWaku);
    this->addChild(pItemDetailLayer);
    
    // アイテム個数
    auto item_count_label = Label::create(StringUtils::format("%d/%d", 0, USE_ITEM_MAX), GAME_FONT(title_font_size), GAME_FONT_SIZE(title_font_size));
    item_count_label->setPosition(Point(item_count_label->getContentSize().width / 2, item_count_label->getContentSize().height / 2 + this->getContentSize().height));
    item_count_label->setTag(ItemWindowLayer::ItemCountLabelTag);
    this->addChild(item_count_label);
    
    return true;
}

// pointProportion （height * 0.5とかのやつ）
Label* ItemWindowLayer::createDetailTitleLabel(const Node* base, std::string text, float fontSize, float heightPointProportion) {
    auto titleLabel = Label::create(text, GAME_FONT(fontSize), GAME_FONT_SIZE(fontSize));
    titleLabel->setColor(Color3B::GRAY);
    titleLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    titleLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    titleLabel->setPosition(Point(
                                            titleLabel->getContentSize().width / 2 + titleLabel->getFontSize() / 2,
                                            base->getContentSize().height * heightPointProportion - titleLabel->getContentSize().height / 2 - titleLabel->getFontSize() / 2));
    return titleLabel;
}

// pointProportion （height * 0.5とかのやつ）
Label* ItemWindowLayer::createDetailTextLabel(const Node* base, std::string text, float fontSize, float heightPointProportion) {
    auto textLabel = Label::create(text, GAME_FONT(fontSize), GAME_FONT_SIZE(fontSize));
    textLabel->setColor(Color3B::WHITE);
    textLabel->setPosition(Point(base->getContentSize().width / 2, base->getContentSize().height * heightPointProportion));
    return textLabel;
}

Menu* ItemWindowLayer::initCreateMenu() {
    
    const int font_size = 20;
    
    // -----------------------------
    // メニューボタン
    const Size WAKU_PADDING = Size(8, 4);
    
    // 捨てるボタン
    auto pMenuItemDrop = CommonWindowUtil::createMenuItemLabelWaku(Label::create("すてる", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), WAKU_PADDING, [this](Ref *pSeneder) {
        
        // hoge
        CCLOG("item drop menu pressed");
        if (show_item_detail_idx_ < 0) {
            return;
        }
        if (item_drop_menu_callback_) {
            auto it = item_dto_list_.begin();
            std::advance(it, show_item_detail_idx_);
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            // 削除
            item_dto_list_.erase(it);
            
            item_drop_menu_callback_(pSeneder, dropItemDto);
        }
    });
    
    pMenuItemDrop->setTag(ItemWindowLayer::ItemDetailMenuDropTag);
    
    // 使用ボタン
    auto pMenuItemUse = CommonWindowUtil::createMenuItemLabelWaku(Label::create("つかう", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), WAKU_PADDING, [this](Ref *pSeneder) {
        
        // hoge
        CCLOG("item use menu pressed");
        if (show_item_detail_idx_ < 0) {
            return;
        }
        if (item_use_menu_callback_) {
            auto it = item_dto_list_.begin();
            std::advance(it, show_item_detail_idx_);
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            // 削除
            item_dto_list_.erase(it);
            
            item_use_menu_callback_(pSeneder, dropItemDto);
        }
    });
    pMenuItemUse->setTag(ItemWindowLayer::ItemDetailMenuUseTag);
    
    // 装備（する/はずす）ボタン
    auto pMenuItemEquip = CommonWindowUtil::createMenuItemLabelWaku(Label::create("そうび", GAME_FONT(font_size), GAME_FONT_SIZE(font_size)), WAKU_PADDING, [this](Ref *pSeneder) {
        
        CCLOG("item equip menu pressed");
        if (show_item_detail_idx_ < 0) {
            return;
        }
        if (item_Equip_Menu_Callback_) {
            auto it = item_dto_list_.begin();
            std::advance(it, show_item_detail_idx_);
            // ステータスを装備状態を反転
            it->isEquip = !it->isEquip;
            
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            item_Equip_Menu_Callback_(pSeneder, dropItemDto);
        }
    });
    pMenuItemEquip->setTag(ItemWindowLayer::ItemDetailMenuEquipTag);
    
    auto pMenu = Menu::create(pMenuItemDrop, pMenuItemUse, pMenuItemEquip, NULL);
    pMenu->setTag(ItemWindowLayer::ItemDetailMenuTag);
    pMenu->setPosition(Point(0, pMenuItemDrop->getContentSize().height));
    pMenu->alignItemsHorizontallyWithPadding(4);
    
    return pMenu;
}

#pragma mark
#pragma mark itemList関連

DropItemSprite::DropItemDto ItemWindowLayer::findItem(long itemListIndex) {
    if (itemListIndex >= 0 && item_dto_list_.size() > itemListIndex) {
        auto it = item_dto_list_.begin();
        std::advance(it, itemListIndex);
        auto dropItemDto = (DropItemSprite::DropItemDto) *it;
        return dropItemDto;
    }
    return {0, 0, MUseItem::ItemType::NONE, 0, "", false};
}

void ItemWindowLayer::addItemList(DropItemSprite::DropItemDto dropItemDto) {
    item_dto_list_.push_back(dropItemDto);
}

// 指定したアイテムを装備状態を変更する
void ItemWindowLayer::setItemEquip(long objectId, bool isEquip) {
    for (DropItemSprite::DropItemDto& itemDto : item_dto_list_) {
        if (itemDto.objectId == objectId) {
            // TODO: とりあえず装備だけ
            itemDto.isEquip = isEquip;
            break;
        }
    }
}

void ItemWindowLayer::reloadItemList() {
    // 選択をリセット
    show_item_detail_idx_ = -1;
    
    auto pItemTabelLayer = static_cast<TableViewTestLayer*>(getChildByTag(ItemWindowLayer::ItemTableLayerTag));
    if (pItemTabelLayer) {
        
        // 装備ソート
        item_dto_list_.sort(DropItemSprite::DropItemDto::compare_dropItem_equip);
        
        std::list<TableViewTestLayer::TableLayout> itemNameList;
        
        for (DropItemSprite::DropItemDto dropItem : item_dto_list_) {
            TableViewTestLayer::TableLayout layout = {
                DropItemSprite::createItemImageFileName(dropItem.imageResId),
                dropItem.name,
                dropItem.isEquip ? Color3B::YELLOW : Color3B::WHITE // 装備中は黄文字
            };
            itemNameList.push_back(layout);
        }
        pItemTabelLayer->makeItemList(itemNameList);
        
        auto item_count_label = static_cast<Label*>(this->getChildByTag(ItemWindowLayer::ItemCountLabelTag));
        if (item_count_label) {
            item_count_label->setString(StringUtils::format("所持数 %d/%d", (int)item_dto_list_.size(), USE_ITEM_MAX));
            item_count_label->setPosition(Point(item_count_label->getContentSize().width / 2, item_count_label->getContentSize().height / 2 + this->getContentSize().height));
            if (item_dto_list_.size() >= USE_ITEM_MAX) {
                item_count_label->setColor(Color3B::RED);
            } else {
                item_count_label->setColor(Color3B::WHITE);
            }
        }
    }
    setItemDetail(show_item_detail_idx_);
}

void ItemWindowLayer::sortItemList() {
    // ソート
    item_dto_list_.sort(DropItemSprite::DropItemDto::compare_dropItem_weapon_with_accessory);
}

#pragma mark
#pragma mark privateメソッド

// アイテム選択時の処理
void ItemWindowLayer::setItemDetail(long itemListIndex) {
    DropItemSprite::DropItemDto dropItemDto = findItem(itemListIndex);
    setItemDetail(&dropItemDto);
}

// アイテム選択時の処理
// アイテム名とか説明とかパラメータを右側に表示する
void ItemWindowLayer::setItemDetail(DropItemSprite::DropItemDto* pDropItemDto) {
    if (!pDropItemDto) {
        return;
    }
    
    auto pItemDetailLayer = this->getChildByTag(ItemWindowLayer::ItemDetailLayerTag);
    if (pItemDetailLayer) {
        // name
        auto pItemNameLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemNameTag));
        if (pItemNameLabel) {
            if (pDropItemDto->name.empty()) {
                pItemNameLabel->setString(ITEM_LAYER_NAME_DEFAULT);
            } else {
                // TODO: とりあえず装備中で文言かえる
                if (pDropItemDto->isEquip) {
                    pItemNameLabel->setString(StringUtils::format("%s(装備中)", pDropItemDto->name.c_str()));
                } else {
                    pItemNameLabel->setString(pDropItemDto->name);
                }
            }
        }
        
        // detail
        auto pItemDetailLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemDetailTag));
        if (pItemDetailLabel) {
            if (pDropItemDto->name.empty()) {
                pItemDetailLabel->setString(ITEM_LAYER_DETAIL_DEFAULT);
            } else {
                if (pDropItemDto->itemType == MUseItem::ItemType::EQUIP_WEAPON) {
                    MWeapon weapon = MWeaponDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemDetailLabel->setString(weapon.getWeaponDetail());
                } else if (pDropItemDto->itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    MAccessory accessory = MAccessoryDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemDetailLabel->setString(accessory.getAccessoryDetail());
                } else if (pDropItemDto->itemType != MUseItem::ItemType::NONE) {
                    MUseItem useItem = MUseItemDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemDetailLabel->setString(useItem.getUseItemDetail());
                }
            }
        }
        
        // param
        auto pItemParamlLabel = static_cast<Label*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemParamTag));
        if (pItemParamlLabel) {
            if (pDropItemDto->name.empty()) {
                pItemParamlLabel->setString(ITEM_LAYER_PARAM_DEFAULT);
            } else {
                if (pDropItemDto->itemType == MUseItem::ItemType::EQUIP_WEAPON) {
                    MWeapon weapon = MWeaponDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemParamlLabel->setString(StringUtils::format("攻撃力: %3d", weapon.getAttackPoint()));
                } else if (pDropItemDto->itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                    MAccessory accessory = MAccessoryDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemParamlLabel->setString(StringUtils::format("防御力: %3d", accessory.getDefensePoint()));
                } else if (pDropItemDto->itemType != MUseItem::ItemType::NONE) {
                    MUseItem useItem = MUseItemDao::getInstance()->selectById(pDropItemDto->itemId);
                    pItemParamlLabel->setString(StringUtils::format("効果: %3d", useItem.getUseItemParam()));
                }
            }
        }
        
        // menu
        auto pItemDetailMenu = static_cast<Menu*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemDetailMenuTag));
        if (pItemDetailMenu) {
            auto pMenuUse = static_cast<MenuItem*>(pItemDetailMenu->getChildByTag(ItemWindowLayer::ItemDetailMenuUseTag));
            auto pMenuDrop = static_cast<MenuItem*>(pItemDetailMenu->getChildByTag(ItemWindowLayer::ItemDetailMenuDropTag));
            auto pMenuEquip = static_cast<MenuItemLabel*>(pItemDetailMenu->getChildByTag(ItemWindowLayer::ItemDetailMenuEquipTag));
            
            // 未指定
            if (pDropItemDto->itemType == MUseItem::ItemType::NONE) {
                pMenuUse->setEnabled(false);pMenuUse->setVisible(false);
                pMenuDrop->setEnabled(false);pMenuDrop->setVisible(false);
                pMenuEquip->setEnabled(false);pMenuEquip->setVisible(false);
            } else if (pDropItemDto->itemType == MUseItem::ItemType::EQUIP_WEAPON ||
                       pDropItemDto->itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
                // 装備可能・置く
                pMenuUse->setEnabled(false);pMenuUse->setVisible(false);
                pMenuDrop->setEnabled(true);pMenuDrop->setVisible(true);
                pMenuEquip->setEnabled(true);pMenuEquip->setVisible(true);
                
                // MenuItemLabelのsetStringを行うとsetContentSizeされてwaku分ずれるので
                CCLOG("begore setString %f %f", pMenuEquip->getPosition().x, pMenuEquip->getPosition().y);
                Size beforeSize = pMenuEquip->getContentSize();
                if (pDropItemDto->isEquip) {
                    pMenuEquip->setString("はずす");
                } else {
                    pMenuEquip->setString("そうび");
                }
                pMenuEquip->setContentSize(beforeSize);
                CCLOG("after setString %f %f", pMenuEquip->getPosition().x, pMenuEquip->getPosition().y);
            } else {
                pMenuUse->setEnabled(true);pMenuUse->setVisible(true);
                pMenuDrop->setEnabled(true);pMenuDrop->setVisible(true);
                pMenuEquip->setEnabled(false);pMenuEquip->setVisible(false);
            }
        }
    }
}

#pragma mark
#pragma mark callback関連

void ItemWindowLayer::setItemUseMenuCallback(const ItemWindowMenuCallback& itemUseMenuCallback) {
    item_use_menu_callback_ = itemUseMenuCallback;
}

void ItemWindowLayer::setItemDropMenuCallback(const ItemWindowMenuCallback& itemDropMenuCallback) {
    item_drop_menu_callback_ = itemDropMenuCallback;
}
void ItemWindowLayer::setItemEquipMenuCallback(const ItemWindowMenuCallback& itemEquipMenuCallback) {
    item_Equip_Menu_Callback_ = itemEquipMenuCallback;
}

