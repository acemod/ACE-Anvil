//------------------------------------------------------------------------------------------------
class ACE_Explosives_ToggleBuryUserAction : ACE_ContinousGadgetUserAction
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
		super.Init(pOwnerEntity, pManagerComponent);
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
			params.Flags = TraceFlags.WORLD;
			params.Start = pos + 0.02 * vector.Up;
			params.End = pos - 0.02 * vector.Up;
			params.Exclude = GetOwner();
			
			// Trace for terrain surface and only fall back to entities when none was found
			if (GetOwner().GetWorld().TraceMove(params, null) >= 1.0)
			{
				params.Flags |= TraceFlags.ENTS;
				GetOwner().GetWorld().TraceMove(params, null);
			}
			
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
