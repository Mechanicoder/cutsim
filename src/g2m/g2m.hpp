/***************************************************************************
 *   Copyright (C) 2010 by Mark Pictor                                     *
 *   mpictor@gmail.com                                                     *
 *   modifications Copyright (C) 2011 by Anders Wallin                     *
 *   anders.e.e.wallin@gmail.com                                           *      
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

#ifndef GTOM_HH
#define GTOM_HH

#include <vector>
#include <limits.h>
#include <iostream>
#include <fstream>

#include <QString>
#include <QProcess>
#include <QObject>

#include "canonLine.hpp"

namespace g2m {

/**
\class g2m
\brief This class runs the interpreter ("rs274"), and creates a canonLine object for each canonical 
* command generated by the interpreter. 
*/
class g2m : public QObject {
    Q_OBJECT;
    public:
        g2m()  { debug=false; }
    public slots:
        void interpret_file();
        void setFile(QString infile) {
            emit debugMessage( tr("g2m: setting file %1").arg(infile) ); 
            file = infile; 
        }
        void setToolTable(QString tbl_file) { 
            emit debugMessage( tr("g2m: setting tooltable %1").arg(tbl_file) ); 
            tooltable = tbl_file; 
        }
        void setInterp(QString interp_binary) { 
            emit debugMessage( tr("g2m: setting rs274 path:  %1").arg(interp_binary) ); 
            interp = interp_binary; 
        }
        
        void setDebug(bool d) {debug=d;}
        std::vector<canonLine*> getCanonLines() { return lineVector; }
    signals:
        void debugMessage(QString s);
    protected:    
        bool chooseToolTable();
        void interpret();
        bool processCanonLine(std::string l);
        bool startInterp(QProcess &tc);
        void infoMsg(std::string s);
        std::vector<canonLine*> lineVector;
        QString file;
        QString tooltable;
        QString interp;
        bool debug;
};


} // end namespace
#endif //GTOM_HH
