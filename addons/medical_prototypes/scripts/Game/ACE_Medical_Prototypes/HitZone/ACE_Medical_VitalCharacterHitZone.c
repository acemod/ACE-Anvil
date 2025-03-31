//-----------------------------------------------------------------------------------------------------------
//! Same as character hit zone, but kills the character when destroyed
class ACE_Medical_VitalCharacterHitZone : SCR_CharacterHitZone
{
	protected ref ACE_Medical_VitalHitZoneHelper m_HitZoneHelper;
	protected const float SECOND_CHANCE_RESCALED_HEALTH = 0.1;
	
	//-----------------------------------------------------------------------------------------------------------
	//! Initialize member variables
	override void OnInit(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.OnInit(pOwnerEntity, pManagerComponent);
		
		if (Replication.IsServer())
			m_HitZoneHelper = new ACE_Medical_VitalHitZoneHelper(pOwnerEntity, pManagerComponent, this);
	}
}
