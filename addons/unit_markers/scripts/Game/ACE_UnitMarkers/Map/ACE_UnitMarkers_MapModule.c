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

	protected float m_fUpdateTimer = 0;
	protected bool m_bActive = false;

	// Parallel arrays - entity reference and its corresponding map marker
	protected ref array<IEntity> m_aTrackedEntities = {};
	protected ref array<SCR_MapMarkerBase> m_aMarkers = {};

	//------------------------------------------------------------------------------------------------
	override protected void OnMapOpen(MapConfiguration config)
	{
		super.OnMapOpen(config);

		if (!IsLocalPlayerGameMaster())
			return;

		m_bActive = true;
		m_fUpdateTimer = UPDATE_INTERVAL_S; // trigger an update on the first Update() call
	}

	//------------------------------------------------------------------------------------------------
	override protected void OnMapClose(MapConfiguration config)
	{
		super.OnMapClose(config);
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
	//! Returns true when the local player has a non-limited editor (GM) instance.
	protected bool IsLocalPlayerGameMaster()
	{
		SCR_EditorManagerEntity editorManager = SCR_EditorManagerEntity.GetInstance();
		return editorManager != null && !editorManager.IsLimited();
	}

	//------------------------------------------------------------------------------------------------
	protected void UpdateMarkers()
	{
		SCR_MapMarkerManagerComponent markerManager = GetMarkerManager();
		if (!markerManager)
			return;

		array<IEntity> currentEntities = {};
		CollectTrackedEntities(currentEntities);

		// Remove markers for entities that are no longer tracked
		for (int i = m_aTrackedEntities.Count() - 1; i >= 0; i--)
		{
			if (!currentEntities.Contains(m_aTrackedEntities[i]))
				RemoveMarkerAt(markerManager, i);
		}

		// Create new markers or refresh positions of existing ones
		foreach (IEntity ent : currentEntities)
		{
			int idx = m_aTrackedEntities.Find(ent);
			if (idx < 0)
			{
				SCR_MapMarkerBase marker = CreateMarker(markerManager, ent);
				if (marker)
				{
					m_aTrackedEntities.Insert(ent);
					m_aMarkers.Insert(marker);
				}
			}
			else
			{
				RefreshMarker(m_aMarkers[idx], ent);
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Iterates all world entities and collects those that should have a GM marker.
	protected void CollectTrackedEntities(out array<IEntity> entities)
	{
		IEntity ent = GetGame().GetWorld().GetFirstEntity();
		while (ent)
		{
			if (ShouldTrack(ent))
				entities.Insert(ent);
			ent = GetGame().GetWorld().GetNextEntity(ent);
		}
	}

	//------------------------------------------------------------------------------------------------
	//! Returns true for living infantry not in a vehicle, and for vehicles that have occupants.
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

			// Vehicle marker covers occupants - skip characters sitting inside a vehicle
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
	//! Walks the entity parent chain to determine whether the character is inside a vehicle.
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
	//! Checks child entities for at least one ChimeraCharacter occupant.
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
	protected SCR_MapMarkerBase CreateMarker(SCR_MapMarkerManagerComponent markerManager, IEntity ent)
	{
		EMilitarySymbolIdentity identity = GetEntityIdentity(ent);
		EMilitarySymbolIcon icon;
		EMilitarySymbolDimension dimension;
		GetEntitySymbol(ent, icon, dimension);

		SCR_MapMarkerBase marker = markerManager.PrepareMilitaryMarker(identity, dimension, icon);

		// Faction identity may not be configured in the scenario - fall back to UNKNOWN
		if (!marker && identity != EMilitarySymbolIdentity.UNKNOWN)
			marker = markerManager.PrepareMilitaryMarker(EMilitarySymbolIdentity.UNKNOWN, dimension, icon);

		if (!marker)
			return null;

		vector pos = ent.GetOrigin();
		marker.SetWorldPos((int)pos[0], (int)pos[2]);
		marker.SetCustomText(BuildMarkerLabel(ent));

		// true = local-only, not broadcast over network
		markerManager.InsertStaticMarker(marker, true);
		return marker;
	}

	//------------------------------------------------------------------------------------------------
	protected void RefreshMarker(SCR_MapMarkerBase marker, IEntity ent)
	{
		if (!marker || !ent)
			return;

		vector pos = ent.GetOrigin();
		marker.SetWorldPos((int)pos[0], (int)pos[2]);
		marker.SetCustomText(BuildMarkerLabel(ent));
	}

	//------------------------------------------------------------------------------------------------
	protected EMilitarySymbolIdentity GetEntityIdentity(IEntity ent)
	{
		FactionAffiliationComponent factionComp = FactionAffiliationComponent.Cast(
			ent.FindComponent(FactionAffiliationComponent)
		);
		if (!factionComp)
			return EMilitarySymbolIdentity.UNKNOWN;

		Faction faction = factionComp.GetAffiliatedFaction();
		if (!faction)
			return EMilitarySymbolIdentity.UNKNOWN;

		return ResolveIdentityForFaction(faction.GetFactionKey());
	}

	//------------------------------------------------------------------------------------------------
	//! Maps vanilla Reforger faction keys to NATO military symbol identities.
	//! Extend this list to support custom faction keys from other mods or missions.
	protected EMilitarySymbolIdentity ResolveIdentityForFaction(string factionKey)
	{
		if (factionKey == "US" || factionKey == "US_Army" || factionKey == "NATO" || factionKey == "BLUFOR")
			return EMilitarySymbolIdentity.BLUFOR;

		if (factionKey == "USSR" || factionKey == "Soviet" || factionKey == "OPFOR" || factionKey == "RU")
			return EMilitarySymbolIdentity.OPFOR;

		if (factionKey == "FIA" || factionKey == "INDFOR" || factionKey == "Guerrilla" || factionKey == "Resistance")
			return EMilitarySymbolIdentity.INDFOR;

		return EMilitarySymbolIdentity.UNKNOWN;
	}

	//------------------------------------------------------------------------------------------------
	//! Assigns the NATO military symbol icon and operational dimension for an entity.
	//! Helicopters use the AIR dimension (circle frame).
	//! Armed ground vehicles receive the ARMOR icon; unarmed ones receive MOTORIZED.
	//! Infantry always receive the INFANTRY icon on the LAND dimension.
	protected void GetEntitySymbol(IEntity ent, out EMilitarySymbolIcon icon, out EMilitarySymbolDimension dimension)
	{
		if (ChimeraCharacter.Cast(ent))
		{
			icon = EMilitarySymbolIcon.INFANTRY;
			dimension = EMilitarySymbolDimension.LAND;
			return;
		}

		Vehicle vehicle = Vehicle.Cast(ent);
		if (vehicle)
		{
			if (Helicopter.Cast(vehicle))
			{
				icon = EMilitarySymbolIcon.ROTARY_WING;
				dimension = EMilitarySymbolDimension.AIR;
				return;
			}

			// Armed ground vehicles (tanks, IFVs, armed APCs) get the ARMOR symbol;
			// unarmed transports (trucks, jeeps) get the MOTORIZED symbol.
			if (vehicle.FindComponent(WeaponManagerComponent))
				icon = EMilitarySymbolIcon.ARMOR;
			else
				icon = EMilitarySymbolIcon.MOTORIZED;

			dimension = EMilitarySymbolDimension.LAND;
			return;
		}

		icon = EMilitarySymbolIcon.INFANTRY;
		dimension = EMilitarySymbolDimension.LAND;
	}

	//------------------------------------------------------------------------------------------------
	//! Builds the text label shown beneath the marker.
	//! Infantry: player name (if applicable) + cardinal direction in brackets, e.g. "Smith [NE]".
	//! Vehicles: player name of the driver if player-controlled, otherwise empty.
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
	//! Returns a cardinal direction string for the entity's current facing, e.g. "[NE]".
	//! Uses the forward vector from the entity's world transform matrix.
	//! In Enfusion: X=east, Z=north; yaw 0=north, increasing clockwise.
	protected string GetCardinalDirection(IEntity ent)
	{
		vector mat[4];
		ent.GetWorldTransform(mat);

		// mat[2] is the forward (look) direction in world space
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
	protected void RemoveMarkerAt(SCR_MapMarkerManagerComponent markerManager, int idx)
	{
		if (m_aMarkers[idx])
			markerManager.RemoveStaticMarker(m_aMarkers[idx]);

		m_aTrackedEntities.Remove(idx);
		m_aMarkers.Remove(idx);
	}

	//------------------------------------------------------------------------------------------------
	protected void ClearAllMarkers()
	{
		SCR_MapMarkerManagerComponent markerManager = GetMarkerManager();
		if (markerManager)
		{
			foreach (SCR_MapMarkerBase marker : m_aMarkers)
			{
				if (marker)
					markerManager.RemoveStaticMarker(marker);
			}
		}

		m_aTrackedEntities.Clear();
		m_aMarkers.Clear();
	}

	//------------------------------------------------------------------------------------------------
	protected SCR_MapMarkerManagerComponent GetMarkerManager()
	{
		BaseGameMode gameMode = GetGame().GetGameMode();
		if (!gameMode)
			return null;

		return SCR_MapMarkerManagerComponent.Cast(
			gameMode.FindComponent(SCR_MapMarkerManagerComponent)
		);
	}
}
