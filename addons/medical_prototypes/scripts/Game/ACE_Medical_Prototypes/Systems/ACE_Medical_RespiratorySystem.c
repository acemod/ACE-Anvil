//------------------------------------------------------------------------------------------------
class ACE_Medical_RespiratorySystem : ACE_Medical_BaseSystem
{
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false);
	}
}
