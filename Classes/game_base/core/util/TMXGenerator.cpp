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
                for (int i = 0; i < (MAP_SIZE_WIDTH * MAP_SIZE_HEIGHT); i++) {
                    dataNode.append_child("tile").append_attribute("gid").set_value(84);
                }
            }
            
            // 壁
            auto colisionLayerNode = mapNode.append_child("layer");
            colisionLayerNode.append_attribute("name").set_value("colision");
            colisionLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
            colisionLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
            {
                auto dataNode = colisionLayerNode.append_child("data");
                for (int x = 0; x < MAP_SIZE_WIDTH; x++) {
                    for (int y = 0; y < MAP_SIZE_HEIGHT; y++) {
                        if (x == 0 || x == (MAP_SIZE_WIDTH-1) || y == 0 || y == (MAP_SIZE_HEIGHT-1)) {
                            dataNode.append_child("tile").append_attribute("gid").set_value(1);
                        } else {
                            dataNode.append_child("tile").append_attribute("gid").set_value(0);
                        }
                    }
                }
            }
            
            // 部屋
            const int FLOOR_COUNT = 6;
            for (int i = 0; i <= FLOOR_COUNT; i++) {
                std::string nodeName = cocos2d::StringUtils::format("floor_%d", (i+1));
                auto floorLayerNode = mapNode.append_child("layer");
                floorLayerNode.append_attribute("name").set_value(nodeName.c_str());
                floorLayerNode.append_attribute("width").set_value(MAP_SIZE_WIDTH);
                floorLayerNode.append_attribute("height").set_value(MAP_SIZE_HEIGHT);
                {
                    auto propertiesNode = floorLayerNode.append_child("properties");
                    {
                        auto property1 = propertiesNode.append_child("property");
                        property1.append_attribute("name").set_value("height");
//                        property1.append_attribute("value").set_value(7);
                        property1.append_attribute("value").set_value(MAP_SIZE_HEIGHT);
                        
                        auto property2 = propertiesNode.append_child("property");
                        property2.append_attribute("name").set_value("no");
                        property2.append_attribute("value").set_value((i+1));
                        
                        auto property3 = propertiesNode.append_child("property");
                        property3.append_attribute("name").set_value("width");
//                        property3.append_attribute("value").set_value(7);
                        property3.append_attribute("value").set_value(MAP_SIZE_WIDTH);
                        
                        auto property4 = propertiesNode.append_child("property");
                        property4.append_attribute("name").set_value("x");
//                        property4.append_attribute("value").set_value(33);
                        property4.append_attribute("value").set_value(0);
                        
                        auto property5 = propertiesNode.append_child("property");
                        property5.append_attribute("name").set_value("y");
//                        property5.append_attribute("value").set_value(13);
                        property5.append_attribute("value").set_value(0);
                    }
                    
                    auto dataNode = floorLayerNode.append_child("data");
                    for (int i = 0; i < (MAP_SIZE_WIDTH * MAP_SIZE_HEIGHT); i++) {
                        dataNode.append_child("tile").append_attribute("gid").set_value(84);
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