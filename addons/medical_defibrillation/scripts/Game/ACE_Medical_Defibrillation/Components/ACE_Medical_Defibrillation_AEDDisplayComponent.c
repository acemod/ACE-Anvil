class ACE_Medical_Defibrillation_AEDDisplayComponentClass : ScriptComponentClass
{
}

// The render target is local to each machine. Only the defib state/timers are replicated.
class ACE_Medical_Defibrillation_AEDDisplayComponent : ScriptComponent
{
	protected ACE_RenderTargetComponent m_RenderTarget;
	protected ACE_Medical_Defibrillation_DefibComponent m_Defib;
	protected TextWidget m_wPatientName;
	protected TextWidget m_wState;
	protected TextWidget m_wCPRCountdown;
	protected ProgressBarWidget m_wChargeProgress;
	protected bool m_bInitialRefreshScheduled;
	protected static const int UPDATE_INTERVAL_MS = 100;
	protected static const int INITIAL_REFRESH_DELAY_MS = 500;

	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (!GetGame().InPlayMode() || System.IsConsoleApp())
			return;

		m_RenderTarget = ACE_RenderTargetComponent.Cast(owner.FindComponent(ACE_RenderTargetComponent));
		m_Defib = ACE_Medical_Defibrillation_DefibComponent.Cast(owner.FindComponent(ACE_Medical_Defibrillation_DefibComponent));
		if (!m_RenderTarget || !m_Defib)
			return;

		m_RenderTarget.GetOnToggleRender().Insert(OnToggleRender);
		if (m_RenderTarget.IsRendered())
			OnToggleRender(m_RenderTarget.GetRTTexture(), true);
	}

	override void OnDelete(IEntity owner)
	{
		GetGame().GetCallqueue().Remove(UpdateDisplay);
		GetGame().GetCallqueue().Remove(RefreshInitialRender);
		if (m_RenderTarget)
			m_RenderTarget.GetOnToggleRender().Remove(OnToggleRender);

		super.OnDelete(owner);
	}

	protected void OnToggleRender(Widget rtTexture, bool active)
	{
		GetGame().GetCallqueue().Remove(UpdateDisplay);
		m_wPatientName = null;
		m_wState = null;
		m_wCPRCountdown = null;
		m_wChargeProgress = null;
		if (!active || !rtTexture)
			return;

		Widget layoutRoot = rtTexture.GetParent();
		if (!layoutRoot)
			return;

		m_wPatientName = TextWidget.Cast(layoutRoot.FindAnyWidget("Title"));
		m_wState = TextWidget.Cast(layoutRoot.FindAnyWidget("InstructionText"));
		m_wCPRCountdown = TextWidget.Cast(layoutRoot.FindAnyWidget("CPRCountdown"));
		m_wChargeProgress = ProgressBarWidget.Cast(layoutRoot.FindAnyWidget("ChargeProgress"));
		if (m_wChargeProgress)
		{
			m_wChargeProgress.SetMin(0);
			m_wChargeProgress.SetMax(1);
		}
		UpdateDisplay();
		GetGame().GetCallqueue().CallLater(UpdateDisplay, UPDATE_INTERVAL_MS, true);

		// On proxies the first render target may bind before the replicated entity is ready.
		// Recreating it once has the same effect as leaving and re-entering render range.
		if (!Replication.IsServer() && !m_bInitialRefreshScheduled)
		{
			m_bInitialRefreshScheduled = true;
			GetGame().GetCallqueue().CallLater(RefreshInitialRender, INITIAL_REFRESH_DELAY_MS);
		}
	}

	protected void RefreshInitialRender()
	{
		if (m_RenderTarget)
			m_RenderTarget.ACE_Medical_Defibrillation_RefreshLocalRender();
	}

	protected void UpdateDisplay()
	{
		if (!m_wState || !m_Defib)
			return;

		if (m_wPatientName)
			m_wPatientName.SetText(GetPatientName());

		ACE_Medical_Defibrillation_EDefibStateID state = m_Defib.GetDefibStateID();
		string stateText;
		float cprTimeMs;
		ACE_Medical_Defibrillation_DefibProgressData progress = m_Defib.GetDefibProgressData();
		if (progress)
			cprTimeMs = progress.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.CPRCooldown);

		float charge;
		if (state == ACE_Medical_Defibrillation_EDefibStateID.CHARGED)
			charge = 1;
		else if (state == ACE_Medical_Defibrillation_EDefibStateID.CHARGING && progress)
		{
			float duration = progress.GetDuration(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge);
			if (duration > 0)
				charge = Math.Clamp(progress.GetTimer(ACE_Medical_Defibrillation_EDefibProgressCategory.Charge) / duration, 0, 1);
		}

		bool cprActive = state == ACE_Medical_Defibrillation_EDefibStateID.CONNECTED && cprTimeMs > 0;
		switch (state)
		{
			case ACE_Medical_Defibrillation_EDefibStateID.DISCONNECTED: stateText = "CONNECT PADS"; break;
			case ACE_Medical_Defibrillation_EDefibStateID.CONNECTED:
			{
				stateText = "READY TO ANALYZE";
				if (cprActive)
					stateText = "PERFORM CPR";
				break;
			}
			case ACE_Medical_Defibrillation_EDefibStateID.ANALYSING: stateText = "ANALYZING"; break;
			case ACE_Medical_Defibrillation_EDefibStateID.ANALYSED: stateText = "RHYTHM ANALYZED"; break;
			case ACE_Medical_Defibrillation_EDefibStateID.CHARGING: stateText = "CHARGING"; break;
			case ACE_Medical_Defibrillation_EDefibStateID.CHARGED: stateText = "SHOCK READY"; break;
			default: stateText = "AED STANDBY"; break;
		}

		m_wState.SetText(stateText);
		if (m_wChargeProgress)
			m_wChargeProgress.SetCurrent(charge);
		if (m_wCPRCountdown)
		{
			m_wCPRCountdown.SetVisible(cprActive);
			if (cprActive)
			{
				int secondsLeft = Math.Ceil(cprTimeMs / 1000);
				int minutes = secondsLeft / 60;
				int seconds = Math.Mod(secondsLeft, 60);
				string secondsText = seconds.ToString();
				if (seconds < 10)
					secondsText = "0" + secondsText;
				m_wCPRCountdown.SetText(string.Format("%1:%2", minutes, secondsText));
			}
		}
	}

	protected string GetPatientName()
	{
		IEntity patient = m_Defib.GetPatient();
		if (!patient)
			return "NO PATIENT";

		PlayerManager playerManager = GetGame().GetPlayerManager();
		if (playerManager)
		{
			int playerId = playerManager.GetPlayerIdFromControlledEntity(patient);
			if (playerId > 0)
			{
				string playerName = playerManager.GetPlayerName(playerId);
				if (!playerName.IsEmpty())
					return playerName;
			}
		}

		CharacterIdentityComponent identityComponent = CharacterIdentityComponent.Cast(patient.FindComponent(CharacterIdentityComponent));
		if (identityComponent && identityComponent.GetIdentity())
		{
			string characterName = identityComponent.GetIdentity().GetFullName();
			if (!characterName.IsEmpty())
				return characterName;
		}

		return "UNKNOWN PATIENT";
	}
}
