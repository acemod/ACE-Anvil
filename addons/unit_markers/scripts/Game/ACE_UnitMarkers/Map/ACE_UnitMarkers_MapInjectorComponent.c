[ComponentEditorProps(category: "ACE/UnitMarkers", description: "Injects ACE_UnitMarkers_MapModule into editor (GM) map configs at runtime.")]
class ACE_UnitMarkers_MapInjectorComponentClass : SCR_BaseGameModeComponentClass {}

class ACE_UnitMarkers_MapInjectorComponent : SCR_BaseGameModeComponent
{
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);

		if (SCR_Global.IsEditMode())
			return;

		SCR_MapEntity.GetOnMapInit().Insert(OnMapInit);
	}

	//------------------------------------------------------------------------------------------------
	protected void OnMapInit(MapConfiguration config)
	{
		// Only inject into editor/GM maps, not gadget or respawn maps
		if (config.MapEntityMode != EMapEntityMode.EDITOR)
			return;

		// Check if our component is already present (e.g. added via map config file)
		foreach (SCR_MapUIBaseComponent comp : config.Components)
		{
			if (comp.IsInherited(ACE_UnitMarkers_MapModule))
				return;
		}

		config.Components.Insert(new ACE_UnitMarkers_MapModule());
		Print("[ACE_UnitMarkers] Injected ACE_UnitMarkers_MapModule into editor map config", LogLevel.NORMAL);
	}
}
