#include "Gromacs/GromacsPreferences.h"

namespace IQmol {
namespace Gmx { 
    GromacsPreferences::GromacsPreferences(){
        topology = "topol.top"
        positions = "posre.itp"

    }

    GromacsPreferences::getTopology(){
        return topology;
    }
    GromacsPreferences::getPositions(){
        return positions;
    }
    GromacsPreferences::setPositions(QString pos){
        positions = pos;
    }
    GromacsPreferences::setTopology(QString top){
        topology = top;

    }

GromacsPreferences* GroPref = new GromacsPreferences();

} }