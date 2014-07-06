/**

@file  GameLogLayout

@brief description

Cocos2dRogueLike

@author kyokomi
@date 2014/07/06

*/

#include "GameLogLayout.h"

#include "AppMacros.h"

#include "CommonWindowUtil.h"

USING_NS_CC;

std::size_t f_r(const std::string& s, char c)
{
    std::string::size_type pos = s.find(c);
    return (pos == std::string::npos) ? 0 : (1 + f_r(s.substr(pos+1), c));
}

GameLogLayout::GameLogLayout()
: _baseLayer()
, _logTextLabel()
{
    
}

GameLogLayout::~GameLogLayout()
{
    
}

bool GameLogLayout::init()
{
    if (!Node::init()) {
        return false;
    }
    
    auto win_size = Director::getInstance()->getWinSize();
    
    auto pGameLogLayer = LayerColor::create(Color4B(0, 0, 0, 192));
    pGameLogLayer->setContentSize(Size(win_size.width * 0.5, win_size.height * 0.2));
    pGameLogLayer->setPosition(win_size.width / 2 - pGameLogLayer->getContentSize().width / 2, 0);
    CommonWindowUtil::attachWindowWaku(pGameLogLayer);
    _baseLayer = pGameLogLayer;
    {
        auto pLogTextLabel = Label::createWithTTF(FontUtils::getDefaultFontTTFConfig(), "");
        pLogTextLabel->setDimensions(0, 0);
        pLogTextLabel->setAlignment(TextHAlignment::LEFT, TextVAlignment::TOP);
        pLogTextLabel->setPosition(Point(pLogTextLabel->getContentSize().width / 2 + pLogTextLabel->getSystemFontSize() / 4, pGameLogLayer->getContentSize().height - pLogTextLabel->getContentSize().height / 2 - pLogTextLabel->getSystemFontSize() / 4));
        _logTextLabel = pLogTextLabel;
        pGameLogLayer->addChild(pLogTextLabel);
    }
    this->addChild(_baseLayer);
    
    return true;
}

void GameLogLayout::logMessage(const char * pszFormat, ...) {
    va_list ap;
    va_start(ap, pszFormat);
    char buf[GameLogLayout::MAX_LOG_LENGTH];
    vsnprintf(buf, GameLogLayout::MAX_LOG_LENGTH-3, pszFormat, ap);
    va_end(ap);
    
    CCLOG("logMessage: %s", buf);
    
    // とりあえず子要素がないなら無理
    if (!_baseLayer || _baseLayer->getChildrenCount() <= 0) {
        return;
    }
    
    if (_logTextLabel) {
        
        // TODO: 別クラスにしてログをlistで保持する。デフォルトの表示は1件だけで、center寄せ表示でいいかと
        auto pMessage = String::create(buf);
        
        pMessage->append("\n");
        
        std::string nowString = _logTextLabel->getString();
        
        long count = f_r(nowString, '\n');
        
        // 3行まで表示
        if (count >= 3) {
            long size = nowString.size();
            unsigned long loc = nowString.find_last_of('\n', size);
            CCLOG("logMessage: loc = %ld size = %ld", loc, size);
            if (loc != std::string::npos) {
                nowString.erase(loc, nowString.size() - loc);
            }
        }
        pMessage->append(nowString);
        _logTextLabel->setString(pMessage->getCString());
        _logTextLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
        _logTextLabel->setHorizontalAlignment(cocos2d::TextHAlignment::LEFT);
        _logTextLabel->setPosition(Vec2(_logTextLabel->getSystemFontSize() / 4 + _logTextLabel->getContentSize().width / 2,
                                        _baseLayer->getContentSize().height - _logTextLabel->getContentSize().height / 2 - _logTextLabel->getSystemFontSize() / 4));
    }
}
