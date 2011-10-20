/*  
 *  Copyright 2010 Anders Wallin (anders.e.e.wallin "at" gmail.com)
 *  
 *  This file is part of OpenCAMlib.
 *
 *  OpenCAMlib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  OpenCAMlib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with OpenCAMlib.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <cassert>
#include <cmath>


#include "volume.hpp"

namespace cutsim {



//************* Sphere **************/

/// sphere at center
SphereVolume::SphereVolume() {
    center = GLVertex(0,0,0);
    radius = 1.0;
    calcBB();
}

double SphereVolume::dist(const GLVertex& p ) const {
    double d = (center-p).norm();
    return radius-d; // positive inside. negative outside.
}

/// set the bounding box values
void SphereVolume::calcBB() {
    bb.clear();
    GLVertex maxpt = GLVertex(center.x + radius, center.y + radius, center.z + radius);
    GLVertex minpt = GLVertex(center.x - radius, center.y - radius, center.z - radius);
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

//************* Rectangle **************/

RectVolume::RectVolume() {
    corner = GLVertex(0,0,0); 
    v1 = GLVertex(1,0,0); 
    v2 = GLVertex(0,1,0);
    v3 = GLVertex(0,0,1);
}

void RectVolume::calcBB() {
    bb.clear();
    GLVertex maxp;
    GLVertex minp;
    double bignum = 1e6;
    maxp = GLVertex(bignum,bignum,bignum);
    minp = GLVertex( -bignum,-bignum,-bignum);
    bb.addPoint( maxp  );
    bb.addPoint( minp );
}

double RectVolume::dist(const GLVertex& p) const {
    // translate to origo
    double max_x = corner.x + v1.x;
    double min_x = corner.x;
    double max_y = corner.y + v2.y;
    double min_y = corner.y;
    double max_z = corner.z + v3.z;
    double min_z = corner.z;
    double dOut = 0.0;

    if ( (min_x <= p.x) && (p.x <= max_x) && (min_y <= p.y) && (p.y <= max_y) && (min_z <= p.z) && (p.z <= max_z) )   {
        double xdist,ydist,zdist;
        if ( (p.x-min_x) > (max_x-p.x) )
            xdist = max_x-p.x;                    
        else
            xdist = p.x-min_x;

        if ( (p.y-min_y) > (max_y-p.y) )
            ydist = max_y-p.y;                    
        else
            ydist = p.y-min_y;
        
        if ( (p.z-min_z) > (max_z-p.z) )
            zdist = max_z-p.z;                    
        else
            zdist = p.z-min_z;

        if ( xdist <= ydist && xdist <= zdist )
            dOut = -xdist;
        else if ( ydist < xdist && ydist < zdist )
            dOut = -ydist;
        else if ( zdist < xdist && zdist< xdist )
            dOut = -zdist;
        else {
            assert(0);
            return -1;
        }
    }else if ( (min_y <= p.y) && (p.y <= max_y) && (min_z <= p.z) && (p.z <= max_z) )   {
        if (p.x < min_x) {
            dOut = min_x - p.x;
        } else if ( p.x > max_x ) {
            dOut = p.x-max_x;
        }
    }else if ( (min_x <= p.x) && (p.x <= max_x) && (min_z <= p.z) && (p.z <= max_z) )   {
        if (p.y < min_y) {
            dOut = min_y - p.y;
        } else if ( p.y > max_y ) {
            dOut = p.y-max_y;
        }
    }else if ( (min_x <= p.x) && (p.x <= max_x) && (min_y <= p.y) && (p.y <= max_y) )   {
        if (p.z < min_z) {
            dOut = min_z - p.z;
        }else if ( p.z > max_z ) {
            dOut = p.z-max_z;
        }
    }else if ( (p.x > max_x) && (p.y > max_y))
        dOut = sqrt((p.x - max_x)*(p.x - max_x)+(p.y - max_y)*(p.y - max_y));
    else if ( (p.x > max_x) && (p.z < min_z))
        dOut = sqrt((p.x - max_x)*(p.x - max_x)+(min_z - p.z)*(min_z - p.z));
    else if ( (p.x < min_x) && (p.y > max_y))
        dOut = sqrt((min_x - p.x)*(min_x - p.x)+(p.y - max_y)*(p.y - max_y));
    else if ( (p.y > max_y) && (p.z > max_z))
        dOut = sqrt((p.y - max_y)*(p.y - max_y)+(p.z - max_z)*(p.z - max_z));
    else if ( (p.x > max_x) && (p.z > max_z))
        dOut = sqrt((p.x - max_x)*(p.x - max_x)+(p.z - max_z)*(p.z - max_z));
    else if ( (p.x > max_x) && (p.y < min_y))
        dOut = sqrt((p.x - max_x)*(p.x - max_x)+(min_y - p.y)*(min_y - p.y));
    else if ( (p.x < min_x) && (p.y < min_y))
        dOut = sqrt((min_x - p.x)*(min_x - p.x)+(p.y - max_y)*(p.y - max_y));
    else if ( (p.y < min_y) && (p.z > max_z))
        dOut = sqrt((min_y - p.y)*(min_y - p.y)+(p.z - max_z)*(p.z - max_z));
    else if ( (p.x < min_x) && (p.z < min_z) )
        dOut = sqrt((p.x - max_x)*(p.x - max_x)+(min_z - p.z)*(min_z - p.z));
    else if ( (p.y > max_y) && (p.z < min_z))
        dOut = sqrt((p.y - max_y)*(p.y - max_y)+(min_y - p.y)*(min_y - p.y));
    else if ( (p.x < min_x) && (p.z > max_z))
        dOut = sqrt((min_x - p.x)*(min_x - p.x)+(p.z - max_z)*(p.z - max_z));
    else if ( (p.y < min_y) && (p.z < min_z))
        dOut = sqrt((min_y - p.y)*(min_y - p.y)+(min_z - p.z)*(min_z - p.z));
        
    return -dOut;
}
/*
bool SphereVolume::isInside(GLVertex& p) const {
    std::cout << " isInside !!! \n";
    if (!isInsideBB(p))
        return false;
    
    if ( (center-p).norm() <= radius ) 
        return true;
    else
        return false;
}*/


//************* Cube **************/

/*
/// cube at center with side length side
CubeVolume::CubeVolume() {
    center = GLVertex(0,0,0);
    side = 1.234;
}

/// set bbox values
void CubeVolume::calcBB() {
    bb.clear();
    GLVertex maxpt = GLVertex(center.x + side/2, center.y + side/2, center.z + side/2);
    GLVertex minpt = GLVertex(center.x - side/2, center.y - side/2, center.z - side/2);
    bb.addPoint( maxpt );
    bb.addPoint( minpt );
}

double CubeVolume::dist(GLVertex& p) const {
    double m;
    m = p.x-center.x;
    
    if ( fabs(m) < fabs(p.y-center.y) ) 
        m = p.y-center.y;
    if (fabs(m) < fabs(p.z-center.z) )
        m = p.z-center.z;
    // m is now the maximum coordinate
    bool sign = (invert ? -1.0 : 1.0 );
    return -(sign)*( fabs(m)- side/2.0);
    
    //return 0;
}*/

/*
bool CubeVolume::isInside(GLVertex& p) const
{
    bool x,y,z;
    x = ( (p.x >= (center.x-side/2)) && (p.x <= (center.x+side/2)) );
    y = ( (p.y >= (center.y-side/2)) && (p.y <= (center.y+side/2)) );
    z = ( (p.z >= (center.z-side/2)) && (p.z <= (center.z+side/2)) );
    
    bool r_val;
    if (invert)
        r_val=true;
    else
        r_val=false;
    
    if ( x && y && z )
        return r_val;
    else
        return !r_val;
}*/


//************* Box *******************/

/*
BoxOCTVolume::BoxOCTVolume() {
    corner = GLVertex(0,0,0); 
    v1 = GLVertex(1,0,0); 
    v2 = GLVertex(0,1,0);
    v3 = GLVertex(0,0,1);
}

void BoxOCTVolume::calcBB() {
    bb.clear();
    bb.addPoint(corner);
    bb.addPoint(corner+v1);
    bb.addPoint(corner+v2);
    bb.addPoint(corner+v3);
}*/

//bool BoxOCTVolume::isInside(GLVertex& p) const {
    /*
    // translate to origo
    Point pt = p - corner;
    
    // box is = a*v1 + b*v2 + c*v3
    // where a,b,c are in [0,1]
    
    // v1 radial
    // v2 along move
    // v3 axial(z-dir)
    
    Point v1xy = v1;
    v1xy.z = 0;
    
    Point v2xy = v2;
    v2xy.z = 0;
    
    // projection along each vector, in turn
    // this only works if the vectors are orthogonal
    double t1 = pt.dot(v1xy)/v1xy.dot(v1xy);
    if ( (t1 < 0.0) || (t1>1.0) )
        return false;
        
    double t2 = pt.dot(v2xy)/v2xy.dot(v2xy);
    if ( (t2 < 0.0) || (t2>1.0) )
        return false;
    
    // this ensures we are OK in the XY plane
    // now check the z-coordinate.
    double zmin = corner.z + t2*v2.z;
    if (p.z < zmin)
        return false;
    double zmax = corner.z + v3.z + t2*v2.z;
    if (p.z > zmax)
        return false;
        
    //t = pt.dot(v3)/v3.dot(v3);
    //if ( (t < 0.0) || (t>1.0) )
    //    return false;
    */
  //  return (dist(p) <= 0.0);
    
//}

//************* Cylinder **************/

/*
CylinderOCTVolume::CylinderOCTVolume()
{
    p1 = GLVertex(0,0,0);
    p2 = GLVertex(1,1,1);
    radius = 1.234;
}*/

/*
bool CylinderOCTVolume::isInside(GLVertex& p) const 
{
    // closest point on axis
    GLVertex c = p.closestPoint(p1, p2);
    // line = p1 + t*(p2-p1)
    // t is in [0,1] for points on the line
    double t = (c.dot(p2-p1) - p1.dot( p2-p1)) / (p2-p1).dot(p2-p1);
    if ( (t>1.0) || (t < 0.0))
        return false;
        
    if ( (c-p).norm() <= radius)
        return true;
    else
        return false;
}*/

/*
/// calculate the bounding-box 
void CylinderOCTVolume::calcBB()
{
    // this is an approximate bounding-box, not exact
    GLVertex xmax1 = p1 + GLVertex(radius,0,0);
    GLVertex xmin1 = p1 - GLVertex(radius,0,0);
    GLVertex xmax2 = p2 + GLVertex(radius,0,0);
    GLVertex xmin2 = p2 - GLVertex(radius,0,0);
    
    GLVertex ymax1 = p1 + GLVertex(0,radius,0);
    GLVertex ymin1 = p1 - GLVertex(0,radius,0);
    GLVertex ymax2 = p2 + GLVertex(0,radius,0);
    GLVertex ymin2 = p2 - GLVertex(0,radius,0);    
    
    GLVertex zmax1 = p1 + GLVertex(0,0,radius);
    GLVertex zmin1 = p1 - GLVertex(0,0,radius);
    GLVertex zmax2 = p2 + GLVertex(0,0,radius);
    GLVertex zmin2 = p2 - GLVertex(0,0,radius);  
    
    bb.addPoint( xmax1 );   
    bb.addPoint( xmax2 );
    bb.addPoint( xmin1 );
    bb.addPoint( xmin2 );

    bb.addPoint( ymax1 );   
    bb.addPoint( ymax2 );
    bb.addPoint( ymin1 );
    bb.addPoint( ymin2 );
    
    bb.addPoint( zmax1 );   
    bb.addPoint( zmax2 );
    bb.addPoint( zmin1 );
    bb.addPoint( zmin2 );
    
    
}*/

//************* EtubeOCTVolume *************/

/*

EtubeOCTVolume::EtubeOCTVolume() 
{
    p1 = GLVertex(0,0,0);
    p2 = GLVertex(1,0,0);
    a = GLVertex(0,0.1,0);
    b = GLVertex(0,0,0.2);
}

EtubeOCTVolume::EtubeOCTVolume(GLVertex& p1in, GLVertex& p2in, GLVertex& ain, GLVertex& bin) 
{
    p1 = p1in;
    p2 = p2in;
    a = ain;
    b = bin;
}*/

/*
bool EtubeOCTVolume::isInside(GLVertex& p) const 
{
    
    // xy-plane check
    
    //Point v = p2-p1;
    //Point vxy = v;
    //vxy.z =0;
    
    // translate so (0,0) is at p1
    //Point pt = p1 - p;
    
    // restrict to points closer than a.norm() to tool-line   
    double xyd = p.xyDistanceToLine(p1,p2);
    if (xyd > a.norm() )
        return false;
        
    // coordinates along ellipse
    
    // center of ellipse
    GLVertex close = p.closestPoint(p1, p2);
    
    GLVertex ellvec = p-close;
    
    // is ellvec within ellipse?
    double ta = ellvec.dot(a)/a.dot(a);
    double tb = ellvec.dot(b)/b.dot(b);
    if ( (ta>1.0) || (ta<-1.0) )
        return false;
        
    if ( (tb>1.0) || (tb<-1.0) )
        return false;
        
    if ((ta*ta + tb*tb) > 1.0)
        return false;
    
    // projection along 
    // this only works if the vectors are orthogonal
    
    //double t1 = pt.dot(v1xy)/v1xy.dot(v1xy);
    //if ( (t1 < 0.0) || (t1>1.0) )
    //    return false;
        
    7/double t2 = pt.dot(v2xy)/v2xy.dot(v2xy);
    //if ( (t2 < 0.0) || (t2>1.0) )
    //    return false;
    
    
    // z-direction check
    double maxz = p1.z > p2.z ? p1.z : p2.z; 
    double minz = p1.z < p2.z ? p1.z : p2.z;   
    
    if (p.z < minz)
        return false;
        
    if (p.z > maxz)
        return false;
        
    
    //if (p.z 
    // figure out where we are and return true/false
    return true;
}*/


//************* CylCutterMove **************/
/*

CylMoveOCTVolume::CylMoveOCTVolume(const CylCutter& cin, const Point& p1in, const Point& p2in)
{
    p1 = p1in;
    p2 = p2in;
    c = cin;
    
    Point cutter_zvec = Point(0,0,c.getLength());
    // start of move, extreme point(s)
    bb.addPoint( p1 + c.getRadius()*Point(1,0,0) );
    bb.addPoint( p1 + c.getRadius()*Point(-1,0,0) );
    bb.addPoint( p1 + c.getRadius()*Point(0,1,0) );
    bb.addPoint( p1 + c.getRadius()*Point(0,-1,0) );
    // start of move (top)
    bb.addPoint( p1 + c.getRadius()*Point(1,0,0)  + cutter_zvec );
    bb.addPoint( p1 + c.getRadius()*Point(-1,0,0) + cutter_zvec );
    bb.addPoint( p1 + c.getRadius()*Point(0,1,0)  + cutter_zvec );
    bb.addPoint( p1 + c.getRadius()*Point(0,-1,0) + cutter_zvec );
    // end of move (bottom)
    bb.addPoint( p2 + c.getRadius()*Point(1,0,0) );
    bb.addPoint( p2 + c.getRadius()*Point(-1,0,0) );
    bb.addPoint( p2 + c.getRadius()*Point(0,1,0) );
    bb.addPoint( p2 + c.getRadius()*Point(0,-1,0) );
    // end of move (top)
    bb.addPoint( p2 + c.getRadius()*Point(1,0,0)  + cutter_zvec );
    bb.addPoint( p2 + c.getRadius()*Point(-1,0,0) + cutter_zvec );
    bb.addPoint( p2 + c.getRadius()*Point(0,1,0)  + cutter_zvec );
    bb.addPoint( p2 + c.getRadius()*Point(0,-1,0) + cutter_zvec );
    
    
    // cylinder at start of move
    c1.p1 = p1;
    c1.p2 = p1+Point(0,0,c.getLength());
    c1.radius=c.getRadius();
    // std::cout << " startcyl at " << c1.p1 << " to " << c1.p2 << "radius="<< c1.radius << "\n";
    
    // cylinder at end of move
    c2.p1 = p2;
    c2.p2 = p2+Point(0,0,c.getLength());
    c2.radius=c.getRadius();
    // std::cout << " endcyl at " << c2.p1 << " to " << c2.p2 <<" radius=" << c2.radius << "\n";
    
    // for XY-plane moves, a box:
    Point v = p2-p1; // vector along move
    Point v2 = p2-p1; 
    box.v2 = p2-p1;
    
    v.z = 0;
    v.normalize();
    
    box.corner = p1 + c.getRadius()*v.xyPerp();
    box.v1 = -2*c.getRadius()*v.xyPerp();
    box.v2 = v2;
    box.v3 = Point(0,0,c.getLength());
    // std::cout << " box at corner=" << box.corner << "\n";
    // std::cout << "            v1=" << box.v1 << "\n";
    // std::cout << "            v2=" << box.v2 << "\n";
    // std::cout << "            v3=" << box.v3 << "\n";
    
    // the elliptic tube
    etube = EtubeOCTVolume();
    // set the parameters
    etube.p1 = p1;
    etube.p2 = p2;
    etube.a = c.getRadius()*v.xyPerp();
    //etube.b = Point(0,0,0); //fixme
    // angle of move
    double dz = p2.z-p1.z;
    double length = (p2-p1).norm();
        
    double sin = dz/length;
    //double cos = sqrt( 1.0-sin*sin);
    double baxis = fabs(c.getRadius()*sin);
    // std::cout << " Etube baxis length="<< baxis << "\n";
    // direction is cross product 
    Point bdir = (p2-p1).cross(etube.a);
    bdir.normalize();
    etube.b= baxis*bdir;
    // std::cout << " Etube a="<< etube.a << " b=" << etube.b << "\n";
    
}

bool CylMoveOCTVolume::isInside(Point& p) const 
{
    // CL follows line
    // line = p1 + t*(p2-p1)
    // top of cutter follows same line only c.length() higher
    if (c1.isInside(p)) 
        return true;
    
    if (c2.isInside(p))
        return true;
    
    // for XY-plane moves, a box:
    if (box.isInside(p))
        return true;
    
    // the Elliptic tube
    if (etube.isInside(p))
        return true;
        
    // the default is to return false
    return false;

}
*/

//************* CylCutterVolume **************/

/*
CylCutterVolume::CylCutterVolume() {
    radius = 1.0;
    length = 1.0;
    pos = Point(0,0,0);
    calcBB();
}

void CylCutterVolume::setPos(Point& p) {
    pos = p;
    calcBB();
}

void CylCutterVolume::calcBB() {
    bb.clear();
    bb.addPoint( pos + Point(radius,radius,0) ); //FIXME
    bb.addPoint( pos + Point(-radius,-radius,length) ); // FIXME
}

bool CylCutterVolume::isInside(Point& p) const {
    Point t = p-pos;
    if (t.z < 0 )
        return false;
    else {
        double det = std::max( fabs(t.z-length/2)-length/2 , t.x*t.x+t.y*t.y-radius*radius );
        return (det < 0.0); 
    }
}

double CylCutterVolume::dist(Point& p) const {
    Point t = p-pos;
    if (t.z >= 0 )
        return std::max( fabs(t.z-length/2)-length/2 , t.x*t.x+t.y*t.y-radius*radius );
    else {
        // if we are under the cutter, then return distance to flat cutter bottom
        if ( t.x*t.x+t.y*t.y < radius*radius )
            return -t.z; 
        else { // outside the cutter, return a distance to the outer "ring" of the cutter
            Point nxy = t;
            nxy.xyNormalize();
            nxy = radius * nxy;
            return (t.x-nxy.x)*(t.x-nxy.x) + (t.y-nxy.y)*(t.y-nxy.y) + t.z*t.z;
        }
    }
}
*/


//************* BallCutterVolume **************/


/*
BallCutterVolume::BallCutterVolume() {
    radius = 1.0;
    length = 1.0;
    pos = Point(0,0,0);
}

void BallCutterVolume::setPos(Point& p) {
    pos = p;
    calcBB();
}

void BallCutterVolume::calcBB() {
    bb.clear();
    double safety = 1;
    bb.addPoint( pos + Point(safety*radius,safety*radius,safety*length) );
    bb.addPoint( pos + Point(-safety*radius,-safety*radius,-safety*length) );
}

double BallCutterVolume::dist(Point& p) const {
    Point t = p - pos - Point(0,0,radius);
    if (t.z < 0 )
        return  square(t.x) + square(t.y) + square(t.z) -  square( radius ) ;
    else {
        return std::max( fabs(t.z)-length ,  square(t.x) + square(t.y)  - square(radius) ) ;
    }
}
*/


//************* BullCutterVolume **************/
// TOROID 

/*
BullCutterVolume::BullCutterVolume() {
    r1 = 1.0;
    r2 = r1/3.0;
    radius = r1+r2;
    length = 1.0;
    pos = Point(0,0,0);
}

void BullCutterVolume::setPos(Point& p) {
    pos = p;
    calcBB();
}

void BullCutterVolume::calcBB() {
    bb.clear();
    double safety = 1;
    bb.addPoint( pos + Point(safety*radius,safety*radius,safety*length) );
    bb.addPoint( pos + Point(-safety*radius,-safety*radius,-safety*length) );
}

double BullCutterVolume::dist(Point& p) const {
    Point t = p - pos - Point(0,0,r2); // shift up by tube radius
    if (t.z >= 0.0 ) // cylindrical part, above toroid
        return std::max( fabs(t.z)-length , square(t.x) + square(t.y) - square(r1+r2) );
    else if ( square(t.x)+square(t.y) <= square(r1) ) // cylindrical part, inside toroid
                     //was fabs(t.z)-r2
        return std::max( square(t.z)-square(r2) , square(t.x) + square(t.y) - square( r1 ) );
    else // toroid
        return square( square(t.x) + square(t.y) + square(t.z) + square( r1 ) - square( r2 ) ) - 
               4*square(r1)*(square(t.x)+square(t.y));
}
*/


//************* PlaneVolume **************/

/*
PlaneVolume::PlaneVolume(bool s, unsigned int a, double p) {
    sign = s;
    axis = a;
    position = p; // FIXME, position=0 does not seem to work right!
    calcBB();
}

void PlaneVolume::calcBB() {
    bb.clear();
    GLVertex maxp;
    GLVertex minp;
    double bignum = 1e6;
    maxp = GLVertex(bignum,bignum,bignum);
    minp = GLVertex( -bignum,-bignum,-bignum );
    bb.addPoint( maxp );
    bb.addPoint( minp );
}

double PlaneVolume::dist(GLVertex& p) const {
    if (axis==0u) {
        if (sign)
            return p.x - position;
        else
            return -(p.x - position);
    } else if (axis==1u) {
        if (sign)
            return p.y - position;
        else
            return -(p.y - position);
    } else if (axis==2u) {
        if (sign)
            return p.z - position;
        else
            return -(p.z - position);
    } else {
        assert(0);
        return -1;
    }
}
*/

} // end namespace
// end of file volume.cpp
