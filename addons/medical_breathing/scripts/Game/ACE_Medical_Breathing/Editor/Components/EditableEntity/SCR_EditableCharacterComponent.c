//------------------------------------------------------------------------------------------------
modded class SCR_EditableCharacterComponent : SCR_EditableEntityComponent
{
	//-----------------------------------------------------------------------------------------------------------
	//! TO DO: Remove once systems support inheritance
	override void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		ACE_Medical_BaseSystem3 system3 = ACE_Medical_BaseSystem3.GetInstance(ACE_Medical_RespiratorySystem);
		if (system3)
			system3.Unregister(owner);
	}
	//-----------------------------------------------------------------------------------------------------------
}
