/**

@file  EditTextDialogLayout.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/28

*/

#include "EditTextDialogLayout.h"
#include "WidgetUtil.h"

USING_NS_CC;

using namespace cocostudio;
using namespace ui;

EditTextDialogLayout::EditTextDialogLayout()
: _editDialogLayout(nullptr)
, _callback(nullptr)
{
    
}

EditTextDialogLayout::~EditTextDialogLayout()
{
    
}

EditTextDialogLayout* EditTextDialogLayout::create(const EditTextDialogCallback& callback)
{
    auto layout = create();
    if (layout) {
        layout->_callback = callback;
    }
    return layout;
}

bool EditTextDialogLayout::init()
{
    if (!ModalLayer::init()) {
        return false;
    }
    
    _editDialogLayout = initLayout();
    this->addChild(_editDialogLayout);
    
    return true;
}

Node* EditTextDialogLayout::initLayout()
{
    // ダイアログUI作成して真ん中に置く
    auto editDialogLayout = WidgetUtil::createCocoStudioWidget("cocostudio/EditTextDialog_1.json");
    WidgetUtil::settingCenterPosition(editDialogLayout);
    
    // 預けるボタン
    WidgetUtil::onTouchEventEnded(editDialogLayout, "execute_button", [this, editDialogLayout](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        auto goldInputTextField = dynamic_cast<TextField*>(WidgetUtil::getChildByNameRecursion(editDialogLayout, "goldInputTextField"));
        if (_callback) {
            this->_callback(goldInputTextField->getStringValue());
        }
        
        this->setVisible(false);
        this->removeAllChildrenWithCleanup(true);
    });
    
    // とじるボタン
    WidgetUtil::onTouchEventEnded(editDialogLayout, "close_button", [this, editDialogLayout](Ref *ref, cocos2d::ui::Widget::TouchEventType type) {
        
        // なんもしない
        
        this->setVisible(false);
        this->removeAllChildrenWithCleanup(true);
    });
    return editDialogLayout;
}

void EditTextDialogLayout::setExecuteButtonText(const std::string& buttonText)
{
    if (_editDialogLayout) {
        auto buton = dynamic_cast<Button*>(WidgetUtil::getChildByNameRecursion(dynamic_cast<Widget*>(_editDialogLayout), "execute_button"));
        buton->setTitleText(buttonText);
    }
}

