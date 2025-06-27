//------------------------------------------------------------------------------------------------
[BaseContainerProps(configRoot: true)]
class ACE_MedicalDefibrillation_DefibrillationSystemSettings
{
	[Attribute(uiwidget: UIWidgets.Slider, params: "0 1 0.01", defvalue: "0.25", desc: "Percent to add to wake up chance calculation for each shock during cardiac arrest. Based on 'CPR Success Chance Max' above.", precision: 2)]
	float m_fWakeUpChancePerShock;
	
	[Attribute(params: "0 inf 1", defvalue: "120", desc: "Time (s) between allowed shocks on the system. By default, it emulates 2 min of CPR before a rhythm check and shock.")]
	int m_iShockCooldown;
	
	void ACE_MedicalDefibrillation_DefibrillationSystemSettings()
	{
		m_fWakeUpChancePerShock = Math.Clamp(m_fWakeUpChancePerShock, 0.0, 1.0);
	}
}