class ACE_Medical_Defibrillation_ReplicationHelper
{
	static IEntity GetEntityByRplId(RplId id)
	{
		if (!id.IsValid())
			return null;

		Managed instance = Replication.FindItem(id);
		if (!instance)
			return null;

		RplComponent rplComponent = RplComponent.Cast(instance);
		if (rplComponent)
			return rplComponent.GetEntity();

		return IEntity.Cast(instance);
	}

	static RplId GetRplIdByEntity(IEntity entity)
	{
		if (!entity)
			return RplId.Invalid();

		RplComponent rplComponent = RplComponent.Cast(entity.FindComponent(RplComponent));
		if (!rplComponent)
			return RplId.Invalid();

		return rplComponent.Id();
	}
}