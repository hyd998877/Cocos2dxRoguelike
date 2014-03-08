//
//  NovelScene.h
//  NovelGame
//
//  Created by kyokomi on 2013/09/01.
//
//

#ifndef __NovelGame__NovelScene__
#define __NovelGame__NovelScene__

#include "cocos2d.h"
#include "spine/Json.h"

USING_NS_CC;
using namespace std;
using namespace extension;

class NovelScene : public cocos2d::Layer
{
    // コールバック
    typedef std::function<void()> NovelTextEndCallback;
private:
    
    // --------------- 変数 -------------------
    
    /** 選択肢表示中フラグ */ // TODO: Modalレイヤーつかいたいね
    bool m_isMenuSelect;
    /** バックログ表示中フラグ */ // TODO: Modalレイヤー使いたいね
    bool m_isShowTextLog;
    
    /** ノベル情報Json */
    Json* m_novelJson;
    /** ノベルテキスト送りインデックス */
    int m_textIndex;
    /** ノベルパート終了時のコールバック */
    NovelTextEndCallback m_novelTextEndCallback;

    // --------------- 関数 -------------------
    
    /** 初期処理 */
    void initNovelJson(int sceneNo, int novelIndex);
    /** ノベルパート終了 */
    void endNovel();
    
    /** 次のテキストを処理 */
    void nextNovelJson();
    
    /** 選択肢ボタンの生成 */
    void makeSelectSpriteButton(const string& str1, int next1Id, const string& str2, int next2Id);
    /** 選択肢を選んだ時のコールバック */
    void menuSelectCallback(Ref *pSender);
    
    /** テキストを表示させる */
    void dispText(string text);
    /** 名前を表示させる */
    void dispName(string name);
    
    void makeActorImage(const char* imageFilePath, int dict);
    void removeActorImage(int dict);
    void shadeActorImage(int dict);
    void resetShadeActorImage(int dict);
    
    void changeBackgroundAnimation(const string& imgFilePath);
//    FiniteTimeAction* changeBackgroundAnimation(const string&  imgFilePath);
//    void changeBackground(Ref *pSender, void* node);
    
    void logMenuSelectCallback(Ref *pSender);
    void showTextLog(int showTextIndex);
    void hideTextLog();
    
protected:
    
    enum kTag
    {
        kTag_Background = 1,
        kTag_TextLayer = 100,
        kTag_TextLayer_textLabel = 101,
        kTag_TextLayer_name = 200,
        kTag_TextLayer_nameTextLabel = 201,
        
        kTag_ActorDict     = 1000,
        kTag_ActorDictLeft,
        kTag_ActorDictCenter,
        kTag_ActorDictRight,
        
        /** 選択肢用 */
        kTag_MenuSelect   = 10000,
        kTag_MenuSelect1,
        kTag_MenuSelect2,
        
        /** バックログ */
        kTag_TextLogLayer = 20000,
        
        /** メニュー */
        kTag_MenuItem_log = 30000,
    };
    
    enum kZOrder
    {
        kZOrder_Background = 1,
        kZOrder_Actor,
        kZOrder_TextLayer,
        kZOrder_MenuSelect,
        kZOrder_TextLogLayer,
        kZOrder_MenuItem,
    };
    
    enum NovelType
    {
        TextType            = 1, // テキスト表示
        SelectItem          = 2, // 選択肢表示
        ActorImageShow      = 3, // 立ち絵画像表示
        ActorImageHide      = 4, // 立ち絵画像非表示
        BackgroundShow      = 5, // 背景表示
        ActorImageShadeShow = 6, // 立ち絵影表示
        ActorImageShadeHide = 7, // 立ち絵影非表示
    };
    
public:
    NovelScene();
    ~NovelScene();
    
    bool init(int sceneNo, int novelIndex, const NovelTextEndCallback& callback);
    static cocos2d::Scene* scene(int sceneNo, int novelIndex, const NovelTextEndCallback& callback);
    static NovelScene* create(int sceneNo, int novelIndex, const NovelTextEndCallback& callback);
    
    virtual bool onTouchBegan(Touch *touch, Event *unused_event);
    //virtual void onTouchMoved(Touch *touch, Event *unused_event);
    virtual void onTouchEnded(Touch *touch, Event *unused_event);
    //virtual void onTouchCancelled(Touch *touch, Event *unused_event);
};

#endif /* defined(__NovelGame__NovelScene__) */
