#include "TableViewTestLayer.h"

#include "AppMacros.h"

USING_NS_CC;
USING_NS_CC_EXT;

TableViewTestLayer::TableViewTestLayer()
:m_itemList(std::list<TableLayout>()),
m_callback(nullptr),
choice_touch_cell_idx_(0)
{
}

TableViewTestLayer* TableViewTestLayer::createWithTextArray(std::list<TableLayout> itemList, Size contentSize) {
    TableViewTestLayer *pRet = new TableViewTestLayer();
    if (pRet && pRet->init(itemList, contentSize)) {
        pRet->autorelease();
        return pRet;
    } else {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

// on "init" you need to initialize your instance
bool TableViewTestLayer::init(std::list<TableLayout> itemList, Size contentSize) {
    if ( !LayerColor::init() ) {
        return false;
    }
    m_itemList = itemList;

    this->setContentSize(contentSize);
    
	TableView* tableView = TableView::create(this, Size(contentSize.width, contentSize.height * 0.9));
	tableView->setDirection(ScrollView::Direction::VERTICAL);
    // 上がぴったりすぎるので0.02ほど
    tableView->setPosition(Point(tableView->getPositionX(), contentSize.height * 0.1 - contentSize.height * 0.02));
	tableView->setDelegate(this);
//    tableView->setBounceable(false); // スクロールオーバー
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);
    tableView->setTag(TableViewTestLayer::TableViewTag);
	this->addChild(tableView);
	tableView->reloadData();

    this->setColor(Color3B::BLACK);
    this->setOpacity(128);
    
    return true;
}

void TableViewTestLayer::tableCellTouched(TableView* table, TableViewCell* cell) {
    CCLOG("cell touched at index: %ld", cell->getIdx());
    if (m_callback) {
        m_callback(this, cell->getIdx());
    }
    touchCellRefreshColor(table, (int)cell->getIdx());
}

void TableViewTestLayer::touchCellRefreshColor(TableView* pTable, int touchCellIdx) {
    // タッチした位置を覚えておく（スクロール対策）
    choice_touch_cell_idx_ = touchCellIdx;
    
    for (auto pLayer : pTable->getChildren()) {
        for (auto pCell : pLayer->getChildren()) {
            if (static_cast<TableViewCell*>(pCell)->getIdx() == touchCellIdx) {
                pCell->getChildByTag(TableViewTestLayer::ItemLayerTag)->setColor(Color3B::GREEN);
            } else {
                pCell->getChildByTag(TableViewTestLayer::ItemLayerTag)->setColor(Color3B::BLACK);
            }
        }
    }
}

Size TableViewTestLayer::tableCellSizeForIndex(TableView *table, long idx) {
    Size content_size = getContentSize();
    return Size(content_size.width, content_size.height / TableViewTestLayer::LIST_SIZE);
}

TableViewCell* TableViewTestLayer::tableCellAtIndex(TableView *table, long idx) {
    
    // 本文テキスト
    const int kDetailFontSize = 20;
    
    Size content_size = getContentSize();
    TableLayout table_item;
    
    long list_size = m_itemList.size();
    if (list_size > 0 && idx < list_size) {
        auto it = m_itemList.begin();
        std::advance(it, idx);
        TableLayout item = *it;
        table_item = item;
        m_itemList.end();
    }
    CCLOG("idx = %ld size = %ld text = %s", idx, list_size, table_item.labelText.c_str());
    
    auto sprite_frame_cache = SpriteFrameCache::getInstance();
    auto sprite_frame = sprite_frame_cache->getSpriteFrameByName(table_item.imageFileName);
    if (!sprite_frame) {
        sprite_frame = Sprite::create(table_item.imageFileName)->getSpriteFrame();
        SpriteFrameCache::getInstance()->addSpriteFrame(sprite_frame, table_item.imageFileName);
    }
    
    auto cell = table->dequeueCell();
    if (!cell) {
        cell = new TableViewCell();
        cell->autorelease();
        
        LayerColor* text_layer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), content_size.width * 0.9, content_size.height / TableViewTestLayer::LIST_SIZE * 0.9);
        text_layer->setPosition(content_size.width * 0.05, content_size.height / TableViewTestLayer::LIST_SIZE * 0.05);
        text_layer->setTag(TableViewTestLayer::ItemLayerTag);
        
        auto item_image_sprite = Sprite::createWithSpriteFrame(sprite_frame);
        item_image_sprite->setTag(TableViewTestLayer::ItemImageTag);
        item_image_sprite->setPosition(Point(item_image_sprite->getContentSize().width / 2, text_layer->getContentSize().height / 2));
        text_layer->addChild(item_image_sprite);
        
        LabelTTF* text_label = LabelTTF::create(table_item.labelText, GAME_FONT(kDetailFontSize), GAME_FONT_SIZE(kDetailFontSize));
        text_label->setColor(table_item.textColor);
        text_label->setPosition(Point(text_label->getFontSize() + item_image_sprite->getContentSize().width + text_label->getContentSize().width / 2, text_layer->getContentSize().height / 2));
        text_label->setTag(TableViewTestLayer::ItemTextLabelTag);

        text_label->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
        text_label->setHorizontalAlignment(TextHAlignment::LEFT);
        text_layer->addChild(text_label);
        
        cell->addChild(text_layer);
    } else {
        auto text_layer = static_cast<LayerColor*>(cell->getChildByTag(TableViewTestLayer::ItemLayerTag));
        auto item_image_sprite = static_cast<Sprite*>(text_layer->getChildByTag(TableViewTestLayer::ItemImageTag));
        item_image_sprite->setSpriteFrame(sprite_frame);
        item_image_sprite->setPosition(Point(item_image_sprite->getContentSize().width / 2, text_layer->getContentSize().height / 2));
        
        auto text_label = static_cast<LabelTTF*>(text_layer->getChildByTag(TableViewTestLayer::ItemTextLabelTag));
        text_label->setString(table_item.labelText);
        text_label->setPosition(Point(text_label->getFontSize() + item_image_sprite->getContentSize().width + text_label->getContentSize().width / 2, text_layer->getContentSize().height / 2));
        text_label->setColor(table_item.textColor);
        
        // タッチ中を更新
        if (choice_touch_cell_idx_ == idx) {
            text_layer->setColor(Color3B::GREEN);
        } else {
            text_layer->setColor(Color3B::BLACK);
        }
    }

    return cell;
}

long TableViewTestLayer::numberOfCellsInTableView(TableView *table)
{
    return m_itemList.size();
}

#pragma mark
#pragma mark 要素設定とか

void TableViewTestLayer::makeItemList(std::list<TableLayout> itemList) {
    m_itemList = itemList;
    
    auto table_view = static_cast<TableView*>(this->getChildByTag(TableViewTestLayer::TableViewTag));
    
    // タッチなしに初期化
    touchCellRefreshColor(table_view, -1);
    
    table_view->reloadData();
}

#pragma mark
#pragma mark コールバック関連

void TableViewTestLayer::setCallback(const TableCellTouchedCallback &callback) {
    m_callback = callback;
}


