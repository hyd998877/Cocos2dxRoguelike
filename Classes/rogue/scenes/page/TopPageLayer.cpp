/**
 @file TopPageLayer.h
 
 @brief トップ画面
 
 お知らせ的な表示とか
 
 @author kyokomi
 @date 2014/05/07
 
 */

#include "TopPageLayer.h"

#include "AccountData.h"

#include "CommonWindowUtil.h"

#include "MypageHeaderLayer.h"

USING_NS_CC;

TopPageLayer::TopPageLayer()
{
    
}

TopPageLayer::~TopPageLayer()
{
    
}

bool TopPageLayer::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    Size winSize = Director::getInstance()->getWinSize();
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    auto background = Sprite::create("bg/saloon06_c.jpg");
    background->setOpacity(96);
    background->setScale(winSize.width/background->getContentSize().width);
    background->setPosition(Point(winSize.width/2, winSize.height/2));
    this->addChild(background);
    
    // キャラ表示
    auto playerSprite = Sprite::create("novel/actor4_novel_s_0.png");
    playerSprite->setScale(resolutionSize.height/playerSprite->getContentSize().height);
    playerSprite->setPosition(Point(winSize.width * 0.2, winSize.height * 0.4));
    playerSprite->setFlippedX(true);
    this->addChild(playerSprite);
    
    // セリフ表示
    auto messageLayer = LayerColor::create(Color4B::BLACK);
    messageLayer->setContentSize(Size(winSize.width / 2, winSize.height / 3));
    messageLayer->setPosition(Point(winSize.width * 0.7 - messageLayer->getContentSize().width / 2,
                                     winSize.height * 0.5 - messageLayer->getContentSize().height / 2));
    this->addChild(messageLayer);
    
    // TODO: なんかランダムなメッセージとかにしたいです
    std::string messageText = "今日も気合いれてダンジョンいこー";
    auto gameProgress = RogueLikeGame::AccountData::getInstance()->getGamePlayProgress();
    if (gameProgress == RogueLikeGame::AccountData::GamePlayProgress::INIT) {
        messageText = "はじめまして！\nまずはクエストをやってみよう！";
    }
    auto messageTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), messageText);
    messageTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    messageTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    messageLayer->addChild(messageTextLabel);
    messageTextLabel->setPosition(Point(messageLayer->getContentSize().width / 2, messageLayer->getContentSize().height / 2));
    // 枠
    CommonWindowUtil::attachWindowWaku(messageLayer);
    
    auto headerLayer = MypageHeaderLayer::create();
    headerLayer->setTitleText("とっぷぺーじ");
    this->addChild(headerLayer);
    
    return true;
}