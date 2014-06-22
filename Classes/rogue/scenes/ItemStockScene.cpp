/**

@file  ItemStockScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#include "ItemStockScene.h"
#include "ItemStockPageLayer.h"

#include "AccountData.h"

#include "BankScene.h"

#include "CommonWindowUtil.h"
#include "ItemInventoryLayer.h"
#include "AlertDialogLayer.h"

#include "ItemLogic.h"

USING_NS_CC;

using namespace RogueLikeGame;

ItemStockScene::ItemStockScene()
{
    
}

ItemStockScene::~ItemStockScene()
{
    
}

Node* ItemStockScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = ItemStockPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));
    
    layout->setMenuCallback1([this](Ref *ref) {
        this->showItemInventory();
    });
    
    layout->setMenuCallback2([](Ref *ref) {
        Director::getInstance()->replaceScene(MyPageBaseScene::scene<BankScene>());
    });
    return layout;
}

void ItemStockScene::showItemInventory()
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    auto itemWindow = ItemInventoryLayer::create(loadItemInventory());
    
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_DROP, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("本当に捨ててもいいですか？", "は　い", [this, itemDto, itemWindow](Ref *ref) {
                    auto targetInventory = this->findTargetInventory(itemDto.getObjectId());
                    targetInventory->removeItemDto(itemDto.getObjectId());
                    this->refreshItemInventory(itemWindow, *targetInventory);
                }, "いいえ", [](Ref *ref) {}));
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_SALE, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                int gold = ItemLogic::sale(itemDto);
                auto text = GameCore::StringUtils::format("本当に売ってもいいですか？\n\n金額 %d", gold);
                
                this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(text, "は　い", [this, gold, itemDto, itemWindow](Ref *ref) {
                    auto targetInventory = this->findTargetInventory(itemDto.getObjectId());
                    targetInventory->removeItemDto(itemDto.getObjectId());
                    targetInventory->addGold(gold);
                    this->refreshItemInventory(itemWindow, *targetInventory);
                }, "いいえ", [](Ref *ref) {}));
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_STOCK, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                // 倉庫に入れる。逆に倉庫側だと持ち物へ
                auto changeInventory = this->changeInventory(itemDto.getObjectId());
                if (changeInventory) {
                    this->refreshItemInventory(itemWindow, *changeInventory);
                } else {
                    // 交換がない
                    std::string message = "一杯で交換できません。\n捨てるか売るかしてからもう一度試してください。";
                    this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(message, "とじる", [](Ref *ref) {}));
                }
            }
        },
    });
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->setCloseCallback([this]() {
        this->saveInventory();
    });
    this->addChildDialog(itemWindow);
}

std::list<ItemInventoryDto> ItemStockScene::loadItemInventory()
{
    this->_itemInventory = AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    this->_itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
    this->_itemInventoryStock.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    
    return std::list<ItemInventoryDto>{
        this->_itemInventory,
        this->_itemInventoryStock
    };
}
void ItemStockScene::saveInventory()
{
    AccountData::getInstance()->saveInventory(this->_itemInventory, this->_itemInventoryStock);
}

ItemInventoryDto* ItemStockScene::findTargetInventory(long objectId)
{
    return ItemInventoryDto::findByObjectIdInventory(objectId, &this->_itemInventory, &this->_itemInventoryStock);
}

void ItemStockScene::refreshItemInventory(ItemInventoryLayer* itemWindow, const ItemInventoryDto &targetInventory)
{
    itemWindow->refresh(std::list<ItemInventoryDto>{this->_itemInventory, this->_itemInventoryStock}, targetInventory);
}

ItemInventoryDto* ItemStockScene::changeInventory(long objectId)
{
    return ItemInventoryDto::changeInventory(objectId, &this->_itemInventory, &this->_itemInventoryStock);
}


