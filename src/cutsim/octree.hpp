/* 
 *  Copyright 2010-2011 Anders Wallin (anders.e.e.wallin "at" gmail.com)
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

#ifndef OCTREE_H
#define OCTREE_H

#include <iostream>
#include <list>
#include <cassert>

#include "bbox.hpp"
#include "gldata.hpp"
//#include "marching_cubes.hpp"

namespace cutsim {

class Octnode;
class Volume;


/// Octree class for cutting simulation
/// see http://en.wikipedia.org/wiki/Octree
/// The root node is divided into eight sub-octants, and each sub-octant
/// is recursively further divided into octants.
/// The side-length of the root node is root_scale
/// The dept of the root node is zero.
/// Subdivision is continued unti max_depth is reached.
/// A node at tree-dept n is a cube with side-length root_scale/pow(2,n)
///
/// This class stores the root Octnode and allows operations on the tree
///
class Octree {
    public:
        /// create an octree with a root node with scale=root_scale, maximum
        /// tree-depth of max_depth and centered at centerp.
        Octree(double root_scale, unsigned int max_depth, GLVertex& centerPoint, GLData* gl);
        virtual ~Octree();
        
    // bolean operations on tree
        void diff(const Volume* vol) { diff( this->root, vol); }
        void sum(const Volume* vol) { sum( this->root, vol); }
        void intersect(const Volume* vol) { intersect( this->root, vol); }
        
// debug, can be removed?
        void get_leaf_nodes( std::vector<Octnode*>& nodelist) const { get_leaf_nodes( root,  nodelist); }
        void get_leaf_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const;
        void get_invalid_leaf_nodes(std::vector<Octnode*>& nodelist) const;
        void get_invalid_leaf_nodes( Octnode* current, std::vector<Octnode*>& nodelist) const;
        void get_all_nodes(Octnode* current, std::vector<Octnode*>& nodelist) const;
        
        /// initialize by recursively calling subdivide() on all nodes n times
        void init(const unsigned int n);

        unsigned int get_max_depth() const;
        double get_root_scale() const;
        double leaf_scale() const;
        
        
        /// string output
        std::string str() const;
        //void setGLData(GLData* gdata) { g=gdata; }
        //void updateGL() { updateGL(root); }
        //void set_surface_algorithm(IsoSurfaceAlgorithm* m) {surface_algorithm = m;}
        
        bool debug, debug_mc;
        
        /// the root scale
        double root_scale;
        /// the maximum tree-depth
        unsigned int max_depth;
        /// pointer to the root node
        Octnode* root;
        
    protected:
        /// run isosurface-algorithm on current Octnode, and push gl-data to GLData
        //void updateGL(Octnode* current);
        /// recursively traverse the tree subtracting vol
        void diff(Octnode* current, const Volume* vol);
        void sum(Octnode* current, const Volume* vol);
        void intersect(Octnode* current, const Volume* vol);
        
        /// remove vertices associated with the current node
        //void remove_node_vertices(Octnode* current );
    // DATA

        GLData* g;
        //IsoSurfaceAlgorithm* surface_algorithm;
    private:
        Octree() {  }
        
};

} // end namespace
#endif
// end file octree.hpp
