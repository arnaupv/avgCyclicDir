# OpenFOAM post-processing utility: avgCyclicDir
This OpenFOAM post-processing utility is designed for carrying out a spatial averaging in the cyclic direction (if any). The averaged data is saved using a new mesh with a unique control volume in the cyclic direction, considerably reducing the required amount of memory.

**Instalation**
* Use ./Allwmake

**Capabilities**
* Spatial averaging along a cyclic direction. 
* Cyclic direction automatically detected. Any cyclic direction should be allowed (Please report any issue). 
* A new mesh~(called *extrudedMesh*) with a unique control volume in the cyclic direction is created. This part of the code is mostly based on the *extrude2DMesh* tool available by default in OpenFOAM.

**How to use ?**
* The only argument required by this tool is the *interpolationScheme*. 
* It has to be executed in the main OpenFOAM folder.
  For instance: 
  	avgCyclicDir cellPoint

* The tool only postprocess time averaged fields (Following extension: "\*Mean").
* Once executed, a folder located at "*./postProcessing/avgCyclicDir*" is created containing the extruded mesh with the interpolated fields.
* For visualizing purposes, a symbolic link for the system folder is required by paraFOAM. Execute the following commands from the OpenFOAM project folder: 
    cd ./postProcessing/avgCyclicDir
	ln -s ../../system

**Limitiations :**
* Available branches compatible for OpenFOAM v1706 and OpenFOAM-4.1 [Other OpenFOAM versions have not been tested].
* This tool is not yet paralellized.
* Only one cyclic direction is allowed.
* The spatial discretitzation in the cyclic direction should be uniform. Otherwise
the tool will not work, or will provide wrong results.
* Currently, all patch values are set to zero.

**Furtherwork :**
* Tool parallelitzation
* Fix patch zero values
* Mulitple cyclic directions.