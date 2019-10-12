/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2011 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    averaging in the spanwise direction z


\*---------------------------------------------------------------------------*/

#include "argList.H"
#include "polyMesh.H"
#include "extrude2DMesh.H"
#include "extrudeModel.H"
#include "polyTopoChange.H"
#include "MeshedSurface.H"
#include "fvCFD.H"
#include "cyclicPolyPatch.H"
#include "sampledPlane.H"
#include "vtkSurfaceWriter.H"
#include "edgeCollapser.H"
#include "addPatchCellLayer.H"
#include "patchToPoly2DMesh.H"
#include "wordRe.H"
//#include "wordRes.H"
#include "ReadFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

fileName avgCyclicDirFolder = "../postProcessing/avgCyclicDir/";

#include "functions.H"

// -------------INPUT PARAMETERS ---------------------

//cyclic patch ID from which averaging is performed
// -------------INPUT PARAMETERS ---------------------

int main(int argc, char *argv[])
{
	argList::addNote
	(
		"Enjoy the spatial averaging in the cylic direction!"
	); 

	argList::noParallel();

	#include "addRegionOption.H"

	argList::validArgs.append("interpolationScheme");

	#include "setRootCase.H"
    #include "createTime.H"
	// Allow override of time 
    //instantList timeDirs = timeSelector::select0(runTime, args);
	instantList times = timeSelector::selectIfPresent(runTime, args);
	Info << times << endl;

   	//Arguments
	word interpolationScheme 			= args[1];

	#include "createMesh.H"
	
	#include "getDataFromCyclicPatches.H"
	#include "createExtrudedMesh.H"
	#include "readFields.H"
	#include "extrudeFields.H"
 
	Info<< "End" << endl;

    return 0;
}


// ************************************************************************* //
