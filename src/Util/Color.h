#pragma once
/*******************************************************************************

  Copyright (C) 2022 Andrew Gilbert

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

#include "ColorM.h"
#include <QColor>
#include <QList>


namespace IQmol {
namespace Color {

   typedef QList<QColor> List;

   enum class Gradient {Custom , Default, Spectrum, PrimarySpectrum };

   enum class Operation{ Brighten, Darken, Saturate, Desaturate, Rotate};

   List toList(Gradient);

   unsigned toInt(Gradient);

   Gradient fromInt(unsigned);

   QString toString(List const& colors, bool blend); // Required for the stylesheet

   List residueColors();

   List atomColors();

   void maxRange();

   // Generates a list of colors based on the Operation
   List generateGradient(QColor const& base, unsigned const n, Operation const);

   /// Class used to represent a simple color gradient.  The reason for this
   /// is that QGradient does not allow access to the color at an arbitrary 
   /// point on the gradient.  Gradients are made up of equispaced colors 
   /// and the gradient is in the range [0,1].  A Gradient can have a single
   /// color, but that wouldn't be very interesting, would it?
   class Function {

      public:
         Function(List const& colors = toList(Gradient::Default),
            double const min = 0.0, double const max = 1.0, bool blend = true);
         Function(Function const& that) { copy(that); }
         Function& operator=(Function const& that);

         QColor colorAt(double val) const;
         List const& colors() const { return m_colors; }
         List const& resample(int nColors);

      private:
         void copy(Function const& that);
         QColor interpolateRGB(double delta, QColor const& col1, QColor const& col2) const;
         QColor interpolateHSV(double delta, QColor const& col1, QColor const& col2) const;
         QColor interpolateLCH(double delta, QColor const& col1, QColor const& col2) const;
         List m_colors;
         QList<ColorM::ColorScale<ColorM::Rgb>> m_interpolators;
         double m_min;
         double m_max;
         bool   m_blend;
   };

} } // end namespace IQmol::Color
