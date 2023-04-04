#ifndef GromacsPreferences_H
#define GromacsPreferences_H



#include <QDialog>

namespace IQmol {
namespace Gmx {

    class GromacsPreferences
    {
    QString topology;
    QString positions;

    public:
    GromacsPreferences();
    QString getTopology();
    QString getPositions();
    void setPositions(QString positions);
    void setTopology(QString topology);

};

extern GromacsPreferences* GroPref;
} } //end namspaces

#endif