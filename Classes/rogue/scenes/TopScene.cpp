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

USING_NS_CC;

TopScene::TopScene()
{
    
}

TopScene::~TopScene()
{
    
}

Node* TopScene::initLayout()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    auto layout = TopPageLayer::create();
    layout->setPosition(Vec2(visibleSize.width/2 - layout->getContentSize().width/2*layout->getScaleX(),
                             visibleSize.height/2 - layout->getContentSize().height/2*layout->getScaleY()));

    // InitData
    loadMasterData();
    
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

void TopScene::loadMasterData()
{
    auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/RogueGameMaster.json");
    std::string err;
    auto json = json11::Json::parse(jsonStringFile, err);
    if (!err.empty()) {
        CCLOG("error = %s", err.c_str());
    }
    MWeaponDao::getInstance()->init(json["M_WEAPON"]);
    MAccessoryDao::getInstance()->init(json["M_ACCESSORY"]);
    MUseItemDao::getInstance()->init(json["M_USER_ITEM"]);
    MPlayerDao::getInstance()->init(json["M_PLAYER"]);
    MMonsterDao::getInstance()->init(json["M_MONSTER"]);
}
