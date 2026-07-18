//------------------------------------------------------------------------------------------------
//! Applies random spasms while character is registered
class ACE_CBRN_SpasmSystem : GameSystem
{
	[Attribute(defvalue: "0.5", desc: "Min timeout between spasms in seconds.")]
	protected float m_fMinSpasmTimeout;
	
	[Attribute(defvalue: "1.0", desc: "Mid timeout between spasms in seconds.")]
	protected float m_fMidSpasmTimeout;
	
	[Attribute(defvalue: "3.0", desc: "Max timeout between spasms in seconds.")]
	protected float m_fMaxSpasmTimeout;
	
	[Attribute(defvalue: "0.2", desc: "Probability for spasm to be heavy.")]
	protected float m_fHeavySpasmChance;
	
	protected ref array<ref ACE_CBRN_SpasmSystemContext> m_aContexts = {};
	protected ref array<ChimeraCharacter> m_aCharToRemove = {};
	protected bool m_bIsUpdating = false;
	
	//------------------------------------------------------------------------------------------------
	static ACE_CBRN_SpasmSystem GetInstance(ChimeraWorld world)
	{
		return ACE_CBRN_SpasmSystem.Cast(world.FindSystem(ACE_CBRN_SpasmSystem));
	}
	
	//------------------------------------------------------------------------------------------------
	override static void InitInfo(WorldSystemInfo outInfo)
	{
		super.InitInfo(outInfo);
		outInfo.SetAbstract(false)
			.SetUnique(true)
			.SetLocation(WorldSystemLocation.Server)
			.AddPoint(WorldSystemPoint.Frame);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		Enable(!m_aContexts.IsEmpty());
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdatePoint(WorldUpdatePointArgs args)
	{
		m_bIsUpdating = true;
		
		foreach (ACE_CBRN_SpasmSystemContext context : m_aContexts)
		{
			context.m_fTimer += args.GetTimeSliceSeconds();
			if (context.m_fTimer < context.m_fTimeout)
				continue;
			
			context.m_fTimeout = ComputeSpasmTimeout();
			context.m_fTimer = 0;
			
			EHitReactionType reactionType = EHitReactionType.HIT_REACTION_LIGHT;
			if (Math.RandomFloat(0, 1) < m_fHeavySpasmChance)
				reactionType = EHitReactionType.HIT_REACTION_HEAVY;
			
			context.m_CharController.ACE_PerformHitReaction(reactionType, 180 * Math.RandomIntInclusive(0, 1));

		}
		
		m_bIsUpdating = false;
		
		foreach (ChimeraCharacter char : m_aCharToRemove)
		{
			Unregister(char);
		}
		
		m_aCharToRemove.Clear();
	}
	
	//------------------------------------------------------------------------------------------------
	protected float ComputeSpasmTimeout()
	{
		return SCR_Math.RandomGaussFloat(m_fMinSpasmTimeout, m_fMidSpasmTimeout, m_fMaxSpasmTimeout);
	}
	
	//------------------------------------------------------------------------------------------------
	void Register(notnull ChimeraCharacter char)
	{
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		
		foreach (ACE_CBRN_SpasmSystemContext context : m_aContexts)
		{
			if (charController == context.m_CharController)
				return;
		}
		
		ACE_CBRN_SpasmSystemContext context = new ACE_CBRN_SpasmSystemContext();
		context.m_CharController = charController;
		context.m_fTimeout = ComputeSpasmTimeout();
		m_aContexts.Insert(context);
		Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	void Unregister(notnull ChimeraCharacter char)
	{
		if (m_bIsUpdating)
		{
			m_aCharToRemove.Insert(char);
			return;
		}
		
		SCR_CharacterControllerComponent charController = SCR_CharacterControllerComponent.Cast(char.GetCharacterController());
		
		for (int i = m_aContexts.Count() - 1; i >= 0; i--)
		{
			if (charController == m_aContexts[i].m_CharController)
				m_aContexts.Remove(i);
		}
		
		if (m_aContexts.IsEmpty())
			Enable(false);
	}
}

//------------------------------------------------------------------------------------------------
class ACE_CBRN_SpasmSystemContext
{
	float m_fTimer;
	float m_fTimeout;
	SCR_CharacterControllerComponent m_CharController;
}
