#ifdef ENABLE_DIAG
//------------------------------------------------------------------------------------------------
modded class ACE_Medical_VitalStatesSystem : GameSystem
{
	//------------------------------------------------------------------------------------------------
	//! Show vitals for target entity
	override void OnDiag(float timeSlice)
	{
		super.OnDiag(timeSlice);
		
		IEntity target;
		string targetType;
		if (!ACE_DiagTools.GetDiagTargetCharacter(target, targetType))
			return;
		
		ACE_Medical_VitalsComponent component = ACE_Medical_VitalsComponent.Cast(target.FindComponent(ACE_Medical_VitalsComponent));
		if (!component)
			return;

		DbgUI.Begin(string.Format("ACE Medical Breathing O2 (%1)", targetType), 0, 700);
		DbgUI.Text(string.Format("Respiratory Rate:             %1 1/min", component.GetRespiratoryRate()));
		float PalvO2 = component.GetPalvO2() * ACE_PhysicalConstants.KPA2MMHG;
		DbgUI.Text(string.Format("PalvO2:                       %1 mmHg", PalvO2));
		DbgUI.PlotLiveClamped("PalvO2", 500, 400, PalvO2, 0, 150, 100, 1000);
		float SpO2 = Math.Round(component.GetSpO2());
		DbgUI.Text(string.Format("SpO2:                         %1 \%", SpO2));
		DbgUI.PlotLiveClamped("SpO2", 500, 400, SpO2, 0, 100, 100, 1000);
		DbgUI.End();
		
		DbgUI.Begin(string.Format("ACE Medical Breathing CO2 (%1)", targetType), 0, 700);
		float PalvCO2 = component.GetPalvCO2() * ACE_PhysicalConstants.KPA2MMHG;
		DbgUI.Text(string.Format("PalvCO2:                      %1 mmHg", PalvO2));
		DbgUI.PlotLiveClamped("PalvCO2", 500, 400, PalvCO2, 0, 150, 100, 1000);
		float pH = 6.1 + Math.Log10(0.024 / component.GetCvenCO2());
		DbgUI.Text(string.Format("pH:                           %1", pH));
		DbgUI.PlotLiveClamped("pH", 500, 400, pH, 6, 9, 100, 1000);
		DbgUI.End();
	}
}
#endif
