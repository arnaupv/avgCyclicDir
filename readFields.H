//Read field matching the the following Regex expression ".*.Mean"
//Create the new field *AvgCyclicDir using the extruded mesh

Info << "----------------------------------------" << endl;
Info << "########## < READING_FIELDS > ##########" << endl;
Info << "----------------------------------------" << endl;

IOobjectList objects(mesh, runTime.timeName());
wordReList wordrelist{{"^.*.Mean", wordRe::REGEX}};
wordRes wrelist(wordrelist);

//volScalarFields
PtrList<volScalarField> vsFlds, vsFldsAvgCyclicDir;
wordList vsFldsNames
	= ReadFields(mesh,objects,vsFlds,wrelist);

//volVectorFields
PtrList<volVectorField> vvFlds, vvFldsAvgCyclicDir;
wordList vvFldsNames
	= ReadFields(mesh, objects, vvFlds,wrelist);

//volSphericalTensorField
PtrList<volSphericalTensorField> vstFlds, vstFldsAvgCyclicDir;
wordList vstFldsNames
	= ReadFields(mesh, objects, vstFlds, wrelist);

//volSymmTensorField
PtrList<volSymmTensorField> vsymtFlds, vsymtFldsAvgCyclicDir;
wordList vsymtFldsNames 
	= ReadFields(mesh, objects, vsymtFlds, wrelist);

//volTensorField
PtrList<volTensorField> vtFlds,vtFldsAvgCyclicDir;
wordList vtFldsNames 
	= ReadFields(mesh, objects, vtFlds, wrelist);

// CREATING *AvgCyclicDir;

//volScalarFields
CreateFields(extrudedFvMesh,vsFldsAvgCyclicDir,vsFlds);
//volVectorFields
CreateFields(extrudedFvMesh,vvFldsAvgCyclicDir,vvFlds);
//volSphericalTensorField
CreateFields(extrudedFvMesh,vstFldsAvgCyclicDir,vstFlds);
//volSymmTensorField
CreateFields(extrudedFvMesh,vsymtFldsAvgCyclicDir,vsymtFlds);
//volTensorField
CreateFields(extrudedFvMesh,vtFldsAvgCyclicDir,vtFlds);

Info << endl;
