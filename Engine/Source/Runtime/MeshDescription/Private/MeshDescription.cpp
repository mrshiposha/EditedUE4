// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "MeshDescription.h"
#include "Misc/SecureHash.h"
#include "Serialization/MemoryWriter.h"
#include "MeshAttributes.h"
#include "Serialization/ObjectWriter.h"


UMeshDescription::UMeshDescription()
{
}


void UMeshDescription::Serialize( FArchive& Ar )
{
	Super::Serialize( Ar );
	Ar << VertexArray;
	Ar << VertexInstanceArray;
	Ar << EdgeArray;
	Ar << PolygonArray;
	Ar << PolygonGroupArray;

	Ar << VertexAttributesSet;
	Ar << VertexInstanceAttributesSet;
	Ar << EdgeAttributesSet;
	Ar << PolygonAttributesSet;
	Ar << PolygonGroupAttributesSet;
}

void UMeshDescription::Empty()
{
	VertexArray.Reset();
	VertexInstanceArray.Reset();
	EdgeArray.Reset();
	PolygonArray.Reset();
	PolygonGroupArray.Reset();

	//Empty all attributes
	VertexAttributesSet.Initialize( 0 );
	VertexInstanceAttributesSet.Initialize( 0 );
	EdgeAttributesSet.Initialize( 0 );
	PolygonAttributesSet.Initialize( 0 );
	PolygonGroupAttributesSet.Initialize( 0 );
}

#if WITH_EDITORONLY_DATA
FString UMeshDescription::GetIdString()
{
	//The serialisation of the sparse array can be different with the same data, so to get a unique identifier we need to take the data with no IDs(FVertexID...)
	FSHA1 Sha;
	TArray<uint8> TempBytes;

	TVertexAttributeArray<FVector>& VertexPositions = VertexAttributes().GetAttributes<FVector>(MeshAttribute::Vertex::Position);
	TVertexAttributeArray<float>& VertexCornerSharpness = VertexAttributes().GetAttributes<float>(MeshAttribute::Vertex::CornerSharpness);

	TVertexInstanceAttributeIndicesArray<FVector2D>& TextureCoordinates = VertexInstanceAttributes().GetAttributesSet<FVector2D>(MeshAttribute::VertexInstance::TextureCoordinate);
	TVertexInstanceAttributeArray<FVector>& VertexInstanceNormals = VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Normal);
	TVertexInstanceAttributeArray<FVector>& VertexInstanceTangents = VertexInstanceAttributes().GetAttributes<FVector>(MeshAttribute::VertexInstance::Tangent);
	TVertexInstanceAttributeArray<float>& VertexInstanceBiNormals = VertexInstanceAttributes().GetAttributes<float>(MeshAttribute::VertexInstance::BinormalSign);
	TVertexInstanceAttributeArray<FVector4>& VertexInstanceColors = VertexInstanceAttributes().GetAttributes<FVector4>(MeshAttribute::VertexInstance::Color);

	TEdgeAttributeArray<bool>& EdgeHards = EdgeAttributes().GetAttributes<bool>(MeshAttribute::Edge::IsHard);
	TEdgeAttributeArray<float>& EdgeCreaseSharpness = EdgeAttributes().GetAttributes<float>(MeshAttribute::Edge::CreaseSharpness);

	TPolygonAttributeArray<FVector>& PolygonNormals = PolygonAttributes().GetAttributes<FVector>(MeshAttribute::Polygon::Normal);
	TPolygonAttributeArray<FVector>& PolygonTangents = PolygonAttributes().GetAttributes<FVector>(MeshAttribute::Polygon::Tangent);
	TPolygonAttributeArray<FVector>& PolygonBinormals = PolygonAttributes().GetAttributes<FVector>(MeshAttribute::Polygon::Binormal);
	TPolygonAttributeArray<FVector>& PolygonCenters = PolygonAttributes().GetAttributes<FVector>(MeshAttribute::Polygon::Center);

	TPolygonGroupAttributeArray<FName>& PolygonGroupImportedMaterialSlotNames = PolygonGroupAttributes().GetAttributes<FName>(MeshAttribute::PolygonGroup::ImportedMaterialSlotName);
	TPolygonGroupAttributeArray<bool>& PolygonGroupEnableCollisions = PolygonGroupAttributes().GetAttributes<bool>(MeshAttribute::PolygonGroup::EnableCollision);
	TPolygonGroupAttributeArray<bool>& PolygonGroupCastShadows = PolygonGroupAttributes().GetAttributes<bool>(MeshAttribute::PolygonGroup::CastShadow);
	
	auto AddItemToSha = [&](auto ItemSerialization)
	{
		FMemoryWriter Ar(TempBytes);
		ItemSerialization(Ar);
		if (TempBytes.Num() > 0)
		{
			uint8* Buffer = TempBytes.GetData();
			Sha.Update(Buffer, TempBytes.Num());
		}
		TempBytes.Reset();
	};

	//Vertex Identifier
	auto VertexSerializer = [&](FMemoryWriter &Ar)
	{
		for (const FVertexID& ArrayID : Vertices().GetElementIDs())
		{
			const FMeshVertex& MeshItem = GetVertex(ArrayID);
			uint32 ItemCount = MeshItem.ConnectedEdgeIDs.Num();
			Ar.SerializeInt(ItemCount, 0);
			ItemCount = MeshItem.VertexInstanceIDs.Num();
			Ar.SerializeInt(ItemCount, 0);
			Ar << VertexPositions[ArrayID];
			Ar << VertexCornerSharpness[ArrayID];
		}
	};
	AddItemToSha(VertexSerializer);
	
	//Vertex Instance Identifier
	auto VertexInstanceSerializer = [&](FMemoryWriter &Ar)
	{
		for (const FVertexInstanceID& ArrayID : VertexInstances().GetElementIDs())
		{
			const FMeshVertexInstance& MeshItem = GetVertexInstance(ArrayID);
			uint32 ItemCount = MeshItem.ConnectedPolygons.Num();
			Ar.SerializeInt(ItemCount, 0);
			Ar << VertexPositions[MeshItem.VertexID];
			for (int32 UVIndex = 0; UVIndex < TextureCoordinates.GetNumIndices(); ++UVIndex)
			{
				Ar << TextureCoordinates.GetArrayForIndex(UVIndex)[ArrayID];
			}
			Ar << VertexInstanceNormals[ArrayID];
			Ar << VertexInstanceTangents[ArrayID];
			Ar << VertexInstanceBiNormals[ArrayID];
			Ar << VertexInstanceColors[ArrayID];
		}
	};
	AddItemToSha(VertexInstanceSerializer);

	//Edge Identifier
	auto EdgeSerializer = [&](FMemoryWriter &Ar)
	{
		for (const FEdgeID& ArrayID : Edges().GetElementIDs())
		{
			const FMeshEdge& MeshItem = GetEdge(ArrayID);
			uint32 ItemCount = MeshItem.ConnectedPolygons.Num();
			Ar.SerializeInt(ItemCount, 0);
			Ar << VertexPositions[MeshItem.VertexIDs[0]];
			Ar << VertexPositions[MeshItem.VertexIDs[1]];
			Ar << EdgeHards[ArrayID];
			Ar << EdgeCreaseSharpness[ArrayID];
		}
	};
	AddItemToSha(EdgeSerializer);

	//Polygon Identifier
	auto PolygonSerializer = [&](FMemoryWriter &Ar)
	{
		for (const FPolygonID& ArrayID : Polygons().GetElementIDs())
		{
			const FMeshPolygon& MeshItem = GetPolygon(ArrayID);
			uint32 ItemCount = MeshItem.HoleContours.Num();
			Ar.SerializeInt(ItemCount, 0);
			for(const FMeshPolygonContour& MeshPolygonContour : MeshItem.HoleContours)
			{
				ItemCount = MeshPolygonContour.VertexInstanceIDs.Num();
				Ar.SerializeInt(ItemCount, 0);
				for (const FVertexInstanceID& VertexInstanceID : MeshPolygonContour.VertexInstanceIDs)
				{
					Ar << VertexPositions[GetVertexInstance(VertexInstanceID).VertexID];
				}
			}
			ItemCount = MeshItem.PerimeterContour.VertexInstanceIDs.Num();
			Ar.SerializeInt(ItemCount, 0);
			for (const FVertexInstanceID& VertexInstanceID : MeshItem.PerimeterContour.VertexInstanceIDs)
			{
				Ar << VertexPositions[GetVertexInstance(VertexInstanceID).VertexID];
			}
			FPolygonGroupID PolygonGroupId = MeshItem.PolygonGroupID;
			ItemCount = (uint32)PolygonGroupId.GetValue();
			Ar.SerializeInt(ItemCount, 0);

			Ar << GetPolygon(ArrayID);
			Ar << PolygonNormals[ArrayID];
			Ar << PolygonTangents[ArrayID];
			Ar << PolygonBinormals[ArrayID];
			Ar << PolygonCenters[ArrayID];
		}
	};
	AddItemToSha(PolygonSerializer);

	//PolygonGroup Identifier
	auto PolygonGroupSerializer = [&](FMemoryWriter &Ar)
	{
		for (const FPolygonGroupID& ArrayID : PolygonGroups().GetElementIDs())
		{
			const FMeshPolygonGroup& MeshItem = GetPolygonGroup(ArrayID);
			uint32 ItemCount = MeshItem.Polygons.Num();
			Ar.SerializeInt(ItemCount, 0);
			Ar << PolygonGroupImportedMaterialSlotNames[ArrayID];
			Ar << PolygonGroupEnableCollisions[ArrayID];
			Ar << PolygonGroupCastShadows[ArrayID];
		}
	};
	AddItemToSha(PolygonGroupSerializer);
	
	Sha.Final();
	TempBytes.Empty();
	// Retrieve the hash and use it to construct a pseudo-GUID.
	uint32 Hash[5];
	Sha.GetHash((uint8*)Hash);
	FGuid Guid = FGuid(Hash[0] ^ Hash[4], Hash[1], Hash[2], Hash[3]);
	return Guid.ToString(EGuidFormats::Digits);
}
#endif //WITH_EDITORONLY_DATA

void UMeshDescription::Compact( FElementIDRemappings& OutRemappings )
{
	VertexArray.Compact( OutRemappings.NewVertexIndexLookup );
	VertexInstanceArray.Compact( OutRemappings.NewVertexInstanceIndexLookup );
	EdgeArray.Compact( OutRemappings.NewEdgeIndexLookup );
	PolygonArray.Compact( OutRemappings.NewPolygonIndexLookup );
	PolygonGroupArray.Compact( OutRemappings.NewPolygonGroupIndexLookup );

	RemapAttributes( OutRemappings );
	FixUpElementIDs( OutRemappings );
}


void UMeshDescription::Remap( const FElementIDRemappings& Remappings )
{
	VertexArray.Remap( Remappings.NewVertexIndexLookup );
	VertexInstanceArray.Remap( Remappings.NewVertexInstanceIndexLookup );
	EdgeArray.Remap( Remappings.NewEdgeIndexLookup );
	PolygonArray.Remap( Remappings.NewPolygonIndexLookup );
	PolygonGroupArray.Remap( Remappings.NewPolygonGroupIndexLookup );

	RemapAttributes( Remappings );
	FixUpElementIDs( Remappings );
}


void UMeshDescription::FixUpElementIDs( const FElementIDRemappings& Remappings )
{
	for( const FVertexID VertexID : VertexArray.GetElementIDs() )
	{
		FMeshVertex& Vertex = VertexArray[ VertexID ];

		// Fix up vertex instance index references in vertices array
		for( FVertexInstanceID& VertexInstanceID : Vertex.VertexInstanceIDs )
		{
			VertexInstanceID = Remappings.GetRemappedVertexInstanceID( VertexInstanceID );
		}

		// Fix up edge index references in the vertex array
		for( FEdgeID& EdgeID : Vertex.ConnectedEdgeIDs )
		{
			EdgeID = Remappings.GetRemappedEdgeID( EdgeID );
		}
	}

	// Fix up vertex index references in vertex instance array
	for( const FVertexInstanceID VertexInstanceID : VertexInstanceArray.GetElementIDs() )
	{
		FMeshVertexInstance& VertexInstance = VertexInstanceArray[ VertexInstanceID ];

		VertexInstance.VertexID = Remappings.GetRemappedVertexID( VertexInstance.VertexID );

		for( FPolygonID& PolygonID : VertexInstance.ConnectedPolygons )
		{
			PolygonID = Remappings.GetRemappedPolygonID( PolygonID );
		}
	}

	for( const FEdgeID EdgeID : EdgeArray.GetElementIDs() )
	{
		FMeshEdge& Edge = EdgeArray[ EdgeID ];

		// Fix up vertex index references in Edges array
		for( int32 Index = 0; Index < 2; Index++ )
		{
			Edge.VertexIDs[ Index ] = Remappings.GetRemappedVertexID( Edge.VertexIDs[ Index ] );
		}

		// Fix up references to section indices
		for( FPolygonID& ConnectedPolygon : Edge.ConnectedPolygons )
		{
			ConnectedPolygon = Remappings.GetRemappedPolygonID( ConnectedPolygon );
		}
	}

	for( const FPolygonID PolygonID : PolygonArray.GetElementIDs() )
	{
		FMeshPolygon& Polygon = PolygonArray[ PolygonID ];

		// Fix up references to vertex indices in section polygons' contours
		for( FVertexInstanceID& VertexInstanceID : Polygon.PerimeterContour.VertexInstanceIDs )
		{
			VertexInstanceID = Remappings.GetRemappedVertexInstanceID( VertexInstanceID );
		}

		for( FMeshPolygonContour& HoleContour : Polygon.HoleContours )
		{
			for( FVertexInstanceID& VertexInstanceID : HoleContour.VertexInstanceIDs )
			{
				VertexInstanceID = Remappings.GetRemappedVertexInstanceID( VertexInstanceID );
			}
		}

		for( FMeshTriangle& Triangle : Polygon.Triangles )
		{
			for( int32 TriangleVertexNumber = 0; TriangleVertexNumber < 3; ++TriangleVertexNumber )
			{
				const FVertexInstanceID OriginalVertexInstanceID = Triangle.GetVertexInstanceID( TriangleVertexNumber );
				const FVertexInstanceID NewVertexInstanceID = Remappings.GetRemappedVertexInstanceID( OriginalVertexInstanceID );
				Triangle.SetVertexInstanceID( TriangleVertexNumber, NewVertexInstanceID );
			}
		}

		Polygon.PolygonGroupID = Remappings.GetRemappedPolygonGroupID( Polygon.PolygonGroupID );
	}

	for( const FPolygonGroupID PolygonGroupID : PolygonGroupArray.GetElementIDs() )
	{
		FMeshPolygonGroup& PolygonGroup = PolygonGroupArray[ PolygonGroupID ];

		for( FPolygonID& Polygon : PolygonGroup.Polygons )
		{
			Polygon = Remappings.GetRemappedPolygonID( Polygon );
		}
	}
}


void UMeshDescription::RemapAttributes( const FElementIDRemappings& Remappings )
{
	VertexAttributesSet.Remap( Remappings.NewVertexIndexLookup );
	VertexInstanceAttributesSet.Remap( Remappings.NewVertexInstanceIndexLookup );
	EdgeAttributesSet.Remap( Remappings.NewEdgeIndexLookup );
	PolygonAttributesSet.Remap( Remappings.NewPolygonIndexLookup );
	PolygonGroupAttributesSet.Remap( Remappings.NewPolygonGroupIndexLookup );
}


bool UMeshDescription::IsVertexOrphaned( const FVertexID VertexID ) const
{
	for( const FVertexInstanceID VertexInstanceID : GetVertex( VertexID ).VertexInstanceIDs )
	{
		if( GetVertexInstance( VertexInstanceID ).ConnectedPolygons.Num() > 0 )
		{
			return false;
		}
	}

	return true;
}


void UMeshDescription::GetPolygonPerimeterVertices( const FPolygonID PolygonID, TArray<FVertexID>& OutPolygonPerimeterVertexIDs ) const
{
	const FMeshPolygon& Polygon = GetPolygon( PolygonID );

	OutPolygonPerimeterVertexIDs.SetNumUninitialized( Polygon.PerimeterContour.VertexInstanceIDs.Num(), false );

	int32 Index = 0;
	for( const FVertexInstanceID VertexInstanceID : Polygon.PerimeterContour.VertexInstanceIDs )
	{
		const FMeshVertexInstance& VertexInstance = VertexInstanceArray[ VertexInstanceID ];
		OutPolygonPerimeterVertexIDs[ Index ] = VertexInstance.VertexID;
		Index++;
	}
}


void UMeshDescription::GetPolygonHoleVertices( const FPolygonID PolygonID, const int32 HoleIndex, TArray<FVertexID>& OutPolygonHoleVertexIDs ) const
{
	const FMeshPolygon& Polygon = GetPolygon( PolygonID );

	OutPolygonHoleVertexIDs.SetNumUninitialized( Polygon.HoleContours[ HoleIndex ].VertexInstanceIDs.Num(), false );

	int32 Index = 0;
	for( const FVertexInstanceID VertexInstanceID : Polygon.HoleContours[ HoleIndex ].VertexInstanceIDs )
	{
		const FMeshVertexInstance& VertexInstance = VertexInstanceArray[ VertexInstanceID ];
		OutPolygonHoleVertexIDs[ Index ] = VertexInstance.VertexID;
		Index++;
	}
}


/** Given three direction vectors, indicates if A and B are on the same 'side' of Vec. */
bool UMeshDescription::VectorsOnSameSide(const FVector& Vec, const FVector& A, const FVector& B, const float SameSideDotProductEpsilon)
{
	const FVector CrossA = Vec ^ A;
	const FVector CrossB = Vec ^ B;
	float DotWithEpsilon = SameSideDotProductEpsilon + (CrossA | CrossB);
	return !FMath::IsNegativeFloat(DotWithEpsilon);
}

/** Util to see if P lies within triangle created by A, B and C. */
bool UMeshDescription::PointInTriangle(const FVector& A, const FVector& B, const FVector& C, const FVector& P, const float InsideTriangleDotProductEpsilon)
{
	// Cross product indicates which 'side' of the vector the point is on
	// If its on the same side as the remaining vert for all edges, then its inside.	
	if (VectorsOnSameSide(B - A, P - A, C - A, InsideTriangleDotProductEpsilon) &&
		VectorsOnSameSide(C - B, P - B, A - B, InsideTriangleDotProductEpsilon) &&
		VectorsOnSameSide(A - C, P - C, B - C, InsideTriangleDotProductEpsilon))
	{
		return true;
	}
	else
	{
		return false;
	}

}

FPlane UMeshDescription::ComputePolygonPlane(const FPolygonID PolygonID) const
{
	// NOTE: This polygon plane computation code is partially based on the implementation of "Newell's method" from Real-Time 
	//       Collision Detection by Christer Ericson, published by Morgan Kaufmann Publishers, (c) 2005 Elsevier Inc

	// @todo mesheditor perf: For polygons that are just triangles, use a cross product to get the normal fast!
	// @todo mesheditor perf: We could skip computing the plane distance when we only need the normal
	// @todo mesheditor perf: We could cache these computed polygon normals; or just use the normal of the first three vertices' triangle if it is satisfactory in all cases
	// @todo mesheditor: For non-planar polygons, the result can vary. Ideally this should use the actual polygon triangulation as opposed to the arbitrary triangulation used here.

	FVector Centroid = FVector::ZeroVector;
	FVector Normal = FVector::ZeroVector;

	static TArray<FVertexID> PerimeterVertexIDs;
	GetPolygonPerimeterVertices(PolygonID, /* Out */ PerimeterVertexIDs);

	const TVertexAttributeArray<FVector>& VertexPositions = VertexAttributes().GetAttributes<FVector>(MeshAttribute::Vertex::Position);

	// Use 'Newell's Method' to compute a robust 'best fit' plane from the vertices of this polygon
	for (int32 VertexNumberI = PerimeterVertexIDs.Num() - 1, VertexNumberJ = 0; VertexNumberJ < PerimeterVertexIDs.Num(); VertexNumberI = VertexNumberJ, VertexNumberJ++)
	{
		const FVertexID VertexIDI = PerimeterVertexIDs[VertexNumberI];
		const FVector PositionI = VertexPositions[VertexIDI];

		const FVertexID VertexIDJ = PerimeterVertexIDs[VertexNumberJ];
		const FVector PositionJ = VertexPositions[VertexIDJ];

		Centroid += PositionJ;

		Normal.X += (PositionJ.Y - PositionI.Y) * (PositionI.Z + PositionJ.Z);
		Normal.Y += (PositionJ.Z - PositionI.Z) * (PositionI.X + PositionJ.X);
		Normal.Z += (PositionJ.X - PositionI.X) * (PositionI.Y + PositionJ.Y);
	}

	Normal.Normalize();

	// Construct a plane from the normal and centroid
	return FPlane(Normal, FVector::DotProduct(Centroid, Normal) / (float)PerimeterVertexIDs.Num());
}


FVector UMeshDescription::ComputePolygonNormal(const FPolygonID PolygonID) const
{
	// @todo mesheditor: Polygon normals are now computed and cached when changes are made to a polygon.
	// In theory, we can just return that cached value, but we need to check that there is nothing which relies on the value being correct before
	// the cache is updated at the end of a modification.
	const FPlane PolygonPlane = ComputePolygonPlane(PolygonID);
	const FVector PolygonNormal(PolygonPlane.X, PolygonPlane.Y, PolygonPlane.Z);
	return PolygonNormal;
}

void UMeshDescription::ComputePolygonTriangulation(const FPolygonID PolygonID, TArray<FMeshTriangle>& OutTriangles)
{
	// NOTE: This polygon triangulation code is partially based on the ear cutting algorithm described on
	//       page 497 of the book "Real-time Collision Detection", published in 2005.

	struct Local
	{
		// Returns true if the triangle formed by the specified three positions has a normal that is facing the opposite direction of the reference normal
		static inline bool IsTriangleFlipped(const FVector ReferenceNormal, const FVector VertexPositionA, const FVector VertexPositionB, const FVector VertexPositionC)
		{
			const FVector TriangleNormal = FVector::CrossProduct(
				VertexPositionC - VertexPositionA,
				VertexPositionB - VertexPositionA).GetSafeNormal();
			return (FVector::DotProduct(ReferenceNormal, TriangleNormal) <= 0.0f);
		}

	};


	OutTriangles.Reset();

	// @todo mesheditor holes: Does not support triangles with holes yet!
	// @todo mesheditor: Perhaps should always attempt to triangulate by splitting polygons along the shortest edge, for better determinism.

	//	const FMeshPolygon& Polygon = GetPolygon( PolygonID );
	const TArray<FVertexInstanceID>& PolygonVertexInstanceIDs = GetPolygonPerimeterVertexInstances(PolygonID);

	// Polygon must have at least three vertices/edges
	const int32 PolygonVertexCount = PolygonVertexInstanceIDs.Num();
	check(PolygonVertexCount >= 3);

	// First figure out the polygon normal.  We need this to determine which triangles are convex, so that
	// we can figure out which ears to clip
	const FVector PolygonNormal = ComputePolygonNormal(PolygonID);

	// Make a simple linked list array of the previous and next vertex numbers, for each vertex number
	// in the polygon.  This will just save us having to iterate later on.
	static TArray<int32> PrevVertexNumbers, NextVertexNumbers;
	static TArray<FVector> VertexPositions;

	{
		const TVertexAttributeArray<FVector>& MeshVertexPositions = VertexAttributes().GetAttributes<FVector>(MeshAttribute::Vertex::Position);
		PrevVertexNumbers.SetNumUninitialized(PolygonVertexCount, false);
		NextVertexNumbers.SetNumUninitialized(PolygonVertexCount, false);
		VertexPositions.SetNumUninitialized(PolygonVertexCount, false);

		for (int32 VertexNumber = 0; VertexNumber < PolygonVertexCount; ++VertexNumber)
		{
			PrevVertexNumbers[VertexNumber] = VertexNumber - 1;
			NextVertexNumbers[VertexNumber] = VertexNumber + 1;

			VertexPositions[VertexNumber] = MeshVertexPositions[GetVertexInstanceVertex(PolygonVertexInstanceIDs[VertexNumber])];
		}
		PrevVertexNumbers[0] = PolygonVertexCount - 1;
		NextVertexNumbers[PolygonVertexCount - 1] = 0;
	}

	int32 EarVertexNumber = 0;
	int32 EarTestCount = 0;
	for (int32 RemainingVertexCount = PolygonVertexCount; RemainingVertexCount >= 3; )
	{
		bool bIsEar = true;

		// If we're down to only a triangle, just treat it as an ear.  Also, if we've tried every possible candidate
		// vertex looking for an ear, go ahead and just treat the current vertex as an ear.  This can happen when 
		// vertices are colinear or other degenerate cases.
		if (RemainingVertexCount > 3 && EarTestCount < RemainingVertexCount)
		{
			const FVector PrevVertexPosition = VertexPositions[PrevVertexNumbers[EarVertexNumber]];
			const FVector EarVertexPosition = VertexPositions[EarVertexNumber];
			const FVector NextVertexPosition = VertexPositions[NextVertexNumbers[EarVertexNumber]];

			// Figure out whether the potential ear triangle is facing the same direction as the polygon
			// itself.  If it's facing the opposite direction, then we're dealing with a concave triangle
			// and we'll skip it for now.
			if (!Local::IsTriangleFlipped(
				PolygonNormal,
				PrevVertexPosition,
				EarVertexPosition,
				NextVertexPosition))
			{
				int32 TestVertexNumber = NextVertexNumbers[NextVertexNumbers[EarVertexNumber]];

				do
				{
					// Test every other remaining vertex to make sure that it doesn't lie inside our potential ear
					// triangle.  If we find a vertex that's inside the triangle, then it cannot actually be an ear.
					const FVector TestVertexPosition = VertexPositions[TestVertexNumber];
					if (PointInTriangle(
						PrevVertexPosition,
						EarVertexPosition,
						NextVertexPosition,
						TestVertexPosition,
						SMALL_NUMBER))
					{
						bIsEar = false;
						break;
					}

					TestVertexNumber = NextVertexNumbers[TestVertexNumber];
				} while (TestVertexNumber != PrevVertexNumbers[EarVertexNumber]);
			}
			else
			{
				bIsEar = false;
			}
		}

		if (bIsEar)
		{
			// OK, we found an ear!  Let's save this triangle in our output buffer.
			{
				OutTriangles.Emplace();
				FMeshTriangle& Triangle = OutTriangles.Last();

				Triangle.SetVertexInstanceID(0, PolygonVertexInstanceIDs[PrevVertexNumbers[EarVertexNumber]]);
				Triangle.SetVertexInstanceID(1, PolygonVertexInstanceIDs[EarVertexNumber]);
				Triangle.SetVertexInstanceID(2, PolygonVertexInstanceIDs[NextVertexNumbers[EarVertexNumber]]);
			}

			// Update our linked list.  We're effectively cutting off the ear by pointing the ear vertex's neighbors to
			// point at their next sequential neighbor, and reducing the remaining vertex count by one.
			{
				NextVertexNumbers[PrevVertexNumbers[EarVertexNumber]] = NextVertexNumbers[EarVertexNumber];
				PrevVertexNumbers[NextVertexNumbers[EarVertexNumber]] = PrevVertexNumbers[EarVertexNumber];
				--RemainingVertexCount;
			}

			// Move on to the previous vertex in the list, now that this vertex was cut
			EarVertexNumber = PrevVertexNumbers[EarVertexNumber];

			EarTestCount = 0;
		}
		else
		{
			// The vertex is not the ear vertex, because it formed a triangle that either had a normal which pointed in the opposite direction
			// of the polygon, or at least one of the other polygon vertices was found to be inside the triangle.  Move on to the next vertex.
			EarVertexNumber = NextVertexNumbers[EarVertexNumber];

			// Keep track of how many ear vertices we've tested, so that if we exhaust all remaining vertices, we can
			// fall back to clipping the triangle and adding it to our mesh anyway.  This is important for degenerate cases.
			++EarTestCount;
		}
	}

	check(OutTriangles.Num() > 0);
}

void UMeshDescription::TriangulateMesh()
{
	// Perform triangulation directly into mesh polygons
	for (const FPolygonID PolygonID : Polygons().GetElementIDs())
	{
		FMeshPolygon& Polygon = PolygonArray[PolygonID];
		ComputePolygonTriangulation(PolygonID, Polygon.Triangles);
	}
}

void UMeshDescription::ReversePolygonFacing(const FPolygonID PolygonID)
{
	FMeshPolygon& Polygon = GetPolygon(PolygonID);

	//Build a reverse perimeter
	TArray<FVertexInstanceID> ReverseVertexInstanceIDs;
	int32 PerimeterReverseIndex = Polygon.PerimeterContour.VertexInstanceIDs.Num() - 1;
	for(const FVertexInstanceID& VertexInstanceID : Polygon.PerimeterContour.VertexInstanceIDs)
	{
		ReverseVertexInstanceIDs[PerimeterReverseIndex] = VertexInstanceID;
		PerimeterReverseIndex--;
	}
	//Assign the reverse perimeter
	for (int32 VertexInstanceIndex = 0; VertexInstanceIndex < ReverseVertexInstanceIDs.Num(); ++VertexInstanceIndex)
	{
		Polygon.PerimeterContour.VertexInstanceIDs[VertexInstanceIndex] = ReverseVertexInstanceIDs[VertexInstanceIndex];
	}
	
	//Triangulate the polygon since we reverse the indices
	ComputePolygonTriangulation(PolygonID, Polygon.Triangles);
}

void UMeshDescription::ReverseAllPolygonFacing()
{
	// Perform triangulation directly into mesh polygons
	for (const FPolygonID PolygonID : Polygons().GetElementIDs())
	{
		FMeshPolygon& Polygon = PolygonArray[PolygonID];
		ReversePolygonFacing(PolygonID);
	}
}