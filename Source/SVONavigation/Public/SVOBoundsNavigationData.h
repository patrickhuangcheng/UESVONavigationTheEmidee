#pragma once
#include "SVONavigationTypes.h"

struct FSVOBoundsNavigationDataGenerationSettings
{
    float VoxelSize;
    UWorld * World;
    FSVODataGenerationSettings GenerationSettings;
};

class SVONAVIGATION_API FSVOBoundsNavigationData
{
public:
    friend FArchive & operator<<( FArchive & archive, FSVOBoundsNavigationData & data );

    typedef FSVOOctreeLink FNodeRef;

    // Used by FGraphAStar
    bool IsValidRef( const FSVOOctreeLink ref ) const
    {
        return ref.IsValid();
    }

    const FSVOBoundsNavigationDataGenerationSettings & GetDataGenerationSettings() const;
    const FBox & GetVolumeBounds() const;
    const FBox & GetNavigationBounds() const;
    const FSVOOctreeData & GetOctreeData() const;
    FVector GetNodePosition( LayerIndex layer_index, MortonCode morton_code ) const;
    FVector GetLinkPosition( const FSVOOctreeLink & link ) const;
    const FSVOOctreeNode & GetNodeFromLink( const FSVOOctreeLink & link ) const;
    bool GetLinkFromPosition( FSVOOctreeLink & link, const FVector & position ) const;
    void GetNeighbors( TArray< FSVOOctreeLink > & neighbors, const FSVOOctreeLink & link ) const;
    float GetLayerRatio( LayerIndex layer_index ) const;
    float GetLayerInverseRatio( LayerIndex layer_index ) const;

    void GenerateNavigationData( const FBox & volume_bounds, const FSVOBoundsNavigationDataGenerationSettings & generation_settings );

private:
    int32 GetLayerMaxNodeCount( LayerIndex layer_index ) const;
    bool IsPositionOccluded( const FVector & position, float box_size ) const;
    void FirstPassRasterization();
    void AllocateLeafNodes();
    void RasterizeLeaf( const FVector & node_position, int32 leaf_index );
    void RasterizeInitialLayer();
    void RasterizeLayer( LayerIndex layer_index );
    TOptional< NodeIndex > GetNodeIndexFromMortonCode( LayerIndex layer_index, MortonCode morton_code ) const;
    void BuildNeighborLinks( LayerIndex layer_index );
    bool FindNeighborInDirection( FSVOOctreeLink & link, const LayerIndex layer_index, const NodeIndex node_index, const NeighborDirection direction, const FVector & node_position );
    void GetLeafNeighbors( TArray< FSVOOctreeLink > & neighbors, const FSVOOctreeLink & link ) const;

    FSVOBoundsNavigationDataGenerationSettings Settings;
    int VoxelExponent;
    uint8 LayerCount = 0;
    FBox NavigationBounds;
    FBox VolumeBounds;
    float UsedBoxExtent;
    FSVOOctreeData SVOData;
    TArray< TSet< MortonCode > > BlockedIndices;
};

FORCEINLINE FArchive & operator<<( FArchive & archive, FSVOBoundsNavigationData & data )
{
    archive << data.NavigationBounds;
    archive << data.VolumeBounds;
    archive << data.VoxelExponent;
    archive << data.LayerCount;
    archive << data.UsedBoxExtent;
    archive << data.SVOData;

    archive << data.VolumeBounds;
    return archive;
}

FORCEINLINE const FSVOBoundsNavigationDataGenerationSettings & FSVOBoundsNavigationData::GetDataGenerationSettings() const
{
    return Settings;
}

FORCEINLINE const FBox & FSVOBoundsNavigationData::GetVolumeBounds() const
{
    return VolumeBounds;
}

FORCEINLINE const FBox & FSVOBoundsNavigationData::GetNavigationBounds() const
{
    return NavigationBounds;
}

FORCEINLINE const FSVOOctreeData & FSVOBoundsNavigationData::GetOctreeData() const
{
    return SVOData;
}

FORCEINLINE int32 FSVOBoundsNavigationData::GetLayerMaxNodeCount( const LayerIndex layer_index ) const
{
    // :TODO: check if valid
    return FMath::Pow( 2, VoxelExponent - layer_index );
}

FORCEINLINE const FSVOOctreeNode & FSVOBoundsNavigationData::GetNodeFromLink( const FSVOOctreeLink & link ) const
{
    return link.LayerIndex < 15
               ? SVOData.GetLayer( link.LayerIndex ).GetNode( link.NodeIndex )
               : SVOData.GetLastLayer().GetNode( 0 );
}