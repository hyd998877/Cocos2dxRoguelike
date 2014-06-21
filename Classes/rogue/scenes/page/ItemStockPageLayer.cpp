/**
 @file ItemStockPageLayer.cpp
 
 @brief 倉庫画面
 
 倉庫のアイテムと所持アイテムの交換。
 アイテムを捨てたり売ったり整理できます。
 
 @author kyokomi
 @date 2014/05/07
 
 */

#include "ItemStockPageLayer.h"

#include "AccountData.h"

#include "ItemInventoryLayer.h"
#include "AlertDialogLayer.h"

#include "ItemLogic.h"

#include "BankScene.h"

USING_NS_CC;

ItemStockPageLayer::ItemStockPageLayer()
: _itemInventory()
, _itemInventoryStock()
{
}

ItemStockPageLayer::~ItemStockPageLayer()
{
}

bool ItemStockPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    this->loadItemInventory();
    
    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/ci_shopbakery01a_b.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width/background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    // キャラ表示
    auto playerSprite = Sprite::create("novel/actor11_novel_s_0.png");
    playerSprite->setScale(resolutionSize.height/playerSprite->getContentSize().height);
    playerSprite->setPosition(Point(winSize.width * 0.8, winSize.height * 0.4));
    this->addChild(playerSprite);
    
    // メッセージ
    auto messageLayer = LayerColor::create(Color4B::BLACK);
    messageLayer->setContentSize(Size(winSize.width / 3.5, winSize.height / 4.0));
    messageLayer->setPosition(Point(winSize.width * 0.55 - messageLayer->getContentSize().width / 2,
                                    winSize.height * 0.7 - messageLayer->getContentSize().height / 2));
    auto messageTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "大事なものは\n預けておけば安心！");
    messageTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    messageTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    messageLayer->addChild(messageTextLabel);
    messageTextLabel->setPosition(Point(messageLayer->getContentSize().width / 2, messageLayer->getContentSize().height / 2));
    // メッセージ枠
    CommonWindowUtil::attachWindowWaku(messageLayer);
    this->addChild(messageLayer);
    
    Size layerSize = Size(winSize.width*0.35, winSize.height*0.3);
    
    auto iconNode1 = Sprite::create("icon_set/item_696.png");
    auto menuItem1 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode1, FontUtils::getDefaultFontTTFConfig(), "倉庫を開く", [this](Ref *ref) {
        this->showItemInventory();
    });
    auto iconNode2 = Sprite::create("icon_set/item_671.png");
    auto menuItem2 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode2, FontUtils::getDefaultFontTTFConfig(), "銀行を開く", [this](Ref *ref) {
        // TODO: 銀行表示
        Director::getInstance()->replaceScene(MyPageBaseScene::scene<BankScene>());
    });
    
    auto menu = Menu::create(menuItem1, menuItem2, NULL);
    menu->setPosition(Point(winSize.width/5, winSize.height/2));
    menu->alignItemsVerticallyWithPadding(10);
    this->addChild(menu);

    return true;
}

void ItemStockPageLayer::showItemInventory()
{
    // アイテムリストを設定
    Size winSize = Director::getInstance()->getWinSize();
    
    auto itemWindow = ItemInventoryLayer::create(loadItemInventory());
    
    itemWindow->initMenuActionCallback(std::list<ItemInventoryLayer::ActionCallback> {
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_DROP, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                this->addChild(AlertDialogLayer::createWithContentSizeModal("本当に捨ててもいいですか？", "は　い", [this, itemDto, itemWindow](Ref *ref) {
                    auto targetInventory = this->findTargetInventory(itemDto.getObjectId());
                    targetInventory->removeItemDto(itemDto.getObjectId());
                    this->refreshItemInventory(itemWindow, *targetInventory);
                }, "いいえ", [](Ref *ref) {}), ZOrders::DIALOG);
            }
        },
        ItemInventoryLayer::ActionCallback{ItemWindowLayer::ItemWindowMenuType::ITEM_SALE, ItemInventoryLayer::CloseType::UN_CLOSE,
            [this, itemWindow](ItemWindowLayer::ItemWindowMenuType menuType, Ref *ref, const ItemDto &itemDto) {
                int gold = ItemLogic::sale(itemDto);
                auto text = GameCore::StringUtils::format("本当に売ってもいいですか？\n\n金額 %d", gold);
                
                this->addChild(AlertDialogLayer::createWithContentSizeModal(text, "は　い", [this, gold, itemDto, itemWindow](Ref *ref) {
                    auto targetInventory = this->findTargetInventory(itemDto.getObjectId());
                    targetInventory->removeItemDto(itemDto.getObjectId());
                    targetInventory->addGold(gold);
                    this->refreshItemInventory(itemWindow, *targetInventory);
                }, "いいえ", [](Ref *ref) {}), ZOrders::DIALOG);
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
                    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(message, "とじる", [](Ref *ref) {});
                    this->addChild(dialogLayer, ZOrders::DIALOG);
                }
            }
        },
    });
    itemWindow->setPosition(CommonWindowUtil::createPointCenter(itemWindow, this));
    itemWindow->setCloseCallback([this]() {
        this->saveInventory();
    });
    this->addChild(itemWindow, ZOrders::DIALOG);
}

std::list<ItemInventoryDto> ItemStockPageLayer::loadItemInventory()
{
    this->_itemInventory = RogueLikeGame::AccountData::getInstance()->getItemInventory();
    this->_itemInventory.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    this->_itemInventoryStock = RogueLikeGame::AccountData::getInstance()->getItemInventoryStock();
    this->_itemInventoryStock.sortItemList(ItemDto::compare_dropItem_weapon_with_accessory);
    
    return std::list<ItemInventoryDto>{
        this->_itemInventory,
        this->_itemInventoryStock
    };
}
void ItemStockPageLayer::saveInventory()
{
    RogueLikeGame::AccountData::getInstance()->saveInventory(this->_itemInventory, this->_itemInventoryStock);
}

ItemInventoryDto* ItemStockPageLayer::findTargetInventory(long objectId)
{
    return ItemInventoryDto::findByObjectIdInventory(objectId, &this->_itemInventory, &this->_itemInventoryStock);
}
void ItemStockPageLayer::refreshItemInventory(ItemInventoryLayer* itemWindow, const ItemInventoryDto &targetInventory)
{
    itemWindow->refresh(std::list<ItemInventoryDto>{this->_itemInventory, this->_itemInventoryStock}, targetInventory);
}
ItemInventoryDto* ItemStockPageLayer::changeInventory(long objectId)
{
    return ItemInventoryDto::changeInventory(objectId, &this->_itemInventory, &this->_itemInventoryStock);
}

