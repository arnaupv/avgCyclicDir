//Getting data from cyclic patches (if any)

Info << "----------------------------------------" << endl;
Info << "### < GET_DATA_FROM_CYCLIC_PATCHES > ###" << endl;
Info << "----------------------------------------" << endl;

word cycPatchName, nbrPatchName;
scalar sL(0.0), sD(0.0);
vector sN(0,0,0);

const polyBoundaryMesh& mainPatches = mesh.boundaryMesh();                      
forAll(mainPatches, patchi)                                                     
{                                                                           
	if                                                                     
	(                                                                       
		isA<cyclicPolyPatch>(mainPatches[patchi])                               
		&& refCast<const cyclicPolyPatch>(mainPatches[patchi]).owner()             
	)
	{
		const cyclicPolyPatch& cycPatch =                                   
			refCast<const cyclicPolyPatch>(mainPatches[patchi]);

		const cyclicPolyPatch& nbrPatch = cycPatch.neighbPatch();           
		const vector& s = cycPatch.separation()[0];

		cycPatchName = cycPatch.name();
		nbrPatchName = nbrPatch.name();
		sL = mag(s); 
		sN = s/sL; 
		sD = mag(sN&vector(1,0,0)) > 1e-10 ? 0 : 
		  	  mag(sN&vector(0,1,0)) > 1e-10 ? 1 : 2; 
	}
}

if(sL==0.0){
	Info << "ERROR: Cyclic Patches have not been detected!" << endl;
	return 0;
} else
	Info << "[CyclicPatches]" << "\t" 
	     << cycPatchName << "-" 
	 	 << nbrPatchName << endl; 

//min-max values cyclic direction averaging                                            
scalar sLMin = boundBox(mesh.points()).min()[sD];
scalar sLMax = boundBox(mesh.points()).max()[sD];

const faceList & ff   = mesh.faces();
const pointField & pp = mesh.points();

const cell & cc = mesh.cells()[0];
labelList pLabels(cc.labels(ff));
pointField pLocal(pLabels.size(), vector::zero);

forAll (pLabels, pointi)
	pLocal[pointi] = pp[pLabels[pointi]];

scalar sDInc = Foam::max(pLocal & sN) - Foam::min(pLocal & sN);
scalar N = (sLMax-sLMin)/sDInc;
N = ceil(N)-N<N-floor(N)?ceil(N):floor(N);

Info << "[CyclicDirection]" << "\t" 
     << sLMin << "-" 
 	 << sLMax << ":\t"
 	 << sL << "\t" 
 	 << sN << "\t" 
 	 << sD << "\t" 
 	 << sDInc << "\t" 
 	 << N << endl; 

dictionary extrudeFieldsDict;

extrudeFieldsDict.add("interpolationScheme","cellPoint");
extrudeFieldsDict.add("sLMax",sLMax);
extrudeFieldsDict.add("sLMin",sLMin);
extrudeFieldsDict.add("sL",sL);
extrudeFieldsDict.add("sDInc",sDInc);
extrudeFieldsDict.add("sD",sD);
extrudeFieldsDict.add("sN",sN);

Info << endl;
