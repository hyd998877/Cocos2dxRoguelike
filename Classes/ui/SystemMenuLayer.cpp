//
//  SystemMenuLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/19.
//
//

#include "SystemMenuLayer.h"
#include "AppMacros.h"
#include "CommonWindowUtil.h"

USING_NS_CC;

SystemMenuLayer::SystemMenuLayer()
{
}

SystemMenuLayer::~SystemMenuLayer()
{
}

SystemMenuLayer* SystemMenuLayer::create(const Size& contentSize, const std::string& titleText, const cocos2d::Color3B& color /* = cocos2d::Color3B::BLACK */, const GLubyte& opacity /* = 192 */)
{
    SystemMenuLayer *pRet = new SystemMenuLayer();
    if (pRet && pRet->init(contentSize, titleText, color, opacity)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool SystemMenuLayer::init(const Size& contentSize, const std::string& titleText, const Color3B& color, const GLubyte& opacity)
{
    if (!LayerColor::init()) {
        return false;
    }
    
    this->setColor(color);
    this->setOpacity(opacity);
    this->setContentSize(contentSize);

    // 枠込でサイズを再設定
    auto windowWaku = CommonWindowUtil::createWindowWaku(this, Size(4, 4));
    this->addChild(windowWaku);
    this->setContentSize(windowWaku->getContentSize());
    
    // TitleLabel
    initCreateTitleLabel(titleText);
    
    // Menu
//    initCreateMenuList(menuButtonInfoList);
    
    return true;
}

void SystemMenuLayer::initCreateTitleLabel(const std::string &titleText)
{
    auto titleLabel = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), titleText);
    titleLabel->setAlignment(TextHAlignment::CENTER, TextVAlignment::CENTER);
    titleLabel->setPosition(CommonWindowUtil::createPointCenterXTopY(Size(0, titleLabel->getContentSize().height), this->getContentSize()));
    addChild(titleLabel);
}

void SystemMenuLayer::setMenuButtonList(const std::list<SystemMenuButtonInfo> menuButtonInfoList)
{
    Vector<cocos2d::MenuItem *> arrayOfItems;
    
    for (SystemMenuButtonInfo systemMenuButtonInfo : menuButtonInfoList) {
        // MenuLabelつくる
        auto label = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), systemMenuButtonInfo.buttonText);
        label->setAlignment(cocos2d::TextHAlignment::CENTER, cocos2d::TextVAlignment::CENTER);
        auto menuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(label, Size(8, 6), [this, systemMenuButtonInfo](Ref *ref) {
            systemMenuButtonInfo.systemMenuButtonCallback();
        });
        arrayOfItems.pushBack(menuItemLabel);
    }
    
    // 画面8割超えで折り返す
    auto menu = Menu::createWithArray(arrayOfItems);
    menu->setPosition(Point::ZERO);

    Point center = Point(this->getContentSize().width / 2, this->getContentSize().height / 2);
    
    Size gridSize;
    int count = menu->getChildrenCount();
    if (count == 8 || count == 7 || count == 5 || count == 0) {
        // 8個 無理
        // 7個 無理
        // 5個 無理
        // 0個 無理
        menu->setPosition(CommonWindowUtil::createPointCenter(this, menu));
        menu->alignItemsHorizontallyWithPadding(4);
    } else {
        if (count % 3 == 0) {
            // 9個 = 3, 3   9 / 3 = 3...0
            // 6個 = 3, 2   6 / 3 = 2...0
            // 3個 = 3, 1   3 / 3 = 1...0
            gridSize = Size(3, count / 3);
        } else if (count % 2 == 0) {
            // 4個 = 2, 2   4 / 3 = 1...1  4 / 2 = 2...0
            // 2個 = 2, 1   2 / 3 = 0...2  2 / 2 = 1...0
            gridSize = Size(2, count / 2);
        } else {
            // 1個 = 1, 1   1 / 3 = 0...1  1 / 2 = 0...1  1 / 1 = 1...0
            gridSize = Size(1, 1);
        }
        
        // 3 -> -2.0
        // 2 -> -1.5
        // 1 ->  0.0
        float startX =  -1.5 * (gridSize.width - 1) + (1.0 * (gridSize.width - 2));
        float startY =   0.5 * (gridSize.height - 1);
        
        float paddingRateX = 6.0 / gridSize.width;
        float paddingRateY = 6.0 / gridSize.height;
        
        for (int y = 0; y < gridSize.height; y++) {
            for (int x = 0; x < gridSize.width; x++) {
                
                int idx = (y * gridSize.width) + x;
                
                auto node = menu->getChildren().at(idx);
                Size baseSize = node->getContentSize() / 2;
                Point basePoint = Point(baseSize.width *  startX, baseSize.height *  startY);
                
                basePoint.x = basePoint.x + (baseSize.width * paddingRateX) * x;
                basePoint.y = basePoint.y - (baseSize.height * paddingRateY) * y;
                
                node->setPosition(node->getPosition() + center + basePoint);
            }
        }
    }
    
    this->addChild(menu);
}

