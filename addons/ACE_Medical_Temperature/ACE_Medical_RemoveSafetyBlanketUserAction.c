class ACE_Medical_RemoveSafetyBlanketUserAction : SCR_HealingUserAction
{
	
	override bool CanBeShown(IEntity user)
	{	
		ACE_EquipmentStorageComponent storageComponent = ACE_EquipmentStorageComponent.Cast(target.FindComponent(ACE_EquipmentStorageComponent));
		if (!storageComponent)
			return false;
		
		if (storageComponent.IsSlotOccupied(ACE_EEquipementSlot.SAFETY_BLANKET))
		{
			return true;
		}
		
		return false;	
	}
	
	override bool CanBePerformed()
	{
		return true;
	
	}

}