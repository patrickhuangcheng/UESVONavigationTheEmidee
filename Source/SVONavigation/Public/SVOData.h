#pragma once

#include "SVOBoundsNavigationData.h"

#include <CoreMinimal.h>

struct FSVOData
{
public:
    const TArray< FSVOBoundsNavigationData > & GetNavigationBoundsData() const;

    void Serialize( FArchive & archive );
    void ClearData();
    void RemoveDataInBounds( const FBox & bounds );
    void AddNavigationBoundsData( FSVOBoundsNavigationData data );
    FBox GetBoundingBox() const;
    const FSVOBoundsNavigationData * GetBoundsNavigationDataContainingPoints( const TArray< FVector > & points ) const;

#if !UE_BUILD_SHIPPING
    uint32 GetAllocatedSize() const;
#endif

private:
    TArray< FSVOBoundsNavigationData > NavigationBoundsData;
};

typedef TUniquePtr< FSVOData > FUniqueSVODataPtr;

FORCEINLINE const TArray< FSVOBoundsNavigationData > & FSVOData::GetNavigationBoundsData() const
{
    return NavigationBoundsData;
}