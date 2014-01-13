#include "TableViewTestLayer.h"
#include "CustomTableViewCell.h"

#include "AppMacros.h"

USING_NS_CC;
USING_NS_CC_EXT;

TableViewTestLayer::TableViewTestLayer()
:m_itemList(std::list<TableLayout>()),
m_callback(nullptr)
{

}

TableViewTestLayer* TableViewTestLayer::createWithTextArray(std::list<TableLayout> itemList, Size contentSize)
{
    TableViewTestLayer *pRet = new TableViewTestLayer();
    if (pRet && pRet->init(itemList, contentSize))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = NULL;
        return NULL;
    }
}

// on "init" you need to initialize your instance
bool TableViewTestLayer::init(std::list<TableLayout> itemList, Size contentSize)
{
    if ( !LayerColor::init() )
    {
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

void TableViewTestLayer::tableCellTouched(TableView* table, TableViewCell* cell)
{
    CCLOG("cell touched at index: %ld", cell->getIdx());
    if (m_callback)
    {
        m_callback(this, cell->getIdx());
    }
    
    touchCellRefreshColor(table, cell->getIdx());
}

void TableViewTestLayer::touchCellRefreshColor(TableView* pTable, int touchCellIdx)
{
    for (auto pLayer : pTable->getChildren())
    {
        for (auto pCell : pLayer->getChildren())
        {
            if (static_cast<TableViewCell*>(pCell)->getIdx() == touchCellIdx)
            {
                pCell->getChildByTag(TableViewTestLayer::ItemLayerTag)->setColor(Color3B::GREEN);
            }
            else
            {
                pCell->getChildByTag(TableViewTestLayer::ItemLayerTag)->setColor(Color3B::BLACK);
            }
        }
    }
}

Size TableViewTestLayer::tableCellSizeForIndex(TableView *table, long idx)
{
    Size contentSize = getContentSize();
    return Size(contentSize.width, contentSize.height / TableViewTestLayer::LIST_SIZE);
}

TableViewCell* TableViewTestLayer::tableCellAtIndex(TableView *table, long idx)
{
    Size contentSize = getContentSize();
    TableLayout tableItem;
    
    long listSize = m_itemList.size();
    if (listSize > 0 && idx < listSize)
    {
        auto it = m_itemList.begin();
        std::advance(it, idx);
        TableLayout item = *it;
        tableItem = item;
        m_itemList.end();
    }
    CCLOG("idx = %ld size = %ld text = %s", idx, listSize, tableItem.labelText.c_str());
    
    
    auto pSpriteFrameCache = SpriteFrameCache::getInstance();
    auto pSpriteFrame = pSpriteFrameCache->getSpriteFrameByName(tableItem.imageFileName);
    if (!pSpriteFrame)
    {
        pSpriteFrame = Sprite::create(tableItem.imageFileName)->getSpriteFrame();
        SpriteFrameCache::getInstance()->addSpriteFrame(pSpriteFrame, tableItem.imageFileName);
    }
    
    TableViewCell *cell = table->dequeueCell();
    if (!cell) {
        cell = new CustomTableViewCell();
        cell->autorelease();
        
        LayerColor* pTextLayer = LayerColor::create(Color4B(0, 0, 0, 255 * 0.7), contentSize.width * 0.9, contentSize.height / TableViewTestLayer::LIST_SIZE * 0.9);
        pTextLayer->setPosition(contentSize.width * 0.05, contentSize.height / TableViewTestLayer::LIST_SIZE * 0.05);
        pTextLayer->setTag(TableViewTestLayer::ItemLayerTag);
        
        auto pItemImageSprite = Sprite::createWithSpriteFrame(pSpriteFrame);
        pItemImageSprite->setTag(TableViewTestLayer::ItemImageTag);
        pItemImageSprite->setPosition(Point(pItemImageSprite->getContentSize().width / 2, pTextLayer->getContentSize().height / 2));
        pTextLayer->addChild(pItemImageSprite);
        
        // 本文テキスト
        int baseFontSize = 10;
        
        LabelTTF* textLabel = LabelTTF::create(tableItem.labelText, GAME_FONT(baseFontSize), baseFontSize);
        textLabel->setColor(Color3B::WHITE);
        textLabel->setPosition(Point(baseFontSize + pItemImageSprite->getContentSize().width + textLabel->getContentSize().width / 2, pTextLayer->getContentSize().height / 2));
        textLabel->setTag(TableViewTestLayer::ItemTextLabelTag);

        textLabel->setVerticalAlignment(cocos2d::TextVAlignment::TOP);
        textLabel->setHorizontalAlignment(TextHAlignment::LEFT);
        pTextLayer->addChild(textLabel);
        
        cell->addChild(pTextLayer);
    }
    else
    {
        LayerColor* pTextLayer = static_cast<LayerColor*>(cell->getChildByTag(TableViewTestLayer::ItemLayerTag));
        auto pItemImageSprite = static_cast<Sprite*>(pTextLayer->getChildByTag(TableViewTestLayer::ItemImageTag));
        pItemImageSprite->setSpriteFrame(pSpriteFrame);
        pItemImageSprite->setPosition(Point(pItemImageSprite->getContentSize().width / 2, pTextLayer->getContentSize().height / 2));
        
        LabelTTF* pTextLabel = static_cast<LabelTTF*>(pTextLayer->getChildByTag(TableViewTestLayer::ItemTextLabelTag));
        pTextLabel->setString(tableItem.labelText);
        pTextLabel->setPosition(Point(pTextLabel->getFontSize() + pItemImageSprite->getContentSize().width + pTextLabel->getContentSize().width / 2, pTextLayer->getContentSize().height / 2));
    }

    return cell;
}

long TableViewTestLayer::numberOfCellsInTableView(TableView *table)
{
    return m_itemList.size();
}

#pragma mark
#pragma mark 要素設定とか

void TableViewTestLayer::makeItemList(std::list<TableLayout> itemList)
{
    m_itemList = itemList;
    
    auto pTableView = static_cast<TableView*>(this->getChildByTag(TableViewTestLayer::TableViewTag));
    
    touchCellRefreshColor(pTableView, -1);
    pTableView->reloadData();
}

#pragma mark
#pragma mark コールバック関連

void TableViewTestLayer::setCallback(const TableCellTouchedCallback &callback)
{
    m_callback = callback;
}


