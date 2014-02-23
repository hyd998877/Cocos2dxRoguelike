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
    Size win_size = Director::getInstance()->getWinSize();

    // 背景表示
    // TODO: とりあえずなし
    
    // キャラ表示
    auto actor_sprite = Sprite::create("novel/actor4_novel_s_0.png");
    actor_sprite->setPosition(Point(win_size.width * 0.25, actor_sprite->getContentSize().height * 0.25));
    actor_sprite->setFlippedX(true);
    this->addChild(actor_sprite);
    
    // タイトル表示
    auto title_label = LabelTTF::create("すてーたす画面", GAME_FONT(16), 16);
    title_label->setHorizontalAlignment(cocos2d::TextHAlignment::CENTER);
    title_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    this->addChild(title_label);
    // 枠
    auto window_waku = CommonWindowUtil::createWindowWaku(Size(win_size.width, title_label->getContentSize().height + title_label->getFontSize()));
    window_waku->setPosition(title_label->getContentSize().width / 2, title_label->getContentSize().height / 2);
    title_label->addChild(window_waku);
    title_label->setPosition(Point(win_size.width / 2, win_size.height - window_waku->getContentSize().height / 2));

    // セリフ表示
    auto comment_label = LabelTTF::create("ここにはお知らせとか表示する予定よ！\nまだ開発中です。", GAME_FONT(10), 10);
    comment_label->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
    comment_label->setVerticalAlignment(cocos2d::TextVAlignment::CENTER);
    this->addChild(comment_label);
    comment_label->setTag(100); // TODO: とりあえず
    
    // 枠
    auto comment_window_waku = CommonWindowUtil::createWindowWaku(Size(win_size.width / 2, win_size.height / 3));
    comment_window_waku->setPosition(comment_label->getContentSize().width / 2, comment_label->getContentSize().height / 2);
    comment_label->addChild(comment_window_waku);
    comment_label->setPosition(Point(win_size.width * 0.7, win_size.height * 0.5));
    
    // グロナビ生成
    initGlobalMenu();

    return true;
}


/**
 * グロナビ生成
 */
void MypageScene::initGlobalMenu()
{
    // メニュー
    const Size WAKU_PADDING = Size(8, 8);
    
    auto item_menu1 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("すてーたす", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        auto comment_label = dynamic_cast<LabelTTF*>(this->getChildByTag(100));
        comment_label->setString("この画面は、ユーザー自身のステータスを\n表示する予定です。");
        comment_label->getChildren().at(0)->setPosition(comment_label->getContentSize().width / 2, comment_label->getContentSize().height / 2);
    });
    item_menu1->setColor(Color3B::GREEN);
    
    auto item_menu2 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("くえ　すと", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        
        CCLOG("tappedMenuItem3");
        Scene* scene = NovelScene::scene(2, 0, [this]() {
            CCLOG("novel2 end");
            auto scene = RogueScene::scene(1);
            auto trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu3 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("そ　う　こ", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        auto comment_label = dynamic_cast<LabelTTF*>(this->getChildByTag(100));
        comment_label->setString("まだ未実装です！");
        comment_label->getChildren().at(0)->setPosition(comment_label->getContentSize().width / 2, comment_label->getContentSize().height / 2);
    });
    
    auto item_menu4 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("の　べ　る", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        
        Scene* scene = NovelScene::scene(1, 0, [this]() {
            CCLOG("novel end");
            Scene* scene = MypageScene::scene();
            TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
            Director::getInstance()->replaceScene(trans);
        });
        TransitionProgressOutIn* trans = TransitionProgressOutIn::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu5 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("ば　と　る", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        
        Scene* scene = BattleScene::scene();
        TransitionProgressInOut* trans = TransitionProgressInOut::create(1, scene);
        Director::getInstance()->replaceScene(trans);
    });
    
    auto item_menu6 = CommonWindowUtil::createMenuItemLabelWaku(LabelTTF::create("て　す　と", GAME_FONT(10), 10), WAKU_PADDING, [this](Object* pSeneder) {
        
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

