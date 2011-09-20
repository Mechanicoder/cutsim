/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <iostream>
#include <sstream>

#include "machineStatus.hpp"


machineStatus::machineStatus(const machineStatus& oldStatus) {
    //FIXME: segfault on next line when modelling a second file?!
    spindleStat = oldStatus.spindleStat;
    F = oldStatus.F;
    S = oldStatus.S;
    coolant = oldStatus.coolant;
    plane = oldStatus.plane;
    endPose = startPose = oldStatus.endPose;
    myTool = oldStatus.myTool;
    endDir = gp_Dir(0,0,-1);
    prevEndDir = oldStatus.endDir;
    first = oldStatus.first;
    motionType = NOT_DEFINED;
    /// lastMotionWasTraverse gets copied from the previous machine status, 
    /// and only gets changed if the prev status was motion at feedrate 
    /// (this way, motionless cmds don't mess things up)
    lastMotionWasTraverse = oldStatus.lastMotionWasTraverse;
    if ( oldStatus.motionType == STRAIGHT_FEED || oldStatus.motionType == HELICAL) {
      lastMotionWasTraverse = false;
    }
}

/**
This constructor is only to be used when initializing the simulation; it would not be useful elsewhere.
\param initial is the initial pose of the machine, as determined by the interp from the variable file.
\sa machineStatus(machineStatus const& oldStatus)
*/
machineStatus::machineStatus(gp_Ax1 initial) {
  clearAll();
  //theTool = new ballnoseTool(0.0625,0.3125); //1/16" tool. TODO: use EMC's tool table for tool sizes
  startPose = endPose = initial;
  first = true;
  setTool(1);
}

void machineStatus::clearAll() {
  F=S=0.0;
  plane = CANON_PLANE_XY;
  coolant.flood = false;
  coolant.mist = false;
  coolant.spindle = false;
  endPose = startPose = gp_Ax1(gp_Pnt(0,0,0),gp_Dir(0,0,1));
  endDir = prevEndDir = gp_Dir(0,0,-1);
  spindleStat = OFF;
  myTool = -1;
  motionType = NOT_DEFINED;
  lastMotionWasTraverse = false;
}

///sets motion type, and checks whether this is the second (or later) motion command.
void machineStatus::setMotionType(MOTION_TYPE m) {
    motionType = m;
    if (motionType == NOT_DEFINED) {
        std::cout << "motion type undef!! \n";
    } else if (motionType == TRAVERSE) {
        lastMotionWasTraverse = true;
    }
    static int count = 0;
    if ((first) && ((m == STRAIGHT_FEED) || (m == TRAVERSE) || (m == HELICAL)) ) {
        if (count == 0)
          count++;
        else
          first = false;
    }
}


/** \fn setEndPose
Set end points, and call addToBounds to add points to bndbox. For an arc or helix,
*  the edge must be added from its ctor with addArcToBbox.
\sa addArcToBbox(TopoDS_Edge e)
*/
void machineStatus::setEndPose(gp_Pnt p) {
  endPose = gp_Ax1( p, gp_Dir(0,0,1) );

}
void machineStatus::setEndPose(gp_Ax1 newPose) {
  endPose = newPose;

}


void machineStatus::setTool(toolNumber n) {
    //std::cout << "adding tool " << n << ".\n";
    myTool = n;
    //canon::addTool(n);
}





