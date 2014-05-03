//
//  ItemWindow.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/27.
//
//

#include "ItemWindow.h"
#include "AppMacros.h"
#include "CommonWindowUtil.h"

ModalLayer* ItemWindow::createItemWindowLayer(cocos2d::Size contentSize, std::list<DropItemSprite::DropItemDto> itemList) {
    
    auto modalLayer = ModalLayer::create();
    
    // アイテムの詳細ウィンドウ（以下のボタン操作のみ可能なモーダルウィンドウ）
    auto itemWindowLayer = ItemWindowLayer::createWithContentSize(contentSize);
    itemWindowLayer->setItemList(itemList);
    itemWindowLayer->reloadItemList();
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer->getContentSize(), contentSize));
    itemWindowLayer->setItemDropMenuCallback([modalLayer, itemWindowLayer](Ref* ref, DropItemSprite::DropItemDto drop_item) {
        CCLOG("RogueScene::itemDropMenuCallback");
        
//            auto player_sprite = getPlayerActorSprite(1);
//            
//            // すでにアイテムが置いてある場合は置けない
//            if (MapManager::getInstance()->getDropMapItem(&player_sprite->getActorMapItem()->mapIndex)->mapDataType != MapDataType::NONE) {
//                this->logMessage("%sを床におけなかった。", DropItemSprite::DropItemDto::createItemName(drop_item).c_str());
//                // アイテムを戻す
//                this->getItemWindowLayer()->addItemList(drop_item);
//                
//            } else {
//                
//                // 装備してたら外す
//                if (drop_item.isEquip) {
//                    drop_item.isEquip = false;
//                    
//                    if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
//                        player_sprite->getActorDto()->equipReleaseWeapon();
//                    } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
//                        player_sprite->getActorDto()->equipReleaseAccessory();
//                    }
//                    
//                    // 装備変更でステータス更新
//                    refreshStatusEquip(*player_sprite->getActorDto());
//                }
//                
//                // アイテムをマップのプ
//                // レイヤーの足元に置く
//                if (this->getRogueMapLayer()->tileSetDropMapItem(drop_item, player_sprite->getActorMapItem()->mapIndex)) {
//                    this->logMessage("%sを床においた。", DropItemSprite::DropItemDto::createItemName(drop_item).c_str());
//                    
//                    // ターン消費
//                    this->changeGameStatus(GameStatus::ENEMY_TURN);
//                }
//            }
//            
//            // インベントリは閉じる
//            this->hideItemList();
        
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    itemWindowLayer->setItemUseMenuCallback([](Ref* ref, DropItemSprite::DropItemDto drop_item) {
        CCLOG("RogueScene::itemUseMenuCallback");
//
//            auto player_sprite = getPlayerActorSprite(1);
//            
//            // itemIdで処理してくれるlogicへ
//            std::string use_message = ItemLogic::use(drop_item.itemId, player_sprite->getActorDto());
//            
//            this->logMessage(use_message.c_str());
//            
//            // インベントリは閉じる
//            this->hideItemList();
//            
//            // ターン消費
//            this->changeGameStatus(GameStatus::ENEMY_TURN);
    });
    
    itemWindowLayer->setItemEquipMenuCallback([modalLayer](Ref* ref, DropItemSprite::DropItemDto drop_item) {
        CCLOG("RogueScene::itemEquipMenuCallback itemType = %d", drop_item.itemType);
        
//            auto player_sprite = getPlayerActorSprite(1);
//            
//            if (drop_item.isEquip) {
//                if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
//                    // 解除
//                    itemWindowLayer_->setItemEquip(player_sprite->getActorDto()->getWeaponEquip().getObjectId(), false);
//                    
//                    // 武器装備
//                    MWeapon mWeapon = MWeaponDao::getInstance()->selectById(drop_item.itemId);
//                    player_sprite->getActorDto()->equipWeapon(drop_item.objectId, drop_item.param, mWeapon);
//                    
//                } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
//                    // 解除
//                    itemWindowLayer_->setItemEquip(player_sprite->getActorDto()->getAccessoryEquip().getObjectId(), false);
//                    // 防具装備
//                    MAccessory mAccessory = MAccessoryDao::getInstance()->selectById(drop_item.itemId);
//                    player_sprite->getActorDto()->equipAccessory(drop_item.objectId, drop_item.param, mAccessory);
//                }
//                this->logMessage("%sを装備した。", DropItemSprite::DropItemDto::createItemName(drop_item).c_str());
//            } else {
//                if (drop_item.itemType == MUseItem::ItemType::EQUIP_WEAPON) {
//                    // 武器解除
//                    player_sprite->getActorDto()->equipReleaseWeapon();
//                } else if (drop_item.itemType == MUseItem::ItemType::EQUIP_ACCESSORY) {
//                    // 防具装備
//                    player_sprite->getActorDto()->equipReleaseAccessory();
//                }
//                this->logMessage("%sの装備をはずした。", DropItemSprite::DropItemDto::createItemName(drop_item).c_str());
//            }
//            
//            // 装備解除、装備によってステータス変動するためステータスバーを更新
//            this->refreshStatusEquip(*player_sprite->getActorDto());
//            
//            // インベントリは閉じる
//            this->hideItemList();
//            
//            // ターン消費
//            this->changeGameStatus(GameStatus::ENEMY_TURN);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    
    // ---- button -----
    
    // 並び替えボタン
    auto sort_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "並び替え"), Size(12, 4), [itemWindowLayer](Ref* ref) {
        // 並び替え
        itemWindowLayer->sortItemList();
        itemWindowLayer->reloadItemList();
    });
    sort_menu_item_label->setPosition(Point(itemWindowLayer->getContentSize().width, itemWindowLayer->getContentSize().height + sort_menu_item_label->getContentSize().height / 2));
    
    // 閉じるボタン
    auto clone_menu_item_label = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "とじる"), Size(12, 4), [modalLayer](Ref* ref) {
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    clone_menu_item_label->setPosition(Point(contentSize.width, clone_menu_item_label->getPositionY() - clone_menu_item_label->getContentSize().height / 2));
    
    auto menu = Menu::create(sort_menu_item_label, clone_menu_item_label, NULL);
    menu->setPosition(Point::ZERO);
    
    itemWindowLayer->addChild(menu);
    
    modalLayer->addChild(itemWindowLayer);
    itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer, modalLayer));
    
    return modalLayer;
}