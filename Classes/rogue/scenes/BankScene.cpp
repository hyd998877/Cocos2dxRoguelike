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
#include "ModalLayer.h"

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
    auto layout = createCocoStudioWidget("cocostudio/bank_ui2/bank_ui.json");
    layout->setScale(0.75f);
    WidgetUtil::settingCenterPosition(layout);
    
    auto messageLabel = dynamic_cast<Text*>(WidgetUtil::getChildByNameRecursion(layout, "message_label"));
    messageLabel->setString("ここはゴールドを預けたり、\n引き出したりできるわ。");

    auto itemInventoryStock = AccountData::getInstance()->getItemInventoryStock();
    auto goldLabel = dynamic_cast<Text*>(WidgetUtil::getChildByNameRecursion(layout, "gold_label"));
    goldLabel->setColor(Color3B::YELLOW);
    goldLabel->setString(cocos2d::StringUtils::format("預け金 %10d G", itemInventoryStock.getGold()));
    
    // 預けるボタン
    WidgetUtil::onTouchEventEnded(layout, "main_button_1", [this](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        // モーダルレイヤー
        auto modalLayer = ModalLayer::create();
        {
            // TODO: ここで別のクラスにしたほうがよさそう
            
            // ダイアログUI作成して真ん中に置く
            auto editDialogLayout = createCocoStudioWidget("cocostudio/EditTextDialog_1.json");
            WidgetUtil::settingCenterPosition(editDialogLayout);
            
            // 預けるボタン
            WidgetUtil::onTouchEventEnded(editDialogLayout, "execute_button", [this, editDialogLayout, modalLayer](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
                auto goldInputTextField = dynamic_cast<TextField*>(WidgetUtil::getChildByNameRecursion(editDialogLayout, "goldInputTextField"));
                CCLOG("input gold = %s", goldInputTextField->getStringValue().c_str());
                
                // TODO: ゴールド預けるロジックよびだし
                
                modalLayer->setVisible(false);
                modalLayer->removeAllChildrenWithCleanup(true);
            });
            
            // とじるボタン
            WidgetUtil::onTouchEventEnded(editDialogLayout, "close_button", [this, editDialogLayout, modalLayer](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
                
                // なんもしない
                
                modalLayer->setVisible(false);
                modalLayer->removeAllChildrenWithCleanup(true);
            });
            
            modalLayer->addChild(editDialogLayout);
        }
        this->addChildDialog(modalLayer);
    });
    return layout;
}

