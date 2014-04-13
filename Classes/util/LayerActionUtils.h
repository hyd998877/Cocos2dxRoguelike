//
//  LayerActionUtils.h
//
//  Created by kyokomi on 2014/04/13.
//
//

#ifndef __kyokomi__LayerActionUtils__
#define __kyokomi__LayerActionUtils__

#include "cocos2d.h"

class LayerActionUtils
{
public:
    static cocos2d::ActionInterval* createCutInActionFadeOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        return cocos2d::Sequence::create(cocos2d::CallFunc::create([node, duration, callback]() {
            auto listener = static_cast<cocos2d::EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
            listener->setSwallowTouches(true);
            listener->onTouchBegan = [node, duration, callback](cocos2d::Touch* touch, cocos2d::Event* event) -> bool {
                // フェードアウトして自身をremoveする
                node->runAction(cocos2d::Sequence::create(FadeOut::create(duration), cocos2d::CallFunc::create([node, callback]() {
                    node->setVisible(false);
                    node->removeAllChildrenWithCleanup(true);
                    
                    callback();
                }), NULL));
                return false;
            };
            node->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, node);
        }), NULL);
    }
    
    static cocos2d::ActionInterval* createCutInActionFadeInOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        // 一応実行前に透明にする
        return cocos2d::Sequence::create(cocos2d::CallFunc::create([node] {
            node->setOpacity(0);
        }), FadeIn::create(duration), createCutInActionFadeOut(node, duration, callback), NULL);
    }

};

#endif /* defined(__kyokomi__LayerActionUtils__) */
