#pragma once

#include "SVOPathFindingAlgorithm.h"
#include "SVOPathfindingRenderingComponent.h"

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>

#include "SVOPathFinderTest.generated.h"

class FSVOPathFinder;
class USphereComponent;

UCLASS( hidecategories = ( Object, Actor, Input, Rendering, Replication, LOD, Cooking, Physics, Collision, Lighting, VirtualTexture, HLOD ), showcategories = ( "Input|MouseInput", "Input|TouchInput" ), Blueprintable )
class SVONAVIGATION_API ASVOPathFinderTest : public AActor
{
    GENERATED_BODY()

public:
    ASVOPathFinderTest();

#if WITH_EDITOR
    void PreEditChange( FProperty * property_about_to_change ) override;
    void PostEditChangeProperty( FPropertyChangedEvent & property_changed_event ) override;
    void PostEditMove( bool is_finished ) override;
#endif

    FVector GetStartLocation() const;
    FVector GetEndLocation() const;
    const FSVOPathFinderDebugInfos & GetPathFinderDebugInfos() const;
    const FSVOPathRenderingDebugDrawOptions & GetDebugDrawOptions() const;
    ESVOPathFindingAlgorithmStepperStatus GetStepperLastStatus() const;
    EGraphAStarResult GetPathFindingResult() const;
    void BeginDestroy() override;

private:
    void UpdateDrawing();

#if WITH_EDITOR
    static void OnEditorSelectionChanged( UObject * new_selection );
#endif

    UFUNCTION( CallInEditor )
    void InitPathFinding();

    UFUNCTION( CallInEditor )
    void Step();

    UFUNCTION( CallInEditor )
    void AutoCompleteStepByStep();

    UFUNCTION( CallInEditor )
    void AutoCompleteUntilNextNode();

    UFUNCTION( CallInEditor )
    void AutoCompleteInstantly();

    UFUNCTION( CallInEditor )
    void PauseAutoCompletion();

    UPROPERTY( VisibleAnywhere, BlueprintReadOnly, meta = ( AllowPrivateAccess = "true" ) )
    USphereComponent * SphereComponent;

#if WITH_EDITORONLY_DATA
    UPROPERTY( Transient )
    USVOPathFindingRenderingComponent * RenderingComponent;
#endif

    UPROPERTY( EditAnywhere )
    uint8 bUpdatePathAfterMoving : 1;

    UPROPERTY( EditAnywhere )
    FNavAgentProperties NavAgentProperties;

    UPROPERTY( EditAnywhere )
    TSubclassOf< UNavigationQueryFilter > NavigationQueryFilter;

    UPROPERTY( EditAnywhere )
    FSVOPathRenderingDebugDrawOptions DebugDrawOptions;

    TSharedPtr< FSVOPathFindingAlgorithmStepper > Stepper;

    UPROPERTY( EditAnywhere )
    float AutoStepTimer;

    UPROPERTY( EditInstanceOnly )
    ASVOPathFinderTest * OtherActor;

    FNavigationPath NavigationPath;

    UPROPERTY( VisibleInstanceOnly, AdvancedDisplay )
    FSVOPathFinderDebugInfos PathFinderDebugInfos;

    uint8 bAutoComplete : 1;
    FTimerHandle AutoCompleteTimerHandle;
    ESVOPathFindingAlgorithmStepperStatus LastStatus;
    EGraphAStarResult PathFindingResult;
};

FORCEINLINE FVector ASVOPathFinderTest::GetStartLocation() const
{
    return GetActorLocation();
}

FORCEINLINE FVector ASVOPathFinderTest::GetEndLocation() const
{
    return OtherActor != nullptr
               ? OtherActor->GetActorLocation()
               : FVector::ZeroVector;
}

FORCEINLINE const FSVOPathFinderDebugInfos & ASVOPathFinderTest::GetPathFinderDebugInfos() const
{
    return PathFinderDebugInfos;
}

FORCEINLINE const FSVOPathRenderingDebugDrawOptions & ASVOPathFinderTest::GetDebugDrawOptions() const
{
    return DebugDrawOptions;
}

FORCEINLINE ESVOPathFindingAlgorithmStepperStatus ASVOPathFinderTest::GetStepperLastStatus() const
{
    return LastStatus;
}

FORCEINLINE EGraphAStarResult ASVOPathFinderTest::GetPathFindingResult() const
{
    return PathFindingResult;
}