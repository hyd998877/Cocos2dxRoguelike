//
//  MapManager.cpp
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/06.
//
//

#include "MapManager.h"
#include <iostream>
#include <string>
#include <cassert>

#define DEBUG_LOG_MAP_ITEM_LAYER_FLG 1

MapManager::MapManager()
:map_data_()
{
}

MapManager::~MapManager()
{
    map_data_.map_cursor_data_array.clear();
    map_data_.map_drop_item_data_array.clear();
    map_data_.map_object_data_array.clear();
    map_data_.mapping_array.clear();
    map_data_.map_move_cursor_list.clear();
    map_data_.map_move_point_list.clear();
}

void MapManager::init()
{
    
}

void MapManager::initMapping(int top, int bottom, int left, int right)
{
    map_data_.map_data_setting.top = top;
    map_data_.map_data_setting.bottom = bottom;
    map_data_.map_data_setting.left = left;
    map_data_.map_data_setting.right = right;
    
    assert(vaildateInit());
    
    clearCursor();
    clearMapItemArray(&map_data_.map_object_data_array);
    clearMapItemArray(&map_data_.map_drop_item_data_array);

    clearArray(&map_data_.mapping_array, false);
    
    map_data_.map_move_point_list.clear();
}

/**
 * キャラクター移動範囲検索.
 * @param mapIndex 移動するキャラクターの座標
 * @param dist 移動可能な距離
 */
std::list<MapIndex> MapManager::createActorFindDist(const MapIndex& mapIndex, int dist)
{
    assert(vaildateInit());
    
    // カーソル情報を初期化
    clearCursor();
    
    // 検索開始(再帰呼び出し)
    findDist(mapIndex.x, mapIndex.y, dist, true);
    
    // cursorListを作成
    return map_data_.map_move_cursor_list;
}

void MapManager::clearCursor()
{
    assert(vaildateInit());
    
    clearMapItemArray(&map_data_.map_cursor_data_array);
    map_data_.map_move_cursor_list.clear();
}

/**
 * キャラクター移動範囲検索.
 */
std::list<MapIndex> MapManager::createMovePointList(const MapIndex& moveFromMapIndex, const MapItem& moveToMapItem)
{
    assert(vaildateInit());
    
    // 経路情報を初期化
    map_data_.map_move_point_list.clear();
    
    // 移動先を取得
    MapItem moveFromMapItem = getMapItem(moveFromMapIndex);
    
    // 経路探索(再帰呼び出し)
    findMovePointList(moveFromMapItem.mapIndex.x, moveFromMapItem.mapIndex.y, moveFromMapItem.moveDist, moveToMapItem);
    
    // TODO: 1件も取れない時に目的地だけ返してしまう
    
    // 目的地を最終到達点として最後に追加（ただしカーソル情報なので移動の向きをここで指定する）
    MapIndex moveEndMapIndex = moveFromMapIndex;
    
    MoveDirectionType moveDict = checkMoveDirectionType(moveEndMapIndex, map_data_.map_move_point_list.back());
    moveEndMapIndex.moveDictType = moveDict;
    
    map_data_.map_move_point_list.push_back(moveEndMapIndex);
    
    // movePointListを返却
    return map_data_.map_move_point_list;
}

MoveDirectionType MapManager::checkMoveDirectionType(MapIndex fromMapIndex, MapIndex toMapIndex)
{
    assert(vaildateInit());
    
    int moveX = fromMapIndex.x - toMapIndex.x;
    int moveY = fromMapIndex.y - toMapIndex.y;
    
    if (moveX == 1 && moveY == 0)
    {
        return MoveDirectionType::MOVE_RIGHT;
    }
    else if (moveX == -1 && moveY == 0)
    {
        return MoveDirectionType::MOVE_LEFT;
    }
    else if (moveX == 0 && moveY == 1)
    {
        return MoveDirectionType::MOVE_UP;
    }
    else if (moveX == 0 && moveY == -1)
    {
        return MoveDirectionType::MOVE_DOWN;
    }
    return MoveDirectionType::MOVE_NONE;
}

/**
 * カーソル情報を元に最短距離のMapItemを返します。
 * createActorFindDist後に呼ばないと何も意味がありません.
 */
MapItem MapManager::searchTargetMapItem(const std::list<MapIndex>& searchMapIndexList)
{
    assert(vaildateInit());
    
    MapItem targetMoveDistMapItem = {
        MapDataType::NONE,
        {0, 0, MoveDirectionType::MOVE_NONE},
        0,
        0
    };
    
    for (MapIndex mapIndex : searchMapIndexList) {
        auto mapItem = getMapItem(mapIndex);
        if (mapItem.mapDataType == MapDataType::MOVE_DIST) {
            if (targetMoveDistMapItem.moveDist < mapItem.moveDist) {
                targetMoveDistMapItem = mapItem;
            }
        }
    }
    return targetMoveDistMapItem;
}

/**
 * 移動範囲検索.
 */
void MapManager::findDist(int x, int y, int dist, bool first)
{
    assert(vaildateInit());
    
    // 初期位置は移動対象外とする制御
    if (!first)
    {
        // 移動可能範囲に追加
        addDistCursor(x, y, dist);
    }
    if (dist == 0)
    {
        return;
    }
    
    // 上にいけるか?
    if (y > map_data_.map_data_setting.top && chkMove(x, y - 1, dist))
    {
        findDist(x, y - 1, dist - 1, false);
    }
    // 下にいけるか?
    if (y < (map_data_.map_data_setting.bottom - 1) && chkMove(x, y + 1, dist))
    {
        findDist(x, y + 1, dist - 1, false);
    }
    // 左にいけるか?
    if (x > map_data_.map_data_setting.left && chkMove(x - 1, y, dist))
    {
        findDist(x - 1, y, dist - 1, false);
    }
    // 右にいけるか?
    if (x < (map_data_.map_data_setting.right - 1) && chkMove(x + 1, y, dist))
    {
        findDist(x + 1, y, dist - 1, false);
    }
}

bool MapManager::chkMove(int mapPointX, int mapPointY, int dist) const
{
    assert(vaildateInit());
    
    MapIndex mapIndex = {mapPointX, mapPointY, MoveDirectionType::MOVE_NONE};
    MapItem mapItem = getMapItem(mapIndex);
    if (mapItem.mapDataType == MapDataType::NONE ||
        mapItem.mapDataType == MapDataType::MAP_ITEM ||
        mapItem.mapDataType == MapDataType::KAIDAN ||
        (mapItem.mapDataType == MapDataType::MOVE_DIST && mapItem.moveDist < dist))
    {
        return true;
    }
    return false;
}

bool MapManager::chkMovePoint(int mapPointX, int mapPointY, int dist, MapDataType ignoreMapDataType)
{
    assert(vaildateInit());
    
    MapIndex mapIndex = {mapPointX, mapPointY, MoveDirectionType::MOVE_NONE};
    MapItem mapItem = getMapItem(mapIndex);
    if (mapItem.mapDataType == MapDataType::OBSTACLE) {
        return false;
    }
    if (mapItem.moveDist == dist && mapItem.mapDataType != ignoreMapDataType)
    {
        return true;
    }
    return false;
}

/**
 * 移動カーソル追加.
 */
void MapManager::addDistCursor(int mapPointX, int mapPointY, int dist)
{
    assert(vaildateInit());
    
    // 未設定 or 移動オブジェクトで移動力が上の場合
    MapItem mapItem = map_data_.map_cursor_data_array[mapPointX][mapPointY];
    if (mapItem.mapDataType == MapDataType::NONE ||
        mapItem.mapDataType == MapDataType::MAP_ITEM ||
        mapItem.mapDataType == MapDataType::KAIDAN ||
        (mapItem.mapDataType == MapDataType::MOVE_DIST && mapItem.moveDist < dist))
    {
        MapIndex mapIndex;
        mapIndex.x = mapPointX;
        mapIndex.y = mapPointY;
        mapIndex.moveDictType = MoveDirectionType::MOVE_NONE;
        
		// リストに入れたやつだけあとで描画する
        MapItem cursorItem;
        cursorItem.mapDataType = MapDataType::MOVE_DIST;
        cursorItem.moveDist  = dist;
        cursorItem.attackDist = 0;
        cursorItem.mapIndex = mapIndex;
        map_data_.map_cursor_data_array[mapPointX][mapPointY] = cursorItem;
        
        // 描画用のリスト
        if (mapItem.mapDataType == MapDataType::NONE)
        {
            map_data_.map_move_cursor_list.push_back(mapIndex);
        }
    }
}

/**
 * プレイヤー追加
 */
void MapManager::addActor(const ActorMapItem& actorMapItem)
{
    assert(vaildateInit());
    
    map_data_.map_object_data_array[actorMapItem.mapIndex.x][actorMapItem.mapIndex.y] = actorMapItem;
    DEBUG_LOG_MAP_ITEM_LAYER();
}

/**
 * プレイヤー移動
 */
void MapManager::moveActor(const ActorMapItem& actorMapItem, const MapIndex& beforeMapIndex, const MapIndex& afterMapIndex)
{
    assert(vaildateInit());
    
    map_data_.map_object_data_array[afterMapIndex.x][afterMapIndex.y] = actorMapItem;
    
    ActorMapItem mapItem;
    mapItem.mapDataType = MapDataType::NONE;
    mapItem.mapIndex = beforeMapIndex;
    mapItem.moveDist = 0;
    mapItem.attackDist = 0;
    mapItem.seqNo = 0;
    mapItem.moveDone = false;
    mapItem.attackDone = false;
    map_data_.map_object_data_array[beforeMapIndex.x][beforeMapIndex.y] = mapItem;
    DEBUG_LOG_MAP_ITEM_LAYER();
}

/**
 * ドロップアイテムの追加
 */
void MapManager::addDropItem(const DropMapItem& dropMapItem)
{
    assert(vaildateInit());
    
    map_data_.map_drop_item_data_array[dropMapItem.mapIndex.x][dropMapItem.mapIndex.y] = dropMapItem;
    DEBUG_LOG_MAP_ITEM_LAYER();
}

/**
 * 障害物の追加
 */
void MapManager::addObstacle(const MapIndex& obsMapIndex)
{
    assert(vaildateInit());
    
    // TODO: とりあえずactorと同じにする。。。大丈夫か？
    ActorMapItem mapItem = createNoneMapItem<ActorMapItem>(obsMapIndex.x, obsMapIndex.y);
    mapItem.mapDataType = MapDataType::OBSTACLE;
    mapItem.seqNo = 0;
    mapItem.moveDone = false;
    mapItem.attackDone = false;
    
    map_data_.map_object_data_array[obsMapIndex.x][obsMapIndex.y] = mapItem;
}

/**
 * マップアイテムの削除
 */
void MapManager::removeMapItem(const MapItem& mapItem)
{
    assert(vaildateInit());
    
    if (map_data_.map_cursor_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y].mapDataType == mapItem.mapDataType)
    {
        MapItem noneMapItem = createNoneMapItem<MapItem>(mapItem.mapIndex.x, mapItem.mapIndex.y);
        map_data_.map_cursor_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y] = noneMapItem;
    }
    else if (map_data_.map_object_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y].mapDataType == mapItem.mapDataType)
    {
        ActorMapItem noneMapItem = createNoneMapItem<ActorMapItem>(mapItem.mapIndex.x, mapItem.mapIndex.y);
        noneMapItem.seqNo = 0;
        noneMapItem.moveDone = false;
        noneMapItem.attackDone = false;
        map_data_.map_object_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y] = noneMapItem;
    }
    else if (map_data_.map_drop_item_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y].mapDataType == mapItem.mapDataType)
    {
        DropMapItem noneMapItem = createNoneMapItem<DropMapItem>(mapItem.mapIndex.x, mapItem.mapIndex.y);
        noneMapItem.seqNo = 0;
        noneMapItem.itemId = 0;
        noneMapItem.param = 0;
        map_data_.map_drop_item_data_array[mapItem.mapIndex.x][mapItem.mapIndex.y] = noneMapItem;
    }
    DEBUG_LOG_MAP_ITEM_LAYER();
}

void MapManager::addMapping(const MapIndex& mapIndex)
{
    assert(vaildateInit());
    
    map_data_.mapping_array[mapIndex.x][mapIndex.y] = true;
}

std::vector<std::vector<bool>> MapManager::getMappingData()
{
    assert(vaildateInit());
    
    return map_data_.mapping_array;
}

/**
 * 指定座標のMapItemを取得します。
 * カーソル、オブジェクト、ドロップアイテムの順番に探します。
 * @param pMapIndex 座標
 */
MapItem MapManager::getMapItem(const MapIndex& mapIndex) const
{
    assert(vaildateInit());
    
    if (map_data_.map_cursor_data_array[mapIndex.x][mapIndex.y].mapDataType == MapDataType::NONE)
    {
        auto actorMapItem = getActorMapItem(mapIndex);
        if (actorMapItem.mapDataType == MapDataType::NONE)
        {
           return (map_data_.map_drop_item_data_array[mapIndex.x][mapIndex.y]);
        }
        return MapItem {
            actorMapItem.mapDataType,
            actorMapItem.mapIndex,
            actorMapItem.moveDist,
            actorMapItem.attackDist
        };
    }
    return (map_data_.map_cursor_data_array[mapIndex.x][mapIndex.y]);
}

ActorMapItem MapManager::getActorMapItem(const MapIndex& pMapIndex) const
{
    assert(vaildateInit());
    
    return (map_data_.map_object_data_array[pMapIndex.x][pMapIndex.y]);
}

DropMapItem MapManager::getDropMapItem(const MapIndex& pMapIndex) {
    assert(vaildateInit());
    
    return (map_data_.map_drop_item_data_array[pMapIndex.x][pMapIndex.y]);
}

bool MapManager::isDropMapItemIndex(const MapIndex &dropTargetMapIndex)
{
    if (this->getDropMapItem(dropTargetMapIndex).mapDataType != MapDataType::NONE) {
        return false;
    }
    
    return true;
}


/**
 * 移動ルート情報を作成.
 * 移動先のカーソルから移動元のMapItemまでの最短経路を検索する。
 * movePointListに追加していきます.
 * @param moveX 移動先のx座標
 * @param moveY 移動先のy座標
 * @param moveDist 索敵する移動距離
 * @param moveToMapItem 移動元
 */
void MapManager::findMovePointList(int moveX, int moveY, int moveDist, const MapItem& moveToMapItem)
{
    assert(vaildateInit());
    
    // 自軍キャラでなければ通過不可能とする除外Type
    MapDataType ignoreDataType = MapDataType::NONE;
    if (moveToMapItem.mapDataType == MapDataType::PLAYER) {
        ignoreDataType = MapDataType::ENEMY;
    } else if (moveToMapItem.mapDataType == MapDataType::ENEMY) {
        ignoreDataType = MapDataType::PLAYER;
    } else {
        // プレイヤーと敵以外は移動しない
        return;
    }
    
    // タップ位置から自キャラがいるところまでの最短ルートを探す
    MapIndex moveMapIndex;
    moveMapIndex.x = moveX;
    moveMapIndex.y = moveY;
    MapItem mapItem = getMapItem(moveMapIndex);
    if (!MapManager::equalMapItem(mapItem, moveToMapItem)) {
        // タップした位置のdistの次はどこか探す
        moveDist++;
        
        // 上か
        if (moveY > map_data_.map_data_setting.top && chkMovePoint(moveX, moveY - 1, moveDist, ignoreDataType)) {
            findMovePointList(moveX, moveY - 1, moveDist, moveToMapItem);
            MapIndex movePointIndex = {moveX, moveY - 1, MoveDirectionType::MOVE_UP};
            map_data_.map_move_point_list.push_back(movePointIndex);
        // 下か？
        } else if (moveY < (map_data_.map_data_setting.bottom -1) && chkMovePoint(moveX, moveY + 1, moveDist, ignoreDataType)) {
            findMovePointList(moveX, moveY + 1, moveDist, moveToMapItem);
            MapIndex movePointIndex = {moveX, moveY + 1, MoveDirectionType::MOVE_DOWN};
            map_data_.map_move_point_list.push_back(movePointIndex);
        // 左か?
        } else if (moveX > map_data_.map_data_setting.left && chkMovePoint(moveX - 1, moveY, moveDist, ignoreDataType)) {
            findMovePointList(moveX - 1, moveY, moveDist, moveToMapItem);
            MapIndex movePointIndex = {moveX - 1, moveY, MoveDirectionType::MOVE_RIGHT};
            map_data_.map_move_point_list.push_back(movePointIndex);
        // 右にいけるか?
        } else if (moveX < (map_data_.map_data_setting.right - 1) && chkMovePoint(moveX + 1, moveY, moveDist, ignoreDataType)) {
            findMovePointList(moveX + 1, moveY, moveDist, moveToMapItem);
            MapIndex movePointIndex = {moveX + 1, moveY, MoveDirectionType::MOVE_LEFT};
            map_data_.map_move_point_list.push_back(movePointIndex);
        }
    }
}

std::list<ActorMapItem> MapManager::findActorMapItem()  const
{
    return findMapObjectByDataType(MapDataType::PLAYER);
}

std::list<ActorMapItem> MapManager::findEnemyMapItem() const
{
    return findMapObjectByDataType(MapDataType::ENEMY);
}

std::list<ActorMapItem> MapManager::findMapObjectByDataType(MapDataType mapDataType) const
{
    assert(vaildateInit());
    
    assert(mapDataType == MapDataType::PLAYER || mapDataType == MapDataType::ENEMY);
    
    std::list<ActorMapItem> mapItemList;
    //enemyMapItem.clear();
    
    long xCount = map_data_.map_object_data_array.size();
    for (int x = 0; x < xCount; x++) {
        long yCount = map_data_.map_object_data_array[x].size();
        for (int y = 0; y < yCount; y++) {
            if (map_data_.map_object_data_array[x][y].mapDataType == mapDataType) {
                mapItemList.push_back(map_data_.map_object_data_array[x][y]);
            }
        }
    }
    return mapItemList;
}

std::list<DropMapItem> MapManager::findDropMapItem() const
{
    assert(vaildateInit());
    
    std::list<DropMapItem> mapItemList;
    //enemyMapItem.clear();
    
    long xCount = map_data_.map_drop_item_data_array.size();
    for (int x = 0; x < xCount; x++) {
        long yCount = map_data_.map_drop_item_data_array[x].size();
        for (int y = 0; y < yCount; y++) {
            if (map_data_.map_drop_item_data_array[x][y].mapDataType == MapDataType::MAP_ITEM) {
                mapItemList.push_back(map_data_.map_drop_item_data_array[x][y]);
            }
        }
    }
    return mapItemList;
}

#pragma mark
#pragma mark staticメソッド

/**
 * 指定したMapIndexの周辺MapIndexのListを作成して返却します。
 */
std::list<MapIndex> MapManager::createRelatedMapIndexList(MapIndex baseMapIndex) {
    std::list<MapIndex> relatedMapIndexList;
    relatedMapIndexList.clear();
    
    // 右
    MapIndex searchMapIndex = baseMapIndex;
    searchMapIndex.x += 1;
    searchMapIndex.y += 0;
    searchMapIndex.moveDictType = MoveDirectionType::MOVE_LEFT;
    relatedMapIndexList.push_back(searchMapIndex);
    // 左
    searchMapIndex = baseMapIndex;
    searchMapIndex.x += -1;
    searchMapIndex.y += 0;
    searchMapIndex.moveDictType = MoveDirectionType::MOVE_RIGHT;
    relatedMapIndexList.push_back(searchMapIndex);
    // 上
    searchMapIndex = baseMapIndex;
    searchMapIndex.x += 0;
    searchMapIndex.y += 1;
    searchMapIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
    relatedMapIndexList.push_back(searchMapIndex);
    // 下
    searchMapIndex = baseMapIndex;
    searchMapIndex.x += 0;
    searchMapIndex.y += -1;
    searchMapIndex.moveDictType = MoveDirectionType::MOVE_UP;
    relatedMapIndexList.push_back(searchMapIndex);
    
    return relatedMapIndexList;
}

// ターゲットとタッチした位置が1マス以内の場合に移動距離のMapIndexを返却する
MapIndex MapManager::checkTouchEventIndex(const MapIndex& target_map_index, const MapIndex& touch_point_map_index) {
    // 移動距離計算
    MapIndex add_move_map_index;
    add_move_map_index.x = touch_point_map_index.x - target_map_index.x;
    add_move_map_index.y = touch_point_map_index.y - target_map_index.y;
    
    if (add_move_map_index.x == 1 && add_move_map_index.y == 0) {
        add_move_map_index.moveDictType = MoveDirectionType::MOVE_RIGHT;
    } else if (add_move_map_index.x == -1 && add_move_map_index.y == 0) {
        add_move_map_index.moveDictType = MoveDirectionType::MOVE_LEFT;
    } else if (add_move_map_index.x == 0 && add_move_map_index.y == 1) {
        add_move_map_index.moveDictType = MoveDirectionType::MOVE_UP;
    } else if (add_move_map_index.x == 0 && add_move_map_index.y == -1) {
        add_move_map_index.moveDictType = MoveDirectionType::MOVE_DOWN;
    } else {
        // 上記以外は有効なタッチじゃない
        add_move_map_index.x = 0;
        add_move_map_index.y = 0;
        add_move_map_index.moveDictType = MoveDirectionType::MOVE_DOWN;
        return add_move_map_index;
    }
    
    // プレイヤーから1マス以内か
    return add_move_map_index;
}


#pragma mark
#pragma mark DEBUG関連

void MapManager::showDebug() const
{
    DEBUG_LOG_MAP_ITEM_LAYER();
}

void MapManager::DEBUG_LOG_MAP_ITEM_LAYER() const
{
    assert(vaildateInit());
    
#if DEBUG_LOG_MAP_ITEM_LAYER_FLG
    std::string buffer;
	for (int y = map_data_.map_data_setting.bottom - 1; y >= 0; y--) {
        buffer = "";
		for (int x = 0; x < map_data_.map_data_setting.right; x++) {
            std::string outPutStr = "-";
            std::string dropItemLayerStr = logOutString(map_data_.map_drop_item_data_array[x][y]);
			std::string objectLayerStr = logOutString(map_data_.map_object_data_array[x][y]);
			std::string cursorLayerStr = logOutString(map_data_.map_cursor_data_array[x][y]);
			if (dropItemLayerStr != "") {
                outPutStr = dropItemLayerStr;
            } else if (objectLayerStr != "") {
				outPutStr = objectLayerStr;
			} else if (cursorLayerStr != "") {
				outPutStr = cursorLayerStr;
            }
            buffer += outPutStr + ".";
		}
        printf("%s\n", buffer.c_str());
	}
    printf("\n");
#endif
}

std::string MapManager::logOutString(MapItem mapItem) const
{
    assert(vaildateInit());
    
	if (mapItem.mapDataType == MapDataType::NONE) {
		return "";
	} else if (mapItem.mapDataType == MapDataType::ENEMY) {
		return ("E");
	} else if (mapItem.mapDataType == MapDataType::MAP_ITEM) {
		return ("@");
	} else if (mapItem.mapDataType == MapDataType::OBSTACLE) {
		return ("■");
	} else if (mapItem.mapDataType == MapDataType::PLAYER) {
		return ("P");
	} else {
        char buff[100];
        sprintf(buff, "%d", mapItem.moveDist);
        std::string buffAsStdStr = buff;
		return buffAsStdStr;
	}
}
