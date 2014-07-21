//
//  RogueTMXTiledMap.h
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/03/16.
//
//

#ifndef __Cocos2dRogueLike__RogueTMXTiledMap__
#define __Cocos2dRogueLike__RogueTMXTiledMap__

#include "AppMacros.h"
#include "cocos2d.h"

#include "MapManager.h"

#include "ActorSprite.h"
#include "DropItemSprite.h"

#include "TMXGenerator.h"

USING_NS_CC;

NS_ROGUE_BEGIN
//#define GAME_SCALE (Director::getInstance()->getVisibleSize().width / 960.0f)
#define MINI_MAP_SCALE (10)

class RogueTMXTiledMap : public TMXTiledMap
{
    enum TiledMapTags {
        TiledMapDropItemBaseTag        = 10000, // + seqNo
        TiledMapObjectTag              = 15000, // 階段専用
        TiledMapEnemyBaseTag           = 20000, // + seqNo
        
        TiledMapFrontLayerTag          = 40000,
        FloorLayerTag                  = 40001,
        FloorMaskLayerTag              = 40002,
        FloorMaskPlayerLayerTag        = 40003,
    };
    
    enum TiledMapIndexs {
        TiledMapDropItemBaseZOrder,
        TiledMapObjectZOrder,
        TiledMapEnemyBaseZOrder,
        TiledMapFrontZOrder,
        FloorLayerZOrder,
        FloorMaskLayerZOrder,
    };
    
    // ミニマップ上のタグ
    enum MiniMapLayerTags {
        MiniMapSimbolBatchNodeTag = 100,
    };
    
    enum ZOrders {
        MiniMapLayerZOrder,
        MiniMapLayerMapNoneZOrder,
        MiniMapLayerMapItemZOrder,
        MiniMapLayerMapActorZOrder
    };
    
private:
    LayerColor* mini_map_layer_;
    MapManager _mapManager;
    
    int enemyCount_;
    // 千里眼
    bool itemAllShow_;
    // 地獄耳
    bool enemyAllShow_;
    
    TMXMapData _tmxMapData;
    
public:
    typedef std::function<void(DropItemSprite* dropItemSprite, const MapIndex&, const ItemDto&)> ThrowDropMapItemCallback;
    
    static RogueTMXTiledMap* create(const std::string& tmxFile = "");
    
    ActorMapItem startPlayerRandomPosition(const ActorDto& actor_dto, const MapIndex& base_actor_index);
    void setPlayerActorMapItem(const ActorMapItem& actorMapItem, int tag);
    bool tileSetEnemyActorMapItem(ActorDto enemyActorDto, MapIndex mapIndex);
    void removeEnemyActorSprite(ActorSprite* pEnemySprite);
    
    bool tileSetDropMapItem(const ItemDto& itemDto, MapIndex mapIndex);
    void throwDropMapItem(const ItemDto& itemDto, MapIndex startMapIndex, MapIndex endMapIndex, const ThrowDropMapItemCallback& callback);
    
    void removeDropItemSprite(DropItemSprite* pDropItemSprite);
    ActorSprite* getEnemyActorSprite(int seq_no);
    DropItemSprite* getDropItemSprite(int seq_no);
    
    void movePlayerMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc);
    void moveEnemyMap(int enemy_seq_no, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc);
    
    // 照明
    void refreshAllFloorMapping();
    void refreshPlayerRectAutoMapping(const MapIndex& actor_map_index);
    void refreshFloorRectAutoMapping(const Rect& floorInfoIndexRect);
    Rect getTileMapFloorInfo(const MapIndex& actor_map_index);
    void showFloorLighting(const MapIndex& actor_map_index, const Rect floorInfoIndexRect);
    void hideFloorLighting();
    
    MapIndex getFloorRandomMapIndex(bool isActor);
    MapIndex getRandomMapIndex(bool isColision, bool isActor);
    
    bool isFloorMapIndex(const MapIndex& mapIndex);
    bool isActorInstMapIndex(const MapIndex& mapIndex);
    bool isDropItemInstMapIndex(const MapIndex& mapIndex);
    bool isTiledMapColisionLayer(const MapIndex& touchPointMapIndex);
    bool isMapLayerOver(const MapIndex& touchPointMapIndex) const;
    
    DrawNode* createGridDraw() const;
    
    Point indexToPoint(const MapIndex& mapIndex) const;
    Point indexToPoint(int mapIndex_x, int mapIndex_y)const;
    MapIndex pointToIndex(const Point& point) const;
    MapIndex touchPointToIndex(const Point& point) const;
    Point indexToPointNotTileSize(const MapIndex& mapIndex) const;
    Point indexToPointNotTileSize(int mapIndex_x, int mapIndex_y) const;
    MapIndex mapIndexToTileIndex(const MapIndex& mapIndex) const;
    MapIndex mapIndexToTileIndex(int mapIndex_x, int mapIndex_y) const;
    
    void addMiniMapItem(const MapItem& mapItem, int baseSpriteTag);

    LayerColor* getMiniMapLayer() { return mini_map_layer_; };
    
    void setItemAllShow(bool isAllShow) { itemAllShow_ = isAllShow; };
    void setEnemyAllShow(bool isAllShow) { enemyAllShow_ = isAllShow; };
    
    MapManager* getMapManager() { return &_mapManager; }
    
protected:
    RogueTMXTiledMap();
    virtual ~RogueTMXTiledMap();
    
private:

    void initRogue();
        
    SpriteBatchNode* getGridSpriteBatchNode();
    
    void moveMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, MapDataType mapDataType, float animation_speed, CallFunc* moveFinishedCallFunc);
    
    // 照明
    void showPlayerLighting(ActorSprite* actor_sprite);
    void hidePlayerLighting();
    
    MapIndex getRandomMapIndex(bool isColision, bool isActor, bool isFloor);
    
    void tiledMapItemLighting(const Rect& floorInfoIndexRect, bool isRefresh);
    
    Rect createPlayerRect(const MapIndex& actor_map_index, int rectSize);
    
    void refreshAutoMapping(const Rect& floorInfoIndexRect);
    void refreshAutoMapping(const MapIndex& minMapIndex, const MapIndex& maxMapIndex);
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__RogueTMXTiledMap__) */
