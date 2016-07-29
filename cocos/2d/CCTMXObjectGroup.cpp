/****************************************************************************
Copyright (c) 2010      Neophit
Copyright (c) 2010      Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
#include "2d/CCTMXObjectGroup.h"
#include "base/ccMacros.h"
#include "2d/CCTMXXMLParser.h"
#include "base/CCDirector.h"
#include "renderer/CCTextureCache.h"
#include "2d/CCTMXTiledMap.h"
#include "base/ccTypes.h"

NS_CC_BEGIN

//implementation TMXObject
TMXObject::TMXObject(ValueMap objectInfo)
{
    _objectName = objectInfo["name"].asString();
    _type = (TMXObjectType) objectInfo["type"].asInt();
    _gid = objectInfo["gid"].isNull() ? 0 : objectInfo["gid"].asInt();
    _id = objectInfo["id"].asInt();
    _offset = Vec2(objectInfo["x"].asFloat(), objectInfo["y"].asFloat());
    _objectSize = Size(objectInfo["width"].asFloat(), objectInfo["height"].asFloat());
    _objectVisible = objectInfo["visible"].asBool();
    _objectRotation = objectInfo["rotation"].asFloat();
    setProperties(objectInfo);
}

TMXObject::~TMXObject()
{
    CCLOGINFO("deallocing TMXObject: %p", this);
}

Value TMXObject::getProperty(const std::string& propertyName) const
{
    if (_properties.find(propertyName) != _properties.end())
        return _properties.at(propertyName);
    
    return Value();
}

//implementation TMXObjectImage
TMXObjectImage::TMXObjectImage(const ValueMap& objectInfo, TMXMapInfo* mapInfo)
: TMXObject(objectInfo)
{
    _initWithMapInfo(mapInfo);
}

TMXObjectImage::~TMXObjectImage()
{
    CCLOGINFO("deallocing TMXObjectImage: %p", this);
}

bool TMXObjectImage::_initWithMapInfo(TMXMapInfo* mapInfo)
{
    if (_gid == 0) {
        return false;
    }
    
    TMXTilesetInfo* useTileset = nullptr;
    auto& tilesets = mapInfo->getTilesets();
    if (tilesets.size()>0)
    {
        TMXTilesetInfo* tileset = nullptr;
        for (auto iter = tilesets.crbegin(); iter != tilesets.crend(); ++iter)
        {
            tileset = *iter;
            if( (_gid & kTMXFlippedMask) >= tileset->_firstGid ) {
                useTileset = tileset;
                break;
            }
        }
    }
    
    if (useTileset == nullptr) {
        return false;
    }
    
    setVisible(_objectVisible);
    auto texture = Director::getInstance()->getTextureCache()->addImage(useTileset->_sourceImage);
    if (texture == nullptr)
    {
        return false;
    }
    auto rect = useTileset->getRectForGID(_gid);
    initWithTexture(texture, CC_RECT_PIXELS_TO_POINTS(rect));
    setScale(_objectSize.width / rect.size.width, _objectSize.height / rect.size.height);
    setRotation(_objectRotation);

    setFlippedX(false);
    if (_gid & kTMXTileHorizontalFlag) {
        setFlippedX(true);
    }
    
    setFlippedY(false);
    if (_gid & kTMXTileVerticalFlag) {
        setFlippedY(true);
    }
    
    _initPosWithMapInfo(mapInfo);

    return true;
}

void TMXObjectImage::_initPosWithMapInfo(TMXMapInfo* mapInfo)
{
    auto mapOri = mapInfo->getOrientation();
    auto mapSize = mapInfo->getMapSize();
    auto tileSize = mapInfo->getTileSize();
    switch (mapOri) {
        case TMXOrientationOrtho:
            setAnchorPoint(Vec2(0, 0));
            setPosition(CC_POINT_PIXELS_TO_POINTS(Vec2(_offset.x, mapSize.height * tileSize.height - _offset.y)));
            break;
        case TMXOrientationIso:
            {
                setAnchorPoint(Vec2(0.5, 0));
                auto posIdx = Vec2(_offset.x / tileSize.width * 2, _offset.y / tileSize.height);
                auto pos = Vec2(tileSize.width / 2 * (mapSize.width + posIdx.x - posIdx.y),
                                tileSize.height / 2 * (mapSize.height * 2 - posIdx.x - posIdx.y));
                setPosition(CC_POINT_PIXELS_TO_POINTS(pos));
            }
        case TMXOrientationHex:
            {
                setAnchorPoint(Vec2(0,0));
                auto x = _offset.x;
                float y = 0;
                if (mapInfo->getStaggerAxis() == TMXStaggerAxis_X) {
                    y = tileSize.height * (mapSize.height + 0.5) - _offset.y;
                }
                else if (mapInfo->getStaggerAxis() == TMXStaggerAxis_Y) {
                    y = (tileSize.height + mapInfo->getHexSideLength()) * (int)(mapSize.height / 2) + tileSize.height * ((int)mapSize.height % 2) - _offset.y;
                }
                setPosition(CC_POINT_PIXELS_TO_POINTS(Vec2(x, y)));
            }
        default:
            break;
    }
}

//implementation TMXObjectShape
TMXObjectShape::TMXObjectShape(const ValueMap& objectInfo, TMXMapInfo* mapInfo, const Size& groupSize, const Color3B& color)
: TMXObject(objectInfo)
, DrawNode(1)
{
    _mapOrientation = mapInfo->getOrientation();
    _groupSize = groupSize;
    _groupColor = Color4F((float) color.r / 255, (float) color.g / 255, (float) color.b / 255, 1.0);
    _initShape(objectInfo, mapInfo);
}

TMXObjectShape::~TMXObjectShape() {
    CCLOGINFO("deallocing TMXObjectShape: %p", this);
}

void TMXObjectShape::_initShape(const ValueMap& objectInfo, TMXMapInfo* mapInfo)
{
    DrawNode::init();
    
    Vec2 originPos;
    if (TMXOrientationHex == _mapOrientation || TMXOrientationOrtho == _mapOrientation) {
        Vec2 startPos = Vec2(0, _groupSize.height);
        Vec2 offsetInPoints = CC_POINT_PIXELS_TO_POINTS(_offset);
        originPos = Vec2(startPos.x + offsetInPoints.x, startPos.y - offsetInPoints.y);
    } else {
        originPos = Vec2(0, 0);
    }
    setPosition(originPos);
    setRotation(_objectRotation);

    switch (_type) {
        case TMXObjectTypeRect:
            _drawRect(originPos);
            break;
        case TMXObjectTypeEllipse:
            _drawEllipse(originPos);
            break;
        case TMXObjectTypePolygon:
            _drawPoly(objectInfo, originPos, true);
            break;
        case TMXObjectTypePolyline:
            _drawPoly(objectInfo, originPos, false);
            break;
        default:
            break;
    }
    setVisible(_objectVisible);
}

void TMXObjectShape::_drawRect(const Vec2& originPos)
{
    if (TMXOrientationHex == _mapOrientation || TMXOrientationOrtho == _mapOrientation) {
        Size objSizeInPoints = CC_SIZE_PIXELS_TO_POINTS(_objectSize);
        Vec2 bl = Vec2(0, 0);
        Vec2 tr = Vec2(objSizeInPoints.width, objSizeInPoints.height);
        drawRect(bl, tr, _groupColor);
        
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(_objectSize));
        setAnchorPoint(Vec2(0, 1));
    } else {
        
    }
}

void TMXObjectShape::_drawEllipse(const Vec2& originPos)
{
    if (TMXOrientationHex == _mapOrientation || TMXOrientationOrtho == _mapOrientation) {
        Size objSizeInPoints = CC_SIZE_PIXELS_TO_POINTS(_objectSize);
        
        float scaleX = 1.0, scaleY = 1.0, radius = 0.0;
        Vec2 center = Vec2(objSizeInPoints.width / 2, objSizeInPoints.height / 2);
        if (objSizeInPoints.width > objSizeInPoints.height) {
            scaleX = objSizeInPoints.width / objSizeInPoints.height;
            radius = objSizeInPoints.height / 2;
        } else {
            scaleY = objSizeInPoints.height / objSizeInPoints.width;
            radius = objSizeInPoints.width / 2;
        }
        drawCircle(center, radius, 0, 50, false, scaleX, scaleY, _groupColor);
        
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(_objectSize));
        setAnchorPoint(Vec2(0, 1));
    } else {
        
    }
}

void TMXObjectShape::_drawPoly(const ValueMap& objectInfo, const Vec2& originPos, bool isPolygon)
{
    if (TMXOrientationHex == _mapOrientation || TMXOrientationOrtho == _mapOrientation) {
        ValueVector pointsData;
        if (isPolygon)
            pointsData = objectInfo.at("points").asValueVector();
        else
            pointsData = objectInfo.at("polylinePoints").asValueVector();
        
        Vec2 * points = new Vec2[pointsData.size()];
        int idx = 0;
        float minX = 0, minY = 0, maxX = 0, maxY = 0;
        for (auto& pointData : pointsData) {
            auto pointDict = pointData.asValueMap();
            int x = pointDict["x"].asInt();
            int y = pointDict["y"].asInt();
            Vec2 tempPos = CC_POINT_PIXELS_TO_POINTS(Vec2(x, y));
            points[idx] = Vec2(tempPos.x, -tempPos.y);
            minX = MIN(minX, tempPos.x);
            minY = MIN(minY, tempPos.y);
            maxX = MAX(maxX, tempPos.x);
            maxY = MAX(maxY, tempPos.y);
            idx++;
        }

        // set the content size & anchor point
        float width = maxX - minX, height = maxY - minY;
        setContentSize(Size(width, height));
        setAnchorPoint(Vec2(-minX / width, maxY / height));

        // correct the points data
        for (int i = 0; i < idx; i++) {
            points[i] = Vec2(points[i].x - minX, points[i].y + maxY);
        }
        drawPoly(points, idx, isPolygon, _groupColor);
        delete [] points;
    } else {
        
    }
}

//implementation TMXObjectGroup

TMXObjectGroup::TMXObjectGroup(TMXObjectGroupInfo* groupInfo, TMXMapInfo* mapInfo)
    : _groupName("")
{
    _initGroup(groupInfo, mapInfo);
}

TMXObjectGroup::~TMXObjectGroup()
{
    CCLOGINFO("deallocing TMXObjectGroup: %p", this);
}

TMXObject* TMXObjectGroup::getObject(const std::string& objectName)
{
    CCASSERT(objectName.size() > 0, "Invalid object name!");
    
    for (auto& child : _children)
    {
        TMXObject* object = dynamic_cast<TMXObject*>(child);
        if(object)
        {
            if(objectName.compare( object->getObjectName()) == 0)
            {
                return object;
            }
        }
    }
    
    // object not found
    return nullptr;
}

Vector<TMXObject*> TMXObjectGroup::getObjects()
{
    Vector<TMXObject*> objects;
    for (auto& child : _children)
    {
        TMXObject* object = dynamic_cast<TMXObject*>(child);
        if(object)
        {
            objects.pushBack(object);
        }
    }

    return objects;
}

Value TMXObjectGroup::getProperty(const std::string& propertyName) const
{
    if (_properties.find(propertyName) != _properties.end())
        return _properties.at(propertyName);

    return Value();
}

void TMXObjectGroup::_initGroup(TMXObjectGroupInfo* groupInfo, TMXMapInfo* mapInfo)
{
    _groupName = groupInfo->_groupName;
    _positionOffset = groupInfo->_positionOffset;
    _properties = groupInfo->getProperties();
    auto mapSize = mapInfo->getMapSize();
    auto tileSize = mapInfo->getTileSize();
    if (mapInfo->getOrientation() == TMXOrientationHex) {
        float width, height = 0;
        if (mapInfo->getStaggerAxis() == TMXStaggerAxis_X) {
            height = tileSize.height * (mapSize.height + 0.5);
            width = (tileSize.width + mapInfo->getHexSideLength()) * (int)(mapSize.width / 2) + tileSize.width * ((int)mapSize.width % 2);
        } else {
            width = tileSize.width * (mapSize.width + 0.5);
            height = (tileSize.height + mapInfo->getHexSideLength()) * (int)(mapSize.height / 2) + tileSize.height * ((int)mapSize.height % 2);
        }
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(width, height)));
    } else {
        setContentSize(CC_SIZE_PIXELS_TO_POINTS(Size(mapSize.width * tileSize.width, mapSize.height * tileSize.height)));
    }
    setAnchorPoint(Vec2(0, 0));
    setPosition(CC_POINT_PIXELS_TO_POINTS(Vec2(_positionOffset.x, -_positionOffset.y)));
    setVisible(groupInfo->_visible);
    
    // create objects children
    auto objects = groupInfo->_objects;
    int idx = 0;
    for (auto& object : objects) {
        auto objectInfo = object.asValueMap();
        Node* obj = nullptr;
        if (objectInfo["type"].asInt() == TMXObjectTypeImage) {
            obj = new TMXObjectImage(objectInfo, mapInfo);
        } else {
            obj = new TMXObjectShape(objectInfo, mapInfo, getContentSize(), groupInfo->_color);
        }
        obj->autorelease();
        obj->setOpacity(groupInfo->_opacity);
        addChild(obj, idx, idx);
        idx++;
    }
}

NS_CC_END
