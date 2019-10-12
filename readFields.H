//Read field matching the the following Regex expression ".*.Mean"
//Create the new field *AvgCyclicDir using the extruded mesh

Info << "----------------------------------------" << endl;
Info << "########## < READING_FIELDS > ##########" << endl;
Info << "----------------------------------------" << endl;

IOobjectList objects(mesh, runTime.timeName());
wordRe wordre("^.*.Mean", wordRe::REGEXP);
//wordReList wordrelist{{"^.*.Mean", wordRe::REGEXP}};
//wordRes wrelist(wordrelist);
//IOobjectList filtered_objects = objects.lookup(wrelist);
IOobjectList filtered_objects = objects.lookup(wordre);

//volScalarFields
PtrList<volScalarField> vsFlds, vsFldsAvgCyclicDir;
wordList vsFldsNames
	= ReadFields(mesh,filtered_objects,vsFlds);

//volVectorFields
PtrList<volVectorField> vvFlds, vvFldsAvgCyclicDir;
wordList vvFldsNames
	= ReadFields(mesh, filtered_objects, vvFlds);

//volSphericalTensorField
PtrList<volSphericalTensorField> vstFlds, vstFldsAvgCyclicDir;
wordList vstFldsNames
	= ReadFields(mesh, filtered_objects, vstFlds);

//volSymmTensorField
PtrList<volSymmTensorField> vsymtFlds, vsymtFldsAvgCyclicDir;
wordList vsymtFldsNames 
	= ReadFields(mesh, filtered_objects, vsymtFlds);

//volTensorField
PtrList<volTensorField> vtFlds,vtFldsAvgCyclicDir;
wordList vtFldsNames 
	= ReadFields(mesh, filtered_objects, vtFlds);

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
