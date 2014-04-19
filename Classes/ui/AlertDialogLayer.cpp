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

// モーダル版のアラートダイアログ生成
ModalLayer* AlertDialogLayer::createWithContentSizeModal(Size contentSize, std::string titleText, std::string okText, std::string ngText, const cocos2d::ccMenuCallback& okCallback, const cocos2d::ccMenuCallback& ngCallback) {
    
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
    pTitle->setPosition(Point(this->getContentSize().width * 0.5, this->getContentSize().height * 0.75));
    this->addChild(pTitle);
    
    // --------------
    // ボタン生成
    const Size WAKU_PADDING = Size(8, 4);
    
    // OKボタン
    auto pOkMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), okText), WAKU_PADDING, [this](Ref *pSender) {
        if (m_okMenuItemCallback) {
            this->m_okMenuItemCallback(pSender);
        }
    });
    pOkMenuItemLabel->setPosition(Point(this->getContentSize().width * 0.25, pOkMenuItemLabel->getContentSize().height));
    
    // NGボタン
    auto pNgMenuItemLabel = CommonWindowUtil::createMenuItemLabelWaku(Label::createWithTTF(FontUtils::getTitleFontTTFConfig(), ngText), WAKU_PADDING, [this](Ref *pSender) {
        if (m_ngMenuItemCallback) {
            this->m_ngMenuItemCallback(pSender);
        }
    });
    pNgMenuItemLabel->setPosition(Point(this->getContentSize().width * 0.75, pNgMenuItemLabel->getContentSize().height));
    
    auto pCommonWindowMenu = Menu::create(pOkMenuItemLabel, pNgMenuItemLabel, NULL);
    pCommonWindowMenu->setPosition(Point::ZERO);
    this->addChild(pCommonWindowMenu);
    
    return true;
}
