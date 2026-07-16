//------------------------------------------------------------------------------------------------
class ACE_CBRN_CharacterProtectionComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Holds data on how well the character is protected from CBRN hazards
class ACE_CBRN_CharacterProtectionComponent : ScriptComponent
{
	protected ACE_CBRN_ECharacterProtectedArea m_eProtectedAreas;
	protected ref ScriptInvoker m_OnStateChanged;
	
	//------------------------------------------------------------------------------------------------
	ACE_CBRN_ECharacterProtectedArea GetProtectedAreas()
	{
		return m_eProtectedAreas;
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasRequiredProtection(ACE_CBRN_ECharacterProtectedArea requirements)
	{
		return (m_eProtectedAreas & requirements) == requirements;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Update protection state
	[Friend(ACE_CBRN_ProtectiveGadgetComponent)]
	protected void OnProtectiveEquipmentChanged()
	{
		ACE_CBRN_ECharacterProtectedArea protectedAreas = 0;
		
		EquipedLoadoutStorageComponent loadout = EquipedLoadoutStorageComponent.Cast(GetOwner().FindComponent(EquipedLoadoutStorageComponent));
		if (!loadout)
			return;
		
		array<IEntity> items = {};
		loadout.GetAll(items);
		
		foreach (IEntity item : items)
		{
			ACE_CBRN_ProtectiveGadgetComponent gadget = ACE_CBRN_ProtectiveGadgetComponent.Cast(item.FindComponent(ACE_CBRN_ProtectiveGadgetComponent));
			if (!gadget)
				continue;
			
			ACE_CBRN_ProtectiveGadgetComponentClass gadgetData = ACE_CBRN_ProtectiveGadgetComponentClass.Cast(gadget.GetComponentData(gadget.GetOwner()));
			if (!gadgetData)
				continue;
			
			protectedAreas |= gadgetData.GetProtectedAreas();
		}
		
		if (m_eProtectedAreas == protectedAreas)
			return;
		
		m_eProtectedAreas = protectedAreas;
		
		if (m_OnStateChanged)
			m_OnStateChanged.Invoke(ChimeraCharacter.Cast(GetOwner()), m_eProtectedAreas);
	}
	
	//------------------------------------------------------------------------------------------------
	ScriptInvoker GetOnStateChanged()
	{
		if (!m_OnStateChanged)
			m_OnStateChanged = new ScriptInvoker();
		
		return m_OnStateChanged;
	}
}

//------------------------------------------------------------------------------------------------
[EnumBitFlag()]
enum ACE_CBRN_ECharacterProtectedArea
{
	EYES,
	AIRWAYS,
}
