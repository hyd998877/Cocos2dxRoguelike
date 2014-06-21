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
            RogueLikeGame::AccountData::getInstance()->updateGamePlayProgress(AccountData::GamePlayProgress::TUTORIAL_PLAY);
            int play_quest_id = 1;
            auto scene = RogueScene::scene(RoguePlayDto::QuestType::TUTORIAL, play_quest_id);
            auto trans = TransitionFadeDown::create(1.0f, scene);
            Director::getInstance()->replaceScene(trans);
        });
        auto trans = TransitionFadeDown::create(1.0f, scene);
        Director::getInstance()->replaceScene(trans);
    }
    
    return layout;
}


