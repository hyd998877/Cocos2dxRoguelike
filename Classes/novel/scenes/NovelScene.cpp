//
//  NovelScene.cpp
//  NovelGame
//
//  Created by kyokomi on 2013/09/01.
//
//

#include "AppMacros.h"
#include "NovelScene.h"
// -----------------------------
#include "TableViewTestLayer.h"

#include "CommonWindowUtil.h"
#include "LabelRPG.h"

USING_NS_CC;
USING_NS_CC_EXT;

NovelScene::NovelScene()
:m_textIndex(0),
m_novelJson(NULL),
m_novelTextEndCallback(nullptr)
{
    
}

NovelScene::~NovelScene()
{
    CCLOG("dispose!!!!");
    Json_dispose(m_novelJson);
    m_novelTextEndCallback = nullptr;
}

Scene* NovelScene::scene(int sceneNo, int novelIndex, const NovelTextEndCallback& callback)
{
    Scene* scene = Scene::create();
    NovelScene* layer = NovelScene::create(sceneNo, novelIndex, callback);
    scene->addChild(layer);
    
    return scene;
}

NovelScene* NovelScene::create(int sceneNo, int novelIndex, const NovelTextEndCallback& callback)
{
    NovelScene *pRet = new NovelScene();
    if (pRet && pRet->init(sceneNo, novelIndex, callback))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

bool NovelScene::init(int sceneNo, int novelIndex, const NovelTextEndCallback& callback)
{
    if (!Layer::init())
    {
        return false;
    }
    this->m_novelTextEndCallback = callback;
    
    // TODO: #12 BGM再生
//    AudioUtil::playBGM("audio/tutorial_bgm1.mp3");
    
    // シングルタップイベントを受け付ける
    auto listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(NovelScene::onTouchBegan, this);
    //listener->onTouchMoved = CC_CALLBACK_2(NovelScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(NovelScene::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    Size winSize = Director::getInstance()->getWinSize();
    
    // 背景表示
    
    // -----------------------------
    // テキスト表示 TODO: (リファクタリング) Class化したい・・・
    // -----------------------------
    
    // 本文
    LayerColor * textLayer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), winSize.width, winSize.height * 0.25);
    textLayer->setPosition(Point::ZERO);
    this->addChild(textLayer, kZOrder_TextLayer, kTag_TextLayer);

    // test
    std::string string = cocos2d::StringUtils::format("w = %f.1 h = %f.1", winSize.width, winSize.height);
    CCLOG("init %s", string.c_str());
    
    // 本文テキスト
    auto textLabel = LabelRPG::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "");
    textLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    textLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    
    textLabel->setColor(Color3B::WHITE);
    textLabel->setOriginalPosition(Point(0, textLayer->getContentSize().height));

    textLayer->addChild(textLabel, kZOrder_TextLayer, kTag_TextLayer_textLabel);
    
    CommonWindowUtil::attachWindowWaku(textLayer);
    
    // 名前
    LayerColor * nameTextLayer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), winSize.width * 0.4, winSize.height * 0.1);
    nameTextLayer->setPosition(Point(textLayer->getPositionX(), textLayer->getPositionY() + textLayer->getContentSize().height + nameTextLayer->getContentSize().height * 0.05));
    this->addChild(nameTextLayer, kZOrder_TextLayer, kTag_TextLayer_name);
    // 名前テキスト
    auto nameTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "");
    nameTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
    nameTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    nameTextLabel->setPosition(Point(nameTextLabel->getSystemFontSize() / 2 + nameTextLabel->getContentSize().width / 2, nameTextLayer->getContentSize().height - nameTextLabel->getContentSize().height / 2 - nameTextLabel->getSystemFontSize() / 2));
    nameTextLayer->addChild(nameTextLabel, kZOrder_TextLayer, kTag_TextLayer_nameTextLabel);
    
    CommonWindowUtil::attachWindowWaku(nameTextLayer);

    // -----------------------------
    UserDefault::getInstance()->setIntegerForKey("sceneNo", sceneNo);
    UserDefault::getInstance()->flush();
    // json読み込み
    initNovelJson(sceneNo, novelIndex);
    
    return true;
}

void NovelScene::initNovelJson(int sceneNo, int novelIndex)
{
    // jsonファイル読み込み
    std::string fileName = cocos2d::StringUtils::format("novel/json/scene_%03d.json", sceneNo);
    auto novelJsonFileData = FileUtils::getInstance()->getDataFromFile(fileName);
    
    auto json = Json_create((char *)novelJsonFileData.getBytes());
    m_novelJson = Json_getItem(json, "novel");
    novelJsonFileData.clear();
    if (novelIndex == 0) {
        return;
    }
}

/**
 タッチイベント
 */
bool NovelScene::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    CCLOG("%s", "------ ccTouchBegan ------");
    // 選択肢とバックログ表示中は何もしない
//    if (m_isMenuSelect || m_isShowTextLog)
//    {
//        return false;
//    }
    
    return true;
}
void NovelScene::onTouchEnded(Touch *pTouch, Event *pEvent)
{
    // テキストエリアのタッチを判定
    auto pLayer = static_cast<Layer*>(this->getChildByTag(kTag_TextLayer));
    if (pLayer->getBoundingBox().containsPoint(pTouch->getLocation())) {
        
        // TODO: #12 テキスト送りのタッチ音
//        AudioUtil::playBtnSE();

        // 保存
        UserDefault::getInstance()->setIntegerForKey("novelIndex", this->m_textIndex);
        UserDefault::getInstance()->flush();

        CCLOG("touch text");
        
        // テキストをすすめる
        nextNovelJson();
    }
}

void NovelScene::dispText(const std::string& text)
{
    auto pTextLayer = dynamic_cast<Layer*>(this->getChildByTag(kTag_TextLayer));
    if (pTextLayer)
    {
        // テキストをすすめる
        auto pTextLabel = dynamic_cast<LabelRPG*>(pTextLayer->getChildByTag(kTag_TextLayer_textLabel));
        pTextLabel->setStringWithRunText(text, 0.05f);
    }
}

void NovelScene::dispName(const std::string& name)
{
    auto pLayer = dynamic_cast<Layer*>(this->getChildByTag(kTag_TextLayer_name));
    if (pLayer)
    {
        // テキストをすすめる
        auto nameTextLabel = dynamic_cast<Label*>(pLayer->getChildByTag(kTag_TextLayer_nameTextLabel));
        nameTextLabel->setString(name.c_str());
        nameTextLabel->setPosition(Point(nameTextLabel->getSystemFontSize() / 2 + nameTextLabel->getContentSize().width / 2, pLayer->getContentSize().height / 2));
    }
}

void NovelScene::nextNovelJson()
{
    int novelSize = m_novelJson->size;
    Json* item = nullptr;
    CCLOG("index = %d (%d)", m_textIndex, novelSize);
    // 終了判定
    if (m_textIndex == novelSize)
    {
        endNovel();
        return;
    }
    
    for (int i = 0; i < novelSize; i++)
    {
        if (item == nullptr)
        {
            item = m_novelJson->child;
        }
        else
        {
            item = item->next;
        }
        
        if (i < m_textIndex)
        {
            continue;
        }
        
        int textType = Json_getItem(item, "type")->valueInt;
        if (textType == NovelType::SelectItem)
        {
            // 選択肢表示
            makeSelectSpriteButton(Json_getItem(item, "select1")->valueString, Json_getItem(item, "next1Id")->valueInt,
                                   Json_getItem(item, "select2")->valueString, Json_getItem(item, "next2Id")->valueInt);
        }
        else if (textType == NovelType::ActorImageShow)
        {
            // アクター生成と表示
            int dict = Json_getItem(item, "dict")->valueInt;
            std::string imgFilePath = Json_getItem(item, "imgPath")->valueString;
            makeActorImage(imgFilePath.c_str(), dict);
        }
        else if (textType == NovelType::ActorImageHide)
        {
            // 表示中のアクターを消去
            int dict = Json_getItem(item, "dict")->valueInt;
            removeActorImage(dict);
        }
        else if (textType == NovelType::BackgroundShow)
        {
            // 背景切替
            std::string imgFilePath = Json_getItem(item, "imgPath")->valueString;
            changeBackgroundAnimation(imgFilePath);
        }
        else if(textType == NovelType::ActorImageShadeShow)
        {
            // 影設定
            int dict = Json_getItem(item, "dict")->valueInt;
            shadeActorImage(dict);
        }
        else if(textType == NovelType::ActorImageShadeHide)
        {
            // 影解除
            int dict = Json_getItem(item, "dict")->valueInt;
            resetShadeActorImage(dict);
        }

        m_textIndex++;

        if (Json_getItem(item, "name"))
        {
            dispName(Json_getItem(item, "name")->valueString);
        }
        
        if (Json_getItem(item, "text"))
        {
            // テキスト取得できたらループを抜ける
            dispText(Json_getItem(item, "text")->valueString);
            break;
        }
    }
}

void NovelScene::changeBackgroundAnimation(const std::string& imgFilePath)
{
    Size winSize = Director::getInstance()->getWinSize();
    
    auto node = this->getChildByTag(kTag_Background);
    if (node) {
        auto background = static_cast<Sprite*>(node);
        FadeOut* fadeOut = FadeOut::create(0.5);
        FadeIn* fadeIn = FadeIn::create(0.5);
        
        Texture2D *texture = Director::getInstance()->getTextureCache()->getTextureForKey(imgFilePath);
        if (!texture)
        {
            texture = Director::getInstance()->getTextureCache()->addImage(imgFilePath);
        }
        auto pActionSeq = Sequence::create(fadeOut, CallFunc::create([this, winSize, background, texture]() {
            background->setTexture(texture);
            background->setScale(winSize.width/background->getContentSize().width);
        }), fadeIn, NULL);
        
        background->runAction(pActionSeq);
    } else {
        auto background = Sprite::create(imgFilePath);
        background->setScale(winSize.width/background->getContentSize().width);
        background->setPosition(Point(winSize.width * 0.5, winSize.height * 0.5));
        this->addChild(background, kZOrder_Background, kTag_Background);
    }
}

void NovelScene::makeSelectSpriteButton(const std::string& str1, int next1Id, const std::string& str2, int next2Id)
{

}


void NovelScene::menuSelectCallback(Ref *ref)
{
    return;
}


void NovelScene::makeActorImage(const char* imageFilePath, int dict)
{
    int dictTag = dict + kTag_ActorDict;
    
    Size resolutionSize = Director::getInstance()->getOpenGLView()->getDesignResolutionSize();
    
    // Sprite生成
    Sprite* actor = Sprite::create(imageFilePath);
    actor->setScale(resolutionSize.height/actor->getContentSize().height);
    
    float scaleToWidth = actor->getContentSize().width * actor->getScale();
    // 生成前に念のためremoveしておく
    removeActorImage(dict);
    
    Size winSize = Director::getInstance()->getWinSize();
    Point point = Point::ZERO;
    if (dictTag == kTag_ActorDictLeft)
    {
        point = Point(winSize.width*0.40 - scaleToWidth/2, actor->boundingBox().size.height * 0.5);
        actor->setFlippedX(true); // 右向きにする
        
        // centerとrightを暗くする
        shadeActorImage(kTag_ActorDictCenter-kTag_ActorDict);
        shadeActorImage(kTag_ActorDictRight-kTag_ActorDict);
    }
    else if (dictTag == kTag_ActorDictCenter)
    {
        point = Point(winSize.width*0.75 - scaleToWidth/2, actor->boundingBox().size.height * 0.5);
        
        // leftとrightを暗くする
        shadeActorImage(kTag_ActorDictLeft-kTag_ActorDict);
        shadeActorImage(kTag_ActorDictRight-kTag_ActorDict);
    }
    else if (dictTag == kTag_ActorDictRight)
    {
        point = Point(winSize.width*1.10 - scaleToWidth/2, actor->boundingBox().size.height * 0.5);
        
        // leftとcenterとを暗くする
        shadeActorImage(kTag_ActorDictLeft-kTag_ActorDict);
        shadeActorImage(kTag_ActorDictCenter-kTag_ActorDict);
    }
    actor->setPosition(point);    
    this->addChild(actor, kZOrder_Actor, dictTag);
}

void NovelScene::removeActorImage(int dict)
{
    int dictTag = dict + kTag_ActorDict;
    Sprite* actor = (Sprite*) this->getChildByTag(dictTag);
    if (actor)
    {
        actor->setVisible(false);
        actor->removeFromParentAndCleanup(true);
    }
}

void NovelScene::shadeActorImage(int dict)
{
    int dictTag = dict + kTag_ActorDict;
    Sprite* actor = (Sprite*) this->getChildByTag(dictTag);
    if (actor)
    {
        actor->setColor(Color3B(100, 100, 100));
    }
}

void NovelScene::resetShadeActorImage(int dict)
{
    int dictTag = dict + kTag_ActorDict;
    Sprite* actor = (Sprite*) this->getChildByTag(dictTag);
    if (actor)
    {
        actor->setColor(Color3B::WHITE);
    }
}

void NovelScene::logMenuSelectCallback(cocos2d::Ref *pSender)
{
    
}

void NovelScene::showTextLog(int showTextIndex)
{

}

void NovelScene::hideTextLog()
{

}

void NovelScene::endNovel()
{
    // コールバックよんで終わり
    if (m_novelTextEndCallback)
    {
        m_novelTextEndCallback();
    }
}

