//------------------------------------------------------------------------------------------------
class ACE_Radio_CopyKeyUserAction : ScriptedUserAction
{
	protected ACE_Radio_FillDeviceComponent m_pDevice;
	
	//------------------------------------------------------------------------------------------------
	override bool CanBeShownScript(IEntity user)
	{
		m_pDevice = ACE_Radio_FillDeviceComponent.GetHeldInstance(user);
		return m_pDevice && (m_pDevice != GetOwner().FindComponent(ACE_Radio_FillDeviceComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBePerformedScript(IEntity user)
	{
		if (!m_pDevice)
			return false;
		
		if (!m_pDevice.IsLoaded())
		{
			SetCannotPerformReason("Not Loaded");
			return false;
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		ACE_Radio_FillDeviceComponent device = ACE_Radio_FillDeviceComponent.GetHeldInstance(pUserEntity);
		if (!device)
			return;
		
		BaseRadioComponent radio = BaseRadioComponent.Cast(pOwnerEntity.FindComponent(BaseRadioComponent));
		if (radio)
			device.CopyCurrentKey(radio);
		
		ACE_Radio_FillDeviceComponent other = ACE_Radio_FillDeviceComponent.Cast(pOwnerEntity.FindComponent(ACE_Radio_FillDeviceComponent));
		if (other)
			other.CopyFrom(device);
		
		// Apply encryption key to associated bases
		SCR_AntennaServicePointComponent service = SCR_AntennaServicePointComponent.Cast(pOwnerEntity.FindComponent(SCR_AntennaServicePointComponent));
		if (service)
		{
			array<SCR_MilitaryBaseComponent> bases = {};
			service.GetBases(bases);
			
			foreach (SCR_MilitaryBaseComponent base : bases)
			{
				BaseRadioComponent baseRadio = BaseRadioComponent.Cast(base.GetOwner().FindComponent(BaseRadioComponent));
				if (baseRadio)
					device.CopyCurrentKey(baseRadio);
			}
			
			// Update connectivity
			SCR_RadioCoverageSystem.UpdateAll(true);
			
			SCR_GameModeCampaign conflict = SCR_GameModeCampaign.GetInstance();
			if (conflict)
				conflict.GetBaseManager().EvaluateControlPoints();
		}
		
		AudioSystem.PlaySound("Sounds/ACE_Radio/beep_high.wav");
	}
	
	//------------------------------------------------------------------------------------------------
	override bool GetActionNameScript(out string outName)
	{
		string keyName;
		if (GetOwner().FindComponent(ACE_Radio_FillDeviceComponent))
			keyName = m_pDevice.GetKeyGroupName();
		else
			keyName = m_pDevice.GetCurrentKeyName();
		
		outName = string.Format("Copy %1 to %2", keyName, GetOwnerName());
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	string GetOwnerName()
	{
		InventoryItemComponent item = InventoryItemComponent.Cast(GetOwner().FindComponent(InventoryItemComponent));
		if (item)
			return item.GetUIInfo().GetName();
		
		SCR_EditableEntityComponent editable = SCR_EditableEntityComponent.GetEditableEntity(GetOwner());
		if (editable)
			return editable.GetInfo().GetName();
		
		return "X";
	}
	
	//------------------------------------------------------------------------------------------------
	override bool CanBroadcastScript()
	{
		return false;
	}
}
