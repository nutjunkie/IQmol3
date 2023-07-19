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

#include "JobInfo.h"


namespace IQmol {
namespace Process {

   /// QChemJobInfo holds information about the job to be run and forms the
   /// interface between the options editor (QUI) and the JobMonitor.
   class QChemJobInfo : public JobInfo {

      public:
         /// InputFileName  - the input file name, no path
         /// InputString    - the entire input file as a string 
         /// RunFileName    - the name of the submission script
         /// Note that not all these are serialized

#if 0
         enum Field { 
                 InputFileName,           // 0
                 OutputFileName, 
                 AuxFileName, 
                 EspFileName, 
                 MoFileName, 
                 DensityFileName,         // 5
                 ErrorFileName, 
                 BatchFileName, 
                 RunFileName, 
                 LocalWorkingDirectory, 
                 RemoteWorkingDirectory,  // 10
                 InputString, 
                 Charge, 
                 Multiplicity, 
                 Coordinates, 
                 CoordinatesFsm,          // 15
                 Constraints, 
                 ScanCoordinates,
                 EfpFragments, 
                 EfpParameters, 
                 ExternalCharges,         // 20
                 OnsagerRadius,
                 Isotopes,
                 NElectrons,
                 InputFileTemplate        // This effectively holds the contents of the 
                                          // any input file loaded in the molecule.
              };
#endif

         QChemJobInfo()
         { 
            set("Charge", 0);
            set("Multiplicity", 1);
            set("NumElectrons", 0);
            set("EfpOnly", false);
            set("LocalFilesExist", false);
            set("PromptOnOverwrite", true);
            set("MoleculePointer", 0);
         }


         QString fileExtension(QString const&) const;

   };

} } // end namespace IQmol::Process
