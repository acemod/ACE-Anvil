//------------------------------------------------------------------------------------------------
class ACE_InstantGadgetUserAction : ACE_BaseGadgetUserAction
{
	//------------------------------------------------------------------------------------------------
	//! Start gadget animation
	override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity) 
	{
		super.PerformAction(pOwnerEntity, pUserEntity);		
		TryUseGadget(pUserEntity);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected ItemUseParameters GetGadgetUseParams(IEntity user)
	{
		ItemUseParameters params = super.GetGadgetUseParams(user);
		if (!params)
			return null;
		
		params.SetMaxAnimLength(1);
		return params;
	}
}
