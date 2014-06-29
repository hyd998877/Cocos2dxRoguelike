/**

@file  KeypadLayout.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/29

*/

#include "KeypadLayout.h"
#include "WidgetUtil.h"

USING_NS_CC;

KeypadLayout::KeypadLayout()
: _baseLayout(nullptr)
, _keypadCallbacks(ButtonType::SIZE)
, _longTouch(false)
{
    
}

KeypadLayout::~KeypadLayout()
{
    
}

bool KeypadLayout::init()
{
    if (!Node::init()) {
        return false;
    }
    
    _baseLayout = initLayout();
    
    return true;
}


Node* KeypadLayout::initLayout()
{
    const float baseScale = 0.75f;
    
    auto win_size = Director::getInstance()->getWinSize();
    
    auto keypad = WidgetUtil::createCocoStudioWidget("cocostudio/RogueScene_keypad.json");
    keypad->setScale(baseScale);
    
    keypad->setPositionX(win_size.width/2 - (keypad->getLayoutSize().width/2 * baseScale));
    keypad->setPositionY(win_size.height/12);
    
    auto panelLeft = WidgetUtil::getChildByNameRecursion(keypad, "Panel_L");
    panelLeft->setPositionX(panelLeft->getPositionX() - keypad->getPositionX());
    auto panelRight = WidgetUtil::getChildByNameRecursion(keypad, "Panel_R");
    panelRight->setPositionX(panelRight->getPositionX() + (win_size.width - (keypad->getPositionX() + keypad->getLayoutSize().width * baseScale)));
    this->addChild(keypad);
    
    WidgetUtil::onTouchEventLongTouch(keypad, "Button_Up", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::UP]();
    });
    WidgetUtil::onTouchEventLongTouch(keypad, "Button_Left", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::LEFT]();
    });
    WidgetUtil::onTouchEventLongTouch(keypad, "Button_Right", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::RIGHT]();
    });
    WidgetUtil::onTouchEventLongTouch(keypad, "Button_Down", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::DOWN]();
    });
    
    WidgetUtil::onTouchEventEnded(keypad, "Button_A", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::A]();
    });
    WidgetUtil::onTouchEventEnded(keypad, "Button_B", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::B]();
    });
    WidgetUtil::onTouchEventEnded(keypad, "Button_C", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::C]();
    });
    WidgetUtil::onTouchEventEnded(keypad, "Button_D", [this](Ref *ref, ui::Widget::TouchEventType eventType) {
        _keypadCallbacks[ButtonType::D]();
    });
    
    return keypad;
}

void KeypadLayout::setKeypadCallback(ButtonType buttonType, const KeypadCallback& callback)
{
    _keypadCallbacks[buttonType] = callback;
}

void KeypadLayout::showKeypadMenu()
{
    _baseLayout->setVisible(true);
}

void KeypadLayout::hideKeypadMenu()
{
    _baseLayout->setVisible(false);
}

bool KeypadLayout::isKeypadDashed()
{
    // TODO: ベタ書きやだなぁ
    auto button = dynamic_cast<ui::Button*>(WidgetUtil::getChildByNameRecursion(dynamic_cast<ui::Widget*>(_baseLayout), "Button_B"));
    if (button) {
        if (button->isHighlighted()) {
            return true;
        }
    }
    return false;
}
