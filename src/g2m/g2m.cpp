/***************************************************************************
*   Copyright (C) 2010 by Mark Pictor                                      *
*   mpictor@gmail.com                                                      *
*   modified by Anders Wallin 2011, anders.e.e.wallin@gmail.com            *
* 
*   This program is free software; you can redistribute it and/or modify   *
*   it under the terms of the GNU General Public License as published by   *
*   the Free Software Foundation; either version 2 of the License, or      *
*   (at your option) any later version.                                    *
*                                                                          *
*   This program is distributed in the hope that it will be useful,        *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
*   GNU General Public License for more details.                           *
*                                                                          *
*   You should have received a copy of the GNU General Public License      *
*   along with this program; if not, write to the                          *
*   Free Software Foundation, Inc.,                                        *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              *
***************************************************************************/

#include <iostream>
#include <cmath>
#include <fstream>
#include <stdlib.h>

#include <QProcess>
#include <QStringList>
#include <QString>
#include <QTime>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFileDialog>
#include <QStatusBar>
#include <QFile>
#include <QTextStream>
 
#include "g2m.hpp"
#include "nanotimer.hpp"
#include "machineStatus.hpp"

namespace g2m {

void g2m::interpret_file() {
    lineVector.clear();
    nanotimer timer;
    timer.start();
    gcode_lines=0;
    if ( file.endsWith(".ngc") ) {
        
            // push g-code lines to ui:
        QFile fileHandle( file );
        QString gline;
        if ( fileHandle.open( QIODevice::ReadOnly | QIODevice::Text) ) {       
            // file opened successfully
            QTextStream t( &fileHandle );        // use a text stream
            // until end of file...
            while ( !t.atEnd() ) {           
                // read and parse the command line
                gline = t.readLine();         // line of text excluding '\n'
                emit gcodeLineMessage(gline);
                gcode_lines++;
            }
            fileHandle.close();
        }

        
        emit debugMessage( tr("g2m: interpreting  %1").arg(file) ); 
        interpret(); // reads from file
    } else if (file.endsWith(".canon")) { //just process each line
        if (!chooseToolTable()) {
            infoMsg("Can't find tool table. Aborting.");
            return;
        }
        
        std::ifstream inFile(file.toAscii());
        std::string sLine;
        while(std::getline(inFile, sLine)) {
            if (sLine.length() > 1) {  //helps to prevent segfault in canonLine::cmdMatch()
                processCanonLine(sLine); // requires no interpret()
            }
        }
    } else {
        emit debugMessage( tr("File name must end with .ngc or .canon!") ); 
        return;
    }

    
    
    double e = timer.getElapsedS();
    emit debugMessage( tr("g2m: Total time to process that file: ") +  timer.humanreadable(e)  ) ;
    //std::cout << "Total time to process that file: " << timer.humanreadable(e).toStdString() << std::endl;

}

///ask for a tool table, even if one is configured - user may wish to change it
bool g2m::chooseToolTable() {
  if (!QFileInfo(tooltable).exists()){
    infoMsg(" cannot find tooltable! ");
    return false;
  }
  return true;
}

/// set the tooltable and start interpreting input from stdin. called from interpret()
bool g2m::startInterp(QProcess &tc) {
    if (!chooseToolTable())
        return false;
    // run:  rs274 file.ngc
    tc.start( interp , QStringList(file) );
    tc.write("3\n"); // "read tool file" command to rs274
    tc.write(tooltable.toAscii());
    tc.write("\n"); // "enter"
    tc.write("1\n"); // "start interpreting" command to rs274
    return true;
}

/// called after "file" set in constructor
void g2m::interpret() {
    //success = false;
    QProcess toCanon;
    bool foundEOF; // checked at the end
    
    if (!startInterp(toCanon)) // finds rs274, reads tooltable, start interpreter
        return;
    
    if (!toCanon.waitForReadyRead(1000) ) {
        if ( toCanon.state() == QProcess::NotRunning ){
            infoMsg("Interpreter died.  Bad tool table?");
        } else  
            infoMsg("Interpreter timed out for an unknown reason.");
        std::cout << "stderr: " << (const char*)toCanon.readAllStandardError() << std::endl;
        std::cout << "stdout: " << (const char*)toCanon.readAllStandardOutput() << std::endl;
        toCanon.close();
        return;
    }
    
    // rs274  has now started correctly, and is ready to read ngc-file
    qint64 lineLength;
    char line[260];
    int fails = 0;
    do {
        if (toCanon.canReadLine()) {
            lineLength = toCanon.readLine(line, sizeof(line)); // read one output line from rs274
            if (lineLength != -1 ) {
                QString l(line);
                emit canonLineMessage( l.left(l.size()-1) );
                foundEOF = processCanonLine(line); // line is a canon-line
            } else {  //shouldn't get here!
                std::cout << " ERROR: lineLength= " << lineLength << "  fails="<< fails << "\n";
                fails++;
            }
        } else {
            std::cout << " ERROR: toCanon.canReadLine() fails="<< fails << "\n";
            fails++;
        }
        toCanon.waitForReadyRead();
    } while ( (fails < 100) &&
           ( (toCanon.canReadLine()) ||
            ( toCanon.state() != QProcess::NotRunning ) )  );
  
    if (fails > 1) {
        if (fails < 100) {
            infoMsg("Waited for interpreter over 100  times.");
        } else {
            infoMsg("Waited 100 seconds for interpreter. Giving up.");
            toCanon.close();
            return;
        }
    }
  
  std::string s = (const char *)toCanon.readAllStandardError();
  s.erase(0,s.find("executing"));
  if (s.size() > 10) {
    infoMsg("Interpreter exited with error:\n"+s.substr(10));
    return;
  }
  if (!foundEOF) {
    infoMsg("Warning: file data not terminated correctly. If the file is terminated correctly, this indicates a problem interpreting the file.");
  }

    emit debugMessage( tr("g2m: read %1 lines of g-code which produced %2 canon-lines.").arg(gcode_lines).arg(lineVector.size()) );
    return;
}

/// process a canon-line input string. this is a canon-string from rs274.
/// call canonLineFactory to produce a canonLine and save it to lineVector
bool g2m::processCanonLine (std::string l) {
    canonLine* cl;
    if (lineVector.size()==0) { 
        // no status exists, so make one up.
        cl = canonLine::canonLineFactory(l, machineStatus( Pose( Point(0,0,0), Point(0,0,1) ) )  );
    } else {
        // use the last element status
        cl = canonLine::canonLineFactory(l,  *(lineVector.back())->getStatus()  ); 
    }
    emit signalCanonLine(cl);
    lineVector.push_back(cl); 

    if ( debug ) 
        std::cout << "Line " << cl->getLineNum() << "/N" << cl->getN() <<  std::endl;

    // return true when we reach end-of-program
    if (!cl->isMotion())
        return  cl->isNCend();

    return false;
}

/// output information to std::cout
void g2m::infoMsg(std::string s) {
    std::cout << s << std::endl;
}


} // end namespace



