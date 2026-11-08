//------------------------------------------------------------------------------------------------
modded class SCR_InspectCasualtyWidget : SCR_InfoDisplayExtended
{
	//------------------------------------------------------------------------------------------------
	//! Add fracture data
	override void UpdateWidgetData()
	{
		super.UpdateWidgetData();
		
		if (!m_Target || !m_wCasualtyInspectWidget)
			return;
		
		SCR_InventoryDamageInfoUI damageInfoUI = SCR_InventoryDamageInfoUI.Cast(m_wCasualtyInspectWidget.FindHandler(SCR_InventoryDamageInfoUI));
		if (!damageInfoUI)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(m_Target.FindComponent(SCR_CharacterDamageManagerComponent));
		if (!damageManager)
			return;
		
		damageInfoUI.SetFractureStateVisible(damageManager.GetAimingDamage() > 0, damageManager.GetMovementDamage() > 0);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get global health from ACE getter
	override protected void GetDamageInfo(SCR_InventoryHitZonePointUI hitZonePointUI, IEntity targetEntity, inout float bleedingRate, inout array<bool> hZGroupsBleeding, inout int damageIntensity, inout bool regenerating, inout bool isTourniquetted, inout bool isSalineBagged, inout bool isMorphined)
	{
		super.GetDamageInfo(hitZonePointUI, targetEntity, bleedingRate, hZGroupsBleeding, damageIntensity, regenerating, isTourniquetted, isSalineBagged, isMorphined);
		
		ChimeraCharacter character = ChimeraCharacter.Cast(targetEntity);
		if (!character)
			return;

		SCR_CharacterDamageManagerComponent damageMan = SCR_CharacterDamageManagerComponent.Cast(character.GetDamageManager());
		if (!damageMan)
			return;
		
		float health = damageMan.ACE_Medical_GetHealthScaled();
		if (health == 1)
			damageIntensity = 0;
		else if (health < hitZonePointUI.m_fLowDamageThreshold)
			damageIntensity = 4;
		else if (health < hitZonePointUI.m_fMediumDamageThreshold)
			damageIntensity = 3;
		else if (health < hitZonePointUI.m_fHighDamageThreshold)
			damageIntensity = 2;
		else
			damageIntensity = 1;
	}
}
