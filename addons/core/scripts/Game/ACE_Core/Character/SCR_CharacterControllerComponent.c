//------------------------------------------------------------------------------------------------
modded class SCR_CharacterControllerComponent : CharacterControllerComponent
{
	[RplProp()]
	protected bool m_bACE_IsCarrier = false;
	[RplProp()]
	protected bool m_bACE_IsCarried = false;
	
	protected IEntity m_pCarrier;
	protected IEntity m_pCarried;
	
	//------------------------------------------------------------------------------------------------
	void ACE_Carry(IEntity carriable)
	{
		ACE_CarriableEntityComponent carriableComponent = ACE_CarriableEntityComponent.GetCarriableEntity(carriable);
		if (carriableComponent)
			carriableComponent.Carry(SCR_ChimeraCharacter.Cast(GetOwner()));
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_ReleaseCarried(IEntity carriable)
	{
		ACE_CarriableEntityComponent carriableComponent = ACE_CarriableEntityComponent.GetCarriableEntity(carriable);
		if (!carriableComponent)
			return;
		
		if (GetOwner() != carriableComponent.GetCarrier())
			return;
		
		carriableComponent.Release();
	}
		
	//------------------------------------------------------------------------------------------------
	void ACE_SetCarrier(IEntity carrier)
	{
		m_pCarrier = carrier;
		m_bACE_IsCarried = (carrier != null);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity ACE_GetCarrier()
	{
		return m_pCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	void ACE_SetCarried(IEntity carried)
	{
		m_pCarried = carried;
		m_bACE_IsCarrier = (carried != null);
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	IEntity ACE_GetCarried()
	{
		return m_pCarried;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_IsCarrier()
	{
		return m_bACE_IsCarrier;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_IsCarried()
	{
		return m_bACE_IsCarried;
	}
	
	//------------------------------------------------------------------------------------------------
	bool ACE_CanCarry(IEntity entity, out string cannotPerformReason)
	{
		if (ACE_IsCarrier())
		{
			cannotPerformReason = "#ACE-UserAction_Carrying";
			return false;
		}
		
		// Handle carriable objects
		ACE_CarriableEntityComponent carriableComponent = ACE_CarriableEntityComponent.Cast(entity.FindComponent(ACE_CarriableEntityComponent));
		if (carriableComponent)
		{
			if (carriableComponent.IsCarried())
			{
				cannotPerformReason = "#ACE-UserAction_Carrying";
				return false;
			}
			
			return true;
		}
		
		// Handle carriable characters
		SCR_ChimeraCharacter char = SCR_ChimeraCharacter.Cast(entity);
		if (char)
		{
			SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
			if (!charController)
				return false;
			
			if (charController.ACE_IsCarried())
			{
				cannotPerformReason = "#ACE-UserAction_Carrying";
				return false;
			}
			
			return true;
		}
				
		return false;
	}
}
