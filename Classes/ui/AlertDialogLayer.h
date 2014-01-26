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

class AlertDialogLayer : public cocos2d::LayerColor
{
private:
    cocos2d::ccMenuCallback m_okMenuItemCallback;
    cocos2d::ccMenuCallback m_ngMenuItemCallback;
protected:
    enum Tag {
        TitleTextTag          = 10000,
        MenuTag               = 20000,
        OK_MenuItemLabelTag   = 21000,
        NG_MenuItemLabelTag   = 22000,
    };
    
public:
    AlertDialogLayer()
    :m_okMenuItemCallback(nullptr),
    m_ngMenuItemCallback(nullptr)
    {
        
    };
    ~AlertDialogLayer();
    
    virtual bool initWithContentSize(cocos2d::Size contentSize, std::string titleText, std::string okText, std::string ngText);
    static AlertDialogLayer* createWithContentSize(cocos2d::Size contentSize, std::string titleText, std::string okText, std::string ngText);
    
    void setOkMenuItemCallback(const cocos2d::ccMenuCallback& callback)
    {
        m_okMenuItemCallback = callback;
    }
    
    void setNgMenuItemCallback(const cocos2d::ccMenuCallback& callback)
    {
        m_ngMenuItemCallback = callback;
    }
    
    void setTitleText(const std::string& text);
    
    void setOkText(const std::string& text);
    void setNgText(const std::string& text);
};

#endif /* defined(__Cocos2dRogueLike__AlertDialogLayer__) */
