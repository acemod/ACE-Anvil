//------------------------------------------------------------------------------------------------
modded class SCR_EditableCharacterComponent : SCR_EditableEntityComponent
{
	//------------------------------------------------------------------------------------------------
	//! Unregister from all medical systems
	override void OnDelete(IEntity owner)
	{
		super.OnDelete(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		//-----------------------------------------------------------------------------------------------------------
		//! TO DO: Replace solution once systems support inheritance
		/*
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(owner);
		if (!ownerChar)
			return;
		
		array<Managed> components = {};
		ownerChar.FindComponents(ACE_Medical_BaseComponent, components);
		
		foreach (Managed component : components)
		{
			ACE_Medical_BaseComponent medicalComponent = ACE_Medical_BaseComponent.Cast(component);
			
			ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(medicalComponent.GetAssociatedSystemType());
			if (system)
				system.Unregister(ownerChar);
		}
		*/
		
		SCR_ChimeraCharacter ownerChar = SCR_ChimeraCharacter.Cast(owner);
		if (!ownerChar)
			return;
		
		ACE_Medical_BaseSystem system = ACE_Medical_BaseSystem.GetInstance(ACE_Medical_CardiovascularSystem);
		if (system)
			system.Unregister(ownerChar);
		
		ACE_Medical_BaseSystem2 system2 = ACE_Medical_BaseSystem2.GetInstance(ACE_Medical_MedicationSystem);
		if (system2)
			system2.Unregister(ownerChar);
		
		//-----------------------------------------------------------------------------------------------------------
	}
}
