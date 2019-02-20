namespace Foam {
	template<class Type, template<class> class PatchField, class GeoMesh>
	wordList ReadFields
	(
	 const typename GeoMesh::Mesh& mesh,
	 const IOobjectList& objects,
	 PtrList<GeometricField<Type, PatchField, GeoMesh>>& fields,
	 wordRes wrelist
	)
	{
		typedef GeometricField<Type, PatchField, GeoMesh> GeoField;
		Info << "\nREADING [" << GeoField::typeName << "] " << endl; 
		// Search list of objects for wanted type
		IOobjectList fieldObjects(objects.lookupClass(GeoField::typeName));

		const wordList masterNames(objects.sortedNames(GeoField::typeName, wrelist));

		fields.setSize(masterNames.size());

		// Make sure to read in masterNames order.
		forAll(masterNames, i)
		{
			Info<< "* " << masterNames[i] << endl;

			const IOobject& io = *fieldObjects[masterNames[i]];

			fields.set
				(
				 i,
				 new GeoField
				 (
				  IOobject
				  (
				   io.name(),
				   io.instance(),
				   io.local(),
				   io.db(),
				   IOobject::MUST_READ,
				   IOobject::NO_WRITE,
				   io.registerObject()
				  ),
				  mesh
				 )
				);
		}
		return masterNames;
	}

	template<class Type, template<class> class PatchField, class GeoMesh>
	void CreateFields
	(
		const typename GeoMesh::Mesh& extrudedFvMesh,
		PtrList<GeometricField<Type, PatchField, GeoMesh>>& fieldsAvgCyclicDir,
		const PtrList<GeometricField<Type, PatchField, GeoMesh>>& fields
	)
	{
		typedef GeometricField<Type, PatchField, GeoMesh> GeoField;
		Info << "\nCREATING [" << GeoField::typeName << "] " << endl; 

		fieldsAvgCyclicDir.setSize(fields.size());

		forAll(fieldsAvgCyclicDir, i)
		{
			string varName = string(fields[i].name());
			varName.removeEnd("Mean");
			varName+="AvgCyclicDir";
			Info<< "* " << varName << endl;

			fieldsAvgCyclicDir.set
			(
			 i,
			 new GeoField
			 (
			  IOobject
			  (
			   fileName(varName),
			   extrudedFvMesh.time().timeName(),
			   avgCyclicDirFolder/extrudedFvMesh.time().timeName(),
			   extrudedFvMesh.time(),
			   IOobject::NO_READ,
			   IOobject::AUTO_WRITE
			  ),
			  extrudedFvMesh,
			  fields[i].dimensions()
			  )
			);
		}
	}

	template<class Type, template<class> class PatchField, class GeoMesh>
	void ExtrudeFields
	(
		const dictionary& extrudeFieldsDict,
		const typename GeoMesh::Mesh& mesh,
		const PtrList<GeometricField<Type, PatchField, GeoMesh>>& fields,
		const typename GeoMesh::Mesh& extrudedFvMesh,
		PtrList<GeometricField<Type, PatchField, GeoMesh>>& fieldsAvgCyclicDir
	)
	{
		typedef GeometricField<Type, PatchField, GeoMesh> GeoField;
		Info << "\nEXTRUDING [" << GeoField::typeName << "] " << endl; 

		autoPtr<interpolation<Type>> interpolatorPtr;
		Field<Type> avgField;
		int counter(0);

		word interpolationScheme	= extrudeFieldsDict.lookup("interpolationScheme");

		scalar sLMax 	= readScalar(extrudeFieldsDict["sLMax"]);
		scalar sLMin 	= readScalar(extrudeFieldsDict["sLMin"]);
		scalar sL 		= readScalar(extrudeFieldsDict["sL"]);
		scalar sDInc	= readScalar(extrudeFieldsDict["sDInc"]);
		scalar sD 		= readScalar(extrudeFieldsDict["sD"]);
		vector sN 		= extrudeFieldsDict["sN"];

		bool triangulate = false;

		forAll(fields,iFld){
			Info << "* " << fields[iFld].name() << "\t->\t" << fieldsAvgCyclicDir[iFld].name() << endl;

			autoPtr<interpolation<Type>> interpolatorPtr
				= interpolation<Type>::New(interpolationScheme,fields[iFld]);

			for(scalar sLV=sLMin+sDInc*0.5; sLV<=sLMax; sLV+=sDInc)
			{
				counter+=1;
				vector basePoint = mesh.cellCentres()[0];  
				basePoint[sD] = sLV;
				plane planeRef(basePoint,sN);

				cuttingPlane SurfaceSource(planeRef,mesh,triangulate,labelUList::null());

				Field<Type> values(SurfaceSource.meshCells().size());

				forAll(values, valuei)
				{
					values[valuei] = interpolatorPtr().interpolate                       
						(
						 mesh.cellCentres()[SurfaceSource.meshCells()[valuei]],
						 SurfaceSource.meshCells()[valuei]
						); 
				}
				if(counter==1)
					avgField = values;
				else 
					avgField += values;
			}
			avgField/=counter;

			vector basePoint = mesh.cellCentres()[0];  
			basePoint[sD] = sLMin+sDInc*0.5;
			cuttingPlane SurfaceRef(plane(basePoint,sN),mesh,triangulate,labelUList::null());
			
			const vectorField& Cf = SurfaceRef.Cf();
			
			//Internal Field
			forAll(Cf, facei)
			{
				vector c = Cf[facei];
				vector pi(c.x(),c.y(),c.z());
				pi[sD]=sL*0.5;
				
				//find cell
				label celli    = -1;
				label tetFacei = -1;
				label tetPti   = -1;
				extrudedFvMesh.findCellFacePt
					(
					 pi,
					 celli,
					 tetFacei,
					 tetPti
					);
				if (celli >= 0)
					fieldsAvgCyclicDir[iFld][celli] = avgField[facei];
			}
			
			//Boundary Field
			const polyBoundaryMesh& extrudedMeshPatches = extrudedFvMesh.boundaryMesh();
			forAll(extrudedMeshPatches,patchi){
				Field<Type>& fw = fieldsAvgCyclicDir[iFld].boundaryFieldRef()[patchi];
				forAll(fw,facei){
					fw[facei] *= scalar(Zero);
				}
			}

			fieldsAvgCyclicDir[iFld].write();
		}
	}
}
