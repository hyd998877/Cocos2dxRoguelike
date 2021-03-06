/**

@file  TopScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#include "TopScene.h"
#include "TopPageLayer.h"
#include "AccountData.h"

#include "AlertDialogLayer.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"
#include "MUseItemDao.h"
#include "MPlayerDao.h"
#include "MMonsterDao.h"
#include "json11.hpp"

#include "network/HttpClient.h"

#include "AudioUtil.h"

USING_NS_CC;
using namespace cocos2d::network;

TopScene::TopScene()
{
    
}

TopScene::~TopScene()
{
    
}

Node* TopScene::initLayout()
{
    AudioUtil::playBGM(AudioUtil::BGM_MYPAGE);
    
    auto visibleSize = Director::getInstance()->getVisibleSize();
    
    auto layout = TopPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));

    // セーブあり
    if (RogueLikeGame::AccountData::getInstance()->isQuestSaveData()) {
        this->initQuestSave();
    }
    
    return layout;
}

void TopScene::initQuestSave()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    std::string saveTitle = "前回プレイしたデータが残っています。\n再開しますか？";
    std::string saveDetail = RogueLikeGame::AccountData::getInstance()->createQuestSaveDetailText();
    std::string dialogTitle = saveTitle + "\n\n" + saveDetail + "\n\n※いいえを選択すると、\n所持していたアイテムは消えます";
    
    this->addChildDialog(AlertDialogLayer::createWithContentSizeModal(dialogTitle, "はい", [](Ref *ref) {
        auto scene = RogueLikeGame::RogueScene::scene(RogueLikeGame::AccountData::getInstance()->getRoguePlayData().getQuestType(), RogueLikeGame::AccountData::getInstance()->getRoguePlayData().getQuestId());
        auto trans = TransitionFadeDown::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    }, "いいえ", [this](Ref *ref) {
        RogueLikeGame::AccountData::getInstance()->resetRoguePlayData();
    }));
}
