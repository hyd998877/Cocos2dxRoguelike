//
//  MypageScene.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/02/23.
//
//

#include "MypageScene.h"
#include "AppMacros.h"

#include "CommonWindowUtil.h"
#include "NovelScene.h"
#include "RogueScene.h"
#include "BattleScene.h"
#include "SRPGScene.h"

#include "AccountData.h"

NS_ROGUE_BEGIN

MypageScene::MypageScene()
{
}

MypageScene::~MypageScene()
{
}

Scene* MypageScene::scene()
{
    Scene *scene = Scene::create();
    MypageScene *layer = MypageScene::create();
    scene->addChild(layer);
    return scene;
}

bool MypageScene::init()
{
    if ( !Layer::init() )
    {
        return false;
    }

    // セーブあり
    if (AccountData::getInstance()->isQuestSaveData()) {
        initQuestSave();
    } else {
        initMyPage();
    }

    return true;
}

void MypageScene::initQuestSave()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    std::string saveTitle = "前回プレイしたデータが残っています。\n再開しますか？";
    std::string saveDetail = AccountData::getInstance()->createQuestSaveDetailText();
    std::string dialogTitle = saveTitle + "\n\n" + saveDetail + "\n\n※いいえを選択すると、\n所持していたアイテムは消えます";
    
    auto dialogLayer = AlertDialogLayer::createWithContentSizeModal(win_size*0.6, dialogTitle, "はい", "いいえ", [](Ref *ref) {
        auto scene = RogueScene::scene(AccountData::getInstance()->rogue_play_data_.quest_id);
        auto trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    }, [this](Ref *ref) {
        AccountData::getInstance()->resetRoguePlayData();
        this->initMyPage();
    });
    this->addChild(dialogLayer);
}

void MypageScene::initMyPage()
{
    Size win_size = Director::getInstance()->getWinSize();
    
    // キャラ表示
    auto actor_sprite = Sprite::create("novel/actor4_novel_s_0.png");
    actor_sprite->setPosition(Point(win_size.width * 0.25, actor_sprite->getContentSize().height * 0.25));
    actor_sprite->setFlippedX(true);
    this->addChild(actor_sprite);
    
    // タイトル表示
    auto title_layer = Layer::create();
    title_layer->setContentSize(Size(win_size.width, win_size.height / 8));
    title_layer->setPosition(Point(0, win_size.height - title_layer->getContentSize().height));
    this->addChild(title_layer);
    
    auto title_label = Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), "すてーたす画面");
    title_label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    title_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    title_label->setPosition(Point(title_layer->getContentSize().width /2, title_layer->getContentSize().height / 2));
    title_layer->addChild(title_label);
    
    // 枠
    auto window_waku = CommonWindowUtil::createWindowWaku(Size(win_size.width, title_label->getContentSize().height + title_label->getSystemFontSize()));
    title_layer->addChild(window_waku);
    
    // セリフ表示
    auto comment_layer = Layer::create();
    comment_layer->setContentSize(Size(win_size.width / 2, win_size.height / 3));
    comment_layer->setPosition(Point(win_size.width * 0.7 - comment_layer->getContentSize().width / 2, win_size.height * 0.5 - comment_layer->getContentSize().height / 2));
    comment_layer->setTag(10); // TODO: とりあえず
    this->addChild(comment_layer);
    
    auto comment_label = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "ここにはお知らせとか表示する予定よ！\nまだ開発中です。");
    comment_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    comment_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    comment_label->setTag(100); // TODO: とりあえず
    comment_layer->addChild(comment_label);
    comment_label->setPosition(Point(comment_layer->getContentSize().width / 2, comment_layer->getContentSize().height / 2));
    
    // 枠
    CommonWindowUtil::attachWindowWaku(comment_layer);
    
    // グロナビ生成
    initGlobalMenu();
}

/**
 * グロナビ生成
 */
void MypageScene::initGlobalMenu()
{
    // メニュー
    const Size WAKU_PADDING = Size(8, 8);
    
    auto item_menu1 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "すてーたす"), WAKU_PADDING, [this](Ref *ref) {
        auto comment_label = dynamic_cast<Label*>(this->getChildByTag(10)->getChildByTag(100));
        
        comment_label->setString("この画面は、ユーザー自身のステータスを\n表示する予定です。");
    });
    item_menu1->setColor(Color3B::GREEN);
    
    auto item_menu2 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "くえ　すと"), WAKU_PADDING, [this](Ref *ref) {
        
        CCLOG("tappedMenuItem3");
        
        Scene* scene = NovelScene::scene(2, 0, [this]() {
            CCLOG("novel2 end");
            int play_quest_id = 1;
            auto scene = RogueScene::scene(play_quest_id);
            auto trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu3 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "そ　う　こ"), WAKU_PADDING, [this](Ref *ref) {
        // アイテムリストを設定
        Size winSize = Director::getInstance()->getWinSize();
        auto itemWindowLayer = ItemWindow::createItemWindowLayer(winSize * 0.8, AccountData::getInstance()->item_list_);
        itemWindowLayer->setPosition(CommonWindowUtil::createPointCenter(itemWindowLayer, this));
        this->addChild(itemWindowLayer, 99999);
    });
    
    auto item_menu4 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "の　べ　る"), WAKU_PADDING, [this](Ref *ref) {
        
        Scene* scene = NovelScene::scene(1, 0, [this]() {
            CCLOG("novel end");
            Scene* scene = MypageScene::scene();
            TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu5 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "ば　と　る"), WAKU_PADDING, [this](Ref *ref) {
        
        Scene* scene = BattleScene::scene();
        TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu6 = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "て　す　と"), WAKU_PADDING, [this](Ref *ref) {
        
        Scene* scene = SRPGScene::scene();
        TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto menu = Menu::create(item_menu1, item_menu2, item_menu3, item_menu4, item_menu5, item_menu6, NULL);
    // 4間隔で横に並べる
    menu->alignItemsHorizontallyWithPadding(4);
    menu->setPosition(Point(menu->getContentSize().width / 2, item_menu1->getContentSize().height / 2 + WAKU_PADDING.height / 2));
    this->addChild(menu, 1, 1);
    
    auto black_layer = LayerColor::create(Color4B::BLACK);
    black_layer->setOpacity(192);
    black_layer->setContentSize(Size(menu->getContentSize().width, item_menu1->getContentSize().height + WAKU_PADDING.height));
    this->addChild(black_layer);
}

NS_ROGUE_END
