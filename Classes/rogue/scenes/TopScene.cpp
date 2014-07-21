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

// init MasterData server request.
void TopScene::requestMasterData(RequestMasterDataCallback callback)
{
    HttpRequest* request = new HttpRequest();
    // Google SpreadSheetの公開URL
    request->setUrl("https://script.google.com/macros/s/AKfycbxDIVz0IR0o-5VTTAwqnEo1lsFZCYo5s58vBJMYvNYBi8gwO_I/exec");
    request->setRequestType(HttpRequest::Type::GET);
    request->setResponseCallback([this, callback](HttpClient* client, HttpResponse* response) {
        if (!response) {
            callback(true, json11::Json());
            return;
        }
        
        if (0 != std::strlen(response->getHttpRequest()->getTag())) {
            CCLOG("%s completed", response->getHttpRequest()->getTag());
        }
        
        long statusCode = response->getResponseCode();
        auto statusString = cocos2d::StringUtils::format("HTTP Status Code: %ld, tag = %s", statusCode, response->getHttpRequest()->getTag());
        CCLOG("response code: %ld", statusCode);
        
        if (!response->isSucceed()) {
            CCLOG("response failed");
            CCLOG("error buffer: %s", response->getErrorBuffer());
            callback(true, json11::Json());
            return;
        }
        
        // dropbox/json11 dump data
        std::string responseString(response->getResponseData()->begin(), response->getResponseData()->end());
        std::string err;
        auto json = json11::Json::parse(responseString, err);
        if (!err.empty()) {
            CCLOG("error = %s", err.c_str());
            callback(true, json11::Json());
        }
        CCLOG("response = %s", json.dump().c_str());
        callback(false, json);
    });
    request->setTag("GET test1");
    HttpClient::getInstance()->send(request);
    request->release();
}
