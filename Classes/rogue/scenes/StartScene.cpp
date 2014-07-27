/**
 
 @file  StartScene.cpp
 
 @brief description
 
 Cocos2dRogueLike
 
 @author kyokomi
 @date 2014/06/21
 
 */

#include "StartScene.h"

#include "AppMacros.h"

#include "MDungeonDao.h"
#include "MQuestFloor.h"
#include "MMobGroup.h"
#include "MDropGroup.h"
#include "MItemPlusLimitGroup.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"
#include "MUseItemDao.h"
#include "MPlayerDao.h"
#include "MMonsterDao.h"
#include "MLevelDao.h"

#include "json11.hpp"

#include "network/HttpClient.h"

#include "TopScene.h"

USING_NS_CC;
using namespace cocos2d::network;

StartScene::StartScene()
{
    
}

StartScene::~StartScene()
{
    
}

bool StartScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    auto winSize = Director::getInstance()->getWinSize();
    
    auto label = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), "データロード中   ");
    label->setAlignment(cocos2d::TextHAlignment::LEFT, cocos2d::TextVAlignment::CENTER);
    label->setPosition(winSize.width/2-label->getContentSize().width/2, winSize.height/2-label->getContentSize().height/2);
    this->addChild(label);
    label->runAction(RepeatForever::create(Sequence::create(DelayTime::create(1.0f), CallFunc::create([label]() {
        label->setString("データロード中.  ");
    }), DelayTime::create(1.0f), CallFunc::create([label]() {
        label->setString("データロード中.. ");
    }), DelayTime::create(1.0f), CallFunc::create([label]() {
        label->setString("データロード中...");
    }),NULL)));
    
    // TODO: ローディング入れたほうがよさそう
    // InitData
    requestMasterData([label](bool error, json11::Json response) {
        if (error) {
            auto jsonStringFile = FileUtils::getInstance()->getStringFromFile("test_master/RogueGameMaster.json");
            std::string err;
            response = json11::Json::parse(jsonStringFile, err);
            if (!err.empty()) {
                CCLOG("error = %s", err.c_str());
            }
        }
        
        MDungeonDao::getInstance()->init(response["M_DUNGEON"]);
        MQuestFloorDao::getInstance()->init(response["M_QUEST_FLOOR"]);
        MMobGroupDao::getInstance()->init(response["M_MOB_GROUP"]);
        MDropGroupDao::getInstance()->init(response["M_DROP_GROUP"]);
        MItemPlusLimitGroupDao::getInstance()->init(response["M_ITEM_PLUS_LIMIT_GROUP"]);
        MWeaponDao::getInstance()->init(response["M_WEAPON"]);
        MAccessoryDao::getInstance()->init(response["M_ACCESSORY"]);
        MUseItemDao::getInstance()->init(response["M_USER_ITEM"]);
        MPlayerDao::getInstance()->init(response["M_PLAYER"]);
        MMonsterDao::getInstance()->init(response["M_MONSTER"]);
        MLevelDao::getInstance()->init(response["M_LEVEL"]);
        
        // TODO: ローディング解除
        label->setVisible(false);
        
        auto trans = TransitionFadeDown::create(1, MyPageBaseScene::scene<TopScene>());
        Director::getInstance()->replaceScene(trans);
    });
    
    return true;
}

// init MasterData server request.
void StartScene::requestMasterData(RequestMasterDataCallback callback)
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
