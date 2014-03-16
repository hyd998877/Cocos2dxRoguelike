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

USING_NS_CC;

NS_ROGUE_BEGIN

#define MINI_MAP_SCALE (4.16f / GAME_SCALE)

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
    
public:
    static RogueTMXTiledMap* create(const std::string& tmxFile);
    
    ActorMapItem startPlayerRandomPosition(ActorSprite::ActorDto& actor_dto, const MapIndex& base_actor_index);
    void setPlayerActorMapItem(ActorMapItem* actor_map_item, int tag);
    bool tileSetEnemyActorMapItem(ActorSprite::ActorDto enemyActorDto, MapIndex mapIndex);
    void removeEnemyActorSprite(ActorSprite* pEnemySprite);
    bool tileSetDropMapItem(DropItemSprite::DropItemDto dropItemDto, MapIndex mapIndex);
    void removeDropItemSprite(DropItemSprite* pDropItemSprite);
    ActorSprite* getEnemyActorSprite(int seq_no);
    DropItemSprite* getDropItemSprite(int seq_no);
    
    void movePlayerMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc);
    void moveEnemyMap(int enemy_seq_no, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc);
    
    // 照明
    void refreshPlayerRectAutoMapping(const MapIndex& actor_map_index);
    void refreshFloorRectAutoMapping(const Rect& floorInfoIndexRect);
    Rect getTileMapFloorInfo(const MapIndex& actor_map_index);
    void showFloorLighting(const MapIndex& actor_map_index, const Rect floorInfoIndexRect);
    void hideFloorLighting();
    
    MapIndex getRandomMapIndex(bool isColision, bool isActor);
    bool isTiledMapColisionLayer(MapIndex touchPointMapIndex);
    bool isMapLayerOver(MapIndex touchPointMapIndex);
    DrawNode* createGridDraw();
    
    Point indexToPoint(MapIndex mapIndex);
    Point indexToPoint(int mapIndex_x, int mapIndex_y);
    MapIndex pointToIndex(Point point);
    MapIndex touchPointToIndex(Point point);
    Point indexToPointNotTileSize(MapIndex mapIndex);
    Point indexToPointNotTileSize(int mapIndex_x, int mapIndex_y);
    MapIndex mapIndexToTileIndex(MapIndex mapIndex);
    MapIndex mapIndexToTileIndex(int mapIndex_x, int mapIndex_y);
    
    void addMiniMapItem(MapItem* pMapItem, int baseSpriteTag);

    LayerColor* getMiniMapLayer() { return mini_map_layer_; };
    
protected:
    RogueTMXTiledMap();
    ~RogueTMXTiledMap();
    
private:
    LayerColor* mini_map_layer_;
    long enemyCount_;
    
    void initRogue();
    
    SpriteBatchNode* getGridSpriteBatchNode();
    
    void moveMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, MapDataType mapDataType, float animation_speed, CallFunc* moveFinishedCallFunc);
    
    // 照明
    void showPlayerLighting(ActorSprite* actor_sprite);
    void hidePlayerLighting();
    
    void tiledMapItemLighting(const Rect& floorInfoIndexRect, bool isRefresh);
    
    Rect createPlayerRect(const MapIndex& actor_map_index, int rectSize);
    
    void refreshAutoMapping(const Rect& floorInfoIndexRect);
};

NS_ROGUE_END

#endif /* defined(__Cocos2dRogueLike__RogueTMXTiledMap__) */
