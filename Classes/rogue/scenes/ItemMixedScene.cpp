/**

@file  ItemMixedScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#include "ItemMixedScene.h"
#include "ItemMixedPageLayer.h"
#include "ItemInventoryLayer.h"
#include "CommonWindowUtil.h"

#include "ItemLogic.h"

#include "AlertDialogLayer.h"

#include "AccountData.h"

#include "AudioUtil.h"

USING_NS_CC;

using namespace RogueLikeGame;

ItemMixedScene::ItemMixedScene()
{
    
}

ItemMixedScene::~ItemMixedScene()
{
    
}

Node* ItemMixedScene::initLayout()
{
    AudioUtil::playBGM(AudioUtil::BGM_MIXED);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    this->loadInventory();
    
    auto layout = ItemMixedPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));
    layout->setMenuCallback1([this, layout](Ref *ref) {
        this->showMixedItemSelectWindow([this, layout](const ItemDto &itemDto) {
            layout->refreshSelectBase(itemDto);
            this->_baseItemDto = itemDto;
        });
    });
    layout->setMenuCallback2([this, layout](Ref *ref) {
        this->showMixedItemSelectWindow([this, layout](const ItemDto &itemDto) {
            layout->refreshSelectMaterial(itemDto);
            this->_materialItemDto = itemDto;
        });
    });
    layout->setMenuCallback3([this, layout](Ref *ref) {
        // 合成実行
        this->mixedItem([this, layout]() {
            // セーブ
            this->saveInventory();
            
            layout->mixedItemAfter();
        });
    });
    return layout;
}

void ItemMixedScene::showMixedItemSelectWindow(std::function<void(const ItemDto &itemDto)> selectItemCallback)
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    auto itemWindow = ItemInventoryLayer::create(this->_itemInventory);
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_MIXED, ItemInventoryLayer::CloseType::CLOSE,
            [this, selectItemCallback](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                CCLOG("itemName = %s", itemDto.getName().c_str());
                
                if (_baseItemDto.getObjectId() != 0) {
                    this->_itemInventory.addItemDto(_baseItemDto);
                }
                
                this->_itemInventory.removeItemDto(itemDto.getObjectId());
                selectItemCallback(itemDto);
            }
        }
    });
    itemWindow->setCloseCallback([this]() { });
    this->addChildDialog(itemWindow);
}

void ItemMixedScene::mixedItem(std::function<void(void)> mixedCallback)
{
    if (!this->checkMixedItem()) {
        return;
    }
    
    int mixedGold = ItemLogic::calcMixedItemGold(this->_baseItemDto, this->_materialItemDto);
    std::string messageText = GameCore::StringUtils::format("合成に %d ゴールド必要ですが\nよろしいですか？\n\n所持ゴールド %10ld",
                                                            mixedGold, this->_itemInventory.getGold());
    this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(messageText, "は　い", [this, mixedGold, mixedCallback](Ref *ref) {
        // 合成!
        auto mixedItemDto = ItemLogic::mixedItem(this->_baseItemDto, this->_materialItemDto);
        
        // ベースと素材のアイテムを削除
        this->_itemInventory.addGold(-1 * mixedGold);
        this->_itemInventory.removeItemDto(this->_baseItemDto.getObjectId());
        this->_itemInventory.removeItemDto(this->_materialItemDto.getObjectId());
        
        // 合成結果のアイテムをインベントリに追加してsaveする
        this->_itemInventory.addItemDto(mixedItemDto);
        std::string messageText = GameCore::StringUtils::format("%d ゴールドと%sを素材にして\n\n%s\n\nの合成に成功しました。",
                                                                mixedGold,
                                                                this->_materialItemDto.createItemName().c_str(),
                                                                mixedItemDto.createItemName().c_str());
        mixedCallback();
        this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(messageText, "とじる", [](Ref *ref) {}));
    }, "いいえ", [](Ref *ref) {}));
}

bool ItemMixedScene::checkMixedItem()
{
    return checkMixedItem(this->_baseItemDto, this->_materialItemDto, this->_itemInventory);
}

bool ItemMixedScene::checkMixedItem(const ItemDto &baseItem, const ItemDto &materialItem, const ItemInventoryDto &itemInventory)
{
    // 選択チェック
    if (baseItem.getObjectId() == 0 || materialItem.getObjectId() == 0) {
        CCLOG("ベースか素材が選択されていません");
        this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("ベースか素材が選択されていません", "とじる", [](Ref *ref) {}));
        return false;
    }
    
    // 合成可能かチェック
    if (!ItemLogic::isMixedItem(baseItem, materialItem)) {
        CCLOG("その組み合わせは合成できません");
        this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("その組み合わせは合成できません", "とじる", [](Ref *ref) {}));
        return false;
    }
    // 金額計算して支払う
    int mixedGold = ItemLogic::calcMixedItemGold(baseItem, materialItem);
    if (itemInventory.getGold() < mixedGold) {
        CCLOG("お金がたりません");
        std::string messageText = GameCore::StringUtils::format("お金がたりません。\n\n合成金額 %d ゴールド", mixedGold);
        this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(messageText, "とじる", [](Ref *ref) {}));
        return false;
    }
    // 合成できる
    return true;
}

void ItemMixedScene::loadInventory()
{
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
}

void ItemMixedScene::saveInventory()
{
    AccountData::getInstance()->saveInventory(this->_itemInventory);
}

