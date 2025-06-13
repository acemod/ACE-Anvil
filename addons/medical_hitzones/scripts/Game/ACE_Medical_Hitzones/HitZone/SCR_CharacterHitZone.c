//------------------------------------------------------------------------------------------------
//! Add handling for vital hit zones
modded class SCR_CharacterHitZone : SCR_RegeneratingHitZone
{
	[Attribute(defvalue: "false", desc: "Whether the hit zone is vital and will kill the character when destroyed")]
	protected bool m_bACE_Medical_IsVital;
	
	[Attribute(desc: "Organ hit zones", uiwidget: UIWidgets.ComboBox, enums: SCR_ParamEnumArray.ACE_Medical_FromCharacterBaseHitZones())]
	protected ref array<string> m_aACE_Medical_OrganHitZoneNames;
	protected ref array<ACE_Medical_OrganHitZone> m_aACE_Medical_OrganHitZones = {};
	
	//-----------------------------------------------------------------------------------------------------------
	//! Friend method of SCR_CharacterDamageManagerComponent
	void ACE_Medical_InitOrganHitZones(SCR_CharacterDamageManagerComponent damageManager)
	{
		foreach (string name : m_aACE_Medical_OrganHitZoneNames)
		{
			m_aACE_Medical_OrganHitZones.Insert(ACE_Medical_OrganHitZone.Cast(damageManager.GetHitZoneByName(name)));
		}
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Vital hit zones trigger registration at ACE_Medical_SecondChanceSystem when destroyed
	override void OnDamageStateChanged(EDamageState newState, EDamageState previousDamageState, bool isJIP)
	{
		super.OnDamageStateChanged(newState, previousDamageState, isJIP);
		
		if (!Replication.IsServer() || newState != EDamageState.DESTROYED)
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		//! Add bleeding to physical hit zones that get destroyed
		damageManager.AddBleedingEffectOnHitZone(this);
		
		if (!m_bACE_Medical_IsVital)
			return;
		
		ACE_Medical_SecondChanceSystem system = ACE_Medical_SecondChanceSystem.GetInstance();
		if (system)
			system.Register(damageManager, this);
		else
			damageManager.Kill(damageManager.GetInstigator());
	}
	
	//-----------------------------------------------------------------------------------------------------------
	//! Handling for organ hit zones
	override void OnDamage(notnull BaseDamageContext damageContext)
	{
		super.OnDamage(damageContext);
		
		if (!Replication.IsServer())
			return;
		
		SCR_CharacterDamageManagerComponent damageManager = SCR_CharacterDamageManagerComponent.Cast(GetHitZoneContainer());
		if (!damageManager)
			return;
		
		foreach (ACE_Medical_OrganHitZone organHitZone : m_aACE_Medical_OrganHitZones)
		{
			if (!organHitZone.ShouldCountAsHit(damageContext))
				continue;
			
			BaseDamageContext organDamageContext = BaseDamageContext.Cast(damageContext.Clone());
			organDamageContext.struckHitZone = organHitZone;
			damageManager.HandleDamage(organDamageContext);
		}
	}
}
