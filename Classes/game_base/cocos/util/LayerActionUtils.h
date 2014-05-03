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
    // 即実行版(FadeOut)
    static void runCutInActionFadeOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        node->runAction(createCutInActionFadeOut(node, duration, callback));
    }
    // 即実行版(FadeIn & FadeOut)
    static void runCutInActionFadeInOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        node->runAction(createCutInActionFadeInOut(node, duration, callback));
    }
    
    // nodeに対してTouchEvent登録し、FadeOutして自身をremoveしてcallbackを呼び出す
    static cocos2d::ActionInterval* createCutInActionFadeOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        return cocos2d::Sequence::create(cocos2d::CallFunc::create([node, duration, callback]() {
            auto listener = static_cast<cocos2d::EventListenerTouchOneByOne*>(EventListenerTouchOneByOne::create());
            listener->setSwallowTouches(true);
            listener->onTouchBegan = [node, duration, callback](cocos2d::Touch* touch, cocos2d::Event* event) -> bool {
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
    
    // 最初に自身を透明にしてからFadeIn して FadeOut
    static cocos2d::ActionInterval* createCutInActionFadeInOut(cocos2d::Node * node, const float duration, const std::function<void(void)> callback) {
        return cocos2d::Sequence::create(cocos2d::CallFunc::create([node] {
            node->setOpacity(0);
        }), FadeIn::create(duration), createCutInActionFadeOut(node, duration, callback), NULL);
    }
};

#endif /* defined(__kyokomi__LayerActionUtils__) */
