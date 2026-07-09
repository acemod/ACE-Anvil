//------------------------------------------------------------------------------------------------
class ACE_Radio_CopyKeyUserAction : ScriptedUserAction
{
	[Attribute(defvalue: "#ACE_Radio-UserAction_CopyKey", desc: "Action string for copying key")]
	protected LocalizedString m_sCopyTo;
	
	[Attribute(defvalue: "#ACE_Radio-FailReason_NotLoaded", desc: "String for when not loaded")]
	protected LocalizedString m_sNotLoaded;
	
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
			SetCannotPerformReason(m_sNotLoaded);
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
		
		outName = WidgetManager.Translate(m_sCopyTo, keyName, GetOwnerName());
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
