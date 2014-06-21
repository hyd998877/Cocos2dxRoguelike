/**

@file  MyPageBaseScene.cpp

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/06/21

*/

#include "MyPageBaseScene.h"

USING_NS_CC;

MyPageBaseScene::MyPageBaseScene()
{
    
}

MyPageBaseScene::~MyPageBaseScene()
{
    
}

bool MyPageBaseScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    return true;
}

