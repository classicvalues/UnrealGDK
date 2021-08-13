// Copyright (c) Improbable Worlds Ltd, All Rights Reserved

#pragma once

#include "CoreMinimal.h"

#include "LoadBalancing/ActorSetSystem.h"
#include "LoadBalancing/LBDataStorage.h"
#include "LoadBalancing/LoadBalancingTypes.h"
#include "LoadBalancing/PlayerInterestManager.h"
#include "Schema/ActorSetMember.h"
#include "Schema/AuthorityIntent.h"
#include "Schema/CrossServerEndpoint.h"
#include "Schema/NetOwningClientWorker.h"
#include "Schema/ServerWorker.h"
#include "Schema/StandardLibrary.h"
#include "SpatialView/SubView.h"
#include "Utils/CrossServerUtils.h"

DECLARE_LOG_CATEGORY_EXTERN(LogSpatialStrategySystem, Log, All)

class SpatialVirtualWorkerTranslator;

namespace SpatialGDK
{
class ISpatialOSWorker;
class FLoadBalancingStrategy;
class FPartitionManager;

class FSpatialStrategySystem
{
public:
	FSpatialStrategySystem(TUniquePtr<FPartitionManager> InPartitionMgr, const FSubView& InLBView, const FSubView& InServerWorkerView,
						   TUniquePtr<FLoadBalancingStrategy> Strategy);

	~FSpatialStrategySystem();

	void Advance(ISpatialOSWorker& Connection);
	void Flush(ISpatialOSWorker& Connection);
	void Destroy(ISpatialOSWorker& Connection);

private:
	void ClearUserStorages();

	const FSubView& LBView;

	TUniquePtr<FPartitionManager> PartitionsMgr;

	// +++ Components watched to implement the strategy +++
	TLBDataStorage<AuthorityIntentACK> AuthACKView;
	TLBDataStorage<NetOwningClientWorker> NetOwningClientView;
	TLBDataStorage<ActorSetMember> SetMemberView;
	FPlayerControllerData PCData;
	FActorSetSystem ActorSetSystem;
	FActorInformation ActorInfo;
	FLBDataCollection DataStorages;
	FLBDataCollection UserDataStorages;
	FLBDataCollection ServerWorkerDataStorages;
	TSet<Worker_ComponentId> UpdatesToConsider;
	// --- Components watched to implement the strategy ---

	// +++ Components managed by the strategy worker +++
	TMap<Worker_EntityId_Key, AuthorityIntentV2> AuthorityIntentView;
	TMap<Worker_EntityId_Key, AuthorityDelegation> AuthorityDelegationView;
	// --- Components managed by the strategy worker ---

	// +++ Migration data +++
	TUniquePtr<FLoadBalancingStrategy> Strategy;
	TSet<Worker_EntityId_Key> MigratingEntities;
	TMap<Worker_EntityId_Key, FPartitionHandle> PendingMigrations;
	// --- Migration data ---

	void UpdateStrategySystemInterest(ISpatialOSWorker& Connection);
	bool bStrategySystemInterestDirty = false;
};
} // namespace SpatialGDK
