//"Extruding fields into the new extruded mesh."

Info << "----------------------------------------" << endl;
Info << "######### < EXTRUDING_FIELDS > #########" << endl;
Info << "----------------------------------------" << endl;

//Extruding Fields into the new extrudedMesh
//volScalarFields
ExtrudeFields(extrudeFieldsDict,mesh,vsFlds,extrudedFvMesh,vsFldsAvgCyclicDir);
//volVectorFields
ExtrudeFields(extrudeFieldsDict,mesh,vvFlds,extrudedFvMesh,vvFldsAvgCyclicDir);
//volSphericalTensorField
ExtrudeFields(extrudeFieldsDict,mesh,vstFlds,extrudedFvMesh,vstFldsAvgCyclicDir);
//volSymmTensorField
ExtrudeFields(extrudeFieldsDict,mesh,vsymtFlds,extrudedFvMesh,vsymtFldsAvgCyclicDir);
//volTensorField
ExtrudeFields(extrudeFieldsDict,mesh,vtFlds,extrudedFvMesh,vtFldsAvgCyclicDir);

Info << endl;
