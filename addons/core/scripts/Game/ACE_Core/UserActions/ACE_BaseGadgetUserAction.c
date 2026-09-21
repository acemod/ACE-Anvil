//------------------------------------------------------------------------------------------------
//! Gadget entity user action
class ACE_BaseGadgetUserAction : ScriptedUserAction
{
	[Attribute(desc: "Class name of required component on gadget.")]
	protected string m_sRequiredGadgetComponent;
	protected typename m_tRequiredGadgetComponent;
	
	[Attribute(defvalue: "1", desc: "Index of the gadget animation to play")]
	protected int m_iAnimationIndex;
	
	protected SCR_ChimeraCharacter m_pUserChar;
	protected SoundComponent m_pItemSoundComponent;
	protected AnimationEventID m_iSoundEventID;
	
	//------------------------------------------------------------------------------------------------
	override void Init(IEntity pOwnerEntity, GenericComponent pManagerComponent)
	{
		super.Init(pOwnerEntity, pManagerComponent);
		m_tRequiredGadgetComponent = m_sRequiredGadgetComponent.ToType();
		m_iSoundEventID = GameAnimationUtils.RegisterAnimationEvent("Sound");
	}
	
	//------------------------------------------------------------------------------------------------
	//! Check whether the held gadget is the correct one
	override bool CanBeShownScript(IEntity user)
	{
		IEntity gadget = GetHeldGadget(user);
		if (!gadget)
			return false;
		
		if (!gadget.FindComponent(m_tRequiredGadgetComponent))
			return false;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected bool TryUseGadget(IEntity user)
	{
		IEntity gadget = GetHeldGadget(user);
		if (gadget)
			m_pItemSoundComponent = SoundComponent.Cast(gadget.FindComponent(SoundComponent));
		
		m_pUserChar = SCR_ChimeraCharacter.Cast(user);
		if (!m_pUserChar)
			return false;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(m_pUserChar.GetCharacterController());
		if (!userCharController)
			return false;
		
		if (m_pItemSoundComponent)			
			userCharController.GetOnAnimationEvent().Insert(HandleItemSoundEvent);
		
		userCharController.m_OnItemUseEndedInvoker.Insert(OnGadgetUseEnded);
		userCharController.TryUseItemOverrideParams(GetGadgetUseParams(m_pUserChar));
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	protected ItemUseParameters GetGadgetUseParams(IEntity user)
	{
		SCR_ChimeraCharacter userChar = SCR_ChimeraCharacter.Cast(user);
		if (!userChar)
			return null;
		
		CharacterAnimationComponent animationComponent = userChar.GetAnimationComponent();
		if (!animationComponent)
			return null;
		
		ItemUseParameters params = new ItemUseParameters();
		params.SetEntity(GetHeldGadget(user));
		params.SetAllowMovementDuringAction(false);
		params.SetKeepInHandAfterSuccess(true);
		params.SetCommandID(animationComponent.BindCommand("CMD_Item_Action"));
		params.SetCommandIntArg(GetAnimationIndex());
		return params;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Pass animation events to sound component
	protected void HandleItemSoundEvent(AnimationEventID animEventType, AnimationEventID animUserString, int intParam, float timeFromStart, float timeToEnd)
	{
		if (m_iSoundEventID != animEventType)
			return;
		
		if (m_pItemSoundComponent)
			m_pItemSoundComponent.SoundEvent(GameAnimationUtils.GetEventString(animUserString));
	}
	
	//------------------------------------------------------------------------------------------------
	//! Remove handlers when gadget usage is finished
	protected void OnGadgetUseEnded(IEntity item, bool successful, ItemUseParameters animParams)
	{
		if (!m_pUserChar)
			return;
		
		SCR_CharacterControllerComponent userCharController = SCR_CharacterControllerComponent.Cast(m_pUserChar.GetCharacterController());
		if (!userCharController)
			return;
		
		if (m_pItemSoundComponent)			
			userCharController.GetOnAnimationEvent().Remove(HandleItemSoundEvent);
		
		userCharController.m_OnItemUseEndedInvoker.Remove(OnGadgetUseEnded);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Get gadget entity
	IEntity GetHeldGadget(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return null;
		
		return gadgetManager.GetHeldGadget();
	}
	
	//------------------------------------------------------------------------------------------------
	int GetAnimationIndex()
	{
		return m_iAnimationIndex;
	}
	
	//------------------------------------------------------------------------------------------------
	//! PerformAction is run on all machines
	override bool CanBroadcastScript()
	{
		return true;
	}
}
