//
//  Octree.h
//  Octree
//
//  Created by JING on 18/05/2018.
//  Copyright © 2018 JING. All rights reserved.
//

#ifndef Octree_h
#define Octree_h

#include <string.h>

#endif /* Octree_h */

class Octree;

//the class for points
class Point
{
public:
    double x, y, z;        // position of points
    double n;              // used in moving trends
    unsigned int code;     // marks the result of classification
    ~Point(){}
    //initialize
    Point():x(0), y(0), z(0), n(0){}
    Point(double pointx, double pointy,double pointz)
    : x(pointx), y(pointy), z(pointz), n(0){}
    Point(double pointx, double pointy,double pointz, double num)
    : x(pointx), y(pointy), z(pointz), n(num){}
    // operator overloading
    Point operator+(Point& p)const{
        Point point;
        point.x = this->x + p.x;
        point.y = this->y + p.y;
        point.z = this->z + p.z;
        return point;
    }
    Point operator+(Point& p){
        Point point;
        point.x = this->x + p.x;
        point.y = this->y + p.y;
        point.z = this->z + p.z;
        return point;
    }
    Point operator+(const Point& p){
        Point point;
        point.x = this->x + p.x;
        point.y = this->y + p.y;
        point.z = this->z + p.z;
        return point;
    }
    Point operator-(Point& p){
        Point point;
        point.x = this->x - p.x;
        point.y = this->y - p.y;
        point.z = this->z - p.z;
        return point;
    }
    Point operator*(double d){
        Point point;
        point.x = this->x * d;
        point.y = this->y * d;
        point.z = this->z * d;
        return point;
    }
};

//defines a bounding volume
typedef struct{
    Point center;   // center
    double radius;  // radius
} Bounds;


// the class for octree
class Octree
{
public:
    // Construction/Destruction
    Octree();
    virtual ~Octree();
    
    inline const Point * const * points() const {return _points;}
    inline const unsigned int    pointCount() const {return _pointCount;}
    inline const Point           getCenter() const {return _center;}
    inline const double          getRadius() const {return _radius;}
    inline const Octree *        getChild(int i) const {return _child[i];}
    // build a new tree with designated points
    virtual const bool build(Point **points,
                             const unsigned int count,
                             const unsigned int threshold,
                             const unsigned int maximumDepth,
                             const Bounds &bounds,
                             const unsigned int currentDepth = 0);
    // calculate the bounding volume of a set of points
    static const Bounds calcCubicBounds(const Point * const * points, const unsigned int count);
    
protected:
    Octree *_child[8];
    unsigned int _pointCount;
    Point **_points;
    Point _center;
    double _radius;
};

//construction
Octree::Octree()
: _pointCount(0), _points(NULL), _center(0,0,0,0), _radius(0.0){
    memset(_child, 0, sizeof(_child));
}

//destruction
Octree::~Octree(){
    delete[] _points;
}

//construct a new tree from points
const bool Octree::build(Point **points,
                         const unsigned int count,
                         const unsigned int threshold,
                         const unsigned int maximumDepth,
                         const Bounds &bounds,
                         const unsigned int currentDepth)
{
    if (count <= threshold || currentDepth >= maximumDepth){
//        store the point as a leaf
        _pointCount = count;
        _points = new Point *[count];
        memcpy(_points, points, sizeof(Point *) * count);
        memcpy(&_center, points[0], sizeof(Point ) * count);
        return true;
    }
    
    unsigned int childPointCounts[8]={0};
    _pointCount=count;
    _center=bounds.center;
    _radius=bounds.radius;
    // classify points to a subtree
    for (unsigned int i = 0; i < count; i++){
        Point   &p = *points[i];
        const Point &c = bounds.center;//center
        p.code = 0;
        if (p.x > c.x) p.code |= 1;
        if (p.y > c.y) p.code |= 2;
        if (p.z > c.z) p.code |= 4;
        childPointCounts[p.code]++;
    }
    
    // to calculate the offset of subtree
    Point boundsOffsetTable[8] ={
        {-0.5, -0.5, -0.5},
        {+0.5, -0.5, -0.5},
        {-0.5, +0.5, -0.5},
        {+0.5, +0.5, -0.5},
        {-0.5, -0.5, +0.5},
        {+0.5, -0.5, +0.5},
        {-0.5, +0.5, +0.5},
        {+0.5, +0.5, +0.5}
    };
    // build a subtree for each class of nodes
    for (unsigned int i = 0; i < 8; i++){
        if (!childPointCounts[i]) continue;
        _child[i] = new Octree;
        Point   **newList = new Point *[childPointCounts[i]];
        Point   **ptr = newList;
        // add points to the subtree
        for (unsigned int j = 0; j < count; j++){
            if (points[j]->code == i){
                *ptr = points[j];
                ptr++;
            }
        }
        int newCount = 0;
        //count the number of points in the subtree
        for (unsigned int j = 0; j < childPointCounts[i]; j++){
            newCount++;
        }
        //bounding volume of the subtree
        Point offset = boundsOffsetTable[i] * bounds.radius * 0.5;
        Bounds newBounds;
        newBounds.radius = bounds.radius * 0.5;
        newBounds.center = bounds.center + offset;
        
        // build subtree recursively
        _child[i]->build(newList, newCount, threshold, maximumDepth,
                         newBounds, currentDepth+1);
        
        // clear
//        delete[] newList;
    }
    return true;
}

// calculate the bounding volume of a set of points
const Bounds Octree::calcCubicBounds(const Point * const * points,
                                     const unsigned int count){
    Bounds b;
    Point min = *points[0];
    Point max = *points[0];
    
    for (unsigned int i = 1; i < count; i++){
        const Point &p = *points[i];
        if (p.x < min.x) min.x = p.x;
        if (p.y < min.y) min.y = p.y;
        if (p.z < min.z) min.z = p.z;
        if (p.x > max.x) max.x = p.x;
        if (p.y > max.y) max.y = p.y;
        if (p.z > max.z) max.z = p.z;
    }
    
    // The radius of the cube
    Point radius = max - min;
    b.radius = radius.x;
    if (b.radius < radius.y) b.radius = radius.y;
    if (b.radius < radius.z) b.radius = radius.z;
    
    // the center of the cube
    b.center = min + radius * 0.5;
    
    return b;
}
