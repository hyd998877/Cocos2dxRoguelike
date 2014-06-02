/**
 @file TopPageLayer.h
 
 @brief トップ画面
 
 お知らせ的な表示とか
 
 @author kyokomi
 @date 2014/05/07
 
 */

#ifndef __Cocos2dRogueLike__TopPageLayer__
#define __Cocos2dRogueLike__TopPageLayer__

#include "cocos2d.h"

/**
 @class TopPageLayer TopPageLayer.h
 
 @brief マイページのトップページ用Layerクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class TopPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(TopPageLayer);
    TopPageLayer();
    virtual ~TopPageLayer();
    
    virtual bool init();
private:
};


#endif /* defined(__Cocos2dRogueLike__TopPageLayer__) */
