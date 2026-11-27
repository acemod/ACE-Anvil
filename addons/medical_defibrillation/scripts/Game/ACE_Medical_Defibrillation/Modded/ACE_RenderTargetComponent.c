// Recreate the render target locally after a proxy has finished spawning.
modded class ACE_RenderTargetComponent
{
	void ACE_Medical_Defibrillation_RefreshLocalRender()
	{
		if (!IsToggledOn() || !IsRendered() || !CanRenderLocal())
			return;

		ToggleRender(false);
		ToggleRender(true);
	}
}
