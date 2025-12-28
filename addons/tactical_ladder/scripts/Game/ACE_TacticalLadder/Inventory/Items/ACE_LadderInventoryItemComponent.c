//------------------------------------------------------------------------------------------------
class ACE_LadderInventoryItemComponentClass : SCR_MineInventoryItemComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_LadderInventoryItemComponent : SCR_MineInventoryItemComponent
{
	//------------------------------------------------------------------------------------------------
	//! Enforce vertical placement
	override bool OverridePlacementTransform(IEntity caller, out vector computedTransform[4])
	{
		super.OverridePlacementTransform(caller, computedTransform);
		TraceParam traceParams = new TraceParam();
		traceParams.TargetLayers = EPhysicsLayerPresets.Building;
		SCR_TerrainHelper.SnapToTerrain(computedTransform, GetOwner().GetWorld(), false, traceParams);
		Math3D.AnglesToMatrix(Vector(computedTransform[2].ToYaw(), 0, 0), computedTransform);
		return true;
	}
}
