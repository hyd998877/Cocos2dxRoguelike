//
//  MapManager.h
//  Cocos2dxSRPGQuest
//
//  Created by kyokomi on 2013/10/06.
//
//

#ifndef __Cocos2dxSRPGQuest__MapManager__
#define __Cocos2dxSRPGQuest__MapManager__

#include <iostream>
#include <vector>
#include <list>

enum MapDataType {
    NONE        = 0,
    MOVE_DIST   = 1,
    ATTACK_DIST = 2,
    MAP_ITEM    = 3,
    PLAYER      = 4,
    ENEMY       = 5,
    OBSTACLE    = 6,
    KAIDAN      = 7,
    MOVE_STEP_DIST = 10, // 移動経路
    SELECTED_DIST  = 11, // 選択位置
};

enum MoveDirectionType {
    MOVE_NONE  = -1,
	MOVE_DOWN  = 0,
	MOVE_LEFT  = 1,
	MOVE_RIGHT = 2,
	MOVE_UP    = 3
};

typedef struct _MapIndex {
    int x;
    int y;
    MoveDirectionType moveDictType;
}MapIndex;

typedef struct _MapItem {
    /** 短形マップ上の区分. */
    MapDataType mapDataType;
    
    MapIndex mapIndex;
    
    /** 移動可能距離. */
    int moveDist;
    /** 攻撃可能距離. */
    int attackDist;
}MapItem;

typedef struct _ActorMapItem : public MapItem {
    /** プレイヤーを一意に識別するID. */
    int seqNo;
    /** 移動済みフラグ. */
    bool moveDone;
    /** 攻撃済みフラグ. */
    bool attackDone;
}ActorMapItem;

typedef struct _DropMapItem : public MapItem {
    /** ドロップアイテムを一意に識別するID. */
    int seqNo;
    /** アイテムID */
    int itemId;
    /** アイテムのパラメータ（装備なら+値 お金ならゴールド） */
    int param;
}DropMapItem;


#define MAP_INDEX_DIFF(mapIndexA, mapIndexB) (mapIndexA.x == mapIndexB.x && mapIndexA.y == mapIndexB.y)

class MapManager
{
public:
    typedef struct _MapDataSetting {
        int top;
        int bottom;
        int left;
        int right;
    } MapDataSetting;
    
    typedef struct _MapData {
        
        MapDataSetting map_data_setting;
        
        // マップカーソル一時データ
        std::vector<std::vector<MapItem>> map_cursor_data_array;
        // マップオブジェクトデータ（ドロップアイテム等）
        std::vector<std::vector<DropMapItem>> map_drop_item_data_array;
        // マップオブジェクトデータ（キャラ、障害物）
        std::vector<std::vector<ActorMapItem>> map_object_data_array;
        // マッピングデータ
        std::vector<std::vector<bool>> mapping_array;
        
        // マップ移動カーソルリスト
        std::list<MapIndex> map_move_cursor_list;
        // マップ移動経路リスト
        std::list<MapIndex> map_move_point_list;
    } MapData;

    static std::list<MapIndex> createRelatedMapIndexList(MapIndex baseMapIndex);
    static MapIndex checkTouchEventIndex(const MapIndex& target_map_index, const MapIndex& touch_point_map_index);
    
    static bool isMapIndexEmpty(const MapIndex& mapIndex) {
        MapIndex emptyMapIndex = createMapIndexEmpty();
        return (mapIndex.x == emptyMapIndex.x && mapIndex.y == emptyMapIndex.y && mapIndex.moveDictType == emptyMapIndex.moveDictType) ? true : false;
    }
    static MapIndex createMapIndexEmpty() { return {-1, -1, MoveDirectionType::MOVE_NONE}; }
private:
    // マップデータ save
    MapData map_data_;
    
public:
    MapManager();
    ~MapManager();
    void initMapping(int top, int bottom, int left, int right);
    
    std::list<MapIndex> createActorFindDist(const MapIndex& mapIndex, int dist);
    std::list<MapIndex> createMovePointList(const MapIndex& moveFromMapIndex, const MapItem& moveToMapItem);
    void clearCursor();
    
    void addActor(const ActorMapItem& actorMapItem);
    void moveActor(const ActorMapItem& actorMapItem, const MapIndex& beforeMapIndex, const MapIndex& afterMapIndex);
    void addDropItem(const DropMapItem& dropMapItem);
    void addObstacle(const MapIndex& obsMapIndex);
    
    void removeMapItem(const MapItem& mapItem);
    
    ActorMapItem getActorMapItem(const MapIndex &pMapIndex) const;
    MapItem getMapItem(const MapIndex &pMapIndex) const;
    DropMapItem getDropMapItem(const MapIndex &mapIndex);
    bool isDropMapItemIndex(const MapIndex &dropTargetMapIndex);
    
    MoveDirectionType checkMoveDirectionType(MapIndex fromMapIndex, MapIndex toMapIndex);
    MapItem searchTargetMapItem(const std::list<MapIndex>& searchMapIndexList);
    
    std::list<ActorMapItem> findActorMapItem() const;
    std::list<ActorMapItem> findEnemyMapItem() const;
    std::list<ActorMapItem> findMapObjectByDataType(MapDataType mapDataType) const;
    
    void addMapping(const MapIndex& mapIndex);
    std::vector<std::vector<bool>> getMappingData();
   
    MapData getMapData() const { return map_data_; }
    
    // MapItemの初期データを作ります
    template <typename TYPE>
    static TYPE createNoneMapItem(int x, int y)
    {
        TYPE mapItem;
        mapItem.mapDataType = MapDataType::NONE;
        mapItem.mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
        mapItem.moveDist = 0;
        mapItem.attackDist = 0;
        return mapItem;
    }
    
    static bool equalMapItem(const MapItem& m1, const MapItem& m2)
    {
        if (m1.mapDataType != m2.mapDataType) {
            return false;
        }
        if (m1.mapIndex.x != m2.mapIndex.x) {
            return false;
        }
        if (m1.mapIndex.y != m2.mapIndex.y) {
            return false;
        }
        if (m1.moveDist != m2.moveDist) {
            return false;
        }
        if (m1.attackDist != m2.attackDist) {
            return false;
        }
        return true;
    }
private:
    void init();
    
    bool vaildateInit() const
    {
        if (map_data_.map_data_setting.bottom == 0 &&
            map_data_.map_data_setting.left == 0 &&
            map_data_.map_data_setting.right == 0 &&
            map_data_.map_data_setting.top == 0)
        {
            return false;
        }
        
        return true;
    }
    
    // 2次元配列をTYPEので初期化します
    template <typename TYPE>
    void clearMapItemArray(std::vector<std::vector<TYPE>> *pMapItemArray)
    {
        pMapItemArray->clear();
        for (int x = 0; x < map_data_.map_data_setting.right; x++)
        {
            std::vector<TYPE> mapItemArray;
            for (int y = 0; y < map_data_.map_data_setting.bottom; y++)
            {
                TYPE noneMapItem = createNoneMapItem<TYPE>(x, y);
                mapItemArray.push_back(noneMapItem);
            }
            pMapItemArray->push_back(mapItemArray);
        }
    }
    
    // 2次元配列をTYPEのvalue値で初期化します
    template <typename TYPE>
    void clearArray(std::vector<std::vector<TYPE>> *pArray, TYPE value)
    {
        pArray->clear();
        for (int x = 0; x < map_data_.map_data_setting.right; x++)
        {
            std::vector<TYPE> temp_array;
            for (int y = 0; y < map_data_.map_data_setting.bottom; y++)
            {
                temp_array.push_back(value);
            }
            pArray->push_back(temp_array);
        }
    }
    
    void findDist(int x, int y, int dist, bool first);
    void findMovePointList(int moveX, int moveY, int moveDist, const MapItem& moveToMapItem);
    bool chkMove(int mapPointX, int mapPointY, int dist) const;
    bool chkMovePoint(int mapPointX, int mapPointY, int dist, MapDataType ignoreMapDataType);
    void addDistCursor(int mapPointX, int mapPointY, int dist);
    
    std::string logOutString(MapItem mapItem);
    void DEBUG_LOG_MAP_ITEM_LAYER(); // デバッグ用のマップログ出力
};

#endif /* defined(__Cocos2dxSRPGQuest__MapManager__) */
