//------------------------------------------------------------------------------------------------
[EntityEditorProps(category: "GameScripted/Gadgets", description: "Handcuffs", color: "0 0 255 255")]
class ACE_Captives_HandcuffsGadgetComponentClass : SCR_ConsumableItemComponentClass
{
}

//------------------------------------------------------------------------------------------------
class ACE_Captives_HandcuffsGadgetComponent : SCR_ConsumableItemComponent
{
	//------------------------------------------------------------------------------------------------
	//! Disable SCR_ConsumableItemComponent::ModeSwitch
	override void ModeSwitch(EGadgetMode mode, IEntity charOwner);

	//------------------------------------------------------------------------------------------------
	//! Disable SCR_ConsumableItemComponent::ModeClear
	override void ModeClear(EGadgetMode mode);

	//------------------------------------------------------------------------------------------------
	//! Disable SCR_ConsumableItemComponent::ActivateAction
	override void ActivateAction();
}
