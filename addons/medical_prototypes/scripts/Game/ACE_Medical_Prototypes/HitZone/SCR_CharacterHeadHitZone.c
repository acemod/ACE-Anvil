//-----------------------------------------------------------------------------------------------------------
//! Make the head a vital hit zone
modded class SCR_CharacterHeadHitZone : SCR_CharacterHitZone
{
	protected ref ACE_Medical_VitalHitZoneHelper m_HitZoneHelper;
	
	//-----------------------------------------------------------------------------------------------------------
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (Replication.IsServer())
			m_HitZoneHelper = new ACE_Medical_VitalHitZoneHelper(pOwnerEntity, pManagerComponent, this);
	}
}
