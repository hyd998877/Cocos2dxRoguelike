/**

@file  BankScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/15

*/

#include "BankScene.h"
#include "WidgetUtil.h"

#include "AccountData.h"
#include "EditTextDialogLayout.h"
#include "AlertDialogLayer.h"

#include "AudioUtil.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;
using namespace RogueLikeGame;

BankScene::BankScene()
{
    
}

BankScene::~BankScene()
{
    
}

Node* BankScene::initLayout()
{
    AudioUtil::playBGM(AudioUtil::BGM_MYPAGE);
    
    auto layout = WidgetUtil::createCocoStudioWidget("cocostudio/bank_ui.json");
    layout->setScale(0.75f);
    WidgetUtil::settingCenterPosition(layout);
    
    auto messageLabel = dynamic_cast<Text*>(WidgetUtil::getChildByNameRecursion(layout, "message_label"));
    messageLabel->setString("ここはゴールドを預けたり、\n引き出したりできるわ。");

    showGold(layout);
    
    // 預けるボタン
    WidgetUtil::onTouchEventEnded(layout, "main_button_1", [this, layout](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        this->addChildDialog(EditTextDialogLayout::create([this, layout](std::string inputText) {
            CCLOG("預け金 : %s", inputText.c_str());
            
            auto itemInventory = AccountData::getInstance()->getItemInventory();
            auto itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
            
            // 数値チェック
            int inputGold = atoi(inputText.c_str());
            
            // 預けるお金があるかチェック
            if (inputGold <= 0 || itemInventory.getGold() < inputGold) {
                this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("金額が不正なため、\n預けるのに失敗しました。", "閉じる", [](Ref *ref) {}));
                return;
            }
            
            // 預ける
            itemInventory.addGold(-inputGold);
            itemInventoryStock.addGold(inputGold);
            // save
            AccountData::getInstance()->saveInventory(itemInventory, itemInventoryStock);
            
            this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("銀行に預けました。", "閉じる", [this, layout](Ref *ref) {
                this->showGold(layout);
            }));
        }));
    });
    
    WidgetUtil::onTouchEventEnded(layout, "main_button_2", [this, layout](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        auto editDialog = EditTextDialogLayout::create([this, layout](std::string inputText) {
            CCLOG("出し金 : %s", inputText.c_str());
            
            auto itemInventory = AccountData::getInstance()->getItemInventory();
            auto itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
            
            // 数値チェック
            int inputGold = atoi(inputText.c_str());
            
            // 預けるお金があるかチェック
            if (inputGold <= 0 || itemInventoryStock.getGold() < inputGold) {
                this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("金額が不正なため、\n引き出すのに失敗しました。", "閉じる", [](Ref *ref) {}));
                return;
            }
            
            // 預ける
            itemInventoryStock.addGold(-inputGold);
            itemInventory.addGold(inputGold);
            // save
            AccountData::getInstance()->saveInventory(itemInventory, itemInventoryStock);
            
            this->addChildDialog(AlertDialogLayer::createWithContentSizeModal("銀行から引き出しました。", "閉じる", [this, layout](Ref *ref) {
                this->showGold(layout);
            }));
        });
        editDialog->setExecuteButtonText("出　す");
        this->addChildDialog(editDialog);
    });
    return layout;
}

void BankScene::showGold(Widget* layout)
{
    // 倉庫取得
    auto itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
    
    auto goldLabel = dynamic_cast<Text*>(WidgetUtil::getChildByNameRecursion(layout, "gold_label"));
    goldLabel->setColor(Color3B::YELLOW);
    goldLabel->setString(cocos2d::StringUtils::format("預け金 %10d G", itemInventoryStock.getGold()));
}

