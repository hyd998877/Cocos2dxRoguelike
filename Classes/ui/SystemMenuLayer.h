//
//  SystemMenuLayer.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/04/19.
//
//

#ifndef __Cocos2dRogueLike__SystemMenuLayer__
#define __Cocos2dRogueLike__SystemMenuLayer__

#include "cocos2d.h"

class SystemMenuLayer : public cocos2d::LayerColor
{
    
public:
    typedef std::function<void(void)> SystemMenuButtonCallback;
    struct SystemMenuButtonInfo {
        std::string buttonText;
        SystemMenuButtonCallback systemMenuButtonCallback;
    };
    SystemMenuLayer();
    ~SystemMenuLayer();
    
    bool init(const cocos2d::Size& contentSize, const std::string& titleText, const std::list<SystemMenuButtonInfo> menuButtonInfoList, const cocos2d::Color3B& color, const GLubyte& opacity);
    static SystemMenuLayer* create(const cocos2d::Size& contentSize, const std::string& titleText, const std::list<SystemMenuButtonInfo> menuButtonInfoList, const cocos2d::Color3B& color = cocos2d::Color3B::BLACK, const GLubyte& opacity = 192);
private:
    void initCreateTitleLabel(const std::string& titleText);
    void initCreateMenuList(const std::list<SystemMenuButtonInfo> menuButtonInfoList);
};


#endif /* defined(__Cocos2dRogueLike__SystemMenuLayer__) */
