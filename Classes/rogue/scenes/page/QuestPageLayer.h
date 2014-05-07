/**
 @file QuestPageLayer.h
 
 @brief クエスト選択画面
 
 @author kyokomi
 @date 2014/05/07
 */

#ifndef __Cocos2dRogueLike__QuestPageLayer__
#define __Cocos2dRogueLike__QuestPageLayer__

#include "cocos2d.h"

/**
 @class QuestPageLayer QuestPageLayer.h
 
 @brief クエストページLayerクラス
 
 @author kyokomi
 @date 2014/05/07
 */
class QuestPageLayer : public cocos2d::Layer
{
public:
    CREATE_FUNC(QuestPageLayer);
    QuestPageLayer();
    virtual ~QuestPageLayer();
    
    virtual bool init();
private:
};

#endif /* defined(__Cocos2dRogueLike__QuestPageLayer__) */
