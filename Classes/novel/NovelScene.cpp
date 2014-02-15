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
#include "LabelSprite.h"
#include "MenuItemSelectLabelSprite.h"
#include "TableViewTestLayer.h"

#include "CommonWindowUtil.h"

#include "cocos2d.h"
#include "spine/Json.h"
//#include "AudioUtil.h"
#include "cocos-ext.h"

//#include "Json.h"

using namespace std;
using namespace cocos2d;
using namespace extension;

NovelScene::NovelScene()
:m_textIndex(0),
isMenuSelect(false),
isShowTextLog(false),
m_novelJson(NULL)
//m_novelJsonFile(NULL)
{
    
}

NovelScene::~NovelScene()
{
    Json_dispose(m_novelJson);
}

void NovelScene::initNovelJson(int sceneNo, int novelIndex)
{
    std::string inputJsonFileName = StringUtils::format("json/scene_%03d.json", sceneNo);
    // jsonファイル読み込み
    auto novelJsonFileData = FileUtils::getInstance()->getDataFromFile(inputJsonFileName);
    CCLOG("Data is : %s",novelJsonFileData.getBytes());
    unsigned long size = novelJsonFileData.getSize();
    CCLOG("Size: %lu\n\n", size);
    
    Json* json = Json_create((char *)novelJsonFileData.getBytes());
    m_novelJson = Json_getItem(json, "novel");
    if (novelIndex == 0) {
        return;
    }
    
    int arrayCount = m_novelJson->size;
    for (int i = 0; i < arrayCount; i++) {
        Json* item = m_novelJson->next;
        
        if (i >= novelIndex) {
            continue;
        }
        
        int textType = Json_getItem(item, "type")->valueInt;
        if (textType == kSelectItem)
        {
            i++;
            continue;
        }
        m_textIndex = i;
        nextNovelJson();
    }
}

Scene* NovelScene::scene(int sceneNo, int novelIndex)
{
    Scene* scene = Scene::create();
    NovelScene* layer = NovelScene::create(sceneNo, novelIndex);
    scene->addChild(layer);
    
    return scene;
}

NovelScene* NovelScene::create(int sceneNo, int novelIndex)
{
    NovelScene *pRet = new NovelScene();
    if (pRet && pRet->init(sceneNo, novelIndex))
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

bool NovelScene::init()
{
    return init(1, 0);
}

bool NovelScene::init(int sceneNo, int novelIndex)
{
    if (!Layer::init())
    {
        return false;
    }
    
    // BGM再生
//    AudioUtil::playBGM("audio/tutorial_bgm1.mp3");
    
    // シングルタップイベントを受け付ける
//    this->setTouchEnabled(true);
//    this->setTouchMode(kCCTouchesOneByOne);
    auto listener = static_cast<EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
    listener->setSwallowTouches(true);
    
    listener->onTouchBegan = CC_CALLBACK_2(NovelScene::onTouchBegan, this);
    //listener->onTouchMoved = CC_CALLBACK_2(NovelScene::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(NovelScene::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    
    Size winSize = Director::getInstance()->getWinSize();
    
    // 背景表示
    Sprite* background = Sprite::create("novel/013-PostTown01.jpg");
    background->setPosition(Point(winSize.width * 0.5, winSize.height * 0.5));
    this->addChild(background, kZOrder_Background, kTag_Background);
    
    // -----------------------------
    // TODO: テキスト表示 Class化したい・・・
    // -----------------------------
    
    // 本文
    int fontSize = 14;
    LayerColor * textLayer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), winSize.width, winSize.height * 0.25);
    textLayer->setPosition(Point::ZERO);
    this->addChild(textLayer, kZOrder_TextLayer, kTag_TextLayer);

    // test
    std::string string = StringUtils::format("w = %f.1 h = %f.1 f = %d", winSize.width, winSize.height, fontSize);
    
    // 本文テキスト
    LabelTTF* textLabel = LabelTTF::create(string, GAME_FONT(fontSize), fontSize);
    textLabel->setColor(Color3B::WHITE);
    textLabel->setPosition(Point(fontSize + textLabel->getContentSize().width / 2, textLayer->getContentSize().height - fontSize));

    textLayer->addChild(textLabel, kZOrder_TextLayer, kTag_TextLayer_textLabel);
    
    CommonWindowUtil::attachWindowWaku(textLayer);
    
    // 名前
    LayerColor * nameTextLayer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), winSize.width * 0.4, winSize.height * 0.1);
    nameTextLayer->setPosition(Point(textLayer->getPositionX(), textLayer->getPositionY() + textLayer->getContentSize().height + nameTextLayer->getContentSize().height * 0.05));
    this->addChild(nameTextLayer, kZOrder_TextLayer, kTag_TextLayer_name);
    // 名前テキスト
    LabelTTF* nameTextLabel = LabelTTF::create("SYSTEM", GAME_FONT(fontSize), fontSize);
    nameTextLabel->setColor(Color3B::GREEN);
    nameTextLabel->setPosition(Point(fontSize + nameTextLabel->getContentSize().width / 2, nameTextLayer->getContentSize().height / 2));
    nameTextLayer->addChild(nameTextLabel, kZOrder_TextLayer, kTag_TextLayer_nameTextLabel);
    
    CommonWindowUtil::attachWindowWaku(nameTextLayer);
    
    // -----------------------------
    // TODO: キャラ顔画像表示
    
    // -----------------------------
    // ログ表示用ボタン配置
    
    LabelTTF* logButtonLabel = LabelTTF::create("Log", GAME_FONT(fontSize), fontSize);
    MenuItemLabel* logButtonMenuItem = MenuItemLabel::create(logButtonLabel, [this](Object *pSender) {
        this->logMenuSelectCallback(pSender);
    });
    
    logButtonMenuItem->setPosition(Point(winSize.width * 0.95, logButtonMenuItem->getContentSize().height));
    
    Menu* pMenu = Menu::create(logButtonMenuItem, NULL);
    pMenu->setPosition(Point::ZERO);
    this->addChild(pMenu, kZOrder_MenuItem, kTag_MenuItem_log);

    // -----------------------------
    // json読み込み
    UserDefault::getInstance()->setIntegerForKey("sceneNo", sceneNo);
    UserDefault::getInstance()->flush();
    initNovelJson(sceneNo, novelIndex);
    
    return true;
}

/**
 タッチイベント
 */
bool NovelScene::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    CCLOG("%s", "------ ccTouchBegan ------");
    // 選択肢とバックログ表示中は何もしない
    if (isMenuSelect || isShowTextLog)
    {
        return false;
    }
    
    return true;
}
void NovelScene::onTouchEnded(Touch *pTouch, Event *pEvent)
{
    // テキストエリアのタッチを判定
    Layer* pLayer = (Layer*) this->getChildByTag(kTag_TextLayer);
    if (pLayer->getBoundingBox().containsPoint(pTouch->getLocation()))
    {
//        AudioUtil::playBtnSE();

        // 保存
        UserDefault::getInstance()->setIntegerForKey("novelIndex", m_textIndex);
        UserDefault::getInstance()->flush();

        CCLOG("touch text");
        // テキストをすすめる
        nextNovelJson();
    }
}

void NovelScene::dispText(string text)
{
    Layer* pTextLayer = (Layer*) this->getChildByTag(kTag_TextLayer);
    if (pTextLayer)
    {
        // テキストをすすめる
        LabelTTF* pTextLabel = (LabelTTF*) pTextLayer->getChildByTag(kTag_TextLayer_textLabel);
        pTextLabel->setString(text.c_str());
        pTextLabel->setPosition(Point(pTextLabel->getFontSize() + pTextLabel->getContentSize().width / 2, pTextLayer->getContentSize().height - pTextLabel->getFontSize()));
    }
}

void NovelScene::dispName(string name)
{
    Layer* pLayer = (Layer*) this->getChildByTag(kTag_TextLayer_name);
    if (pLayer)
    {
        // テキストをすすめる
        LabelTTF* nameTextLabel = (LabelTTF*) pLayer->getChildByTag(kTag_TextLayer_nameTextLabel);
        nameTextLabel->setString(name.c_str());
    }
}

void NovelScene::nextNovelJson()
{
    int novelSize = m_novelJson->size;
    Json* item = nullptr;
    CCLOG("index = %d (%d)", m_textIndex, novelSize);
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
        
        //CCLOG("type = %d", Json_getItem(item, "type")->valueInt);
        if (i < m_textIndex)
        {
            continue;
        }
        
        int textType = Json_getItem(item, "type")->valueInt;
        if (textType == kSelectItem)
        {
            // 選択肢表示
            isMenuSelect = true;
            makeSelectSpriteButton(Json_getItem(item, "select1")->valueString, Json_getItem(item, "next1Id")->valueInt,
                                   Json_getItem(item, "select2")->valueString, Json_getItem(item, "next2Id")->valueInt);
        }
        else if (textType == kActorImageShow)
        {
            // アクター生成と表示
            int dict = Json_getItem(item, "dict")->valueInt;
            string imgFilePath = Json_getItem(item, "imgPath")->valueString;
            makeActorImage(imgFilePath.c_str(), dict);
        }
        else if (textType == kActorImageHide)
        {
            // 表示中のアクターを消去
            int dict = Json_getItem(item, "dict")->valueInt;
            removeActorImage(dict);
        }
        else if (textType == kBackgroundShow)
        {
            // 背景切替
            string imgFilePath = Json_getItem(item, "imgPath")->valueString;
            changeBackgroundAnimation(imgFilePath);
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

void NovelScene::changeBackgroundAnimation(const string& imgFilePath)
{
    auto pBackground = dynamic_cast<Sprite*>(this->getChildByTag(kTag_Background));
    
    FadeOut* fadeOut = FadeOut::create(0.5);
    FadeIn* fadeIn = FadeIn::create(0.5);
    auto pActionSeq = Sequence::create(fadeOut, CallFunc::create([this, pBackground, imgFilePath]() {
        Texture2D *texture = Director::getInstance()->getTextureCache()->addImage(imgFilePath);
        pBackground->setTexture(texture);
    }), fadeIn, NULL);
    
    pBackground->runAction(pActionSeq);
}

void NovelScene::makeSelectSpriteButton(const string& str1, int next1Id, const string& str2, int next2Id)
{
    Size winSize = Director::getInstance()->getWinSize();
    
    Menu* pMenu = (Menu*) this->getChildByTag(kTag_MenuSelect);
    if (pMenu)
    {
        pMenu->setVisible(true);
        MenuItemSelectLabelSprite* menuSprite1 = (MenuItemSelectLabelSprite*) pMenu->getChildByTag(kTag_MenuSelect1);
        menuSprite1->setNovelText(str1.c_str());
        
        MenuItemSelectLabelSprite* menuSprite2 = (MenuItemSelectLabelSprite*) pMenu->getChildByTag(kTag_MenuSelect2);
        menuSprite2->setNovelText(str2.c_str());
    }
    else
    {
        // 選択肢1
        const int FONT_SIZE = 14;
        MenuItemSelectLabelSprite* menuSprite1 = MenuItemSelectLabelSprite::createWithLabelSprite("menu_button.png", str1.c_str(), GAME_FONT(FONT_SIZE), FONT_SIZE, Color3B::RED, Color3B::BLUE, Color3B::RED, [this](Object *pSender) {
            this->menuSelectCallback(pSender);
        });
        menuSprite1->setPosition(Point(winSize.width * 0.5, winSize.height * 0.55));
        menuSprite1->setTag(kTag_MenuSelect1);
        menuSprite1->setZOrder(kZOrder_MenuSelect);
        // 選択肢2
        MenuItemSelectLabelSprite* menuSprite2 = MenuItemSelectLabelSprite::createWithLabelSprite("menu_button.png", str2.c_str(), GAME_FONT(FONT_SIZE), FONT_SIZE, Color3B::BLACK, Color3B::BLUE, Color3B::RED, [this](Object *pSender) {
            this->menuSelectCallback(pSender);
        });
        menuSprite2->setPosition(Point(winSize.width * 0.5, winSize.height * 0.45));
        menuSprite2->setTag(kTag_MenuSelect2);
        menuSprite2->setZOrder(kZOrder_MenuSelect);
        
        //メニュー作成
        pMenu = Menu::create(menuSprite1, menuSprite2, NULL);
        pMenu->setPosition(Point::ZERO);
        this->addChild(pMenu, kZOrder_MenuSelect, kTag_MenuSelect);
    }
}


void NovelScene::menuSelectCallback(cocos2d::Object *pSender)
{
    if (isShowTextLog)
    {
        return;
    }
    
//    AudioUtil::playBtnSE();
    
    this->getChildByTag(kTag_MenuSelect)->setVisible(false);
    isMenuSelect = false;

    MenuItemSelectLabelSprite* menuItem = (MenuItemSelectLabelSprite*) pSender;
    dispText(menuItem->m_labelText);
    
    m_textIndex++;
//    if (menuItem->m_nextId > 0)
//    {
//        m_textIndex = menuItem->m_nextId - 1;
//        CCLOG("index set = %d", m_textIndex);
//    }
}


void NovelScene::makeActorImage(const char* imageFilePath, int dict)
{
    int dictTag = dict + kTag_ActorDict;
    
    // Sprite生成
    Sprite* actor = Sprite::create(imageFilePath);
    actor->setScale(0.75);
    
    // 生成前に念のためremoveしておく
    removeActorImage(dict);
    
    Size winSize = Director::getInstance()->getWinSize();
    Point point = Point::ZERO;
    if (dictTag == kTag_ActorDictLeft)
    {
        point = Point(actor->boundingBox().size.width * 0.5, actor->boundingBox().size.height * 0.5);
        actor->setFlippedX(true); // 右向きにする
        
        // centerとrightを暗くする
        shadeActorImage(kTag_ActorDictCenter-kTag_ActorDict);
        shadeActorImage(kTag_ActorDictRight-kTag_ActorDict);
    }
    else if (dictTag == kTag_ActorDictCenter)
    {
        // TODO: センターあとで
        
        // leftとrightを暗くする
        shadeActorImage(kTag_ActorDictLeft-kTag_ActorDict);
        shadeActorImage(kTag_ActorDictRight-kTag_ActorDict);
    }
    else if (dictTag == kTag_ActorDictRight)
    {
        point = Point(winSize.width - actor->boundingBox().size.width * 0.5, actor->boundingBox().size.height * 0.5);
        
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
        actor->removeFromParent();
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

void NovelScene::logMenuSelectCallback(cocos2d::Object *pSender)
{
//    AudioUtil::playBtnSE();
    
    if (isShowTextLog)
    {
        hideTextLog();
    }
    else
    {
        showTextLog(m_textIndex);
    }
}

void NovelScene::showTextLog(int showTextIndex)
{
//    if (showTextIndex <= 0)
//    {
//        return;
//    }
//    isShowTextLog = true;
//    
//    std::vector<std::string> textArray;
//    
//    for (int i = 0; i < showTextIndex; i++)
//    {
//        Json* item = m_novelJson->next;
////        Json* item = Json_getItemAt(m_novelJson, i);
//        if (Json_getItem(item, "text"))
//        {
//            textArray.push_back(Json_getItem(item, "text")->valueString);
//        }
//    }
//    
//    TableViewTestLayer* logLayer = (TableViewTestLayer*) this->getChildByTag(kTag_TextLogLayer);
//    if (logLayer)
//    {        
//        logLayer->makeItemList(textArray);
//        logLayer->setVisible(true);
//    }
//    else
//    {
//        logLayer = TableViewTestLayer::createWithTextArray(textArray);
//        logLayer->setPosition(PointointZero);
//        this->addChild(logLayer, kZOrder_TextLogLayer, kTag_TextLogLayer);
//    }
}

void NovelScene::hideTextLog()
{
    TableViewTestLayer* logLayer = (TableViewTestLayer*) this->getChildByTag(kTag_TextLogLayer);
    if (logLayer)
    {
        logLayer->setVisible(false);
    }
    
    isShowTextLog = false;
}

