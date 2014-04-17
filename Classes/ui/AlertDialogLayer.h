//
//  AlertDialogLayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/01/26.
//
//

#ifndef __Cocos2dRogueLike__AlertDialogLayer__
#define __Cocos2dRogueLike__AlertDialogLayer__

#include "CommonWindowUtil.h"
#include "ModalLayer.h"

class AlertDialogLayer : public cocos2d::LayerColor
{
private:
    cocos2d::ccMenuCallback m_okMenuItemCallback;
    cocos2d::ccMenuCallback m_ngMenuItemCallback;
protected:
    
public:
    AlertDialogLayer()
    :m_okMenuItemCallback(nullptr),
    m_ngMenuItemCallback(nullptr)
    {
        
    };
    virtual ~AlertDialogLayer()
    {
        m_okMenuItemCallback = nullptr;
        m_ngMenuItemCallback = nullptr;
    }
    
    bool initWithContentSize(cocos2d::Size contentSize, std::string titleText, std::string okText, std::string ngText);
    
    static ModalLayer* createWithContentSizeModal(cocos2d::Size contentSize, std::string titleText, std::string okText, std::string ngText, const cocos2d::ccMenuCallback& okCallback, const cocos2d::ccMenuCallback& ngCallback);
    static AlertDialogLayer* createWithContentSize(cocos2d::Size contentSize, std::string titleText, std::string okText, std::string ngText);
    
    void setOkMenuItemCallback(const cocos2d::ccMenuCallback& callback) {
        m_okMenuItemCallback = callback;
    }
    
    void setNgMenuItemCallback(const cocos2d::ccMenuCallback& callback) {
        m_ngMenuItemCallback = callback;
    }
};

#endif /* defined(__Cocos2dRogueLike__AlertDialogLayer__) */
