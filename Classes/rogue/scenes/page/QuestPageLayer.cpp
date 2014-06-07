//
//  QuestPageLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/07.
//
//

#include "QuestPageLayer.h"

#include "AccountData.h"

#include "CommonWindowUtil.h"
#include "MypageHeaderLayer.h"
#include "AlertDialogLayer.h"

#include "RogueScene.h"

USING_NS_CC;

NS_ROGUE_BEGIN

QuestPageLayer::QuestPageLayer()
{
    
}

QuestPageLayer::~QuestPageLayer()
{
    
}

bool QuestPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }

    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/ad_forest01_c.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width/background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    Size layerSize = Size(winSize.width*0.35, winSize.height*0.2);
    
    auto iconNode1 = Sprite::create("icon_set/item_0.png");
    auto menuItem1 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode1, FontUtils::getDefaultFontTTFConfig(), "初心者の洞窟", [this](Ref *ref) {
        // 初心者クエスト
        int play_quest_id = 1;
        auto scene = RogueLikeGame::RogueScene::scene(RoguePlayDto::QuestType::TUTORIAL, play_quest_id);
        auto trans = TransitionFadeDown::create(1.0f, scene);
        Director::getInstance()->replaceScene(trans);
    });
    auto iconNode2 = Sprite::create("icon_set/item_0.png");
    auto menuItem2 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode2, FontUtils::getDefaultFontTTFConfig(), "不思議な洞窟", [this](Ref *ref) {
        // チュートリアルクリアしてないとプレイできない
        if (RogueLikeGame::AccountData::getInstance()->getGamePlayProgress() < RogueLikeGame::AccountData::GamePlayProgress::TUTORIAL_CLEAR) {
            this->addChild(AlertDialogLayer::createWithContentSizeModal("このダンジョンはまだプレイできません。", "とじる", [](Ref *ref) {}));
            return;
        }
        
        // メインクエスト
        int play_quest_id = 1;
        auto scene = RogueLikeGame::RogueScene::scene(RoguePlayDto::QuestType::MAIN_QUEST, play_quest_id);
        auto trans = TransitionFadeDown::create(1.0f, scene);
        Director::getInstance()->replaceScene(trans);
    });
    auto iconNode3 = Sprite::create("icon_set/item_0.png");
    auto menuItem3 = CommonWindowUtil::createMenuItemLabelWithSpriteIcon(layerSize, iconNode3, FontUtils::getDefaultFontTTFConfig(), "最果ての洞窟", [this](Ref *ref) {
        // メインクリアしてないとプレイできない
        if (RogueLikeGame::AccountData::getInstance()->getGamePlayProgress() < RogueLikeGame::AccountData::GamePlayProgress::MAINQUEST_CLEAR) {
            this->addChild(AlertDialogLayer::createWithContentSizeModal("このダンジョンはまだプレイできません。", "とじる", [](Ref *ref) {}));
            return;
        }
        
        // アイテムとゴールド所持の警告
        this->addChild(AlertDialogLayer::createWithContentSizeModal("このダンジョンは\nアイテムやゴールドを持ち込むことはできません。\n持っているアイテムやゴールドをすべて失いますが\nそれでもいいですか？", "は　い", [](Ref *ref) {
            // データリセット
            RogueLikeGame::AccountData::getInstance()->resetRoguePlayDataAndInventory();
            
            // クエスト開始
            int play_quest_id = 1;
            auto scene = RogueLikeGame::RogueScene::scene(RoguePlayDto::QuestType::DEEP_QUEST, play_quest_id);
            auto trans = TransitionFadeDown::create(1.0f, scene);
            Director::getInstance()->replaceScene(trans);
        }, "いいえ", [](Ref *ref) {}));
    });
    
    auto menu = Menu::create(menuItem1, menuItem2, menuItem3, NULL);
    menu->setPosition(Point(winSize.width/5, winSize.height/2));
    menu->alignItemsVerticallyWithPadding(5);
    this->addChild(menu);
    
    auto headerLayer = MypageHeaderLayer::create();
    headerLayer->setTitleText("くえすと選択");
    this->addChild(headerLayer);
    
    return true;
}

NS_ROGUE_END

