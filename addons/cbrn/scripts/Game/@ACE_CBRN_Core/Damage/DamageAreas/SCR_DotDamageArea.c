//------------------------------------------------------------------------------------------------
//! Add handling for protective equipment
modded class SCR_DotDamageArea : DamageArea
{
	[Attribute(defvalue: "AIRWAYS", desc: "Required protection to suvive in this area.", uiwidget: UIWidgets.Flags, enums: ParamEnumArray.FromEnum(ACE_CBRN_ECharacterProtectedArea))]
	protected ACE_CBRN_ECharacterProtectedArea m_eACE_CBRN_RequiredProtection;
	
	//------------------------------------------------------------------------------------------------
	//! Same as vanilla, but compartment handlers removed, as we do handling in SCR_PoisonDamageEffect instead
	override void OnAreaEntered(notnull IEntity entity)
	{
		AddEffect(entity);
	}
}
