#pragma once

#include "SVONavigationTypes.generated.h"

class USVOPathFindingAlgorithm;
class USVOPathFindingAlgorithm;
class USVOHeuristicCalculator;
class USVOTraversalCostCalculator;

typedef uint_fast64_t MortonCode;
typedef uint8 LayerIndex;
typedef int32 NodeIndex;
typedef int32 LeafIndex;
typedef uint8 SubNodeIndex;
typedef uint8 NeighborDirection;

DECLARE_DELEGATE_ThreeParams( FSVONavigationPathQueryDelegate, uint32, ENavigationQueryResult::Type, FNavPathSharedPtr );

struct FSVOPathFindingResult
{
    FNavPathSharedPtr Path;
    ENavigationQueryResult::Type Result;

    explicit FSVOPathFindingResult( ENavigationQueryResult::Type result = ENavigationQueryResult::Invalid ) :
        Result( result )
    {
    }

    bool IsSuccessful() const;
};

FORCEINLINE bool FSVOPathFindingResult::IsSuccessful() const
{
    return Result == ENavigationQueryResult::Success;
}

USTRUCT()
struct SVONAVIGATION_API FSVONavigationBoundsDataDebugInfos
{
    GENERATED_USTRUCT_BODY()

    FSVONavigationBoundsDataDebugInfos() :
        bDebugDrawsBounds( false ),
        bDebugDrawsLayers( false ),
        LayerIndexToDraw( 1 ),
        bDebugDrawsLeaves( false ),
        bDebugDrawsOccludedLeaves( false ),
        bDebugDrawsLinks( false ),
        LinksLayerIndexToDraw( false ),
        bDebugDrawsNeighborLinks( false ),
        bDebugDrawsParentLinks( false ),
        bDebugDrawsFirstChildLinks( false ),
        DebugLineThickness( 5.0f ),
        bDebugDrawsMortonCodes( false ),
        MortonCodeLayerIndexToDraw( 0 )
    {
    }

    friend FArchive & operator<<( FArchive & archive, FSVONavigationBoundsDataDebugInfos & data );

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsBounds;

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsLayers;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsLayers", ClampMin = "1", UIMin = "1" ) )
    uint8 LayerIndexToDraw;

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsLeaves;

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsOccludedLeaves;

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsLinks;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsLinks", ClampMin = "1", UIMin = "1" ) )
    uint8 LinksLayerIndexToDraw;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsLinks" ) )
    bool bDebugDrawsNeighborLinks;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsLinks" ) )
    bool bDebugDrawsParentLinks;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsLinks" ) )
    bool bDebugDrawsFirstChildLinks;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "ItHasDebugDrawingEnabled", ClampMin = "1", UIMin = "1" ) )
    float DebugLineThickness;

    UPROPERTY( EditInstanceOnly )
    bool bDebugDrawsMortonCodes;

    UPROPERTY( EditInstanceOnly, meta = ( EditCondition = "bDebugDrawsMortonCodes" ) )
    uint8 MortonCodeLayerIndexToDraw;
};

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVONavigationBoundsDataDebugInfos & data )
{
    archive << data.bDebugDrawsBounds;
    archive << data.bDebugDrawsLayers;
    archive << data.LayerIndexToDraw;
    archive << data.bDebugDrawsLeaves;
    archive << data.bDebugDrawsOccludedLeaves;
    archive << data.bDebugDrawsLinks;
    archive << data.LinksLayerIndexToDraw;
    archive << data.bDebugDrawsNeighborLinks;
    archive << data.bDebugDrawsParentLinks;
    archive << data.DebugLineThickness;
    archive << data.bDebugDrawsMortonCodes;
    archive << data.MortonCodeLayerIndexToDraw;

    return archive;
}

USTRUCT()
struct FSVODataGenerationSettings
{
    GENERATED_USTRUCT_BODY()

    FSVODataGenerationSettings()
    {
        CollisionChannel = ECollisionChannel::ECC_WorldStatic;
        Clearance = 0.0f;

        CollisionQueryParameters.bFindInitialOverlaps = true;
        CollisionQueryParameters.bTraceComplex = false;
        CollisionQueryParameters.TraceTag = "SVONavigationRasterize";
    }

    UPROPERTY( EditAnywhere, Category = "Generation" )
    TEnumAsByte< ECollisionChannel > CollisionChannel;

    UPROPERTY( EditAnywhere, Category = "Generation" )
    float Clearance;

    FCollisionQueryParams CollisionQueryParameters;
};

struct FSVOOctreeLeaf
{
    bool GetSubNodeAt( uint_fast32_t X, uint_fast32_t Y, uint_fast32_t Z ) const;
    void SetSubNodeAt( uint_fast32_t X, uint_fast32_t Y, uint_fast32_t Z );

    void SetSubNode( const SubNodeIndex index );
    bool GetSubNode( const MortonCode morton_code ) const;
    void ClearSubNode( const SubNodeIndex index );
    bool IsOccluded() const;
    bool IsEmpty() const;

    uint_fast64_t SubNodes = 0;
};

FORCEINLINE void FSVOOctreeLeaf::SetSubNode( const SubNodeIndex index )
{
    SubNodes |= 1ULL << index;
}

FORCEINLINE bool FSVOOctreeLeaf::GetSubNode( const MortonCode morton_code ) const
{
    return ( SubNodes & 1ULL << morton_code ) != 0;
}

FORCEINLINE void FSVOOctreeLeaf::ClearSubNode( const SubNodeIndex index )
{
    SubNodes &= !( 1ULL << index );
}

FORCEINLINE bool FSVOOctreeLeaf::IsOccluded() const
{
    return SubNodes == -1;
}

FORCEINLINE bool FSVOOctreeLeaf::IsEmpty() const
{
    return SubNodes == 0;
}

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVOOctreeLeaf & data )
{
    archive << data.SubNodes;
    return archive;
}

struct FSVOOctreeLink
{
    FSVOOctreeLink() :
        LayerIndex( 15 ),
        NodeIndex( 0 ),
        SubNodeIndex( 0 )
    {
    }

    explicit FSVOOctreeLink( const int32 index ) :
        LayerIndex( index << 28 ),
        NodeIndex( index << 6 ),
        SubNodeIndex( index )
    {
    }

    FSVOOctreeLink( const LayerIndex layer_index, const MortonCode node_index, const SubNodeIndex sub_node_index ) :
        LayerIndex( layer_index ),
        NodeIndex( node_index ),
        SubNodeIndex( sub_node_index )
    {
    }

    bool IsValid() const;
    void Invalidate();

    bool operator==( const FSVOOctreeLink & other ) const
    {
        return LayerIndex == other.LayerIndex && NodeIndex == other.NodeIndex && SubNodeIndex == other.SubNodeIndex;
    }

    bool operator!=( const FSVOOctreeLink & other ) const
    {
        return !operator==( other );
    }

    static FSVOOctreeLink InvalidEdge()
    {
        return FSVOOctreeLink();
    }

    uint8 LayerIndex : 4;
    uint_fast32_t NodeIndex : 22;
    uint8 SubNodeIndex : 6;
};

FORCEINLINE bool FSVOOctreeLink::IsValid() const
{
    return LayerIndex != 15;
}

FORCEINLINE void FSVOOctreeLink::Invalidate()
{
    LayerIndex = 15;
}

FORCEINLINE uint32 GetTypeHash( const FSVOOctreeLink & link )
{
    return HashCombine( HashCombine( GetTypeHash( link.LayerIndex ), GetTypeHash( link.NodeIndex ) ), GetTypeHash( link.SubNodeIndex ) );
}

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVOOctreeLink & data )
{
    archive.Serialize( &data, sizeof( FSVOOctreeLink ) );
    return archive;
}

struct FSVOOctreeNode
{
    MortonCode MortonCode;
    FSVOOctreeLink Parent;
    FSVOOctreeLink FirstChild;
    FSVOOctreeLink Neighbors[ 6 ];

    FSVOOctreeNode() :
        MortonCode( 0 ),
        Parent( FSVOOctreeLink::InvalidEdge() ),
        FirstChild( FSVOOctreeLink::InvalidEdge() )
    {
    }

    bool HasChildren() const
    {
        return FirstChild.IsValid();
    }
};

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVOOctreeNode & data )
{
    archive << data.MortonCode;
    archive << data.Parent;
    archive << data.FirstChild;

    for ( int32 neighbor_index = 0; neighbor_index < 6; neighbor_index++ )
    {
        archive << data.Neighbors[ neighbor_index ];
    }

    return archive;
}

struct FSVOOctreeData
{
    void Reset();

    TArray< TArray< FSVOOctreeNode > > NodesByLayers;
    TArray< FSVOOctreeLeaf > Leaves;

    int GetAllocatedSize() const;
};

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVOOctreeData & data )
{
    archive << data.NodesByLayers;
    archive << data.Leaves;

    return archive;
}

USTRUCT()
struct SVONAVIGATION_API FSVONavigationQueryFilterSettings
{
    GENERATED_USTRUCT_BODY()

    FSVONavigationQueryFilterSettings();

    UPROPERTY( EditAnywhere, Instanced )
    USVOPathFindingAlgorithm * PathFinder;
    
    UPROPERTY( EditAnywhere, Instanced )
    USVOTraversalCostCalculator * TraversalCostCalculator;

    UPROPERTY( EditAnywhere, Instanced )
    USVOHeuristicCalculator * HeuristicCalculator;

    UPROPERTY( EditDefaultsOnly )
    float HeuristicScale;

    UPROPERTY( EditDefaultsOnly )
    uint8 bUseNodeSizeCompensation : 1;

    UPROPERTY( EditDefaultsOnly )
    float NodeSizeCompensation;

    UPROPERTY( EditDefaultsOnly )
    uint8 bOffsetPathVerticallyByAgentRadius : 1;
};
