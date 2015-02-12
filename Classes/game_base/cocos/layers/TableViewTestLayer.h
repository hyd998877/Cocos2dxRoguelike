#ifndef __TABLEVIEWTESTSCENE_H__
#define __TABLEVIEWTESTSCENE_H__

#include "cocos2d.h"
#include "extensions/cocos-ext.h"

class TableViewTestLayer :
public cocos2d::LayerColor,
public cocos2d::extension::TableViewDataSource,
public cocos2d::extension::TableViewDelegate {
    const int LIST_SIZE = 6;
protected:
    
    enum Tag {
        ItemLayerTag = 1,
        ItemTextLabelTag,
        ItemImageTag,
        TableViewTag,
    };
    
public:
    typedef struct _TableLayout {
        std::string imageFileName;
        std::string labelText;
        cocos2d::Color3B textColor;
    } TableLayout;
    
    // 一覧タップ時のコールバック
    typedef std::function<void(Ref*, long idx)> TableCellTouchedCallback;
    void setCallback(const TableCellTouchedCallback& callback);
    
    // 初期
    virtual bool init(std::list<TableLayout> itemList, cocos2d::Size contentSize);
    TableViewTestLayer();
    virtual ~TableViewTestLayer() {
        
    }
    static TableViewTestLayer* createWithTextArray(std::list<TableLayout> itemList, cocos2d::Size contentSize);
    
    // デリゲート関連
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, ssize_t idx);
    virtual ssize_t numberOfCellsInTableView(cocos2d::extension::TableView *table);
    
    void touchCellRefreshColor(cocos2d::extension::TableView* pTable, int touchCellIdx);
    
    // TableViewDelegate
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
    // リスト作成
    void makeItemList(std::list<TableLayout> itemList);
    
private:
    
    std::list<TableLayout> m_itemList;
    
    // コールバック
    TableCellTouchedCallback m_callback;
    
    // 現在選択中にセルインデックス
    int choice_touch_cell_idx_;
};

#endif // __TABLEVIEWTESTSCENE_H__
