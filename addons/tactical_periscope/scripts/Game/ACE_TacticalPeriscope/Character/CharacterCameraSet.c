//------------------------------------------------------------------------------------------------
//! Register periscopic camera
modded class CharacterCameraSet : ScriptedCameraSet
{
	static const int ACE_TACTICAL_PERISCOPE_CHARACTERCAMERA = 43;
	
	//------------------------------------------------------------------------------------------------
	override void Init()
	{
		super.Init();
		RegisterCameraCreator(ACE_TACTICAL_PERISCOPE_CHARACTERCAMERA, ACE_TacticalPeriscope_CharacterCamera);
	}
}
