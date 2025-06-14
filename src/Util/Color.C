/*******************************************************************************

  Copyright (C) 2023 Andrew Gilbert

  This file is part of IQmol, a free molecular visualization program. See
  <http://iqmol.org> for more details.

  IQmol is free software: you can redistribute it and/or modify it under the
  terms of the GNU General Public License as published by the Free Software  
  Foundation, either version 3 of the License, or (at your option) any later  
  version.

  IQmol is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
  details.

  You should have received a copy of the GNU General Public License along
  with IQmol.  If not, see <http://www.gnu.org/licenses/>.

********************************************************************************/

#include "Preferences.h"
#include "Color.h"
#include "ColorM.h"

#include <QDebug>


namespace IQmol {
namespace Color {

static QColor Maroon("#800000");
static QColor Brown("#9A6324");
static QColor Olive("#808000");
static QColor Teal("#469990");
static QColor Navy("#000075");
static QColor Black("#000000");
static QColor Red("#e6194B");
static QColor Orange("#f58231");
static QColor Yellow("#ffe119");
static QColor Lime("#bfef45");
static QColor Green("#3cb44b");
static QColor Cyan("#42d4f4");
static QColor Blue("#4363d8");
static QColor Purple("#911eb4");
static QColor Magenta("#f032e6");
static QColor Grey("#a9a9a9");
static QColor Pink("#fabed4");
static QColor Apricot("#ffd8b1");
static QColor Beige("#fffac8");
static QColor Mint("#aaffc3");
static QColor Lavender("#dcbeff");
static QColor White("#ffffff");

// Old spectrum
static QColor red("#c42724");
static QColor blue("#485fa6");
static QColor orange("#da8b2a");
static QColor yellow("#f4e940");
static QColor green("#6cab57");
static QColor violet("#603886");


//https://www.materialpalette.com/colors
static QColor Reds[] = 
{
   QColor("#ffebee"),  QColor("#ffcdd2"),  QColor("#ef9a9a"),  QColor("#e57373"), 
   QColor("#ef5350"),  QColor("#f44336"),  QColor("#e53935"),  QColor("#d32f2f"), 
   QColor("#c62828"),  QColor("#b71c1c"), 
   QColor("#ff8a80"),  QColor("#ff5252"),  QColor("#ff1744"),  QColor("#d50000") 
};

static QColor Pinks[] =
{
   QColor("#fce4ec"),  QColor("#f8bbd0"),  QColor("#f48fb1"),  QColor("#f06292"), 
   QColor("#ec407a"),  QColor("#e91e63"),  QColor("#d81b60"),  QColor("#c2185b"), 
   QColor("#ad1457"),  QColor("#880e4f"), 
   QColor("#ff80ab"),  QColor("#ff4081"),  QColor("#f50057"),  QColor("#c51162") 
};

static QColor Purples[] =
{
   QColor("#f3e5f5"),  QColor("#e1bee7"),  QColor("#ce93d8"),  QColor("#ba68c8"), 
   QColor("#ab47bc"),  QColor("#9c27b0"),  QColor("#8e24aa"),  QColor("#7b1fa2"), 
   QColor("#6a1b9a"),  QColor("#4a148c"),
   QColor("#ea80fc"),  QColor("#e040fb"),  QColor("#d500f9"),  QColor("#aa00ff"), 
};

static QColor DeepPurples[] =
{
   QColor("#ede7f6"),  QColor("#d1c4e9"),  QColor("#b39ddb"),  QColor("#9575cd"), 
   QColor("#7e57c2"),  QColor("#673ab7"),  QColor("#5e35b1"),  QColor("#512da8"), 
   QColor("#4527a0"),  QColor("#311b92"),
   QColor("#b388ff"),  QColor("#7c4dff"),  QColor("#651fff"),  QColor("#6200ea"), 
};

static QColor Indigos[] =
{
   QColor("#e8eaf6"),  QColor("#c5cae9"),  QColor("#9fa8da"),  QColor("#7986cb"), 
   QColor("#5c6bc0"),  QColor("#3f51b5"),  QColor("#3949ab"),  QColor("#303f9f"), 
   QColor("#283593"),  QColor("#1a237e"),
   QColor("#8c9eff"),  QColor("#536dfe"),  QColor("#3d5afe"),  QColor("#304ffe"), 
};

static QColor Blues[] =
{
   QColor("#e3f2fd"),  QColor("#bbdefb"),  QColor("#90caf9"),  QColor("#64b5f6"), 
   QColor("#42a5f5"),  QColor("#2196f3"),  QColor("#1e88e5"),  QColor("#1976d2"), 
   QColor("#1565c0"),  QColor("#0d47a1"),
   QColor("#82b1ff"),  QColor("#448aff"),  QColor("#2979ff"),  QColor("#2962ff"), 
};

static QColor LightBlues[] =
{
   QColor("#e1f5fe"),  QColor("#b3e5fc"),  QColor("#81d4fa"),  QColor("#4fc3f7"), 
   QColor("#29b6f6"),  QColor("#03a9f4"),  QColor("#039be5"),  QColor("#0288d1"), 
   QColor("#0277bd"),  QColor("#01579b"),
   QColor("#80d8ff"),  QColor("#40c4ff"),  QColor("#00b0ff"),  QColor("#0091ea"), 
};

static QColor Cyans[] =
{
   QColor("#e0f7fa"),  QColor("#b2ebf2"),  QColor("#80deea"),  QColor("#4dd0e1"), 
   QColor("#26c6da"),  QColor("#00bcd4"),  QColor("#00acc1"),  QColor("#0097a7"), 
   QColor("#00838f"),  QColor("#006064"),
   QColor("#84ffff"),  QColor("#18ffff"),  QColor("#00e5ff"),  QColor("#00b8d4"), 
};

static QColor Teals[] =
{
   QColor("#e0f2f1"),  QColor("#b2dfdb"),  QColor("#80cbc4"),  QColor("#4db6ac"), 
   QColor("#26a69a"),  QColor("#009688"),  QColor("#00897b"),  QColor("#00796b"), 
   QColor("#00695c"),  QColor("#004d40"),
   QColor("#a7ffeb"),  QColor("#64ffda"),  QColor("#1de9b6"),  QColor("#00bfa5"), 
};

static QColor Greens[] =
{
   QColor("#e8f5e9"),  QColor("#c8e6c9"),  QColor("#a5d6a7"),  QColor("#81c784"), 
   QColor("#66bb6a"),  QColor("#4caf50"),  QColor("#43a047"),  QColor("#388e3c"), 
   QColor("#2e7d32"),  QColor("#1b5e20"),
   QColor("#b9f6ca"),  QColor("#69f0ae"),  QColor("#00e676"),  QColor("#00c853"), 
};

static QColor LightGreens[] =
{
   QColor("#f1f8e9"),  QColor("#dcedc8"),  QColor("#c5e1a5"),  QColor("#aed581"), 
   QColor("#9ccc65"),  QColor("#8bc34a"),  QColor("#7cb342"),  QColor("#689f38"), 
   QColor("#558b2f"),  QColor("#33691e"),
   QColor("#ccff90"),  QColor("#b2ff59"),  QColor("#76ff03"),  QColor("#64dd17"), 
};

static QColor Limes[] =
{
   QColor("#f9fbe7"),  QColor("#f0f4c3"),  QColor("#e6ee9c"),  QColor("#dce775"), 
   QColor("#d4e157"),  QColor("#cddc39"),  QColor("#c0ca33"),  QColor("#afb42b"), 
   QColor("#9e9d24"),  QColor("#827717"),
   QColor("#f4ff81"),  QColor("#eeff41"),  QColor("#c6ff00"),  QColor("#aeea00"), 
};

static QColor Yellows[] =
{
   QColor("#fffde7"),  QColor("#fff9c4"),  QColor("#fff59d"),  QColor("#fff176"), 
   QColor("#ffee58"),  QColor("#ffeb3b"),  QColor("#fdd835"),  QColor("#fbc02d"), 
   QColor("#f9a825"),  QColor("#f57f17"),
   QColor("#ffff8d"),  QColor("#ffff00"),  QColor("#ffea00"),  QColor("#ffd600"), 
};

static QColor Ambers[] =
{
   QColor("#fff8e1"),  QColor("#ffecb3"),  QColor("#ffe082"),  QColor("#ffd54f"), 
   QColor("#ffca28"),  QColor("#ffc107"),  QColor("#ffb300"),  QColor("#ffa000"), 
   QColor("#ff8f00"),  QColor("#ff6f00"),
   QColor("#ffe57f"),  QColor("#ffd740"),  QColor("#ffc400"),  QColor("#ffab00"), 
};

static QColor Oranges[] =
{
   QColor("#fff3e0"),  QColor("#ffe0b2"),  QColor("#ffcc80"),  QColor("#ffb74d"), 
   QColor("#ffa726"),  QColor("#ff9800"),  QColor("#fb8c00"),  QColor("#f57c00"), 
   QColor("#ef6c00"),  QColor("#e65100"),
   QColor("#ffd180"),  QColor("#ffab40"),  QColor("#ff9100"),  QColor("#ff6d00"), 
};

static QColor DeepOranges[] =
{
   QColor("#fbe9e7"),  QColor("#ffccbc"),  QColor("#ffab91"),  QColor("#ff8a65"), 
   QColor("#ff7043"),  QColor("#ff5722"),  QColor("#f4511e"),  QColor("#e64a19"), 
   QColor("#d84315"),  QColor("#bf360c"),
   QColor("#ff9e80"),  QColor("#ff6e40"),  QColor("#ff3d00"),  QColor("#dd2c00"), 
};

static QColor Browns[] =
{
   QColor("#efebe9"),  QColor("#d7ccc8"),  QColor("#bcaaa4"),  QColor("#a1887f"), 
   QColor("#8d6e63"),  QColor("#795548"),  QColor("#6d4c41"),  QColor("#5d4037"), 
   QColor("#4e342e"),  QColor("#3e2723"),
};

static QColor Greys[] =
{
   QColor("#fafafa"),  QColor("#f5f5f5"),  QColor("#eeeeee"),  QColor("#e0e0e0"), 
   QColor("#bdbdbd"),  QColor("#9e9e9e"),  QColor("#757575"),  QColor("#616161"), 
   QColor("#424242"),  QColor("#212121"),
};

static QColor BlueGreys[] =
{
   QColor("#eceff1"),  QColor("#cfd8dc"),  QColor("#b0bec5"),  QColor("#90a4ae"), 
   QColor("#78909c"),  QColor("#607d8b"),  QColor("#546e7a"),  QColor("#455a64"), 
   QColor("#37474f"),  QColor("#263238"),
};

static QColor* AllColors[] = 
{
   Reds,         Pinks,       Purples,  DeepPurples,  Indigos,
   Blues,        LightBlues,  Cyans,    Teals,        Greens,
   LightGreens,  Limes,       Yellows,  Ambers,       Oranges,
   DeepOranges,  Browns,      Greys,    BlueGreys
};



List residueColors()
{
   size_t k(4);
   List colors;
      
   colors << Reds[k]         // ALA 
          << Pinks[k]        // ARG
          << Purples[k]      // ASN
          << DeepPurples[k]  // ASP
          << Indigos[k]      // CYS
          << Blues[k]        // GLN
          << LightBlues[k]   // GLU
          << Cyans[k]        // GLY
          << Teals[k]        // HIS
          << Greens[k]       // ILE
          << LightGreens[k]  // LEU
          << Limes[k]        // LYS
          << Yellows[k]      // MET
          << Ambers[k]       // PHE
          << Oranges[k]      // PRO
          << DeepOranges[k]  // SER
          << Browns[k]       // THR
          << Greys[k]        // TRP
          << BlueGreys[k]    // TYR
          << BlueGreys[0]    // VAL
          << BlueGreys[9]    // ASX
          << BlueGreys[9];   // GLX

    return colors;
}


List atomColors()
{
   List colors;
   colors
     << QColor("#1280B3") // X
     << QColor("#FFFFFF") // H    
     << QColor("#D9FFFF") // He   
     << QColor("#CC80FF") // Li   
     << QColor("#C2FF00") // Be   
     << QColor("#FFB5B5") // B    
     << QColor("#909090") // C    
     << QColor("#3050F8") // N    
     << QColor("#FF0D0D") // O    
     << QColor("#90E050") // F    
     << QColor("#B3E3F5") // Ne   
     << QColor("#AB5CF2") // Na   
     << QColor("#8AFF00") // Mg   
     << QColor("#BFA6A6") // Al   
     << QColor("#F0C8A0") // Si   
     << QColor("#FF8000") // P    
     << QColor("#FFFF30") // S    
     << QColor("#1FF01F") // Cl   
     << QColor("#80D1E3") // Ar   
     << QColor("#8F40D4") // K    
     << QColor("#3DFF00") // Ca   
     << QColor("#E6E6E6") // Sc   
     << QColor("#BFC2C7") // Ti   
     << QColor("#A6A6AB") // V    
     << QColor("#8A99C7") // Cr   
     << QColor("#9C7AC7") // Mn   
     << QColor("#E06633") // Fe   
     << QColor("#F090A0") // Co   
     << QColor("#50D050") // Ni   
     << QColor("#C88033") // Cu   
     << QColor("#7D80B0") // Zn   
     << QColor("#C28F8F") // Ga   
     << QColor("#668F8F") // Ge   
     << QColor("#BD80E3") // As   
     << QColor("#FFA100") // Se   
     << QColor("#A62929") // Br   
     << QColor("#5CB8D1") // Kr   
     << QColor("#702EB0") // Rb   
     << QColor("#00FF00") // Sr   
     << QColor("#94FFFF") // Y    
     << QColor("#94E0E0") // Zr   
     << QColor("#73C2C9") // Nb   
     << QColor("#54B5B5") // Mo   
     << QColor("#3B9E9E") // Tc   
     << QColor("#248F8F") // Ru   
     << QColor("#0A7D8C") // Rh   
     << QColor("#006985") // Pd   
     << QColor("#C0C0C0") // Ag   
     << QColor("#FFD98F") // Cd   
     << QColor("#A67573") // In   
     << QColor("#668080") // Sn   
     << QColor("#9E63B5") // Sb   
     << QColor("#D47A00") // Te   
     << QColor("#940094") // I    
     << QColor("#429EB0") // Xe   
     << QColor("#57178F") // Cs   
     << QColor("#00C900") // Ba   
     << QColor("#70D4FF") // La   
     << QColor("#FFFFC7") // Ce   
     << QColor("#D9FFC7") // Pr   
     << QColor("#C7FFC7") // Nd   
     << QColor("#A3FFC7") // Pm   
     << QColor("#8FFFC7") // Sm   
     << QColor("#61FFC7") // Eu   
     << QColor("#45FFC7") // Gd   
     << QColor("#30FFC7") // Tb   
     << QColor("#1FFFC7") // Dy   
     << QColor("#00FF9C") // Ho   
     << QColor("#00E675") // Er   
     << QColor("#00D452") // Tm   
     << QColor("#00BF38") // Yb   
     << QColor("#00AB24") // Lu   
     << QColor("#4DC2FF") // Hf   
     << QColor("#4DA6FF") // Ta   
     << QColor("#2194D6") // W    
     << QColor("#267DAB") // Re   
     << QColor("#266696") // Os   
     << QColor("#175487") // Ir   
     << QColor("#D0D0E0") // Pt   
     << QColor("#FFD123") // Au   
     << QColor("#B8B8D0") // Hg   
     << QColor("#A6544D") // Tl   
     << QColor("#575961") // Pb   
     << QColor("#9E4FB5") // Bi   
     << QColor("#AB5C00") // Po   
     << QColor("#754F45") // At   
     << QColor("#428296") // Rn   
     << QColor("#420066") // Fr   
     << QColor("#007D00") // Ra   
     << QColor("#70ABFA") // Ac   
     << QColor("#00BAFF") // Th   
     << QColor("#00A1FF") // Pa   
     << QColor("#008FFF") // U    
     << QColor("#0080FF") // Np   
     << QColor("#006BFF") // Pu   
     << QColor("#545CF2") // Am   
     << QColor("#785CE3") // Cm   
     << QColor("#8A4FE3") // Bk   
     << QColor("#A136D4") // Cf   
     << QColor("#B31FD4") // Es   
     << QColor("#B31FBA") // Fm   
     << QColor("#B30DA6") // Md   
     << QColor("#BD0D87") // No   
     << QColor("#C70066") // Lr   
     << QColor("#CC0059") // Rf   
     << QColor("#D1004F") // Db   
     << QColor("#D90045") // Sg   
     << QColor("#E00038") // Bh   
     << QColor("#E6002E") // Hs   
     << QColor("#EB0026") // Mt   
     << QColor("#EC0037") // Ds   
     << QColor("#ED0036") // Rg   
     << QColor("#EE0035") // Cn   
     << QColor("#F00034") // Nh   
     << QColor("#F10033") // Fl   
     << QColor("#F20032") // Mc   
     << QColor("#F30031") // Lv   
     << QColor("#F40030") // Ts   
     << QColor("#F50029");// Og   

   return colors;
}


QString toString(Gradient gradient)
{
   QString s;
   switch (gradient) {
      case Gradient::Default:          s = "Default";  break;
      case Gradient::Spectrum:         s = "Spectrum";  break;
      case Gradient::PrimarySpectrum:  s = "Spectrum 2";  break;
      case Gradient::Custom:           s = "Custom";  break;
   }
   return s;
}


QString toString(List const& colors, bool blend)
{
   QString grad;
   int nColors(colors.size());

   switch (nColors) {
      case 0:
         grad = "#000";  // black
         break;
      case 1:
         grad = colors.first().name(QColor::HexArgb);
         break;
      default:
         grad = "QLinearGradient(x1: 0, y1: 1, x2: 1, y2: 1, ";

         if (blend) {
            double step(1.0/(nColors-1));
            for (int i = 0; i < nColors-1; ++i) {
                grad += "stop: " + QString::number(i*step) + " " 
                     + colors[i].name(QColor::HexArgb) + ", ";
            }  
            grad += "stop: 1.00  " + colors.last().name(QColor::HexArgb) + ");";
         }else {
            double step(1.0/(nColors));
            grad += "stop: " + QString::number(0) + " " + colors[0].name(QColor::HexArgb) + ", ";
            for (int i = 1; i < nColors; ++i) {
                grad += "stop: " + QString::number(i*step-0.0001) + " " 
                                 + colors[i-1].name(QColor::HexArgb) + ", ";
                grad += "stop: " + QString::number(i*step) + " " 
                                 + colors[i].name(QColor::HexArgb) + ", ";
            }  
            grad += "stop: 1.00  " + colors.last().name(QColor::HexArgb) + ");";
         }

         break;
   }   

   return grad;
}


// --------------- Gradients ---------------

List toList(Gradient const gradient)
{
   List colors;

   switch(gradient)
   {
      case Gradient::Custom:
         colors = Preferences::CustomGradientColors();
         break;

      case Gradient::Default:
         colors << red << Qt::white << blue;
         break;

      case Gradient::Spectrum:
         colors << Red << Orange << Yellow << Green
                << Blue << Purple;
         break;

      case Gradient::PrimarySpectrum:
         colors << Qt::red << orange << Qt::yellow << Qt::green
                << Qt::blue << Qt::magenta;
         break;

   }

   return colors; 
}


List generateGradient(QColor const& base, unsigned const n, Operation const operation)
{
   List list;
   ColorM::Rgb color(base.red(), base.green(), base.blue());

   for (unsigned i(0); i < n; ++i) {
       QColor qcol(color.red(), color.green(), color.blue());
       list << qcol;

       switch (operation) {
          case Operation::Brighten:    color.brighten();           break;
          case Operation::Darken:      color.darken();             break;
          case Operation::Saturate:    color.saturate();           break;
          case Operation::Desaturate:  color.desaturate();         break;
          case Operation::Rotate:      color.rotate(360.0/(n+1));  break;
       }
   }

   return list;
}


unsigned toInt(Gradient const gradient)
{
   return static_cast<unsigned>(gradient);
}


Gradient fromInt(unsigned idx)
{
   Gradient gradient(static_cast<Gradient>(idx));
   return gradient;
}


// --------------- Function ---------------

Function::Function(List const& colors, double const min, double const max, bool blend) 
  : m_colors(colors), m_min(min), m_max(max), m_blend(blend)
{
   if (m_colors.isEmpty()) m_colors << Qt::black;

   // Set up the interpolators
   if (blend) {
      for (size_t i(0); i < colors.size()-1; ++i) {
          QColor const& first(m_colors[i]);
          QColor const& last(m_colors[i+1]);

          ColorM::Rgb col0(first.red(),first.green(),first.blue());
          col0.setAlpha8(first.alpha());
          ColorM::Rgb col1(last.red(),last.green(),last.blue());
          col1.setAlpha8(last.alpha());

          ColorM::ColorScale<ColorM::Rgb> scale({col0,col1});
          m_interpolators.append(scale);
      }
   }
}


Function& Function::operator=(Function const& that)
{
   if (this != &that) copy(that);
   return *this;
}


void Function::copy(Function const& that) 
{
   m_colors = that.m_colors; 
   m_min = that.m_min;
   m_max = that.m_max;
}


QColor Function::colorAt(double val) const
{
   // Deal with out of bounds first
   if (m_colors.size() == 1) return m_colors.first();
   if (val <= m_min ) return m_colors.first();
   if (val >= m_max ) return m_colors.last();

   val = (val-m_min) / (m_max-m_min);
   int index((m_colors.size()-1)*val);
   double step(1.0/(m_colors.size()-1));
   double delta((val-index*step)/step);

   QColor qcol;
   if (m_blend) {
      ColorM::Rgb color(m_interpolators[index](delta));
      color.clip();
      qcol = QColor(color.red(), color.green(), color.blue(), color.alpha8());
   }else {
      qcol = (delta < 0.5) ? m_colors[index] : m_colors[index+1];
   }
   
   //qDebug() << "Interpolation" << qcol;
   return qcol;
}


// Performs a linear interpolation between two colors, delta in [0.0,1.0]
QColor Function::interpolateRGB(double delta, QColor const& col1, QColor const& col2) const
{
   if (delta <= 0.0) return col1;
   if (delta >= 1.0) return col2;

   double r1, r2, g1, g2, b1, b2, a1, a2;
   col1.getRgbF(&r1, &g1, &b1, &a1);
   col2.getRgbF(&r2, &g2, &b2, &a2);

   r1 += delta*(r2-r1);
   g1 += delta*(g2-g1);
   b1 += delta*(b2-b1);
   a1 += delta*(a2-a1);

   return QColor(r1, g1, b1, a1);
}


QColor Function::interpolateHSV(double delta, QColor const& col1, QColor const& col2) const
{
   if (delta <= 0.0) return col1;
   if (delta >= 1.0) return col2;

   double h1, h2, s1, s2, v1, v2;
   col1.getHsvF(&h1, &s1, &v1);
   col2.getHsvF(&h2, &s2, &v2);

   if (h1 < 0.0) h1 = h2;
   if (h2 < 0.0) h2 = h1;
   if (h1 < 0.0 && h2 < 0.0) { h1 = 0.0; h2 = 0.0; }

   s1 += delta*(s2-s1);
   v1 += delta*(v2-v1);

   // For hue we need to work out which direction we are going on the wheel.
   double dh(h2-h1);
   if (dh > 0.5) {
      dh -= 1.0;
   }else if(dh < -0.5) {
      dh += 1.0;
   }

   h1 += delta*dh;
   if (h1 > 1.0) h1 -= 1.0;
   if (h1 < 0.0) h1 += 1.0;

   QColor color;
   color.setHsvF(h1, s1, v1);
   return color;
}


QColor Function::interpolateLCH(double delta, QColor const& col1, QColor const& col2) const
{
   if (delta <= 0.0) return col1;
   if (delta >= 1.0) return col2;

   double h1, h2, s1, s2, v1, v2;
   col1.getHsvF(&h1, &s1, &v1);
   col2.getHsvF(&h2, &s2, &v2);

   if (h1 < 0.0) h1 = h2;
   if (h2 < 0.0) h2 = h1;
   if (h1 < 0.0 && h2 < 0.0) { h1 = 0.0; h2 = 0.0; }

   s1 += delta*(s2-s1);
   v1 += delta*(v2-v1);

   // For hue we need to work out which direction we are going on the wheel.
   double dh(h2-h1);
   if (dh > 0.5) {
      dh -= 1.0;
   }else if(dh < -0.5) {
      dh += 1.0;
   }

   h1 += delta*dh;
   if (h1 > 1.0) h1 -= 1.0;
   if (h1 < 0.0) h1 += 1.0;

   QColor color;
   color.setHsvF(h1, s1, v1);
   return color;
}


List const& Function::resample(int nColors)
{
   if (nColors == m_colors.size() || nColors < 1) return m_colors;

   List newColors;
   if (nColors == 1) {
      newColors.append(colorAt(0.5));
   }else {
      double step(1.0/(nColors-1));
      for (int i = 0; i < nColors; ++i) {
         newColors.append(colorAt(i*step));
      }
   }
   m_colors = newColors;
   return m_colors;
}



} } // end namespace IQmol::Color
