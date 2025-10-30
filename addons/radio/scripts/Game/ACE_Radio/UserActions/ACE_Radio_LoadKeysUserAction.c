//------------------------------------------------------------------------------------------------
class ACE_Radio_LoadKeysUserAction : ScriptedUserAction
{
	[Attribute(desc: "ID of the encryption key group")]
	protected int m_iGroupID;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		return ACE_Radio_FillDeviceComponent.GetHeldInstance(user);
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		Faction ownerFaction = GetOwnerFaction();
		if (!ownerFaction)
			return false;
		
		FactionAffiliationComponent userFactionAffiliation = FactionAffiliationComponent.Cast(user.FindComponent(FactionAffiliationComponent));
		if (!userFactionAffiliation)
			return false;
		
		Faction userFaction = userFactionAffiliation.GetAffiliatedFaction();
		if (!userFaction)
			return false;
		
		if (ownerFaction.IsFactionEnemy(userFaction))
		{
			SetCannotPerformReason("#AR-UserAction_SeatHostile");
			return false;
		}
		
	#ifndef ENABLE_DIAG
		// Make sure that you can use other encryption keys than the default for the faction
		if (!GetGame().GetVONCanTransmitCrossFaction())
		{
			SetCannotPerformReason("VONCanTransmitCrossFaction disabled");
			return false;
		}
	#endif
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Radio_FillDeviceComponent device = ACE_Radio_FillDeviceComponent.GetHeldInstance(pUserEntity);
		if (!device)
			return;
		
		Faction ownerFaction = GetOwnerFaction();
		if (!ownerFaction)
			return;
		
		FactionManager factionManager = GetGame().GetFactionManager();
		if (!factionManager)
			return;
		
		device.LoadKeys(factionManager.GetFactionIndex(ownerFaction), m_iGroupID);
		AudioSystem.PlaySound("Sounds/ACE_Radio/beep_high.wav");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		SCR_Faction ownerFaction = GetOwnerFaction();
		if (!ownerFaction)
			return false;
		
		SCR_FactionCallsignInfo info = ownerFaction.GetCallsignInfo();
		if (!info)
			return false;
		
		outName = string.Format("Load %1", WidgetManager.Translate(info.GetCompanyCallsignName(m_iGroupID)));
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected SCR_Faction GetOwnerFaction()
	{
		FactionAffiliationComponent ownerFactionAffiliation = FactionAffiliationComponent.Cast(GetOwner().FindComponent(FactionAffiliationComponent));
		if (!ownerFactionAffiliation)
			return null;
		
		return SCR_Faction.Cast(ownerFactionAffiliation.GetAffiliatedFaction());
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}

#ifndef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
//! Workaround, as VONCanTransmitCrossFaction cannot be enabled in workbench
modded class SCR_VONController
{
	//------------------------------------------------------------------------------------------------
 	override protected bool ActivateVON(notnull SCR_VONEntry entry, EVONTransmitType transmitType = EVONTransmitType.NONE)
	{
		if (!m_VONComp)
			return false;
		
		if (transmitType == EVONTransmitType.NONE)
		{
			if (entry.GetVONMethod() == ECommMethod.SQUAD_RADIO)
				transmitType = EVONTransmitType.CHANNEL;
			else
				transmitType = EVONTransmitType.DIRECT;
		}

		m_eVONType = transmitType;		
		SetActiveTransmit(entry);
		m_VONComp.SetCapture(true);
		m_bIsActive = true;
		return true;
	}
}
#endif