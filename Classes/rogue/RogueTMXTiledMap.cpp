//
//  RogueTMXTiledMap.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/03/16.
//
//

#include "RogueTMXTiledMap.h"

NS_ROGUE_BEGIN

RogueTMXTiledMap::RogueTMXTiledMap()
:mini_map_layer_(nullptr),
enemyCount_(0),
itemAllShow_(false),
enemyAllShow_(false)
{
}

RogueTMXTiledMap::~RogueTMXTiledMap()
{
    mini_map_layer_->removeFromParentAndCleanup(true);
}

RogueTMXTiledMap * RogueTMXTiledMap::create(const std::string& tmxFile) {
    RogueTMXTiledMap *ret = new RogueTMXTiledMap();
    if (ret->initWithTMXFile(tmxFile)) {
        ret->initRogue();
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

void RogueTMXTiledMap::initRogue() {
    
    auto win_size = getContentSize();
    
    MapManager::getInstance()->initMapping(0, (int)this->getMapSize().height, 0, (int)this->getMapSize().width);
    // フロントレイヤー
    auto pFrontLayer = Layer::create();
    this->addChild(pFrontLayer, TiledMapIndexs::TiledMapFrontZOrder, TiledMapTags::TiledMapFrontLayerTag);
    // エネミーレイヤー
    auto pEnemyLayer = Layer::create();
    this->addChild(pEnemyLayer, TiledMapIndexs::TiledMapEnemyBaseZOrder, TiledMapTags::TiledMapEnemyBaseTag);
    // ドロップアイテムレイヤー
    auto pDropItemLayer = Layer::create();
    this->addChild(pDropItemLayer, TiledMapIndexs::TiledMapDropItemBaseZOrder, TiledMapTags::TiledMapDropItemBaseTag);
    
    // ----------------
    // ミニマップ
    
    // 透明
    mini_map_layer_ = LayerColor::create(Color4B(0, 0, 0, 0));
    // 1/8サイズ
    mini_map_layer_->setContentSize(Size(this->getMapSize().width * this->getTileSize().width / MINI_MAP_SCALE,
                                      this->getMapSize().height * this->getTileSize().height / MINI_MAP_SCALE));
    
    // ----------------
    // 障害物
    
    // 障害物をmapManagerに適応する
    auto pColisionLayer = this->getLayer("colision");
    for (int x = 0; x < this->getMapSize().width; x++) {
        for (int y = 0; y < this->getMapSize().height; y++) {
            MapIndex mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
            auto tileMapIndex = mapIndexToTileIndex(mapIndex);
            if (pColisionLayer->getTileAt(Point(x, y))) {
                MapManager::getInstance()->addObstacle(&tileMapIndex);
            } else {
                // ミニマップ更新
                int tag = 10000 * x + 100 * y;
                addMiniMapItem(MapManager::getInstance()->getMapItem(tileMapIndex), tag);
            }
        }
    }
    auto pGridBatchNode = getGridSpriteBatchNode();
    for (auto pNode : pGridBatchNode->getChildren()) {
        if (pNode->getZOrder() == RogueTMXTiledMap::MiniMapLayerMapNoneZOrder) {
            pNode->setVisible(false);
        }
    }
    
    // floor_%dは、部屋の明かり用Layerなので隠す
    int floor_count = 1;
    while (true) {
        auto floor_layer = this->getLayer(StringUtils::format("floor_%d", floor_count));
        if (!floor_layer) {
            break;
        }
        floor_layer->setVisible(false);
        floor_count++;
    }

    // ----------------
    // 階段
    // ----------------
    MapIndex kaidanIndex = getFloorRandomMapIndex(false);
    auto pKaidanSprite = Sprite::create("kaidan.png");
    pKaidanSprite->setPosition(indexToPoint(kaidanIndex));
    pKaidanSprite->setVisible(false);
    this->addChild(pKaidanSprite, TiledMapIndexs::TiledMapObjectZOrder, TiledMapTags::TiledMapObjectTag);
    
    // マップに追加
    DropMapItem kaidanMapItem;
    kaidanMapItem.seqNo = 1;
    kaidanMapItem.mapIndex = kaidanIndex;
    kaidanMapItem.moveDist = 0;
    kaidanMapItem.attackDist = 0;
    kaidanMapItem.mapDataType = MapDataType::KAIDAN;
    MapManager::getInstance()->addDropItem(&kaidanMapItem);
    
    // ミニマップも更新
    addMiniMapItem(&kaidanMapItem, TiledMapTags::TiledMapObjectTag);

}

void RogueTMXTiledMap::setPlayerActorMapItem(ActorMapItem* actor_map_item, int tag) {
    MapManager::getInstance()->addActor(actor_map_item);
    // プレイヤーの位置表示用（同じく1/8サイズ）
    addMiniMapItem(actor_map_item, tag);
}

ActorMapItem RogueTMXTiledMap::startPlayerRandomPosition(ActorDto& actor_dto, const MapIndex& base_actor_index) {
    ActorMapItem actorMapItem;
    
    actorMapItem.mapDataType = MapDataType::PLAYER;
    // 画面の中心（固定）
    MapIndex playerRandMapIndex = getRandomMapIndex(false, true);
    playerRandMapIndex.moveDictType = MoveDirectionType::MOVE_DOWN;
    actorMapItem.mapIndex = playerRandMapIndex;
    actorMapItem.seqNo = 1;
    actorMapItem.moveDist = actor_dto.getMovePoint();
    actorMapItem.attackDist = actor_dto.getAttackRange();
    actorMapItem.moveDone = false;
    actorMapItem.attackDone = false;
    
    // プレイヤー位置にマップを移動
    MapIndex moveIndex = {
        playerRandMapIndex.x - base_actor_index.x,
        playerRandMapIndex.y - base_actor_index.y,
        actorMapItem.mapIndex.moveDictType
    };
    // 移動する距離をPointに変換
    auto addMovePoint = Point(getTileSize().width * moveIndex.x, this->getTileSize().height * moveIndex.y);
    // マップを移動
    this->runAction(MoveTo::create(0.0f, this->getPosition() - addMovePoint));
    
    return actorMapItem;
}

bool RogueTMXTiledMap::tileSetEnemyActorMapItem(ActorDto enemyActorDto, MapIndex mapIndex) {
    
    // すでにプレイヤーが置いてある場合は置けない
    if (MapManager::getInstance()->getActorMapItem(mapIndex)->mapDataType != MapDataType::NONE) {
        return false;
    }
    
    auto pEnemyLayer = this->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    // 出現数のカウントをseqNoにする
    enemyCount_++;
    
    ActorMapItem enemyMapItem;
    enemyMapItem.mapDataType = MapDataType::ENEMY;
    enemyMapItem.mapIndex = mapIndex;
    enemyMapItem.seqNo = enemyCount_;
    enemyMapItem.moveDist = enemyActorDto.getMovePoint();
    enemyMapItem.attackDist = enemyActorDto.getAttackRange();
    enemyMapItem.moveDone = false;
    enemyMapItem.attackDone = false;
    
    auto enemySprite = ActorSprite::createWithActorDto(enemyActorDto);
    enemySprite->setPosition(indexToPoint(enemyMapItem.mapIndex));
    enemySprite->setActorMapItem(enemyMapItem);
    enemySprite->runBottomAction();
    pEnemyLayer->addChild(enemySprite, TiledMapIndexs::TiledMapEnemyBaseZOrder, (TiledMapTags::TiledMapEnemyBaseTag + enemyMapItem.seqNo));
    
    // マップに追加
    MapManager::getInstance()->addActor(enemySprite->getActorMapItem());
    
    // ミニマップも更新
    addMiniMapItem(enemySprite->getActorMapItem(), enemySprite->getTag());
    
    return true;
}

void RogueTMXTiledMap::removeEnemyActorSprite(ActorSprite* pEnemySprite) {
    auto pEnemyMapLayer = this->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    
    // mapManagerから消す
    MapManager::getInstance()->removeMapItem(pEnemySprite->getActorMapItem());
    
    // ミニマップを更新
    getGridSpriteBatchNode()->removeChildByTag(pEnemySprite->getTag());
    
    // Map上からremoveする
    pEnemyMapLayer->removeChild(pEnemySprite);
}

bool RogueTMXTiledMap::tileSetDropMapItem(DropItemSprite::DropItemDto dropItemDto, MapIndex mapIndex) {
    
    // 配置
    if (MapManager::isMapIndexEmpty(mapIndex)) {
        mapIndex = getFloorRandomMapIndex(false);
    }
    
    // すでにアイテムが置いてある場合は置けない
    if (MapManager::getInstance()->getDropMapItem(&mapIndex)->mapDataType != MapDataType::NONE) {
        return false;
    }
    
    auto pDropItemLayer = this->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
    
    DropMapItem dropMapItem;
    // 一意になるようにする x * 100 + y（100マスないからいけるはず）
    dropMapItem.seqNo = mapIndex.x * 100 + mapIndex.y;
    dropMapItem.itemId = dropItemDto.itemId;
    dropMapItem.mapDataType = MapDataType::MAP_ITEM;
    dropMapItem.moveDist = 0;
    dropMapItem.mapIndex = mapIndex;
    
    auto pDropItemSprite = DropItemSprite::createWithDropItemDto(dropItemDto);
    pDropItemSprite->setDropMapItem(dropMapItem);
    pDropItemSprite->setPosition(indexToPoint(dropMapItem.mapIndex));
    pDropItemLayer->addChild(pDropItemSprite, TiledMapIndexs::TiledMapDropItemBaseZOrder, TiledMapTags::TiledMapDropItemBaseTag + dropMapItem.seqNo);
    
    // マップに追加
    MapManager::getInstance()->addDropItem(pDropItemSprite->getDropMapItem());
    
    // ミニマップも更新
    addMiniMapItem(pDropItemSprite->getDropMapItem(), pDropItemSprite->getTag());
    
    return true;
}

void RogueTMXTiledMap::removeDropItemSprite(DropItemSprite* pDropItemSprite) {
    // mapManagerから消す
    MapManager::getInstance()->removeMapItem(pDropItemSprite->getDropMapItem());
    
    // ミニマップを更新
    getGridSpriteBatchNode()->removeChildByTag(pDropItemSprite->getTag());
    
    // Map上からremoveする
    this->getChildByTag(RogueTMXTiledMap::TiledMapDropItemBaseTag)->removeChild(pDropItemSprite);
}

#pragma mark
#pragma mark ミニマップ

SpriteBatchNode* RogueTMXTiledMap::getGridSpriteBatchNode() {
    auto pBatchNode = static_cast<SpriteBatchNode*>(mini_map_layer_->getChildByTag(MiniMapLayerTags::MiniMapSimbolBatchNodeTag));
    if (!pBatchNode) {
        pBatchNode = SpriteBatchNode::create("grid32.png");
        pBatchNode->setTag(MiniMapLayerTags::MiniMapSimbolBatchNodeTag);
        mini_map_layer_->addChild(pBatchNode);
    }
    return pBatchNode;
}

void RogueTMXTiledMap::addMiniMapItem(MapItem* pMapItem, int baseSpriteTag) {
    
    int index = 0;
    Color3B spriteColor;
    int alpha = 255;
    
    if (pMapItem->mapDataType == MapDataType::MAP_ITEM) {
        spriteColor = Color3B(51, 204, 255);
        index = RogueTMXTiledMap::MiniMapLayerMapItemZOrder;
    } else if (pMapItem->mapDataType == MapDataType::ENEMY) {
        spriteColor = Color3B::RED;
        index = RogueTMXTiledMap::MiniMapLayerMapActorZOrder;
    } else if (pMapItem->mapDataType == MapDataType::PLAYER) {
        spriteColor = Color3B::YELLOW;
        index = RogueTMXTiledMap::MiniMapLayerMapActorZOrder;
    } else if (pMapItem->mapDataType == MapDataType::NONE) {
        spriteColor = Color3B(0, 0, 196);
        alpha = 128;
        index = RogueTMXTiledMap::MiniMapLayerMapNoneZOrder;
    } else if (pMapItem->mapDataType == MapDataType::KAIDAN) {
        spriteColor = Color3B::GREEN;
        alpha = 128;
        index = RogueTMXTiledMap::MiniMapLayerMapItemZOrder;
    }
    
    auto pBatchNode = getGridSpriteBatchNode();
    if (pBatchNode) {
        auto pSprite = Sprite::createWithTexture(pBatchNode->getTexture());
        pSprite->setPosition(indexToPoint(pMapItem->mapIndex.x, pMapItem->mapIndex.y));
        
        pSprite->setColor(spriteColor);
        pSprite->setOpacity(alpha);
        
        // タイルの1/8サイズ
        float scale = 1.0f / MINI_MAP_SCALE;
        pSprite->setScale(scale);
        pSprite->setContentSize(this->getTileSize() / MINI_MAP_SCALE);
        // 現在位置からPositionを取得して1/8にする
        auto point = indexToPointNotTileSize(pMapItem->mapIndex) / MINI_MAP_SCALE;
        pSprite->setPosition(point.x + pSprite->getContentSize().width / 2 * scale,
                             point.y + pSprite->getContentSize().height / 2 * scale);
        // 移動時に更新できるようにtag管理
        pSprite->setTag(baseSpriteTag);
        pSprite->setZOrder(index);
        
        // add
        pBatchNode->addChild(pSprite);
    }
}

ActorSprite* RogueTMXTiledMap::getEnemyActorSprite(int seq_no) {
    auto pEnemyMapLayer = this->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    return static_cast<ActorSprite*>(pEnemyMapLayer->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag + seq_no));
}

DropItemSprite* RogueTMXTiledMap::getDropItemSprite(int seq_no) {
    auto pDropItemLayer = this->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
    return static_cast<DropItemSprite*>(pDropItemLayer->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag + seq_no));
}

void RogueTMXTiledMap::movePlayerMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc) {
    moveMap(pActorSprite, addMoveIndex, MapDataType::PLAYER, animation_speed, moveFinishedCallFunc);
}

void RogueTMXTiledMap::moveEnemyMap(int enemy_seq_no, MapIndex addMoveIndex, float animation_speed, CallFunc* moveFinishedCallFunc) {
    auto enemy_sprite = getEnemyActorSprite(enemy_seq_no);
    if (enemy_sprite) {
        // モンスターは普通にモンスターが移動 見えないやつはアニメーション不要
        if (!enemy_sprite->isVisible()) {
            animation_speed = 0;
        }
        moveMap(enemy_sprite, addMoveIndex, MapDataType::ENEMY, animation_speed, moveFinishedCallFunc);
    }
}

void RogueTMXTiledMap::moveMap(ActorSprite* pActorSprite, MapIndex addMoveIndex, MapDataType mapDataType, float animation_speed, CallFunc* moveFinishedCallFunc) {
    // 移動距離計算
    // 移動する距離をPointに変換
    auto addMovePoint = Point(this->getTileSize().width * addMoveIndex.x, this->getTileSize().height * addMoveIndex.y);
    
    Node* pActionTargetNode;
    FiniteTimeAction* pMoveRunAction;
    // マップを指定index分移動
    if (pActorSprite->getActorMapItem()->mapDataType == MapDataType::PLAYER) {
        // プレイヤーならマップが移動
        pMoveRunAction = MoveTo::create(animation_speed, this->getPosition() - addMovePoint);
        pActionTargetNode = this;
    } else {
        pMoveRunAction = MoveTo::create(animation_speed, pActorSprite->getPosition() + addMovePoint);
        pActionTargetNode = pActorSprite;
    }
    
    // マップ情報を更新
    MapIndex moveMapIndex;
    moveMapIndex.moveDictType = addMoveIndex.moveDictType;
    auto actorMapItem = pActorSprite->getActorMapItem();
    moveMapIndex.x = actorMapItem->mapIndex.x + addMoveIndex.x;
    moveMapIndex.y = actorMapItem->mapIndex.y + addMoveIndex.y;
    MapManager::getInstance()->moveActor(actorMapItem, moveMapIndex);
    // actor情報も更新
    actorMapItem->mapIndex = moveMapIndex;
    
    // ミニマップも更新
    auto pMiniMapLayer = getGridSpriteBatchNode();
    if (pMiniMapLayer) {
        auto pMiniMapActorNode = pMiniMapLayer->getChildByTag(pActorSprite->getTag());
        float scale = 1.0f / MINI_MAP_SCALE;
        auto point = indexToPointNotTileSize(pActorSprite->getActorMapItem()->mapIndex) / MINI_MAP_SCALE;
        pMiniMapActorNode->setPosition(point.x + pMiniMapActorNode->getContentSize().width / 2 * scale,
                                       point.y + pMiniMapActorNode->getContentSize().height / 2 * scale);
    }
    // move実行
    pActionTargetNode->runAction(Sequence::create(pMoveRunAction, moveFinishedCallFunc, NULL));
}

#pragma mark
#pragma mark 照明

void RogueTMXTiledMap::refreshPlayerRectAutoMapping(const MapIndex& actor_map_index) {
    Rect floorInfoPlayerIndexRect = createPlayerRect(actor_map_index, 1);
    refreshAutoMapping(floorInfoPlayerIndexRect);
    // プレイヤー視野でマップをリフレッシュする
    tiledMapItemLighting(floorInfoPlayerIndexRect, true);
}

void RogueTMXTiledMap::refreshFloorRectAutoMapping(const Rect& floorInfoIndexRect) {
    // マッピング更新
    refreshAutoMapping(floorInfoIndexRect);
    // マップ情報も更新
    tiledMapItemLighting(floorInfoIndexRect, false);
}
/**
 * タイルマップ上のマップアイテムを視野確認を更新。（ミニマップも）
 *
 * @param floorInfoIndexRect 表示する領域
 * @param isRefresh true : 表示をリフレッシュ / false : 表示中を非表示にはしない
 */
void RogueTMXTiledMap::tiledMapItemLighting(const Rect& floorInfoIndexRect, bool isRefresh) {
    auto pBatchNode = getGridSpriteBatchNode();
    
    // 今いる部屋以外見えなくする
    // アイテム
    auto pDropItemMapLayer = this->getChildByTag(TiledMapTags::TiledMapDropItemBaseTag);
    if (pDropItemMapLayer) {
        for (auto pDropItemNode : pDropItemMapLayer->getChildren()) {
            bool isContains = floorInfoIndexRect.containsPoint(pDropItemNode->getPosition());
            if (isRefresh || isContains || itemAllShow_) {
                // 千里眼優先
                bool settingVisible = itemAllShow_ ? true : isContains;
                
                pDropItemNode->setVisible(settingVisible);
                
                if (pBatchNode) {
                    auto pMiniDropItemNode = pBatchNode->getChildByTag(pDropItemNode->getTag());
                    if (pMiniDropItemNode) {
                        pMiniDropItemNode->setVisible(settingVisible);
                    }
                }
            }
        }
    }
    
    //階段
    auto pKaidan = this->getChildByTag(TiledMapTags::TiledMapObjectTag);
    if (pKaidan) {
        // 階段は1回見えたら消えない
        if (!pKaidan->isVisible()) {
            bool isContains = floorInfoIndexRect.containsPoint(pKaidan->getPosition());
            if (isRefresh || isContains) {
                pKaidan->setVisible(isContains);
                
                if (pBatchNode) {
                    auto pMiniKaidanNode = pBatchNode->getChildByTag(pKaidan->getTag());
                    if (pMiniKaidanNode) {
                        pMiniKaidanNode->setVisible(isContains);
                    }
                }
            }
        }
    }
    
    // 敵
    auto pEnemyMapLayer = this->getChildByTag(TiledMapTags::TiledMapEnemyBaseTag);
    if (pEnemyMapLayer) {
        for (auto pEnemyNode : pEnemyMapLayer->getChildren()) {
            bool isContains = floorInfoIndexRect.containsPoint(pEnemyNode->getPosition());
            if (isRefresh || isContains || enemyAllShow_) {
                // 地獄耳優先
                bool settingVisible = enemyAllShow_ ? true : isContains;
                
                pEnemyNode->setVisible(settingVisible);
                
                if (pBatchNode) {
                    auto pMiniEnemyNode = pBatchNode->getChildByTag(pEnemyNode->getTag());
                    if (pMiniEnemyNode) {
                        pMiniEnemyNode->setVisible(settingVisible);
                    }
                }
            }
        }
    }
}

Rect RogueTMXTiledMap::getTileMapFloorInfo(const MapIndex& actor_map_index) {
    
    for (auto& child : this->getChildren()) {
        TMXLayer* layer = dynamic_cast<TMXLayer*>(child);
        if(layer) {
            if(layer->getLayerName().compare("background") == 0 || layer->getLayerName().compare("colision") == 0) {
                continue;
            }
            
            MapIndex tiledIndex = mapIndexToTileIndex(actor_map_index);
            auto pTile = layer->getTileAt(Point(tiledIndex.x, tiledIndex.y));
            if (pTile) {
                MapIndex tileMapIndex = mapIndexToTileIndex(layer->getProperty("x").asInt(), layer->getProperty("y").asInt());
                // メタデータでyが上から設定されているので逆転している あとindexなので1足してる
                tileMapIndex.y = tileMapIndex.y - layer->getProperty("height").asInt() + 1;
                Point floorMaskPoint = indexToPoint(tileMapIndex);
                Size floorMaskSize = Size(layer->getProperty("width").asInt() * this->getTileSize().width, layer->getProperty("height").asInt() * this->getTileSize().height);
                CCLOG("x[%f] y[%f] w[%f] h[%f]", floorMaskPoint.x, floorMaskPoint.y, floorMaskSize.width, floorMaskSize.height);
                return Rect(floorMaskPoint.x - this->getTileSize().width / 2, floorMaskPoint.y - this->getTileSize().height / 2, floorMaskSize.width, floorMaskSize.height);
            }
        }
    }
    return Rect::ZERO;
}

void RogueTMXTiledMap::showFloorLighting(const MapIndex& actor_map_index, const Rect floorInfoIndexRect) {
    
    auto base_layer = this->getChildByTag(TiledMapTags::TiledMapFrontLayerTag);
    if (base_layer) {
        base_layer->setVisible(true);
        
        // フロントレイヤーにマップを暗くするやつを設定
        auto mask_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(TiledMapTags::FloorLayerTag));
        if (!mask_layer) {
            mask_layer = LayerColor::create(Color4B(0,0,0,128));
            mask_layer->setContentSize(this->getContentSize());
            mask_layer->setPosition(Point::ZERO);
            
            base_layer->addChild(mask_layer, TiledMapIndexs::FloorLayerZOrder, TiledMapTags::FloorLayerTag);
        }
        
        // 部屋の明かり
        auto floor_light_layer = dynamic_cast<LayerColor*>(base_layer->getChildByTag(TiledMapTags::FloorMaskLayerTag));
        if (!floor_light_layer) {
            floor_light_layer = LayerColor::create(Color4B(255,255,255,0));
            base_layer->addChild(floor_light_layer, TiledMapIndexs::FloorMaskLayerZOrder, TiledMapTags::FloorMaskLayerTag);
        }
        floor_light_layer->setContentSize(floorInfoIndexRect.size);
        floor_light_layer->setPosition(Point(floorInfoIndexRect.getMinX(), floorInfoIndexRect.getMinY()));
        
        BlendFunc blendFloor;
        blendFloor.src = GL_DST_COLOR;
        blendFloor.dst = GL_ONE;
        
        floor_light_layer->setBlendFunc(blendFloor);
        
        // プレイヤー周辺の明かり
        auto player_light_layer = dynamic_cast<LayerColor*>(floor_light_layer->getChildByTag(TiledMapTags::FloorMaskPlayerLayerTag));
        if (!player_light_layer) {
            player_light_layer = LayerColor::create(Color4B(255,255,255,0));
            floor_light_layer->addChild(player_light_layer, TiledMapIndexs::FloorMaskLayerZOrder, TiledMapTags::FloorMaskPlayerLayerTag);
        }
        
        // プレイヤー前方のみ
        Rect playerRect = createPlayerRect(actor_map_index, 1); // プレイヤー周辺3*3
        if (playerRect.getMinX() < floorInfoIndexRect.getMinX()) {
            // 左
            player_light_layer->setContentSize(Size(playerRect.size.width / 3, playerRect.size.height));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMinY() < floorInfoIndexRect.getMinY()) {
            // 下
            player_light_layer->setContentSize(Size(playerRect.size.width, playerRect.size.height / 3));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMaxX() > floorInfoIndexRect.getMaxX()) {
            // 右
            player_light_layer->setContentSize(Size(playerRect.size.width / 3, playerRect.size.height));
            player_light_layer->setPosition(Point(playerRect.getMaxX() - player_light_layer->getContentSize().width, playerRect.getMinY()) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else if (playerRect.getMaxY() > floorInfoIndexRect.getMaxY()) {
            // 上
            player_light_layer->setContentSize(Size(playerRect.size.width, playerRect.size.height / 3));
            player_light_layer->setPosition(Point(playerRect.getMinX(), playerRect.getMaxY() - player_light_layer->getContentSize().height) - floor_light_layer->getPosition());
            player_light_layer->setBlendFunc(blendFloor);
            player_light_layer->setVisible(true);
        } else {
            player_light_layer->setVisible(false);
        }
    }
}

void RogueTMXTiledMap::hideFloorLighting() {
    auto pFrontLayer = this->getChildByTag(TiledMapTags::TiledMapFrontLayerTag);
    if (pFrontLayer) {
        pFrontLayer->setVisible(false);
    }
}

Rect RogueTMXTiledMap::createPlayerRect(const MapIndex& actor_map_index, int rectSize) {
    MapIndex actorRectMinMapIndex = {
        actor_map_index.x - rectSize,
        actor_map_index.y - rectSize,
        MoveDirectionType::MOVE_NONE
    };
    Point actorRectMinMapPoint = indexToPointNotTileSize(actorRectMinMapIndex);
    // プレイヤー周辺のrectを作成
    return Rect(actorRectMinMapPoint.x, actorRectMinMapPoint.y,
                this->getTileSize().width * (rectSize * 2 + 1),
                this->getTileSize().width * (rectSize * 2 + 1));
}

void RogueTMXTiledMap::refreshAutoMapping(const Rect& floorInfoIndexRect) {
    
    MapIndex minMapIndex = pointToIndex(Point(floorInfoIndexRect.getMinX() + this->getTileSize().width / 2, floorInfoIndexRect.getMinY() + this->getTileSize().height / 2));
    MapIndex maxMapIndex = pointToIndex(Point(floorInfoIndexRect.getMaxX() + this->getTileSize().width / 2, floorInfoIndexRect.getMaxY()+ this->getTileSize().height / 2));
    
    auto pBatchNode = getGridSpriteBatchNode();
    auto mappingData = MapManager::getInstance()->getMappingData();
    for (int x = minMapIndex.x; x < maxMapIndex.x; x++) {
        for (int y = minMapIndex.y; y < maxMapIndex.y; y++) {
            MapIndex mapIndex = {x, y, MoveDirectionType::MOVE_NONE};
            auto tileMapIndex = mapIndexToTileIndex(mapIndex);
            // TODO: タグが強引すぎる・・・
            int tag = 10000 * tileMapIndex.x + 100 * tileMapIndex.y;
            auto pNode = pBatchNode->getChildByTag(tag);
            if (pNode) {
                MapManager::getInstance()->addMapping(tileMapIndex);
                pNode->setVisible(true);
            }
        }
    }
}

#pragma mark
#pragma mark その他

// getRandomMapIndexでさらに部屋だけが対象のランダムmapIndex取得
MapIndex RogueTMXTiledMap::getFloorRandomMapIndex(bool isActor) {
    return getRandomMapIndex(true, isActor, true);
}

// 壁とか障害物を避けてランダムなmapIndexを取得
MapIndex RogueTMXTiledMap::getRandomMapIndex(bool isColision, bool isActor) {
    return getRandomMapIndex(isColision, isActor, false);
}

// private
MapIndex RogueTMXTiledMap::getRandomMapIndex(bool isColision, bool isActor, bool isFloor) {
    MapIndex randMapIndex;
    int randX = 0;
    int randY = 0;
    while (true) {
        // 出現位置をランダムで決める
        randX = GetRandom(0, this->getMapSize().width-1); // x軸
        randY = GetRandom(0, this->getMapSize().height-1); // y軸
        randMapIndex = {randX, randY, MoveDirectionType::MOVE_NONE};
        
        // 壁以外
        if (!isColision) {
            if (isTiledMapColisionLayer(randMapIndex)) {
                // リトライ
                continue;
            }
        }
        
        // 部屋単位
        if (isFloor && !isFloorMapIndex(randMapIndex)) {
            continue;
        }
        
        // マップ上を確認
        if (isActor && !isActorInstMapIndex(randMapIndex)) {
            continue;
        } else if (!isDropItemInstMapIndex(randMapIndex)) {
            continue;
        }
        
        break;
    }
    return randMapIndex;
}

bool RogueTMXTiledMap::isFloorMapIndex(const MapIndex& mapIndex) {
    auto rect = getTileMapFloorInfo(mapIndex);
    if (!Rect::ZERO.equals(rect)) {
        return true;
    }
    return false;
}

bool RogueTMXTiledMap::isActorInstMapIndex(const MapIndex& mapIndex) {
    auto targetMapItem = MapManager::getInstance()->getMapItem(mapIndex);
    
    // アイテムの上もOK
    if (targetMapItem->mapDataType == MapDataType::NONE || targetMapItem->mapDataType == MapDataType::MAP_ITEM) {
        return true;
    }
    return false;
}

bool RogueTMXTiledMap::isDropItemInstMapIndex(const MapIndex& mapIndex) {
    auto targetMapItem = MapManager::getInstance()->getMapItem(mapIndex);
    
    if (targetMapItem->mapDataType == MapDataType::NONE) {
        return true;
    }
    return false;
}

bool RogueTMXTiledMap::isTiledMapColisionLayer(MapIndex touchPointMapIndex) {
    // 障害物判定
    auto pColisionLayer = this->getLayer("colision");
    // TileMapは左上から0,0なので座標変換する
    auto touchPointTileIndex = mapIndexToTileIndex(touchPointMapIndex);
    auto pTileSprite = pColisionLayer->getTileAt(Point(touchPointTileIndex.x, touchPointTileIndex.y));
    if (pTileSprite) {
        // 障害物なので移動とかできない
        CCLOG("colision touchPointTileIndex x = %d y = %d", touchPointTileIndex.x, touchPointTileIndex.y);
        return true;
    }
    
    return false;
}

bool RogueTMXTiledMap::isMapLayerOver(MapIndex touchPointMapIndex) {
    if (this->getMapSize().width <= touchPointMapIndex.x || this->getMapSize().height <= touchPointMapIndex.y ||
        0 > touchPointMapIndex.x || 0 > touchPointMapIndex.y) {
        return true;
    }
    return false;
}

DrawNode* RogueTMXTiledMap::createGridDraw() {
    auto draw = DrawNode::create();
    draw->setPosition(Point::ZERO);
    
    // 線の太さと色
    float lineSize = 1 * 0.5;
    auto color = Color4F(1, 1, 1, 0.5f);
    
    // 縦線を引く
    for (int x = 1; x < this->getMapSize().width; x++) {
        float xPoint = x * this->getTileSize().width;
        draw->drawSegment(Point(xPoint, 0), Point(xPoint, this->getContentSize().height), lineSize, color);
    }
    // 横線を引く
    for (int y = 1; y < this->getMapSize().height; y++) {
        float yPoint = y * this->getTileSize().height;
        draw->drawSegment(Point(0, yPoint), Point(this->getContentSize().width, yPoint), lineSize, color);
    }
    
    return draw;
}

#pragma mark
#pragma mark マップ座標変換

Point RogueTMXTiledMap::indexToPoint(MapIndex mapIndex) {
    return indexToPoint(mapIndex.x, mapIndex.y);
}
Point RogueTMXTiledMap::indexToPoint(int mapIndex_x, int mapIndex_y) {
    // タイルサイズを考慮
    return Point(this->getTileSize().width * mapIndex_x + this->getTileSize().width * 0.5, this->getTileSize().height * mapIndex_y + this->getTileSize().height * 0.5);
}
MapIndex RogueTMXTiledMap::pointToIndex(Point point) {
    // タイルサイズを考慮
    point.x = point.x - this->getTileSize().width * 0.5;
    point.y = point.y - this->getTileSize().height * 0.5;
    return touchPointToIndex(point);
}
MapIndex RogueTMXTiledMap::touchPointToIndex(Point point) {
    MapIndex mapIndex;
    mapIndex.x = point.x / this->getTileSize().width;
    mapIndex.y = point.y / this->getTileSize().height;
    return mapIndex;
}
Point RogueTMXTiledMap::indexToPointNotTileSize(MapIndex mapIndex) {
    return indexToPointNotTileSize(mapIndex.x, mapIndex.y);
}
Point RogueTMXTiledMap::indexToPointNotTileSize(int mapIndex_x, int mapIndex_y) {
    return Point(this->getTileSize().width * mapIndex_x, this->getTileSize().height * mapIndex_y);
}

MapIndex RogueTMXTiledMap::mapIndexToTileIndex(MapIndex mapIndex) {
    return mapIndexToTileIndex(mapIndex.x, mapIndex.y);
}
MapIndex RogueTMXTiledMap::mapIndexToTileIndex(int mapIndex_x, int mapIndex_y) {
    MapIndex tileIndex;
    tileIndex.x = mapIndex_x;
    tileIndex.y = this->getMapSize().height - mapIndex_y - 1;
    return tileIndex;
}

NS_ROGUE_END
