//------------------------------------------------------------------------------------------------
class ACE_Explosives_ToggleBuryUserAction : ACE_ShovelUserAction
{	
	[Attribute(defvalue: "#ACE-Explosives_UserAction_Bury_Name", desc: "String for bury action")]
	protected LocalizedString m_sBury;
	
	[Attribute(defvalue: "#ACE-Explosives_UserAction_Unbury_Name", desc: "String for unbury action")]
	protected LocalizedString m_sUnbury;
	
	[Attribute(defvalue: "#ACE-Explosives_FailReason_HardSurface", desc: "String for hard surface")]
	protected LocalizedString m_sHardSurface;
		
	protected SCR_MineWeaponComponent m_pWeaponComponent;
	protected vector m_vOwnerPos;
	protected bool m_bOnDiggableSurface = false;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		m_pWeaponComponent = SCR_MineWeaponComponent.Cast(pOwnerEntity.FindComponent(SCR_MineWeaponComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		super.PerformAction(pOwnerEntity, pUserEntity);	
		m_pWeaponComponent.ACE_Exposives_ToggleBurial(!m_pWeaponComponent.ACE_Exposives_IsBuried());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		if (m_pWeaponComponent.ACE_Exposives_IsBuried())
			outName = m_sUnbury;
		else
			outName = m_sBury;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override int GetAnimationIndex()
	{
		return 1 + m_pWeaponComponent.ACE_Exposives_IsBuried();
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!super.CanBePerformedScript(user))
			return false;
		
		// Always allow unburying
		if (m_pWeaponComponent.ACE_Exposives_IsBuried())
			return true;
		
		// Only allow burying when surface is diggable
		vector pos = GetOwner().GetOrigin();
		
		if (m_vOwnerPos != pos)
		{
			TraceParam params = new TraceParam();
			params.Flags = TraceFlags.WORLD | TraceFlags.ENTS;
			params.Start = pos + 0.01 * vector.Up;
			params.End = pos - 0.01 * vector.Up;
			params.Exclude = GetOwner();
			GetGame().GetWorld().TraceMove(params, null);
			m_bOnDiggableSurface = ACE_SurfaceHelper.HasLabel(params.SurfaceProps, ACE_ESurfaceLabel.DIGGABLE);
			m_vOwnerPos = pos;
		}
		
		if (!m_bOnDiggableSurface)
		{
			SetCannotPerformReason(m_sHardSurface);
			return false;
		}

		return true;
	}
}
