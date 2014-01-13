//
//  ItemWindowLayer.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2014/01/07.
//
//
#include "AppMacros.h"

#include "ItemWindowLayer.h"
#include "TableViewTestLayer.h"
#include "DropItemSprite.h"

USING_NS_CC;

#define ITEM_LAYER_NAME_DEFAULT "左のリストを選択すると\nここにアイテム名が\n表示されます"
#define ITEM_LAYER_DETAIL_DEFAULT "左のリストを選択すると\nここにアイテムの説明が\n表示されます"

ItemWindowLayer::ItemWindowLayer()
:m_itemDtoList(std::list<DropItemSprite::DropItemDto>()),
m_showItemDetailIdx(-1),
m_itemDropMenuCallback(nullptr),
m_itemUseMenuCallback(nullptr)
{
}

ItemWindowLayer* ItemWindowLayer::createWithContentSize(Size contentSize)
{
    ItemWindowLayer *pRet = new ItemWindowLayer();
    if (pRet && pRet->initWithContentSize(contentSize))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool ItemWindowLayer::initWithContentSize(Size contentSize)
{
    if (!LayerColor::init())
    {
        return false;
    }
    setContentSize(contentSize);
    
    int padding = 2;

    // アイテム一覧レイヤー（左側）
    std::list<TableViewTestLayer::TableLayout> itemNameList;
    for (DropItemSprite::DropItemDto dropItem : m_itemDtoList)
    {
        TableViewTestLayer::TableLayout layout = {StringUtils::format("item_%d.png", dropItem.imageResId), dropItem.name};
        
        itemNameList.push_back(layout);
    }
    
    auto pItemListLayer = TableViewTestLayer::createWithTextArray(itemNameList,
                                                                  Size(contentSize.width / 2, contentSize.height));
    pItemListLayer->setColor(Color3B::BLACK);
    pItemListLayer->setOpacity(128);
    pItemListLayer->setPosition(Point(
                                      contentSize.width / 2 - pItemListLayer->getContentSize().width - padding,
                                      contentSize.height / 2 - pItemListLayer->getContentSize().height / 2));
    pItemListLayer->setCallback([this](Object* pObject, long touchedIdx) {
        // 行選択時
        CCLOG(" touched idx = %ld", touchedIdx);
        // 同じ行連打は無視
        if (m_showItemDetailIdx == touchedIdx)
        {
            return;
        }
        // touched DropItemDto
        auto it = m_itemDtoList.begin();
        std::advance(it, touchedIdx);
        auto dropItemDto = (DropItemSprite::DropItemDto) *it;
        
        this->setItemDetail(&dropItemDto);
        
        // 表示indexを更新
        m_showItemDetailIdx = touchedIdx;
    });
    auto pItemItemListWaku = CREATE_WINDOW_WAKU();
    pItemItemListWaku->setPreferredSize(pItemListLayer->getContentSize());
    pItemItemListWaku->setPosition(Point(pItemItemListWaku->getContentSize().width / 2, pItemItemListWaku->getContentSize().height / 2));
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
    // アイテム名
    auto pItemNameTitleLabel = LabelTTF::create("なまえ", GAME_FONT(16), 16);
    pItemNameTitleLabel->setColor(Color3B::WHITE);
    pItemNameTitleLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    pItemNameTitleLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    pItemNameTitleLabel->setPosition(Point(
                                           pItemNameTitleLabel->getContentSize().width / 2 + pItemNameTitleLabel->getFontSize() / 2,
                                           pItemDetailLayer->getContentSize().height - pItemNameTitleLabel->getContentSize().height / 2 - pItemNameTitleLabel->getFontSize() / 2));
    pItemDetailLayer->addChild(pItemNameTitleLabel);
    
    auto pItemNameLabel = LabelTTF::create(ITEM_LAYER_NAME_DEFAULT, GAME_FONT(10), 10);
    pItemNameLabel->setColor(Color3B::WHITE);
    pItemNameLabel->setPosition(Point(
                                      pItemDetailLayer->getContentSize().width * 0.5,
                                      pItemDetailLayer->getContentSize().height * 0.75));
    pItemNameLabel->setTag(ItemWindowLayer::ItemNameTag);
    pItemDetailLayer->addChild(pItemNameLabel);
    
    // アイテム説明
    auto pItemDetailTitleLabel = LabelTTF::create("せつめい", GAME_FONT(16), 16);
    pItemDetailTitleLabel->setColor(Color3B::WHITE);
    pItemDetailTitleLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    pItemDetailTitleLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    pItemDetailTitleLabel->setPosition(Point(
                                             pItemDetailTitleLabel->getContentSize().width / 2 + pItemDetailTitleLabel->getFontSize() / 2,
                                             pItemDetailLayer->getContentSize().height / 2 - pItemDetailTitleLabel->getContentSize().height / 2 - pItemDetailTitleLabel->getFontSize() / 2));
    pItemDetailLayer->addChild(pItemDetailTitleLabel);
    
    auto pItemDetailLabel = LabelTTF::create(ITEM_LAYER_DETAIL_DEFAULT, GAME_FONT(10), 10);
    pItemDetailLabel->setColor(Color3B::WHITE);
    pItemDetailLabel->setPosition(Point(
                                        pItemDetailLayer->getContentSize().width * 0.5,
                                        pItemDetailLayer->getContentSize().height * 0.25));
    pItemDetailLabel->setTag(ItemWindowLayer::ItemDetailTag);
    pItemDetailLayer->addChild(pItemDetailLabel);
    // メニュー
    pItemDetailLayer->addChild(initCreateMenu());
    
    auto pItemDetailWaku = CREATE_WINDOW_WAKU();
    pItemDetailWaku->setPreferredSize(pItemDetailLayer->getContentSize());
    pItemDetailWaku->setPosition(Point(pItemDetailWaku->getContentSize().width / 2, pItemDetailWaku->getContentSize().height / 2));
    pItemDetailLayer->addChild(pItemDetailWaku);
    this->addChild(pItemDetailLayer);
    
    return true;
}

Menu* ItemWindowLayer::initCreateMenu()
{
    // -----------------------------
    // メニューボタン
    float addNextMenuPositonX = 4;
    float addNextMenuPositonY = 4;
    
    // 捨てるボタン
    auto pMenuItemDrop = ItemWindowLayer::createMenuItemSprite(Color3B::RED, [this](Object* pSeneder) {
        // hoge
        CCLOG("item drop menu pressed");
        if (m_showItemDetailIdx < 0)
        {
            return;
        }
        if (m_itemDropMenuCallback)
        {
            auto it = m_itemDtoList.begin();
            std::advance(it, m_showItemDetailIdx);
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            // 削除
            m_itemDtoList.erase(it);
            
            m_itemDropMenuCallback(pSeneder, dropItemDto);
        }
    });
    pMenuItemDrop->setPosition(Point(pMenuItemDrop->getContentSize().width / 2 + addNextMenuPositonX , pMenuItemDrop->getContentSize().height / 2 + addNextMenuPositonY));
    addNextMenuPositonX += pMenuItemDrop->getContentSize().width + 4;
    addNextMenuPositonY += 0;
    
    // 使用ボタン
    auto pMenuItemUse = ItemWindowLayer::createMenuItemSprite(Color3B::GRAY, [this](Object* pSeneder) {
        // hoge
        CCLOG("item use menu pressed");
        if (m_showItemDetailIdx < 0)
        {
            return;
        }
        if (m_itemUseMenuCallback)
        {
            auto it = m_itemDtoList.begin();
            std::advance(it, m_showItemDetailIdx);
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            // 削除
            m_itemDtoList.erase(it);
            
            m_itemUseMenuCallback(pSeneder, dropItemDto);
        }
    });
    pMenuItemUse->setPosition(Point(pMenuItemDrop->getContentSize().width / 2 + addNextMenuPositonX, pMenuItemUse->getContentSize().height / 2 + addNextMenuPositonY));
    addNextMenuPositonX += pMenuItemUse->getContentSize().width + 4;
    addNextMenuPositonY += 0;
    
    // 装備（する/はずす）ボタン
    auto pMenuItemEquip = ItemWindowLayer::createMenuItemSprite(Color3B::BLUE, [this](Object* pSeneder) {
        CCLOG("item equip menu pressed");
        if (m_showItemDetailIdx < 0)
        {
            return;
        }
        if (m_itemEquipMenuCallback)
        {
            auto it = m_itemDtoList.begin();
            std::advance(it, m_showItemDetailIdx);
            // ステータスを装備状態を反転
            it->isEquip = !it->isEquip;
            
            auto dropItemDto = (DropItemSprite::DropItemDto) *it;
            m_itemEquipMenuCallback(pSeneder, dropItemDto);
        }
    });
    pMenuItemEquip->setPosition(Point(pMenuItemUse->getContentSize().width / 2 + addNextMenuPositonX, pMenuItemEquip->getContentSize().height / 2 + addNextMenuPositonY));
    addNextMenuPositonX += pMenuItemEquip->getContentSize().width + 4;
    addNextMenuPositonY += 0;
    
    auto pMenu = Menu::create(pMenuItemDrop, pMenuItemUse, pMenuItemEquip, NULL);
    pMenu->setPosition(Point::ZERO);
    
    return pMenu;
}

#pragma mardk
#pragma mardk itemList関連

DropItemSprite::DropItemDto ItemWindowLayer::findItem(int itemListIndex)
{
    if (itemListIndex >= 0 && m_itemDtoList.size() > itemListIndex)
    {
        auto it = m_itemDtoList.begin();
        std::advance(it, itemListIndex);
        auto dropItemDto = (DropItemSprite::DropItemDto) *it;
        return dropItemDto;
    }
    return {0, 0, DropItemSprite::ItemType::NONE, 0, "", false};
}

void ItemWindowLayer::addItemList(DropItemSprite::DropItemDto dropItemDto)
{
    m_itemDtoList.push_back(dropItemDto);
}

void ItemWindowLayer::reloadItemList()
{
    // 選択をリセット
    m_showItemDetailIdx = -1;
    
    auto pItemTabelLayer = static_cast<TableViewTestLayer*>(getChildByTag(ItemWindowLayer::ItemTableLayerTag));
    if (pItemTabelLayer)
    {
        std::list<TableViewTestLayer::TableLayout> itemNameList;
        for (DropItemSprite::DropItemDto dropItem : m_itemDtoList)
        {
            TableViewTestLayer::TableLayout layout = {StringUtils::format("item_%d.png", dropItem.imageResId), dropItem.name};
            itemNameList.push_back(layout);
        }
        pItemTabelLayer->makeItemList(itemNameList);
    }
    setItemDetail(m_showItemDetailIdx);
}

void ItemWindowLayer::setItemDetail(int itemListIndex)
{
    DropItemSprite::DropItemDto dropItemDto = findItem(itemListIndex);
    setItemDetail(&dropItemDto);
}

void ItemWindowLayer::setItemDetail(DropItemSprite::DropItemDto* pDropItemDto)
{
    if (!pDropItemDto)
    {
        return;
    }
    
    auto pItemDetailLayer = this->getChildByTag(ItemWindowLayer::ItemDetailLayerTag);
    if (pItemDetailLayer)
    {
        // name
        auto pItemNameLabel = static_cast<LabelTTF*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemNameTag));
        if (pItemNameLabel)
        {
            if (pDropItemDto->name.empty())
            {
                pItemNameLabel->setString(ITEM_LAYER_NAME_DEFAULT);
            }
            else
            {
                // TODO: とりあえず装備中で文言かえる
                if (pDropItemDto->isEquip)
                {
                    pItemNameLabel->setString(StringUtils::format("%s(装備中)", pDropItemDto->name.c_str()));
                }
                else
                {
                    pItemNameLabel->setString(pDropItemDto->name);
                }
            }
        }
        
        // detail
        auto pItemDetailLabel = static_cast<LabelTTF*>(pItemDetailLayer->getChildByTag(ItemWindowLayer::ItemDetailTag));
        if (pItemDetailLabel)
        {
            if (pDropItemDto->name.empty())
            {
                pItemDetailLabel->setString(ITEM_LAYER_DETAIL_DEFAULT);
            }
            else
            {
                // TODO: とりあえず
                if (pDropItemDto->itemId == 1)
                    pItemDetailLabel->setString("HPが少し回復します。");
                else if (pDropItemDto->itemId == 2)
                    pItemDetailLabel->setString("満腹度が少し回復します。");
                else if (pDropItemDto->itemId == 3)
                    pItemDetailLabel->setString("よくある剣です。");
                
            }
        }
    }
}

#pragma mardk
#pragma mardk callback関連

void ItemWindowLayer::setItemUseMenuCallback(const ItemWindowMenuCallback& itemUseMenuCallback)
{
    m_itemUseMenuCallback = itemUseMenuCallback;
}

void ItemWindowLayer::setItemDropMenuCallback(const ItemWindowMenuCallback& itemDropMenuCallback)
{
    m_itemDropMenuCallback = itemDropMenuCallback;
}
void ItemWindowLayer::setItemEquipMenuCallback(const ItemWindowMenuCallback& itemEquipMenuCallback)
{
    m_itemEquipMenuCallback = itemEquipMenuCallback;
}

#pragma mardk
#pragma mardk static

MenuItemSprite* ItemWindowLayer::createMenuItemSprite(Color3B color, const ccMenuCallback& callback)
{
    auto rect = Rect(0, 0, 300, 30);
    auto capRect = Rect(0, 0, 300, 30);
    
    auto pSprite1 = extension::Scale9Sprite::create("menu_button.png", rect, capRect);
    pSprite1->setColor(color);
    pSprite1->setOpacity(192);
    pSprite1->setContentSize(Size(40, 20));
    auto pSprite2 = extension::Scale9Sprite::create("menu_button.png", rect, capRect);
    pSprite2->setColor(color);
    pSprite2->setOpacity(128);
    pSprite2->setContentSize(Size(40, 20));
    
    // 使用ボタン
    return MenuItemSprite::create(pSprite1, pSprite2, callback);
}
