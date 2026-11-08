//------------------------------------------------------------------------------------------------
modded class SCR_InventoryHitZonePointUI : ScriptedWidgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Show blood classes instead of vanilla states
	override void GetBloodInfoText(float bloodHitZoneHealthScaled, out string bloodLossText)
	{
		ChimeraCharacter char = ChimeraCharacter.Cast(GetGame().GetPlayerController().GetControlledEntity());
		if (!char)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(char.GetDamageManager());
		if (!damageManager)
			return;
		
		switch (damageManager.GetBloodHitZone().GetDamageState())
		{
			case ACE_Medical_EBloodState.CLASS_1_HEMORRHAGE: { bloodLossText = "#ACE_Medical-BloodState_Class1"; return; }
			case ACE_Medical_EBloodState.CLASS_2_HEMORRHAGE: { bloodLossText = "#ACE_Medical-BloodState_Class2"; return; }
			case ACE_Medical_EBloodState.CLASS_3_HEMORRHAGE: { bloodLossText = "#ACE_Medical-BloodState_Class3"; return; }
			case ACE_Medical_EBloodState.CLASS_4_HEMORRHAGE: { bloodLossText = "#ACE_Medical-BloodState_Class4"; return; }
		}
	}
}
