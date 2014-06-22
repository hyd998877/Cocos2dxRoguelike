/**

@file  QuestChoiceScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/22

*/

#include "QuestChoiceScene.h"
#include "QuestPageLayer.h"
#include "AccountData.h"
#include "NovelScene.h"

#include "AlertDialogLayer.h"

USING_NS_CC;

using namespace RogueLikeGame;

QuestChoiceScene::QuestChoiceScene()
{
    
}

QuestChoiceScene::~QuestChoiceScene()
{
    
}

Node* QuestChoiceScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = QuestPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));

    if (AccountData::getInstance()->getGamePlayProgress() == AccountData::GamePlayProgress::INIT) {
        Scene* scene = NovelScene::scene(0, 0, [this]() {
            AccountData::getInstance()->updateGamePlayProgress(AccountData::GamePlayProgress::TUTORIAL_PLAY);
            this->playQuest(RoguePlayDto::QuestType::TUTORIAL, 1);
        });
        auto trans = TransitionFadeDown::create(1.0f, scene);
        Director::getInstance()->replaceScene(trans);
    }

    layout->setMenuCallback1([this](Ref *ref) {
        this->playQuest(RoguePlayDto::QuestType::TUTORIAL, 1);
    });
    
    layout->setMenuCallback2([this](Ref *ref) {
        // チュートリアルクリアしてないとプレイできない
        if (AccountData::getInstance()->getGamePlayProgress() < AccountData::GamePlayProgress::TUTORIAL_CLEAR) {
            this->showBadPlayQuest();
            return;
        }
        // メインクエスト
        this->playQuest(RoguePlayDto::QuestType::MAIN_QUEST, 1);
    });
    
    layout->setMenuCallback3([this](Ref *ref) {
        // メインクリアしてないとプレイできない
        if (AccountData::getInstance()->getGamePlayProgress() < AccountData::GamePlayProgress::MAINQUEST_CLEAR) {
            this->showBadPlayQuest();
            return;
        }

        // アイテムとゴールド所持の警告
        this->getDialogLayer()->addChild(AlertDialogLayer::createWithContentSizeModal("このダンジョンは\nアイテムやゴールドを持ち込むことはできません。\n持っているアイテムやゴールドをすべて失いますが\nそれでもいいですか？", "は　い", [this](Ref *ref) {
            // データリセット
            AccountData::getInstance()->resetRoguePlayDataAndInventory();
            // クエスト開始
            this->playQuest(RoguePlayDto::QuestType::DEEP_QUEST, 1);
        }, "いいえ", [](Ref *ref) {}));
    });
    
    return layout;
}

void QuestChoiceScene::showBadPlayQuest()
{
    this->getDialogLayer()->addChild(AlertDialogLayer::createWithContentSizeModal("このダンジョンはまだプレイできません。", "とじる", [](Ref *ref) {}));
}

void QuestChoiceScene::playQuest(RoguePlayDto::QuestType questType, int questId)
{
    auto scene = RogueScene::scene(questType, questId);
    auto trans = TransitionFadeDown::create(1.0f, scene);
    Director::getInstance()->replaceScene(trans);
}

