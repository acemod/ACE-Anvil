//------------------------------------------------------------------------------------------------
modded class SCR_DetonatorGadgetComponent : SCR_GadgetComponent
{
	//------------------------------------------------------------------------------------------------
	//! Workaround for https://report.bistudio.com/projects/arma-reforger/game-feedback/ARGF-165
	override void ConnectNewCharge(RplId explosiveChargeCompId, bool shouldReplicate = true)
	{
		if (explosiveChargeCompId.IsValid())
			super.ConnectNewCharge(explosiveChargeCompId, shouldReplicate);
	}
}