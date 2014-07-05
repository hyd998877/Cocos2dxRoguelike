/**

@file  HeaderStatusLayout.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/07/05

*/

#include "HeaderStatusLayout.h"
#include "WidgetUtil.h"

#include "ItemLogic.h"

USING_NS_CC;

HeaderStatusLayout::HeaderStatusLayout()
{
    
}

HeaderStatusLayout::~HeaderStatusLayout()
{
    
}

bool HeaderStatusLayout::init()
{
    if (!Node::init()) {
        return false;
    }
    
    this->_baseLayout = initLayout();
    this->addChild(_baseLayout);
    
    return true;
}

Node* HeaderStatusLayout::initLayout()
{
    auto win_size = Director::getInstance()->getWinSize();
    
    auto statusBarLayout = WidgetUtil::createCocoStudioWidget("cocostudio/RogueScene_status.json");
    statusBarLayout->setScale(0.75f);
    statusBarLayout->setPositionY(win_size.height - (statusBarLayout->getLayoutSize().height * 0.75f));
    
    auto winScale = win_size.width / statusBarLayout->getLayoutSize().width;
    {
        auto goldText = WidgetUtil::getChildByNameRecursion<ui::Text>(statusBarLayout, "Label_goldText");
        goldText->setPositionPercent(Vec2(goldText->getPositionPercent().x * winScale, goldText->getPositionPercent().y));
        
        auto equipLayout = WidgetUtil::getChildByNameRecursion<ui::Layout>(statusBarLayout, "equip_layout");
        equipLayout->setPositionPercent(Vec2(equipLayout->getPositionPercent().x * winScale, equipLayout->getPositionPercent().y));
        
        std::string attackPointText = cocos2d::StringUtils::format("%3d", 0);
        auto equip1Label = WidgetUtil::getChildByNameRecursion<ui::Text>(equipLayout, "equip1_label");
        equip1Label->setString(attackPointText);
        
        std::string defencePointText = cocos2d::StringUtils::format("%3d", 0);
        auto equip2Label = WidgetUtil::getChildByNameRecursion<ui::Text>(equipLayout, "equip2_label");
        equip2Label->setString(defencePointText);
    }
    
    return statusBarLayout;
}

void HeaderStatusLayout::setStatus(int floorCount, const ActorDto& actorDto, int gold)
{
    auto statusWidget = dynamic_cast<ui::Widget*>(_baseLayout);
    if (!statusWidget) {
        return;
    }
    
    auto pStatusText = WidgetUtil::getChildByNameRecursion<ui::Text>(statusWidget, "Label_statusText");
    if (pStatusText) {
        
        // プレイヤー取得
        auto pStr = String::createWithFormat("%2dF Lv %2d HP %3d/%3d 満腹度 %3d/%3d",
                                             floorCount,
                                             actorDto.getLv(),
                                             actorDto.getHitPoint(),
                                             actorDto.getHitPointLimit(),
                                             actorDto.getMagicPoint(),
                                             actorDto.getMagicPointLimit());
        pStatusText->setString(pStr->getCString());
        
        auto hpBar = WidgetUtil::getChildByNameRecursion(statusWidget, "HP_bar");
        hpBar->setScaleX((float)actorDto.getHitPoint() / (float)actorDto.getHitPointLimit());
        auto mpBar = WidgetUtil::getChildByNameRecursion(statusWidget, "MP_bar");
        mpBar->setScaleX((float)actorDto.getMagicPoint() / (float)actorDto.getMagicPointLimit());
    }
    
    auto labelGoldText = WidgetUtil::getChildByNameRecursion<ui::Text>(statusWidget, "Label_goldText");
    if (labelGoldText) {
        auto goldTextStr = String::createWithFormat("%10d G", gold);
        labelGoldText->setString(goldTextStr->getCString());
    }
}

void HeaderStatusLayout::setEquip(const ActorDto& actorDto)
{
    auto statusWidget = dynamic_cast<ui::Widget*>(_baseLayout);
    if (!statusWidget) {
        return;
    }
    auto equip1Layout = WidgetUtil::getChildByNameRecursion<ui::Layout>(statusWidget, "equip1");
    {
        auto equip1LabelText = WidgetUtil::getChildByNameRecursion<ui::Text>(equip1Layout, "equip1_label");
        auto equip1Image = WidgetUtil::getChildByNameRecursion<ui::ImageView>(equip1Layout, "equip1_image");
        
        if (actorDto.isEquipWeapon()) {
            equip1Image->setVisible(true);
            std::string spriteFileName = ItemLogic::createItemImageFileName(actorDto.getWeaponEquip().getImgResId());
            equip1Image->loadTexture(spriteFileName);
            std::string labelText = cocos2d::StringUtils::format("%3d", actorDto.getAttackPoint() + actorDto.getWeaponEquip().getTotalParam());
            equip1LabelText->setString(labelText);
            equip1Image->setPosition(Vec2(0 + equip1Image->getLayoutSize().width/2, equip1Layout->getLayoutSize().height/2 - equip1Image->getLayoutSize().height/2));
        } else {
            equip1Image->setVisible(false);
            std::string labelText = cocos2d::StringUtils::format("%3d", actorDto.getAttackPoint());
            equip1LabelText->setString(labelText);
        }
    }
    
    auto equip2Layout = WidgetUtil::getChildByNameRecursion<ui::Layout>(statusWidget, "equip2");
    {
        auto equip2LabelText = WidgetUtil::getChildByNameRecursion<ui::Text>(equip2Layout, "equip2_label");
        auto equip2Image = WidgetUtil::getChildByNameRecursion<ui::ImageView>(equip2Layout, "equip2_image");
        
        if (actorDto.isEquipAccessory()) {
            equip2Image->setVisible(true);
            std::string spriteFileName = ItemLogic::createItemImageFileName(actorDto.getAccessoryEquip().getImgResId());
            equip2Image->loadTexture(spriteFileName);
            std::string labelText = cocos2d::StringUtils::format("%3d", actorDto.getDefencePoint() + actorDto.getAccessoryEquip().getTotalParam());
            equip2LabelText->setString(labelText);
            equip2Image->setPosition(Vec2(0 + equip2Image->getLayoutSize().width/2, equip2Layout->getLayoutSize().height/2 - equip2Image->getLayoutSize().height/2));
        } else {
            equip2Image->setVisible(false);
            std::string labelText = cocos2d::StringUtils::format("%3d", actorDto.getDefencePoint());
            equip2LabelText->setString(labelText);
        }
    }
}
