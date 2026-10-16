//------------------------------------------------------------------------------------------------
class ACE_Medical_InspectionOverlayComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "{6DCC70A71EF9EB70}Assets/UI/ACE_Medical_Inspection/BodyOverlay/BodyOverlay.xob", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "xob", category: "Meshes")]
	protected ResourceName m_sBodyMeshName;

	[Attribute(defvalue: "{02B40A17682D9DE3}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyUninjuredSelected.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Selected Materials")]
	protected ResourceName m_sUninjuredSelectedMaterial;
	
	[Attribute(defvalue: "{D79E43A625CB810F}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyUninjured.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Deselected Materials")]
	protected ResourceName m_sUninjuredMaterial;

	[Attribute(defvalue: "{B164F14A3D180484}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyHeavyBleeding.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Deselected Materials")]
	protected ResourceName m_sHeavyBleedingMaterial;

	[Attribute(defvalue: "{86E19C5A2FB043D7}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyHeavyBleedingSelected.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Selected Materials")]
	protected ResourceName m_sHeavyBleedingSelectedMaterial;

	[Attribute(defvalue: "{9EBE5A6E2C09E7C3}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyModerateBleeding.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Deselected Materials")]
	protected ResourceName m_sModerateBleedingMaterial;

	[Attribute(defvalue: "{A7C4603D95E218BF}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyModerateBleedingSelected.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Selected Materials")]
	protected ResourceName m_sModerateBleedingSelectedMaterial;

	[Attribute(defvalue: "{67FDFE9009E9F807}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyImproving.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Deselected Materials")]
	protected ResourceName m_sImprovingMaterial;

	[Attribute(defvalue: "{3E92B7D4A06F815C}Assets/UI/ACE_Medical_Inspection/BodyOverlay/Data/BodyImprovingSelected.emat", uiwidget: UIWidgets.ResourcePickerThumbnail, params: "emat", category: "Selected Materials")]
	protected ResourceName m_sImprovingSelectedMaterial;
	
	protected static ref const SCR_InventoryHitZonePointUI s_HitZonePointUI = new SCR_InventoryHitZonePointUI();

	
	//------------------------------------------------------------------------------------------------
	ResourceName GetBodyMeshName()
	{
		return m_sBodyMeshName;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetUninjuredMaterial(bool highlighted = false)
	{
		if (highlighted)
			return m_sUninjuredSelectedMaterial;
		else
			return m_sUninjuredMaterial;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetHeavyBleedingMaterial(bool highlighted = false)
	{
		if (highlighted)
			return m_sHeavyBleedingSelectedMaterial;
		else
			return m_sHeavyBleedingMaterial;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetModerateBleedingMaterial(bool highlighted = false)
	{
		if (highlighted)
			return m_sModerateBleedingSelectedMaterial;
		else
			return m_sModerateBleedingMaterial;
	}

	//------------------------------------------------------------------------------------------------
	ResourceName GetImprovingMaterial(bool highlighted = false)
	{
		if (highlighted)
			return m_sImprovingSelectedMaterial;
		else
			return m_sImprovingMaterial;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Medical_InspectionOverlayComponent : ScriptComponent
{
}

