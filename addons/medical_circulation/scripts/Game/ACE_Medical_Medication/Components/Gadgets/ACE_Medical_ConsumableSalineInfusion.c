//------------------------------------------------------------------------------------------------
[BaseContainerProps()]
class ACE_Medical_ConsumableSalineInfusion : SCR_ConsumableSalineBag
{
	[Attribute(uiwidget: UIWidgets.SearchComboBox, desc: "Type of device that delivers the drug", enums: ParamEnumArray.FromEnum(SCR_EConsumableType))]
	protected SCR_EConsumableType m_eDeviceType;
	[Attribute(desc: "Configuration of the doses that gets administered")]
	protected ref array<ref ACE_Medical_Dose> m_Doses;
	
	//------------------------------------------------------------------------------------------------
	override bool CanApplyEffect(notnull IEntity target, notnull IEntity user, out SCR_EConsumableFailReason failReason)
	{
		failReason = SCR_EConsumableFailReason.NONE;
		m_eConsumableType = m_eDeviceType;
		if (!super.CanApplyEffect(target, user, failReason))
			return false;
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return true;
		
		array<ACE_Medical_EDrugType> drugs;
		array<ref array<ref ACE_Medical_Dose>> doses;
		medicationComponent.GetMedications(drugs, doses);
		
		int idx = drugs.Find(ACE_Medical_EDrugType.SALINE);
		if (idx < 0)
			return true;
		
		array<ref ACE_Medical_Dose> drugDoses = doses[idx];
		foreach (ACE_Medical_Dose dose : drugDoses)
		{
			ACE_Medical_Infusion infusion = ACE_Medical_Infusion.Cast(dose);
			if (!infusion)
				continue;
			
			if (!infusion.IsExpired())
			{
				failReason = SCR_EConsumableFailReason.ALREADY_APPLIED;
				return false;
			}
		}
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	override void ApplyEffect(notnull IEntity target, notnull IEntity user, IEntity item, ItemUseParameters animParams)
	{
		m_eConsumableType = m_eDeviceType;
		super.ApplyEffect(target, user, item, animParams);
		
		if (!Replication.IsServer())
			return;
		
		ACE_Medical_MedicationComponent medicationComponent = ACE_Medical_MedicationComponent.Cast(target.FindComponent(ACE_Medical_MedicationComponent));
		if (!medicationComponent)
			return;
		
		if (!m_Doses)
			return;
		
		foreach (ACE_Medical_Dose protoDose : m_Doses)
		{
			ACE_Medical_Dose doseInstance = protoDose.CloneDose();
			if (!doseInstance)
				continue;
			
			medicationComponent.AddMedication(doseInstance);
		}
		
		InventoryItemComponent itemComponent = InventoryItemComponent.Cast(item.FindComponent(InventoryItemComponent));
		if (!itemComponent)
			return;
		
		medicationComponent.AddLogEntry(itemComponent.GetUIInfo().GetName(), GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(user));
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_Medical_ConsumableSalineInfusion()
	{
		m_eConsumableType = m_eDeviceType;
	}
}
