//------------------------------------------------------------------------------------------------
//! Map UI component that shows unit markers to gamemasters when the map is open.
//! Infantry: NATO infantry symbol with cardinal direction label.
//! Helicopters: NATO rotary-wing symbol (circle frame).
//! Armed ground vehicles: NATO armor symbol.
//! Unarmed ground vehicles: NATO motorized symbol.
//! All markers are colored by faction (BLUFOR=blue, OPFOR=red, INDFOR=green).
//! Markers are local-only and not networked.
class ACE_UnitMarkers_MapModule : SCR_MapUIBaseComponent
{
	protected static const float UPDATE_INTERVAL_S = 1.5;
	protected static const int MARKER_SIZE_PX = 16;
	protected static const string MILITARY_IMAGESET = "{3A89628ECBF03E7C}UI/Textures/Map/icons_map_military/icons_map_military.imageset";

	protected float m_fUpdateTimer = 0;
	protected bool m_bActive = false;

	// Parallel arrays: world entity -> overlay widget root
	protected ref array<IEntity> m_aTrackedEntities = {};
	protected ref array<Widget> m_aWidgets = {};

	//------------------------------------------------------------------------------------------------
	override protected void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);

		bool isGM = IsLocalPlayerGameMaster();
		Print(string.Format("[ACE_UnitMarkers] OnMapOpen - IsGM: %1", isGM), LogLevel.NORMAL);

		if (!isGM)
			return;

		m_bActive = true;
		m_fUpdateTimer = UPDATE_INTERVAL_S;
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
		Print("[ACE_UnitMarkers] OnMapClose", LogLevel.NORMAL);
		m_bActive = false;
		ClearAllMarkers();
	}

	//------------------------------------------------------------------------------------------------
	override void Update(float timeSlice)
	{
		super.Update(timeSlice);

		if (!m_bActive)
			return;

		m_fUpdateTimer += timeSlice;
		if (m_fUpdateTimer < UPDATE_INTERVAL_S)
			return;

		m_fUpdateTimer = 0;
		UpdateMarkers();
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsLocalPlayerGameMaster()
	{
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		return editorManager != null && !editorManager.IsLimited();
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateMarkers()
	{
		array<IEntity> currentEntities = {};
		CollectTrackedEntities(currentEntities);

		Print(string.Format("[ACE_UnitMarkers] UpdateMarkers - found %1 trackable entities", currentEntities.Count()), LogLevel.NORMAL);

		// Remove markers for entities no longer tracked
		for (int i = m_aTrackedEntities.Count() - 1; i >= 0; i--)
		{
			if (!currentEntities.Contains(m_aTrackedEntities[i]))
				RemoveMarkerAt(i);
		}

		// Create or refresh
		int created = 0;
		int failed = 0;
		foreach (IEntity ent : currentEntities)
		{
			int idx = m_aTrackedEntities.Find(ent);
			if (idx < 0)
			{
				Widget w = CreateMarkerWidget(ent);
				if (w)
				{
					m_aTrackedEntities.Insert(ent);
					m_aWidgets.Insert(w);
					created++;
				}
				else
				{
					failed++;
				}
			}
			else
			{
				RefreshMarkerWidget(m_aWidgets[idx], ent);
			}
		}

		if (created > 0 || failed > 0)
			Print(string.Format("[ACE_UnitMarkers] UpdateMarkers - created: %1, failed: %2, total tracked: %3", created, failed, m_aTrackedEntities.Count()), LogLevel.NORMAL);
	}

	//------------------------------------------------------------------------------------------------
	protected void CollectTrackedEntities(out array<IEntity> entities)
	{
		GetGame().GetWorld().QueryEntitiesByAABB(
			Vector(-1000000, 0, -1000000),
			Vector(1000000, 10000, 1000000),
			QueryEntitiesCallback_ShouldTrack,
			null,
			EQueryEntitiesFlags.ALL
		);

		entities = m_aQueryResults;
		m_aQueryResults = new array<IEntity>();
	}

	protected ref array<IEntity> m_aQueryResults = new array<IEntity>();

	protected bool QueryEntitiesCallback_ShouldTrack(IEntity ent)
	{
		if (ShouldTrack(ent))
			m_aQueryResults.Insert(ent);
		return true;
	}

	//------------------------------------------------------------------------------------------------
	protected bool ShouldTrack(IEntity ent)
	{
		ChimeraCharacter character = ChimeraCharacter.Cast(ent);
		if (character)
		{
			SCR_CharacterControllerComponent ctrl = SCR_CharacterControllerComponent.Cast(
				character.FindComponent(SCR_CharacterControllerComponent)
			);
			if (ctrl && ctrl.IsDead())
				return false;

			if (IsInVehicle(character))
				return false;

			return true;
		}

		Vehicle vehicle = Vehicle.Cast(ent);
		if (vehicle)
			return VehicleHasOccupants(vehicle);

		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected bool IsInVehicle(ChimeraCharacter character)
	{
		IEntity parent = character.GetParent();
		while (parent)
		{
			if (Vehicle.Cast(parent))
				return true;
			parent = parent.GetParent();
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	protected bool VehicleHasOccupants(Vehicle vehicle)
	{
		IEntity child = vehicle.GetChildren();
		while (child)
		{
			if (ChimeraCharacter.Cast(child))
				return true;
			child = child.GetSibling();
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------
	//! Creates an overlay widget parented to the map canvas for this entity.
	protected Widget CreateMarkerWidget(IEntity ent)
	{
		CanvasWidget canvas = m_MapEntity.GetMapWidget();
		if (!canvas)
		{
			Print("[ACE_UnitMarkers] CreateMarkerWidget - map canvas null", LogLevel.WARNING);
			return null;
		}

		WorkspaceWidget workspace = GetGame().GetWorkspace();

		// Root frame - positioned via FrameSlot, children use absolute positions within it
		Widget root = workspace.CreateWidget(WidgetType.FrameWidgetTypeID, WidgetFlags.VISIBLE, new Color(1, 1, 1, 1), 0, canvas);
		if (!root)
		{
			Print("[ACE_UnitMarkers] CreateMarkerWidget - failed to create root frame", LogLevel.WARNING);
			return null;
		}

		Color factionColor = GetFactionColor(ent);

		// Icon image - centered on the anchor point
		ImageWidget icon = ImageWidget.Cast(workspace.CreateWidget(WidgetType.ImageWidgetTypeID, WidgetFlags.VISIBLE, factionColor, 0, root));
		if (icon)
		{
			icon.LoadImageFromSet(0, MILITARY_IMAGESET, GetIconName(ent));
			FrameSlot.SetSize(icon, MARKER_SIZE_PX, MARKER_SIZE_PX);
			FrameSlot.SetPos(icon, -MARKER_SIZE_PX * 0.5, -MARKER_SIZE_PX * 0.5);
		}

		// Label text - below the icon
		TextWidget label = TextWidget.Cast(workspace.CreateWidget(WidgetType.TextWidgetTypeID, WidgetFlags.VISIBLE, factionColor, 0, root));
		if (label)
		{
			label.SetText(BuildMarkerLabel(ent));
			FrameSlot.SetPos(label, -MARKER_SIZE_PX * 0.5, MARKER_SIZE_PX * 0.5);
		}

		// Position on map
		RefreshMarkerWidget(root, ent);

		Print(string.Format("[ACE_UnitMarkers] CreateMarkerWidget - created for entity %1", ent.GetName()), LogLevel.NORMAL);
		return root;
	}

	//------------------------------------------------------------------------------------------------
	protected void RefreshMarkerWidget(Widget root, IEntity ent)
	{
		if (!root || !ent)
			return;

		vector pos = ent.GetOrigin();
		int screenX, screenY;
		m_MapEntity.WorldToScreen(pos[0], pos[2], screenX, screenY, true);

		WorkspaceWidget workspace = GetGame().GetWorkspace();
		FrameSlot.SetPos(root, workspace.DPIUnscale(screenX), workspace.DPIUnscale(screenY));

		// Update label text (second child of root)
		Widget firstChild = root.GetChildren();
		if (firstChild)
		{
			TextWidget label = TextWidget.Cast(firstChild.GetSibling());
			if (label)
				label.SetText(BuildMarkerLabel(ent));
		}
	}

	//------------------------------------------------------------------------------------------------
	protected string GetIconName(IEntity ent)
	{
		if (ChimeraCharacter.Cast(ent))
			return "infantry";

		Vehicle vehicle = Vehicle.Cast(ent);
		if (vehicle)
		{
			if (vehicle.FindComponent(HelicopterControllerComponent))
				return "helicopter";

			if (vehicle.FindComponent(TurretControllerComponent))
				return "armor";

			return "motorized";
		}

		return "infantry";
	}

	//------------------------------------------------------------------------------------------------
	protected Color GetFactionColor(IEntity ent)
	{
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(ent.FindComponent(FactionAffiliationComponent));
		if (factionComp)
		{
			Faction faction = factionComp.GetAffiliatedFaction();
			if (faction)
			{
				string key = faction.GetFactionKey();

				if (key == "US" || key == "US_Army" || key == "NATO" || key == "BLUFOR")
					return new Color(0.2, 0.4, 1, 1); // blue

				if (key == "USSR" || key == "Soviet" || key == "OPFOR" || key == "RU")
					return new Color(1, 0.2, 0.2, 1); // red

				if (key == "FIA" || key == "INDFOR" || key == "Guerrilla" || key == "Resistance")
					return new Color(0.2, 0.8, 0.2, 1); // green
			}
		}

		return new Color(1, 1, 1, 1); // white = unknown
	}

	//------------------------------------------------------------------------------------------------
	protected string BuildMarkerLabel(IEntity ent)
	{
		string playerName = string.Empty;
		int playerID = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(ent);
		if (playerID > 0)
			playerName = GetGame().GetPlayerManager().GetPlayerName(playerID);

		if (ChimeraCharacter.Cast(ent))
		{
			string direction = GetCardinalDirection(ent);
			if (playerName.IsEmpty())
				return direction;
			return playerName + " " + direction;
		}

		return playerName;
	}

	//------------------------------------------------------------------------------------------------
	protected string GetCardinalDirection(IEntity ent)
	{
		vector mat[4];
		ent.GetWorldTransform(mat);

		float yaw = Math.Atan2(mat[2][0], mat[2][2]) * Math.RAD2DEG;
		if (yaw < 0)
			yaw += 360;

		if (yaw < 22.5 || yaw >= 337.5) return "[N]";
		if (yaw < 67.5)                 return "[NE]";
		if (yaw < 112.5)                return "[E]";
		if (yaw < 157.5)                return "[SE]";
		if (yaw < 202.5)                return "[S]";
		if (yaw < 247.5)                return "[SW]";
		if (yaw < 292.5)                return "[W]";
		return "[NW]";
	}

	//------------------------------------------------------------------------------------------------
	protected void RemoveMarkerAt(int idx)
	{
		if (m_aWidgets[idx])
			m_aWidgets[idx].RemoveFromHierarchy();

		m_aTrackedEntities.Remove(idx);
		m_aWidgets.Remove(idx);
	}

	//------------------------------------------------------------------------------------------------
	protected void ClearAllMarkers()
	{
		foreach (Widget w : m_aWidgets)
		{
			if (w)
				w.RemoveFromHierarchy();
		}

		m_aTrackedEntities.Clear();
		m_aWidgets.Clear();
	}
}
