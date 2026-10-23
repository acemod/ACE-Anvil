//------------------------------------------------------------------------------------------------
class ACE_RadialMenuComponentClass : ScriptComponentClass
{
}

//------------------------------------------------------------------------------------------------
//! Component on player controller for registering radial menus
class ACE_RadialMenuComponent : ScriptComponent
{
	[Attribute()]
	protected ref array<ref SCR_RadialMenuController> m_aRadialMenuControllers;
}
