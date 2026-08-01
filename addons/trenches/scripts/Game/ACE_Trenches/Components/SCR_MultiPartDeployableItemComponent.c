//------------------------------------------------------------------------------------------------
modded class SCR_MultiPartDeployableItemComponent : SCR_BaseDeployableInventoryItemComponent
{
	//------------------------------------------------------------------------------------------------
	//! Consider items in nearby containers
	override protected bool EvaluateFoundEntity(IEntity ent)
	{
		if (!super.EvaluateFoundEntity(ent))
			return false;
		
		if (ent.GetParent() || ChimeraCharacter.Cast(ent))
			return true;
		
		BaseUniversalInventoryStorageComponent storageComponent = BaseUniversalInventoryStorageComponent.Cast(ent.FindComponent(BaseUniversalInventoryStorageComponent));
		if (!storageComponent)
			return true;
		
		float distToEnt = vector.DistanceSq(GetOwner().GetOrigin(), ent.GetOrigin());
		
		array<IEntity> items = {};
		storageComponent.GetAll(items);

		if (m_aIgnoredItems)
		{
			foreach (IEntity ignoredItem : m_aIgnoredItems)
			{
				items.RemoveItem(ignoredItem);
			}
		}

		BaseContainer foundPrefab;
		foreach (IEntity item : items)
		{
			foundPrefab = item.GetPrefabData().GetPrefab();
			foreach (SCR_RequiredDeployablePart partReq : m_aFoundElements)
			{
				if (partReq.EvaluateFoundEntity(ent.GetOrigin(), distToEnt, foundPrefab, item))
					break; // skip further evaluation as it already matches one of the requirements
			}
		}

		return true;
	}
}
