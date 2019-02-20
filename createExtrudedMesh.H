//The mesh use by the average variables is created.
//It is generated from an extrusion of a cuttingPlane.
//nLayers = 1, length=OriginalLength"

Info << "----------------------------------------" << endl;
Info << "####### < CREATE_EXTRUDED_MESH > #######" << endl;
Info << "----------------------------------------" << endl;

bool triangulate = false;
vector basePoint = mesh.cellCentres()[0];                                   
basePoint[sD] = sLMin+sDInc*0.5;                                            
cuttingPlane SurfaceRef(plane(basePoint,sN),mesh,triangulate,labelUList::null());

MeshedSurface<face> pPatch(SurfaceRef);
EdgeMap<label> edgeRegionMap;
wordList patchNames(1, "default");
labelList patchSizes(1, pPatch.nEdges() - pPatch.nInternalEdges());

const edgeList& edges = pPatch.edges();                                
forAll(edges, edgeI)                                                    
{                                                                       
	if(!pPatch.isInternalEdge(edgeI))                                 
	{                                                                   
		edgeRegionMap.insert(edges[edgeI], 0);                          
	}                                                                   
}                                                                       

patchToPoly2DMesh poly2DMesh                                            
	(                                                                       
	 pPatch,
	 patchNames,
	 patchSizes,
	 edgeRegionMap
	);

poly2DMesh.createMesh();

polyMesh extrudedMesh(
    IOobject
    (
        avgCyclicDirFolder/"constant",
        runTime.constant(),
        runTime,
        IOobject::NO_READ,
        IOobject::NO_WRITE,
        false
    ),
    xferMove(poly2DMesh.points()),
    xferMove(poly2DMesh.faces()),
    xferMove(poly2DMesh.owner()),
    xferMove(poly2DMesh.neighbour())
);

List<polyPatch*> patches(poly2DMesh.patchNames().size());
forAll(patches, patchi)
{
    patches[patchi] = new polyPatch
    (
        poly2DMesh.patchNames()[patchi],
        poly2DMesh.patchSizes()[patchi],
        poly2DMesh.patchStarts()[patchi],
        patchi,
        mesh.boundaryMesh(),
        polyPatch::typeName
    );
}
extrudedMesh.addPatches(patches);

dictionary extrude2DMeshDict;
extrude2DMeshDict.add("extrudeModel","linearDirection");
extrude2DMeshDict.add("patchType","empty");
extrude2DMeshDict.add("nLayers",1);
extrude2DMeshDict.add("expansionRatio",1.0);

dictionary linearDirectionCoeffsDict;
linearDirectionCoeffsDict.add("direction",sN);
linearDirectionCoeffsDict.add("thickness",sL);
extrude2DMeshDict.add("linearDirectionCoeffs",linearDirectionCoeffsDict);

autoPtr<extrudeModel> model(extrudeModel::New(extrude2DMeshDict));

// Engine to extrude mesh
extrude2DMesh extruder(extrudedMesh, extrude2DMeshDict, model());

extruder.addFrontBackPatches();

polyTopoChange meshMod(extrudedMesh.boundaryMesh().size());

extruder.setRefinement(meshMod);

autoPtr<mapPolyMesh> morphMap = meshMod.changeMesh(extrudedMesh, false);        
//
extrudedMesh.updateMesh(morphMap);

{
    edgeCollapser collapser(extrudedMesh);

    const edgeList& edges = extrudedMesh.edges();
    const pointField& points = extrudedMesh.points();

    const boundBox& bb = extrudedMesh.bounds();
    const scalar mergeDim = 1e-4 * bb.minDim();

    PackedBoolList collapseEdge(extrudedMesh.nEdges());
    Map<point> collapsePointToLocation(extrudedMesh.nPoints());

    forAll(edges, edgeI)
    {
        const edge& e = edges[edgeI];

        scalar d = e.mag(points);

        if (d < mergeDim)
        {
            Info<< "Merging edge " << e << " since length " << d
                << " << " << mergeDim << nl;

            collapseEdge[edgeI] = true;
            collapsePointToLocation.set(e[1], points[e[0]]);
        }
    }

    List<pointEdgeCollapse> allPointInfo;
    const globalIndex globalPoints(extrudedMesh.nPoints());
    labelList pointPriority(extrudedMesh.nPoints(), 0);

    collapser.consistentCollapse
    (
        globalPoints,
        pointPriority,
        collapsePointToLocation,
        collapseEdge,
        allPointInfo
    );

    polyTopoChange meshModCollapse(extrudedMesh);

    collapser.setRefinement(allPointInfo, meshModCollapse);

    // Create a mesh from topo changes.
    autoPtr<mapPolyMesh> morphMap
        = meshModCollapse.changeMesh(extrudedMesh, false);

    extrudedMesh.updateMesh(morphMap);
}

extrudedMesh.write();

Foam::fvMesh extrudedFvMesh                                                             
(                                                                               
    IOobject
    (
        avgCyclicDirFolder/"constant",
        runTime.constant(),
        runTime,
        IOobject::NO_READ,
        IOobject::NO_WRITE,
        false
    ),
    xferCopy(extrudedMesh.points()),
	xferCopy(extrudedMesh.faces()),
	xferCopy(extrudedMesh.cells())
); 

const polyBoundaryMesh& extrudedMeshPatches = extrudedMesh.boundaryMesh();
List<polyPatch*> newPatches(extrudedMeshPatches.size());
forAll(newPatches, patchi)
{
	Info << "Adding Patches: " << patchi << endl;
	Info << extrudedMeshPatches[patchi].name() << "\t" <<
        extrudedMeshPatches[patchi].size() << "\t" <<
        extrudedMeshPatches[patchi].start() << "\t" << 
        extrudedMeshPatches[patchi].type() << "\t" << 
		polyPatch::typeName << endl;

	if(extrudedMeshPatches[patchi].type()=="empty")
    	newPatches[patchi] = new emptyPolyPatch
    	(
    	    extrudedMeshPatches[patchi].name(),
    	    extrudedMeshPatches[patchi].size(),
    	    extrudedMeshPatches[patchi].start(),
    	    patchi,
    	    extrudedMesh.boundaryMesh(),
    	    extrudedMeshPatches[patchi].type()
    	);
	else if(extrudedMeshPatches[patchi].type()=="patch")
    	newPatches[patchi] = new polyPatch
    	(
    	    extrudedMeshPatches[patchi].name(),
    	    extrudedMeshPatches[patchi].size(),
    	    extrudedMeshPatches[patchi].start(),
    	    patchi,
    	    extrudedMesh.boundaryMesh(),
    	    extrudedMeshPatches[patchi].type()
    	);

    /*newPatches[patchi] = new polyPatch
    (
        extrudedMeshPatches[patchi].name(),
        extrudedMeshPatches[patchi].size(),
        extrudedMeshPatches[patchi].start(),
        patchi,
        extrudedMesh.boundaryMesh(),
        extrudedMeshPatches[patchi].type()
    );*/

	Info << "final: " << newPatches[patchi]->type() << endl;
}
extrudedFvMesh.addFvPatches(newPatches);

Info << endl;
