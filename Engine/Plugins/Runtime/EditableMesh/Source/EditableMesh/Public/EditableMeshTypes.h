// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "MeshTypes.h"
#include "EditableMeshTypes.generated.h"

// @todo mesheditor: Move elsewhere
namespace LogHelpers
{
	template<typename ArrayType>
	inline FString ArrayToString( const TArray<ArrayType>& Array )
	{
		FString String;
		if( Array.Num() == 0 )
		{
			String = TEXT( "Empty" );
		}
		else
		{
			for( const ArrayType& Element : Array )
			{
				if( !String.IsEmpty() )
				{
					String.Append( TEXT( ", " ) );
				}
				String.Append( Element.ToString() );
			}
			String = TEXT( "[" ) + String + TEXT( "]" );
		}
		return String;
	}

	template<>
	inline FString ArrayToString<int32>( const TArray<int32>& Array )
	{
		FString String;
		if( Array.Num() == 0 )
		{
			String = TEXT( "Empty" );
		}
		else
		{
			for( const int32& Element : Array )
			{
				if( !String.IsEmpty() )
				{
					String.Append( TEXT( ", " ) );
				}
				String.Append( FString::FormatAsNumber( Element ) );
			}
			String = TEXT( "[" ) + String + TEXT( "]" );
		}
		return String;
	}


	template<typename ArrayType>
	inline FString ArrayToString( const TArray<TArray<ArrayType>>& Array )
	{
		FString String;
		if( Array.Num() == 0 )
		{
			String = TEXT( "Empty" );
		}
		else
		{
			for( const TArray<ArrayType>& SubArray : Array )
			{
				if( !String.IsEmpty() )
				{
					String.Append( TEXT( ", " ) );
				}
				String.Append( FString::Printf( TEXT( "%s" ), *ArrayToString( SubArray ) ) );
			}
			String = TEXT( "[" ) + String + TEXT( "]" );
		}
		return String;
	}

	inline FString BoolToString( const bool Bool )
	{
		return Bool ? TEXT( "true" ) : TEXT( "false" );
	}
}


/**
 * The different components that make up a typical mesh
 */
enum class EEditableMeshElementType
{
	/** Invalid mesh element (or "none") */
	Invalid,

	/** A unique point in 3D space */
	Vertex,

	/** An edge that connects two vertices */
	Edge,

	/** A polygon with at least three 3D points.  It could be triangle, quad, or more complex shape */
	Polygon,

	/** Represents any element type */
	Any,
};


/**
 * Unique identifies a specific sub-mesh within a component
 */
struct FEditableMeshSubMeshAddress
{
	/** Pointer that uniquely identifies the mesh object being edited (not the instance), for hashing/comparison purposes */
	void* MeshObjectPtr;

	/** The type of mesh format */
	// @todo mesheditor: Should SHOULD be OK, but if it somehow was deallocated while we were using it, we'd want to store an FName or ID here instead of a pointer.  Should be fine though.
	class IEditableMeshFormat* EditableMeshFormat;

	/** The index of the mesh within the component, for components that may define more than one mesh */
	int32 MeshIndex;

	/** The mesh level of detail index, or zero if not applicable to the type of mesh. */
	int32 LODIndex;


	/** Default constructor that initializes variables to an default sub-mesh address */
	FEditableMeshSubMeshAddress()
		: MeshObjectPtr( nullptr ),
		  EditableMeshFormat( nullptr ),
		  MeshIndex( 0 ),
		  LODIndex( 0 )
	{
	}

	/** Equality check */
	inline bool operator==( const FEditableMeshSubMeshAddress& Other ) const
	{
		return
			MeshObjectPtr == Other.MeshObjectPtr &&
			EditableMeshFormat == Other.EditableMeshFormat &&
			MeshIndex == Other.MeshIndex &&
			LODIndex == Other.LODIndex;
	}

	/** Hashing */
	FORCEINLINE friend uint32 GetTypeHash( const FEditableMeshSubMeshAddress& Other )
	{
		// @todo mesheditor: Hash could be improved a bit to consider LOD/MeshIndex, etc.
		return GetTypeHash( Other.MeshObjectPtr );
	}

	/** Convert to a string */
	inline FString ToString() const
	{
		return FString::Printf(
			TEXT( "PtrHash:%lu, FmtHash:%lu, MeshIndex:%lu, LODIndex:%lu" ),
			GetTypeHash( MeshObjectPtr ),
			GetTypeHash( EditableMeshFormat ),
			MeshIndex,
			LODIndex );
	}
};


UENUM()
enum class EMeshModificationType : uint8
{
	/** The first Interim change since the last Final change.  This must be followed by either an Interim change or a Final change */
	FirstInterim,

	/** User is still in the middle of their interaction.  More changes to come, so don't bother finalizing everything yet (smoother performance) */
	Interim,

	/** User has finished their current interaction with this mesh, and everything needs to be finalized at this time */
	Final
};


UENUM()
enum class EMeshTopologyChange : uint8
{
	/** We won't be changing the mesh topology, but values could be changed (vertex positions, UVs, colors, etc.) */
	NoTopologyChange,

	/** Topology is changing with this edit, potentially along with other changes */
	TopologyChange
};



USTRUCT( BlueprintType )
struct FSubdividedQuadVertex
{
	GENERATED_BODY()

	/** The index of the vertex position (into the FSubdivisionLimitData's VertexPositions array) used for this vertex */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	int32 VertexPositionIndex;

	/** Texture coordinates for this vertex.  We only support up to two, for now. (Just to avoid TArrays/allocations) */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FVector2D TextureCoordinate0;
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FVector2D TextureCoordinate1;

	/** Vertex color */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FColor VertexColor;

	/** Quad vertex normal */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FVector VertexNormal;

	/** Quad vertex tangent */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FVector VertexTangent;

	/** Quad vertex binormal sign (-1.0 or 1.0)*/
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	float VertexBinormalSign;
};


USTRUCT( BlueprintType )
struct FSubdividedQuad
{
	GENERATED_BODY()

	// NOTE: The reason we're using separate variables instead of a static array is so that we can expose these to Blueprints, which doesn't support static array properties

	/** The vertices for the four corners of this quad */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FSubdividedQuadVertex QuadVertex0;
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FSubdividedQuadVertex QuadVertex1;
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FSubdividedQuadVertex QuadVertex2;
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	FSubdividedQuadVertex QuadVertex3;

	const FSubdividedQuadVertex& GetQuadVertex( const int32 Index ) const
	{
		switch( Index )
		{
			default:
			case 0:
				return QuadVertex0;

			case 1:
				return QuadVertex1;

			case 2:
				return QuadVertex2;
			
			case 3:
				return QuadVertex3;
		}
	}

	FSubdividedQuadVertex& AccessQuadVertex( const int32 Index )
	{
		switch( Index )
		{
			default:
			case 0:
				return QuadVertex0;

			case 1:
				return QuadVertex1;

			case 2:
				return QuadVertex2;

			case 3:
				return QuadVertex3;
		}
	}
};


USTRUCT( BlueprintType )
struct FSubdividedWireEdge
{
	GENERATED_BODY()

	// NOTE: The reason we're using separate variables instead of a static array is so that we can expose these to Blueprints, which doesn't support static array properties

	/** The vertex indices for the two corners of this quad */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	int32 EdgeVertex0PositionIndex;
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )	int32 EdgeVertex1PositionIndex;

	/** True if this edge is a counterpart to an original base cage edge of the mesh.  Otherwise it's a new edge that exists only
	    in the subdivision surfaces */
	FEdgeID CounterpartEdgeID;
};


USTRUCT( BlueprintType )
struct FSubdivisionLimitSection
{
	GENERATED_BODY()

	/** All of the quads in this section, as a result from subdividing the mesh */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FSubdividedQuad> SubdividedQuads;
};


USTRUCT( BlueprintType )
struct FSubdivisionLimitData
{
	GENERATED_BODY()

	/** Positions of all of the vertices for this subdivision level.  Many vertex positions may be shared between subdivided quads. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVector> VertexPositions;

	/** Data for each of the sections in the mesh.  This array will have the same number of elements as the editable mesh's 
	    section list (not necessarily the same indices though, due to sparseness). */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FSubdivisionLimitSection> Sections;	

	/** All of the wire edges in the entire mesh (for all sections) */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FSubdividedWireEdge> SubdividedWireEdges;
};


USTRUCT( BlueprintType )
struct FMeshElementAttributeData
{
	GENERATED_BODY()

	/** Name of the attribute */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FName AttributeName;

	/** Index of the attribute */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	int32 AttributeIndex;

	/** The value of this attribute */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVector4 AttributeValue;

	/** Default constructor */
	FMeshElementAttributeData()
		: AttributeName( NAME_None ),
		  AttributeIndex( 0 ),
		  AttributeValue( 0.0f )
	{
	}

	/** Constructor that fills in all fields */
	FMeshElementAttributeData( const FName InitAttributeName, const int32 InitAttributeIndex, const FVector4 InitAttributeValue )
		: AttributeName( InitAttributeName ),
		  AttributeIndex( InitAttributeIndex ),
		  AttributeValue( InitAttributeValue )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "Name:%s, Index:%lu, Value:%s" ),
			*AttributeName.ToString(),
			AttributeIndex,
			*AttributeValue.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FMeshElementAttributeList
{
	GENERATED_BODY()

	/** List of attributes to apply to a mesh element */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FMeshElementAttributeData> Attributes;

	/** Default constructor */
	FMeshElementAttributeList()
		: Attributes()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "Attributes:%s" ),
			*LogHelpers::ArrayToString( Attributes ) );
	}
};


USTRUCT( BlueprintType )
struct FVertexToCreate
{
	GENERATED_BODY()

	/** Attributes of this vertex itself */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList VertexAttributes;

	/** The original ID of the vertex.  Should only be used by the undo system. */
	UPROPERTY()
	FVertexID OriginalVertexID;

	FVertexToCreate()
		: VertexAttributes(),
		  OriginalVertexID( FVertexID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexAttributes:%s, OriginalVertexID:%s" ),
			*VertexAttributes.ToString(),
			*OriginalVertexID.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FVertexInstanceToCreate
{
	GENERATED_BODY()

	/** Vertex ID which is being instanced */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID;

	/** Attributes of this vertex instance */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList VertexInstanceAttributes;

	/** The original ID of the vertex instance.  Should only be used by the undo system. */
	UPROPERTY()
	FVertexInstanceID OriginalVertexInstanceID;

	FVertexInstanceToCreate()
		: VertexID( FVertexID::Invalid ),
		  VertexInstanceAttributes(),
		  OriginalVertexInstanceID( FVertexInstanceID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexID:%s, VertexInstanceAttributes:%s, OriginalVertexInstanceID:%s" ),
			*VertexID.ToString(),
			*VertexInstanceAttributes.ToString(),
			*OriginalVertexInstanceID.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FEdgeToCreate
{
	GENERATED_BODY()
		
	/** The first vertex this edge connects */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID0;

	/** The second vertex this edge connects */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID1;

	/** The polygons that are connected to this edge. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FPolygonID> ConnectedPolygons;

	/** Attributes of this edge itself */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList EdgeAttributes;

	/** The original ID of the edge.  Should only be used by the undo system. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FEdgeID OriginalEdgeID;

	/** Default constructor */
	FEdgeToCreate()
		: VertexID0( FVertexID::Invalid ),
		  VertexID1( FVertexID::Invalid ),
		  ConnectedPolygons(),
		  EdgeAttributes(),
		  OriginalEdgeID( FEdgeID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexID0:%s, VertexID1:%s, ConnectedPolygons:%s, OriginalEdgeID:%s" ),
			*VertexID0.ToString(), 
			*VertexID1.ToString(),
			*LogHelpers::ArrayToString( ConnectedPolygons ),
			*EdgeAttributes.ToString(),
			OriginalEdgeID.GetValue() );
	}
};



USTRUCT( BlueprintType )
struct FVertexAndAttributes
{
	GENERATED_BODY()

	/** The vertex instance ID to insert into the polygon, or FVertexInstanceID::Invalid to create a new vertex instance with the given attributes */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexInstanceID VertexInstanceID;

	/** The vertex ID to insert into the polygon, if no valid vertex instance ID was supplied. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID;

	/** A list of polygon attributes to set for the vertex on the polygon we're inserting it into, if no valid vertex instance ID was supplied. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList PolygonVertexAttributes;

	/** Default constructor */
	FVertexAndAttributes()
		: VertexInstanceID( FVertexInstanceID::Invalid ),
		  VertexID( FVertexID::Invalid ),
		  PolygonVertexAttributes()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexInstanceID:%s, VertexID:%s, PolygonVertexAttributes:%s" ),
			*VertexInstanceID.ToString(),
			*VertexID.ToString(),
			*PolygonVertexAttributes.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FPolygonHoleVertices
{
	GENERATED_BODY()

	/** Ordered list of vertices that defines the hole's contour, along with the polygon vertex attributes to set for each vertex */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexAndAttributes> HoleVertices;

	/** Default constructor */
	FPolygonHoleVertices()
		: HoleVertices()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "HoleVertices:%s" ),
			*LogHelpers::ArrayToString( HoleVertices ) );
	}
};


UENUM( BlueprintType )
enum class EPolygonEdgeHardness : uint8
{
	/** Create any new edges required by the polygon as soft, leaving existing edges as they are */
	NewEdgesSoft,

	/** Create any new edges required by the polygon as hard, leaving existing edges as they are */
	NewEdgesHard,

	/** Set all edges as soft, overriding edge hardness on existing edges */
	AllEdgesSoft,

	/** Set all edges as hard, overriding edge hardness on existing edges */
	AllEdgesHard
};


USTRUCT( BlueprintType )
struct FPolygonToCreate
{
	GENERATED_BODY()

	/** The group the polygon will be added to */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonGroupID PolygonGroupID;

	/** Ordered list of vertices that defines the polygon's perimeter, along with the polygon vertex attributes to set for each vertex */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexAndAttributes> PerimeterVertices;
	
	/** For each hole in the polygon, an ordered list of vertices that defines that hole's boundary */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FPolygonHoleVertices> PolygonHoles;

	/** The original ID of the polygon.  Should only be used by the undo system. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonID OriginalPolygonID;

	/** Whether to create a hard-edged polygon */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	EPolygonEdgeHardness PolygonEdgeHardness;

	/** Default constructor */
	FPolygonToCreate()
		: PolygonGroupID( FPolygonGroupID::Invalid ),
		  PerimeterVertices(),
		  PolygonHoles(),
		  OriginalPolygonID( FPolygonID::Invalid ),
		  PolygonEdgeHardness( EPolygonEdgeHardness::NewEdgesSoft )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "PolygonGroupID:%s, PerimeterVertices:%s, PolygonHoles:%s, OriginalPolygonID:%s" ),
			*PolygonGroupID.ToString(),
			*LogHelpers::ArrayToString( PerimeterVertices ),
			*LogHelpers::ArrayToString( PolygonHoles ),
			*OriginalPolygonID.ToString() );
	}
};



USTRUCT( BlueprintType )
struct FVertexPair
{
	GENERATED_BODY()

	/** The first vertex ID in this pair */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID0;

	/** The second vertex ID in this pair */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID1;

	/** Default constructor */
	FVertexPair()
		: VertexID0( FVertexID::Invalid ),
		  VertexID1( FVertexID::Invalid )
	{
	}

	// @todo mesheditor: Do we need a ToString() function?  Would it ever be called?
};


USTRUCT( BlueprintType )
struct FPolygonToSplit
{
	GENERATED_BODY()

	/** The polygon that we'll be splitting */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonID PolygonID;

	/** A list of pairs of vertices that new edges will be created at.  The pairs must be ordered, and the vertices
	    must already exist and be connected to the polygon */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexPair> VertexPairsToSplitAt;

	/** Default constructor */
	FPolygonToSplit()
		: PolygonID( FPolygonID::Invalid ),
		  VertexPairsToSplitAt()
	{
	}

	// @todo mesheditor: Do we need a ToString() function?  Would it ever be called?
};


USTRUCT( BlueprintType )
struct FAttributesForVertex
{
	GENERATED_BODY()

	/** The vertex ID to set attributes on */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID;

	/** A list of attributes to set for the vertex */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList VertexAttributes;

	/** Default constructor */
	FAttributesForVertex()
		: VertexID( 0 ),
		  VertexAttributes()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexID:%s, VertexAttributes:%s" ),
			*VertexID.ToString(),
			*VertexAttributes.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FAttributesForVertexInstance
{
	GENERATED_BODY()

	/** The vertex instance ID to set attributes on */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexInstanceID VertexInstanceID;

	/** A list of attributes to set for the vertex instance */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList VertexInstanceAttributes;

	/** Default constructor */
	FAttributesForVertexInstance()
		: VertexInstanceID( 0 ),
		  VertexInstanceAttributes()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexInstanceID:%s, VertexInstanceAttributes:%s" ),
			*VertexInstanceID.ToString(),
			*VertexInstanceAttributes.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FAttributesForEdge
{
	GENERATED_BODY()

	/** The edge ID to set attributes on */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FEdgeID EdgeID;

	/** A list of attributes to set for the edge */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FMeshElementAttributeList EdgeAttributes;

	/** Whether this is being done as part of undo/redo */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	bool bIsUndo;

	/** Default constructor */
	FAttributesForEdge()
		: EdgeID( 0 ),
		  EdgeAttributes(),
		  bIsUndo( false )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "EdgeID:%s, EdgeAttributes:%s, bIsUndo:%s" ),
			*EdgeID.ToString(),
			*EdgeAttributes.ToString(),
			*LogHelpers::BoolToString( bIsUndo ) );
	}
};


USTRUCT( BlueprintType )
struct FVertexAttributesForPolygonHole
{
	GENERATED_BODY()

	/** For each hole vertex, a list of attributes for that vertex.  You can leave a given array empty for 
	    a specific hole index if you don't want to set attributes for select holes. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FMeshElementAttributeList> VertexAttributeList;

	/** Default constructor */
	FVertexAttributesForPolygonHole()
		: VertexAttributeList()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexAttributeList:%s" ),
			*LogHelpers::ArrayToString( VertexAttributeList ) );
	}
};


USTRUCT( BlueprintType )
struct FVertexAttributesForPolygon
{
	GENERATED_BODY()

	/** The polygon to set vertex attributes on */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonID PolygonID;

	/** For each polygon vertex, a list of attributes for that vertex.  Can be left empty if you don't want to set any attributes. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FMeshElementAttributeList> PerimeterVertexAttributeLists;

	/** For each hole vertex, a list of attributes for that vertex.  Can be left empty if you don't want to set any attributes.  Also
	    you can leave a given array empty for a specific hole index if you don't want to set attributes for select holes. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexAttributesForPolygonHole> VertexAttributeListsForEachHole;

	/** Default constructor */
	FVertexAttributesForPolygon()
		: PolygonID( FPolygonID::Invalid ),
		  PerimeterVertexAttributeLists(),
		  VertexAttributeListsForEachHole()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "PolygonID:%s, PerimeterVertexAttributeLists:%s, VertexAttributeListsForEachHole:%s" ),
			*PolygonID.ToString(),
			*LogHelpers::ArrayToString( PerimeterVertexAttributeLists ),
			*LogHelpers::ArrayToString( VertexAttributeListsForEachHole ) );
	}
};


USTRUCT( BlueprintType )
struct FVertexIndexAndInstanceID
{
	GENERATED_BODY()

	/** Contour index of the vertex to change */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	int32 ContourIndex;

	/** New vertex instance ID to assign */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexInstanceID VertexInstanceID;

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "ContourIndex:%d, VertexInstanceIDs:%s" ),
			ContourIndex,
			*VertexInstanceID.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FVertexInstancesForPolygonHole
{
	GENERATED_BODY()

	/** A list of vertex instance IDs for a polygon hole. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexIndexAndInstanceID> VertexIndicesAndInstanceIDs;

	/** Default constructor */
	FVertexInstancesForPolygonHole()
		: VertexIndicesAndInstanceIDs()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexInstanceIDs:%s" ),
			*LogHelpers::ArrayToString( VertexIndicesAndInstanceIDs ) );
	}
};


USTRUCT( BlueprintType )
struct FChangeVertexInstancesForPolygon
{
	GENERATED_BODY()

	/** The polygon to set vertex instances on */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonID PolygonID;

	/** A list of vertex numbers and vertex instance IDs to change for the polygon perimeter. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexIndexAndInstanceID> PerimeterVertexIndicesAndInstanceIDs;

	/** A list of vertex instance IDs for each hole. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	TArray<FVertexInstancesForPolygonHole> VertexIndicesAndInstanceIDsForEachHole;

	/** Default constructor */
	FChangeVertexInstancesForPolygon()
		: PolygonID( FPolygonID::Invalid ),
		  PerimeterVertexIndicesAndInstanceIDs(),
		  VertexIndicesAndInstanceIDsForEachHole()
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "PolygonID:%s, PerimeterVertexIndicesAndInstanceIDs:%s, VertexIndicesAndInstanceIDsForEachHole:%s" ),
			*PolygonID.ToString(),
			*LogHelpers::ArrayToString( PerimeterVertexIndicesAndInstanceIDs ),
			*LogHelpers::ArrayToString( VertexIndicesAndInstanceIDsForEachHole ) );
	}
};


USTRUCT( BlueprintType )
struct FVerticesForEdge
{
	GENERATED_BODY()

	/** The edge ID */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FEdgeID EdgeID;

	/** First new vertex ID for this edge */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID NewVertexID0;

	/** Second new vertex ID for this edge */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID NewVertexID1;

	/** Default constructor */
	FVerticesForEdge()
		: EdgeID( 0 ),
		  NewVertexID0( FVertexID::Invalid ),	// @todo mesheditor urgent: Typesafety isn't working -- able to assign EdgeID to VertexID!
		  NewVertexID1( FVertexID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "EdgeID:%s, NewVertexID0:%s, NewVertexID1:%s]" ),
			*EdgeID.ToString(),
			*NewVertexID0.ToString(), 
			*NewVertexID1.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FVertexToMove
{
	GENERATED_BODY()

	/** The vertex we'll be moving around */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVertexID VertexID;

	/** The new position of the vertex */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FVector NewVertexPosition;


	FVertexToMove()
		: VertexID( FVertexID::Invalid ),
		  NewVertexPosition( FVector::ZeroVector )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "VertexID:%s, NewVertexPosition:%s" ),
			*VertexID.ToString(),
			*NewVertexPosition.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FPolygonGroupToCreate
{
	GENERATED_BODY()

	// @todo mesheditor: Material, bEnableCollision and bCastShadow should all be implemented as PolygonGroup attributes.
	// This would require making Material an asset name, rather than a UObject*, which is also safer.

	/** Material asset path to assign to the new polygon group */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FString MaterialAsset;

	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FName MaterialSlotName;

	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FName ImportedMaterialSlotName;

	/** Whether the new polygon group should have collision enabled */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	bool bEnableCollision;

	/** Whether the new polygon group casts a shadow */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	bool bCastShadow;

	/** The original ID of the polygon group.  Should only be used by the undo system. */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonGroupID OriginalPolygonGroupID;

	/** Default constructor */
	FPolygonGroupToCreate()
		: bEnableCollision( false )
		, bCastShadow( false )
		, OriginalPolygonGroupID( FPolygonGroupID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "Material:%s, MaterialSlotName:%s, ImportedMaterialSlotName:%s, bEnableCollision:%s, bCastShadow:%s, OriginalPolygonGroupID:%s" ),
			*MaterialAsset,
			*MaterialSlotName.ToString(),
			*ImportedMaterialSlotName.ToString(),
			*LexicalConversion::ToString( bEnableCollision ),
			*LexicalConversion::ToString( bCastShadow ),
			*OriginalPolygonGroupID.ToString() );
	}
};


USTRUCT( BlueprintType )
struct FPolygonGroupForPolygon
{
	GENERATED_BODY()

	/** Polygon to assign to a new group */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonID PolygonID;

	/** Polygon group to assign polygon to */
	UPROPERTY( BlueprintReadWrite, Category="Editable Mesh" )
	FPolygonGroupID PolygonGroupID;

	/** Default constructor */
	FPolygonGroupForPolygon()
		: PolygonID( FPolygonID::Invalid )
		, PolygonGroupID( FPolygonGroupID::Invalid )
	{
	}

	FString ToString() const
	{
		return FString::Printf(
			TEXT( "PolygonID:%s, PolygonGroupID:%s" ),
			*PolygonID.ToString(),
			*PolygonGroupID.ToString() );
	}
};
