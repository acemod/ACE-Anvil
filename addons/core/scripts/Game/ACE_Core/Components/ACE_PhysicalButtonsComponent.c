//------------------------------------------------------------------------------------------------
class ACE_PhysicalButtonsComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Component for registering physical buttons on an entity
class ACE_PhysicalButtonsComponent : ScriptComponent
{
	[Attribute()]
	protected ref array<ref ACE_PhysicalButtonConfig> m_aButtonConfigs;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		SetEventMask(owner, EntityEvent.INIT);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnInit(IEntity owner)
	{
		super.EOnInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		foreach (ACE_PhysicalButtonConfig buttonConfig : m_aButtonConfigs)
		{
			buttonConfig.Init(owner);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	void SetButtonState(string colliderName, bool state)
	{
		ACE_PhysicalButtonConfig config = GetButtonConfig(colliderName);
		if (!config)
			return;
		
		Animation anim = GetOwner().GetAnimation();
		if (!anim)
			return;
		
		TNodeId boneID = anim.GetBoneIndex(config.m_sBoneName);
		vector transform[4];
		Math3D.MatrixIdentity4(transform);
		
		if (state)
			transform[3] = config.m_vPressedOffset;
		
		anim.SetBoneMatrix(GetOwner(), boneID, transform);
		
		if (!state)
			PrintFormat("Button %1 clicked", colliderName, level: LogLevel.DEBUG);
		
		if (state)
			AudioSystem.PlaySound(config.m_sPressedSoundPath);
		else
			AudioSystem.PlaySound(config.m_sReleasedSoundPath);
		
		if (!state && config.m_pCommand)
			config.m_pCommand.Execute();
	}
	
	//------------------------------------------------------------------------------------------------
	ACE_PhysicalButtonConfig GetButtonConfig(string colliderName)
	{
		foreach (ACE_PhysicalButtonConfig buttonConfig : m_aButtonConfigs)
		{
			if (buttonConfig.m_sColliderName == colliderName)
				return buttonConfig;
		}
		
		return null;
	}
	
	//------------------------------------------------------------------------------------------------
	array<ref ACE_PhysicalButtonConfig> GetAllButtonConfigs()
	{
		return m_aButtonConfigs;
	}
}

//------------------------------------------------------------------------------------------------
//! Config for a physical button
[BaseContainerProps(), BaseContainerCustomTitleField("m_sColliderName")]
class ACE_PhysicalButtonConfig
{
	[Attribute(desc: "Name of the collider of the button")]
	string m_sColliderName;
	
	[Attribute(desc: "Name of the button's bone")]
	string m_sBoneName;
	
	[Attribute(defvalue: "0 -0.001 0", desc: "Displacement vector for when the button is pressed button")]
	vector m_vPressedOffset;
	
	[Attribute(defvalue: "{9DDDC275FFCC545F}Sounds/Props/Military/Radios/Samples/Props_Radios_Button_Press_1.wav", desc: "Sound for when the button is pressed", uiwidget: UIWidgets.ResourceNamePicker, params: "wav")]
	ResourceName m_sPressedSoundPath;
	
	[Attribute(defvalue: "{A8CB8AE57458C226}Sounds/Props/Military/Radios/Samples/Props_Radios_Button_Release_1.wav", desc: "Sound for when the button is released", uiwidget: UIWidgets.ResourceNamePicker, params: "wav")]
	ResourceName m_sReleasedSoundPath;
	
	[Attribute(desc: "Command that the button executes")]
	ref ACE_IGadgetCommand m_pCommand;
	
	//------------------------------------------------------------------------------------------------
	void Init(IEntity gadget)
	{
		if (m_pCommand)
			m_pCommand.Init(gadget);
	}
}
