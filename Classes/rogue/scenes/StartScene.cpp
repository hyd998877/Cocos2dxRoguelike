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
#include "MQuestFloorDao.h"
#include "MMobGroupDao.h"
#include "MDropGroupDao.h"
#include "MItemPlusLimitGroupDao.h"

#include "MWeaponDao.h"
#include "MAccessoryDao.h"
#include "MUseItemDao.h"
#include "MPlayerDao.h"
#include "MMonsterDao.h"
#include "MLevelDao.h"

#include "json11.hpp"

#include "TopScene.h"

USING_NS_CC;
using namespace cocos2d::network;

StartScene::StartScene()
: _request(nullptr)
{
    
}

StartScene::~StartScene()
{
    _request->release();
    _request = nullptr;
}

bool StartScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    _request = new HttpRequest();
    
    auto winSize = Director::getInstance()->getWinSize();

    // 気合ローディング処理
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
    
    // ログイン
    auto loginUrl = "http://rogue-game-web.herokuapp.com/login/";
    request(HttpRequest::Type::POST, json11::Json::object{{"uuid", "1111111111"}, {"name", "test"}}, loginUrl, [this, label](bool error, json11::Json response) {
        std::string versionCode;
        if (error) {
            CCLOG("login error");
            versionCode = "1";
        } else {
            versionCode = response["VersionCode"].string_value();
        }
        
        // マスターデータの読みこみ
        auto masterLoadUrl = StringUtils::format("http://rogue-game-web.herokuapp.com/load/master/%s", versionCode.c_str());
        this->requestLoadMaster(masterLoadUrl, [label](bool error, json11::Json response) {
            if (error) {
                CCLOG("load master error");
            }
            label->setVisible(false);
            
            auto trans = TransitionFadeDown::create(1.0f, MyPageBaseScene::scene<TopScene>());
            Director::getInstance()->replaceScene(trans);
        });
    });
    
    return true;
}

void StartScene::requestLoadMaster(const std::string &requestUrl, const RequestCallback &callback)
{
    request(HttpRequest::Type::GET, json11::Json::NUL, requestUrl, [callback](bool error, json11::Json response) {
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
        
        // ローディング解除
        callback(false, json11::Json::NUL);
    });
}

// server request json.
void StartScene::request(HttpRequest::Type requestType, json11::Json requestData, const std::string &requestUrl, const RequestCallback &callback)
{
    _request->setUrl(requestUrl.c_str());
    _request->setRequestType(requestType);
    if (!requestData.is_null()) {
        auto data = requestData.dump().c_str();
        _request->setRequestData(data, sizeof(data));
    }
    _request->setResponseCallback([this, callback](HttpClient* client, HttpResponse* response) {
        if (!response) {
            callback(true, json11::Json::NUL);
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
            callback(true, json11::Json::NUL);
            return;
        }
        
        // TODO: check contentType
        
        // dropbox/json11 dump data
        std::string responseString(response->getResponseData()->begin(), response->getResponseData()->end());
        std::string err;
        auto json = json11::Json::parse(responseString, err);
        if (!err.empty()) {
            CCLOG("error = %s", err.c_str());
            callback(true, json11::Json::NUL);
            return;
        }
        CCLOG("response = %s", json.dump().c_str());
        callback(false, json);
    });
    _request->setTag("GET test1");
    HttpClient::getInstance()->send(_request);
}
