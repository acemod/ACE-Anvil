class Wirecutter_UserAction : ScriptedUserAction
{
	[Attribute("", UIWidgets.Auto)]
	protected ref SCR_AudioSourceConfiguration m_AudioSourceConfiguration;
	
	[Attribute("0", UIWidgets.ComboBox, "Instrument type", "", ParamEnumArray.FromEnum(SCR_EInstrumentType) )]
	protected SCR_EInstrumentType m_eInstrumentType;
	
		[Attribute("", UIWidgets.Coords)]
	private vector m_vSoundOffset;
	
	
	protected SCR_GadgetManagerComponent m_GadgetManager;
	protected IEntity m_pUser;
	protected AudioHandle m_AudioHandle = AudioHandle.Invalid;
	protected static ref ScriptInvokerInt2 s_onFenceCut;
	
	override void OnActionStart(IEntity pUserEntity)
	{
		if (s_onFenceCut)
			s_onFenceCut.Invoke(GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(pUserEntity), m_eInstrumentType);
		
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (!soundManagerEntity)
			return;
				
		if (!m_AudioSourceConfiguration || !m_AudioSourceConfiguration.IsValid())
			return;
		
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(GetOwner(), m_AudioSourceConfiguration);
		if (!audioSource)
			return;
		
		vector mat[4];
		IEntity owner = GetOwner();
		owner.GetTransform(mat);		
		mat[3] = owner.CoordToParent(m_vSoundOffset);
					
		AudioSystem.TerminateSound(m_AudioHandle);
		soundManagerEntity.PlayAudioSource(audioSource);			
		m_AudioHandle = audioSource.m_AudioHandle;
	}
	
	override void OnActionCanceled(IEntity pOwnerEntity, IEntity pUserEntity)
	{		
		AudioSystem.TerminateSound(m_AudioHandle);
		
		SCR_SoundManagerEntity soundManagerEntity = GetGame().GetSoundManagerEntity();
		if (!soundManagerEntity)
			return;
				
		if (!m_AudioSourceConfiguration || m_AudioSourceConfiguration.m_sSoundProject == string.Empty)
			return;
		
		SCR_AudioSourceConfiguration audioSourceConfiguration = new SCR_AudioSourceConfiguration;
		audioSourceConfiguration.m_sSoundProject = m_AudioSourceConfiguration.m_sSoundProject;
		audioSourceConfiguration.m_eFlags = m_AudioSourceConfiguration.m_eFlags;
		audioSourceConfiguration.m_sSoundEventName = SCR_SoundEvent.SOUND_STOP_PLAYING;
				
		SCR_AudioSource audioSource = soundManagerEntity.CreateAudioSource(pOwnerEntity, audioSourceConfiguration);
		if (!audioSource)
			return;
		
		vector mat[4];
		pOwnerEntity.GetTransform(mat);
		mat[3] = pOwnerEntity.CoordToParent(m_vSoundOffset);
					
		soundManagerEntity.PlayAudioSource(audioSource, mat);			
		m_AudioHandle = audioSource.m_AudioHandle;
	}
	
	IEntity GetWirecutter(notnull IEntity ent)
	{
		SCR_GadgetManagerComponent gadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(ent);
		if (!gadgetManager)
			return null;
		
		return gadgetManager.GetHeldGadget();
	}
	

	void SetNewGadgetManager(IEntity from, IEntity to)
	{
		m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(to);
	}
	
	
	override bool CanBeShownScript(IEntity user)
	{
		if (!m_GadgetManager)
		{
			m_GadgetManager = SCR_GadgetManagerComponent.GetGadgetManager(user);
			
			SCR_PlayerController playerController = SCR_PlayerController.Cast(GetGame().GetPlayerController());
			if (playerController)
				playerController.m_OnControlledEntityChanged.Insert(SetNewGadgetManager);
			
			return true;
		};
					
		if (!SCR_WirecutterComponent.Cast(m_GadgetManager.GetHeldGadgetComponent()))
			return false;
		
		return true;
	}
	
		override void PerformAction(IEntity pOwnerEntity, IEntity pUserEntity)
	{
		if (!Replication.IsServer())
              return;
		
		SCR_EntityHelper.DeleteEntityAndChildren(pOwnerEntity);
	}

void ~Wirecutter_UserAction()
	{
		AudioSystem.TerminateSound(m_AudioHandle);
	}
};