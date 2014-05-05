//
//  AppMacros.h
//  nyandash
//
//  Created by kyokomi on 2013/09/27.
//
//

#ifndef nyandash_AppMacros_h
#define nyandash_AppMacros_h

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

#define NS_ROGUE_BEGIN                     namespace RogueLikeGame {
#define NS_ROGUE_END                       }

//#define GAME_SCALE (1)
#define GAME_SCALE (0.52f)

typedef struct tagResource
{
    cocos2d::Size size;
    char directory[100];
} Resource;

static Resource smallResource = { cocos2d::Size(512, 384), "resources-small" };
static Resource largeResource = { cocos2d::Size(1024, 768), "resources-large" };
static Resource xlargeResource = { cocos2d::Size(2048, 1536), "resources-xlarge" };

static cocos2d::Size designResolutionSize = cocos2d::Size(1024, 577);

namespace FontUtils {
    const std::string MisakiFont       = "fonts/misaki_gothic.ttf";
    const std::string PixelMpkus10Font = "fonts/PixelMplus10-Regular.ttf";
    const std::string PixelMpkus12Font = "fonts/PixelMplus12-Regular.ttf";
    
    // 最適なフォント取得
    const static int GAME_FONT_SIZE(int font_size) {
        return (int)((float)font_size * GAME_SCALE);
    }
    const static std::string GAME_FONT(int fontSize)
    {
        fontSize = GAME_FONT_SIZE(fontSize);
        if (fontSize % 12 == 0) {
            return PixelMpkus12Font;
        } else if (fontSize % 10 == 0) {
            return PixelMpkus10Font;
        } else if (fontSize % 8 == 0) {
            return MisakiFont;
        } else {
            CCLOG("font_size = %d", fontSize);
            return "";
        }
    }
    
    const static cocos2d::TTFConfig getDefaultFontTTFConfig(const int& fontSize = 20)
    {
        return cocos2d::TTFConfig(FontUtils::GAME_FONT(fontSize).c_str(), FontUtils::GAME_FONT_SIZE(fontSize));
    }
    const static cocos2d::TTFConfig getTitleFontTTFConfig()
    {
        const int fontSize = 32;
        return getDefaultFontTTFConfig(fontSize);
    }
    const static cocos2d::TTFConfig getStrongFontTTFConfig()
    {
        const int fontSize = 47;
        return getDefaultFontTTFConfig(fontSize);
    }
}

class StringUtils
{
public:
    static std::vector<std::string> split(std::string&& s)
    {
        std::vector<std::string> res;
        size_t current = 0, found;
        while((found = s.find_first_of(',', current)) != std::string::npos){
            res.push_back(std::string(s, current, found - current));
            current = found + 1;
        }
        res.push_back(std::string(s, current, s.size() - current));
        return res;
    }
};

#define EVENT_COME_TO_FOREGROUND_ROGUE "EVENT_COME_TO_FOREGROUND_ROGUE"

static int GetRandom(int min,int max)
{
	return min + (int)(rand()*(max-min+1.0)/(1.0+RAND_MAX));
}

// 共通ウィンドウ枠
//#define CREATE_WINDOW_WAKU() extension::Scale9Sprite::create("ui/dot_waku.png", Rect(0, 0, 16, 16), Rect(3, 3, 10, 10));

// マルチレゾリューション対応
#define HOGE() Director::getInstance()->setOpenGLView(eglView); \
Size frameSize = eglView->getFrameSize(); \
std::vector<std::string> searchPath; \
if (frameSize.height > largeResource.size.height) { searchPath.push_back(xlargeResource.directory);Director::getInstance()->setContentScaleFactor(MIN(xlargeResource.size.height / designResolutionSize.height, xlargeResource.size.width / designResolutionSize.width )); \
} else if (frameSize.height > smallResource.size.height) { searchPath.push_back(largeResource.directory); Director::getInstance()->setContentScaleFactor(MIN(largeResource.size.height / designResolutionSize.height, largeResource.size.width / designResolutionSize.width )); \
} else { searchPath.push_back(smallResource.directory); Director::getInstance()->setContentScaleFactor(MIN(smallResource.size.height / designResolutionSize.height, smallResource.size.width / designResolutionSize.width )); } \
FileUtils* fileUtils = FileUtils::getInstance(); \
fileUtils->setSearchPaths(searchPath);

#endif
