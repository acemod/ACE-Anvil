//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableMedication : SCR_ConsumableEffectHealthItems
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of device thad delivers the drug", enums: ParamEnumArray.FromEnum(SCR_EConsumableType))]
	protected SCR_EConsumableType m_eDeviceType;
	
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of administered drug", enums: ParamEnumArray.FromEnum(ACE_Medical_EDrugType))]
	protected ACE_Medical_EDrugType m_eDrugType;
	
	[Attribute(desc: "Configuration of the dose that gets administered")]
	protected ref ACE_Medical_Dose m_Dose;
	
	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Can always be applied in advanced medical
	override bool CanApplyEffectToHZ(notnull IEntity target, notnull IEntity user, ECharacterHitZoneGroup group, out SCR_EConsumableFailReason failReason = SCR_EConsumableFailReason.NONE)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return;
		
		medicationComponent.AddMedication(m_eDrugType, m_Dose);
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ConsumableMedication()
	{
		m_eConsumableType = m_eDeviceType;
	}
}
