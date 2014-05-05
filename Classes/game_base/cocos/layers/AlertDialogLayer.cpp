//
//  AlertDialogLayer.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//
#include "AppMacros.h"

#include "AlertDialogLayer.h"

USING_NS_CC;

AlertDialogLayer::AlertDialogLayer()
:m_okMenuItemCallback(nullptr),
m_ngMenuItemCallback(nullptr)
{
    
};

AlertDialogLayer::~AlertDialogLayer()
{
    m_okMenuItemCallback = nullptr;
    m_ngMenuItemCallback = nullptr;
}

ModalLayer* AlertDialogLayer::createWithContentSizeModal(std::string titleText, std::string okText, const cocos2d::ccMenuCallback& okCallback, std::string ngText, const cocos2d::ccMenuCallback& ngCallback)
{
    auto winSize = Director::getInstance()->getWinSize();
    return createWithContentSizeModal(winSize*0.6, titleText, okText, okCallback, ngText, ngCallback);
}

// モーダル版のアラートダイアログ生成
ModalLayer* AlertDialogLayer::createWithContentSizeModal(Size contentSize, std::string titleText, std::string okText, const cocos2d::ccMenuCallback& okCallback, std::string ngText, const cocos2d::ccMenuCallback& ngCallback) {
    
    // モーダルレイヤー生成
    auto modalLayer = ModalLayer::create();
    // アラートダイアログ生成
    auto alertDialogLayer = createWithContentSize(contentSize, titleText, okText, ngText);
    // モーダルの中心に配置
    alertDialogLayer->setPosition(CommonWindowUtil::createPointCenter(alertDialogLayer->getContentSize(), modalLayer->getContentSize()));
    // OKボタン押した時の処理と閉じる
    alertDialogLayer->setOkMenuItemCallback([modalLayer, okCallback](Ref *ref) {
        okCallback(ref);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    // NGボタン押した時の処理と閉じる
    alertDialogLayer->setNgMenuItemCallback([modalLayer, ngCallback](Ref *ref) {
        ngCallback(ref);
        modalLayer->setVisible(false);
        modalLayer->removeAllChildrenWithCleanup(true);
    });
    // モーダルに乗せて返す
    alertDialogLayer->setScale(0.1f);
    alertDialogLayer->runAction(ScaleTo::create(0.2f, 1.0f));
    modalLayer->addChild(alertDialogLayer);
    return modalLayer;
}

AlertDialogLayer* AlertDialogLayer::createWithContentSize(Size contentSize, std::string titleText, std::string okText, std::string ngText) {
    AlertDialogLayer *pRet = new AlertDialogLayer();
    if (pRet && pRet->initWithContentSize(contentSize, titleText, okText, ngText)) {
        pRet->autorelease();
        return pRet;
    } else {
        CC_SAFE_DELETE(pRet);
        return NULL;
    }
}

bool AlertDialogLayer::initWithContentSize(Size contentSize, std::string titleText, std::string okText, std::string ngText) {
    if (!LayerColor::initWithColor(Color4B::BLACK)) {
        return false;
    }
    setContentSize(contentSize);
    CommonWindowUtil::attachWindowWaku(this);
    
    // タイトル内容
    auto pTitle = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), titleText);
    pTitle->setPosition(Point(this->getContentSize().width * 0.5, this->getContentSize().height * 0.75 - pTitle->getContentSize().height/4));
    this->addChild(pTitle);
    
    // TODO: 本文分けた方がいいのかな？
    
    // --------------
    // ボタン生成
    const Size WAKU_PADDING = Size(12, 4);
    
    cocos2d::Vector<MenuItem*> menuItemArray;
    // OKボタン
    if (!okText.empty()) {
        auto pOkMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), okText), WAKU_PADDING, [this](Ref *pSender) {
            if (m_okMenuItemCallback) {
                this->m_okMenuItemCallback(pSender);
            }
        });
        menuItemArray.pushBack(pOkMenuItemLabel);
    }
    // NGボタン
    if (!ngText.empty()) {
        auto pNgMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), ngText), WAKU_PADDING, [this](Ref *pSender) {
            if (m_ngMenuItemCallback) {
                this->m_ngMenuItemCallback(pSender);
            }
        });
        menuItemArray.pushBack(pNgMenuItemLabel);
    }
    
    auto pCommonWindowMenu = Menu::createWithArray(menuItemArray);
    pCommonWindowMenu->setPosition(Point(this->getContentSize().width/2, this->getContentSize().height/5));
    pCommonWindowMenu->alignItemsHorizontallyWithPadding(20);
    this->addChild(pCommonWindowMenu);
    
    return true;
}
