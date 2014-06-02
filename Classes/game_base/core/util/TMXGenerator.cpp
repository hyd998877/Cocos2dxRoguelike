//
//  TMXGenerator.cpp
//  Cocos2dRogueLike
//
//  Created by kyokomi on 2014/05/11.
//
//

#include "TMXGenerator.h"

#include "cocos2d.h"

#include "pugixml.hpp"

#include <unordered_map>

USING_NS_CC;

class TMXLayerData
{
public:
    struct MapIndex {
        int _x;
        int _y;
    };
    int _no;
    int _width;
    int _height;
    int _x;
    int _y;
    std::list<MapIndex> gateWayMapIndexList;
    
    bool isGateWay(int x, int y) const {
        for (auto mapIndex : gateWayMapIndexList) {
            if (x == mapIndex._x && y == mapIndex._y) {
                return true;
            }
        }
        return false;
    }
    
    bool isKado(int x, int y) const {
        if ((x == _x || maxX() == x) && (y == _y || maxY() == y)) {
            return true;
        }
        return false;
    }
    
    bool isLeft(int x, int y) const {
        if (x == _x && ((y >= _y && maxY() > y))) {
            return true;
        }
        return false;
    }
    
    bool isRight(int x, int y) const {
        if (maxX() == x && ((y >= _y && maxY() > y))) {
            return true;
        }
        return false;
    }
    
    bool isTop(int x, int y) const {
        if ((x >= _x && maxX() > x) && y == _y) {
            return true;
        }
        return false;
    }
    
    bool isBottom(int x, int y) const {
        if ((x >= _x && maxX() > x) && maxY() == y) {
            return true;
        }
        return false;
    }
    
    int maxX() const {
        return (_x + _width-1);
    }
    int maxY() const {
        return (_y + _height-1);
    }
    
    bool isRect(int x, int y) const {
        if (x >= _x && maxX() >= x && y >= _y && maxY() >= y) {
            return true;
        }
        return false;
    }
    
    static const int NonTileId = 0;
    static const int ClearTileId = 1;
    static const int BackgroundTileId = 84;
    static const int FloorTileId = 80;
   
    static int checkFloorTileId(const TMXLayerData& layerData, int x, int y) {
        if (layerData.isRect(x, y)) {
            return TMXLayerData::FloorTileId;
        } else {
            return TMXLayerData::NonTileId;
        }
    }
    static int checkBackgroundTileId(const std::list<TMXLayerData>& floorLayerList, int x, int y) {
        for (TMXLayerData layerData : floorLayerList) {
            if (layerData.isRect(x, y)) {
                return TMXLayerData::BackgroundTileId;
            }
        }
        return TMXLayerData::ClearTileId;
    }
    
    static int checkTileId(const std::list<TMXLayerData>& floorLayerList, int x, int y) {
        for (TMXLayerData layerData : floorLayerList) {
            // 壁判定
            if (layerData.isGateWay(x, y)) {
                return TMXLayerData::NonTileId;
            } else if (layerData.isKado(x, y)) {
                return 178;
            } else if (layerData.isRight(x, y)) {
                return 177;
            } else if (layerData.isLeft(x, y)) {
                return 179;
            } else if (layerData.isTop(x, y)) {
                return 194;
            } else if (layerData.isBottom(x, y)) {
                return 162;
            } else if (layerData.isRect(x, y)) {
                return TMXLayerData::NonTileId;
            }
        }
        return TMXLayerData::ClearTileId;
    }
};

TMXGenerator::TMXGenerator()
{
    
}

TMXGenerator::~TMXGenerator()
{
    
}


std::string TMXGenerator::generator()
{
    /*
     <?xml version="1.0" encoding="UTF-8"?>
     <map version="1.0" orientation="orthogonal" width="40" height="20" tilewidth="32" tileheight="32">
        <tileset firstgid="1" name="tile_e" tilewidth="32" tileheight="32">
            <image source="tile_e.png" width="512" height="512"/>
        </tileset>
        <layer name="background" width="40" height="20">
            <data>
                <tile gid="0"/>
            </data>
        </layer>
     </map>
     */
    
    const int MAP_SIZE_WIDTH = 40;
    const int MAP_SIZE_HEIGHT = 20;
    const int TILE_SIZE = 32;
    
    std::vector<std::vector<int>> tileIdArray(MAP_SIZE_WIDTH);
    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
        tileIdArray[x] = std::vector<int>(MAP_SIZE_HEIGHT);
        for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
            tileIdArray[x][y] = 0;
        }
    }
    for (int y = 5+1; y < 10; y++) {
        tileIdArray[3][y] = 84;
    }
    for (int x = 5+1; x < 10; x++) {
        tileIdArray[x][12] = 84;
    }
    for (int x = 14+1; x < 20; x++) {
        tileIdArray[x][12] = 84;
    }
    for (int y = 5+1; y < 10; y++) {
        tileIdArray[22][y] = 84;
    }
    for (int x = 5+1; x < 10; x++) {
        tileIdArray[x][3] = 84;
    }
    
    // 左、上、下、右
    std::list<TMXLayerData> floorLayerList{
        {1, 5, 5,  1, 1, std::list<TMXLayerData::MapIndex>{ // OK
                              { 3, 5}, { 5, 3}
        }},
        {2, 5, 5, 10, 1, std::list<TMXLayerData::MapIndex>{ // OK
            {10, 3},          {12, 5}, {14, 3}
        }},
        {3, 5, 5, 20, 1, std::list<TMXLayerData::MapIndex>{ // OK
            {20, 3},          {22, 5}
        }},
        {4, 5, 5,  1, 10, std::list<TMXLayerData::MapIndex>{ // OK
                     { 3,10},          { 5,12}
        }},
        {5, 5, 5, 10, 10, std::list<TMXLayerData::MapIndex>{ // OK
            {10,12}, {12,10},          {14,12}
        }},
        {6, 5, 5, 20, 10, std::list<TMXLayerData::MapIndex>{ // OK
            {20,12}, {22,10}
        }},
//        {1,  7,  7,   1,  1},
//        {2,  7,  7,  13,  5},
//        {3,  7,  8,  25,  1},
//        {4, 11, 10,   1,  9},
//        {5,  7,  5,  21, 15},
//        {6,  7,  7,  33, 13},
    };
    
    pugi::xml_document doc;
    auto mapNode = doc.append_child("map");
    {
        mapNode.append_attribute("version").set_value("1.0");
        mapNode.append_attribute("orientation").set_value("orthogonal");
        mapNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
        mapNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
        mapNode.append_attribute("tilewidth").set_value(TILE_SIZE);
        mapNode.append_attribute("tileheight").set_value(TILE_SIZE);
        {
            auto tileSetNode = mapNode.append_child("tileset");
            tileSetNode.append_attribute("firstgid").set_value(1);
            tileSetNode.append_attribute("name").set_value("tile_e");
            tileSetNode.append_attribute("tilewidth").set_value(TILE_SIZE);
            tileSetNode.append_attribute("tileheight").set_value(TILE_SIZE);
            
            auto imageNode = mapNode.append_child("image");
            imageNode.append_attribute("source").set_value("tile_e.png");
            imageNode.append_attribute("width").set_value(512);
            imageNode.append_attribute("height").set_value(512);
        
            //////////////
            
            // 床
            auto backgroundLayerNode = mapNode.append_child("layer");
            backgroundLayerNode.append_attribute("name").set_value("background");
            backgroundLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
            backgroundLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
            {
                auto dataNode = backgroundLayerNode.append_child("data");
                for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
                        if (tileIdArray[x][y] != 0) {
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileIdArray[x][y]);
                        } else {
                            auto tileId = TMXLayerData::checkBackgroundTileId(floorLayerList, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
            
            // 壁
            auto colisionLayerNode = mapNode.append_child("layer");
            colisionLayerNode.append_attribute("name").set_value("colision");
            colisionLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
            colisionLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
            {
                auto dataNode = colisionLayerNode.append_child("data");
                for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                    for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
                        if (tileIdArray[x][y] != 0) {
                            dataNode.append_child("tile").append_attribute("gid").set_value(TMXLayerData::NonTileId);
                        } else {
                            int tileId = TMXLayerData::checkTileId(floorLayerList, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
            
            // 部屋
            for (TMXLayerData layerData : floorLayerList) {
                std::string nodeName = cocos2d::StringUtils::format("floor_%d", layerData._no);
                auto floorLayerNode = mapNode.append_child("layer");
                floorLayerNode.append_attribute("name").set_value(nodeName.c_str());
                floorLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
                floorLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
                {
                    auto propertiesNode = floorLayerNode.append_child("properties");
                    {
                        auto property2 = propertiesNode.append_child("property");
                        property2.append_attribute("name").set_value("no");
                        property2.append_attribute("value").set_value(layerData._no);
                        
                        auto property3 = propertiesNode.append_child("property");
                        property3.append_attribute("name").set_value("width");
                        property3.append_attribute("value").set_value(layerData._width);
                        
                        auto property1 = propertiesNode.append_child("property");
                        property1.append_attribute("name").set_value("height");
                        property1.append_attribute("value").set_value(layerData._height);
                        
                        auto property4 = propertiesNode.append_child("property");
                        property4.append_attribute("name").set_value("x");
                        property4.append_attribute("value").set_value(layerData._x);
                        
                        auto property5 = propertiesNode.append_child("property");
                        property5.append_attribute("name").set_value("y");
                        property5.append_attribute("value").set_value(layerData._y);
                    }
                    
                    auto dataNode = floorLayerNode.append_child("data");
                    for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                        for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
                            int tileId = TMXLayerData::checkFloorTileId(layerData, x, y);
                            dataNode.append_child("tile").append_attribute("gid").set_value(tileId);
                        }
                    }
                }
            }
        }
    }
    std::string tempFilePath = FileUtils::getInstance()->getWritablePath() + "tmp_quest.tmx";
    doc.save_file(tempFilePath.c_str());
    auto tmxString = FileUtils::getInstance()->getStringFromFile(tempFilePath);
    return tmxString;
}