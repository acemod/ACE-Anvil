//------------------------------------------------------------------------------------------------
modded class SCR_ExplosiveChargeComponent : ScriptGameComponent
{
	//------------------------------------------------------------------------------------------------
	//! Workaround for https://report.bistudio.com/projects/arma-reforger/game-feedback/ARGF-165
	override void ConnectDetonator(SCR_EFuzeType fuzeType, RplId detonatorId = RplId.Invalid(), bool shouldReplicate = true, bool silent = false)
	{
		if (detonatorId.IsValid())
			super.ConnectDetonator(fuzeType, detonatorId, shouldReplicate, silent);
	}
}
