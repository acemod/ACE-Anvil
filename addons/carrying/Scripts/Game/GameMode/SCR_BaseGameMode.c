#ifdef GMSTATS 
#define GM_AI_STATS
#endif
#ifdef AISTATS
#define GM_AI_STATS
#endif

/**
@defgroup GameMode Game Mode
Game modes.
*/
class SCR_BaseGameModeClass: BaseGameModeClass
{
};

/// @ingroup GameMode
//------------------------------------------------------------------------------------------------
/*
											SCR_BaseGameMode

Brief:
	SCR_BaseGameMode is the skeleton of all game modes which connects some of the critical aspects of spawning.

Important:
	A. Components and expansion:
		1. SCR_BaseGameMode automatically gathers all attached components of SCR_BaseGameModeComponent type.
		These can be used to receive events from the game mode and expand game mode functionality in modular blocks.

		2. SCR_RespawnSystemComponent is a component that allows to store and select player faction, loadout and spawn points
		This system is closely tied to game modes and also requires FactionManager or LoadoutManager in the world.

		3. SCR_RespawnHandlerComponent is a component that provides a game mode with a way of respawning.
		These can be either very generic (automatic, menu selection) and interchangable or completely game mode specific.

		4. Additional components like
			SCR_RespawnTimerComponent: That allows to add a timer between each respawn
			SCR_ScoringSystemComponent: That allows handling of score when a player kills or is killed


	If you want to make a system which only requires to hook on game mode events, SCR_BaseGameMode is most likely the way to go.
	Inherit the component and implement methods which you require. If you need to have a very game specific implementation that
	cannot work standalone you will have to inherit SCR_BaseGameMode and plug the communication yourself.


	B. Instances and logic:
		1. SCR_BaseGameMode is a unique entity in the world, there can currently be only one game mode.
		2. SCR_RespawnSystemComponent is a unique component attached to the game mode, there can currently be only one respawn system.
		3. SCR_RespawnHandlerComponent is a unique component attached to the game mode, there can currently be only one respawn handler.
		4. SCR_RespawnComponent is a component attached to individual PlayerController instances.
			Due to the nature of PlayerController, only the server can access all RespawnComponent(s).
			The local client will always be only to access their local RespawnComponent.

			For the local component you can use GetGame().GetPlayerController().GetRespawnComponent();
			For other clients as the authority you can use GetGame().GetPlayerManager().GetPlayerRespawnComponent( int playerId );

			This component serves as communication between the authority and client for the SCR_RespawnSystemComponent.
			If you need to request a faction, loadout or spawn point selection as the client, navigate through the component.

			If you need to SET a faction, loadout or spawn point as the AUTHORITY, use SCR_RespawnSystemComponent's
			DoSetPlayerFaction, DoSetPlayerLoadout and DoSetPlayerSpawnPoint method(s).

			If you need to convert object instances to their id/indices, you can use SCR_RespawnSystemComponent's
			GetFactionIndex, GetLoadoutIndex, GetSpawnPointIndex method(s) and/or
			GetFactionByIndex, GetLoadoutByIndex, GetSpawnPointByIdentity.


	C. Game State
		There are three game states in total that you can use, two of which are optional.
			1. Pre-game:
				This state is automatically skipped unless a
					SCR_BaseGameModeStateComponent that has SCR_EGameModeState.PREGAME as its affiliated state
				is attached to the game mode.

				The SCR_PreGameGameModeStateComponent can be used to allow a time-based pre-game duration.

			2. Game
				This is the core game loop.

				By default this state is infinite, unless a
					SCR_BaseGameModeStateComponent that has SCR_EGameModeState.GAME as its affiliated state
				is attached to the game mode with a duration set.

				The SCR_GameGameModeStateComponent can be used to allow a time-based pre-game duration.

			3. Post-game
				This is your game-over state, you can automatically transition into new world, restart the session,
				have player voting, display scoreboard or end-game screens, as desired.

				In addition this transition carries along SCR_GameModeEndData, providing the authority and all
				clients with additional data that can notify the gamemode about e.g. win condition, or end reason
				as implemented per game mode.

				This state can also have its logic expanded similarly to the Pre-game by using
					SCR_BaseGameModeStateComponent that has SCR_EGameModeState.POSTGAME as its affiliated state
				attached to the game mode.

				The SCR_PostGameGameModeStateComponent can be used for this.


		You can always retrieve current game state by calling GetState() (see SCR_GameModeState)
		or IsRunning() if you're interested in the core game loop only.

		On each state change SCR_BaseGameMode.OnGameStateChanged() is called on both the server and all clients.
		In additionaly as implemented in this class it will automatically call OnGameModeStart() and OnGameModeEnd()
		based on the state the game is transitioning into.
*/

//------------------------------------------------------------------------------------------------
//~ ScriptInvokers
void SCR_BaseGameMode_OnPlayerDisconnected(int playerId, KickCauseCode cause = KickCauseCode.NONE, int timeout = -1);
typedef func SCR_BaseGameMode_OnPlayerDisconnected;

void SCR_BaseGameMode_OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer);
typedef func SCR_BaseGameMode_OnPlayerKilled;

//~ Generic Event that sends over player ID
void SCR_BaseGameMode_PlayerId(int playerId);
typedef func SCR_BaseGameMode_PlayerId;

//~ Generic event that sends over player ID and Entity
void SCR_BaseGameMode_PlayerIdAndEntity(int playerId, IEntity player);
typedef func SCR_BaseGameMode_PlayerIdAndEntity;

//~ On Player role changed
void SCR_BaseGameMode_OnPlayerRoleChanged(int playerId, EPlayerRole roleFlags);
typedef func SCR_BaseGameMode_OnPlayerRoleChanged;

void SCR_BaseGameMode_OnResourceEnabledChanged(array<EResourceType> disabledResourceTypes);
typedef func SCR_BaseGameMode_OnResourceEnabledChanged;

void OnPreloadFinished();
typedef func OnPreloadFinished;
typedef ScriptInvokerBase<OnPreloadFinished> OnPreloadFinishedInvoker;


//------------------------------------------------------------------------------------------------
class SCR_BaseGameMode : BaseGameMode
{
	#ifdef ENABLE_DIAG
	#define GAME_MODE_DEBUG
	#endif

	#ifdef GAME_MODE_DEBUG
		static bool s_DebugRegistered = false;
	#endif

	const static string WB_GAME_MODE_CATEGORY = "Game Mode";

	protected ref ScriptInvoker Event_OnGameStart = new ScriptInvoker();
	protected ref ScriptInvoker m_OnGameEnd = new ScriptInvoker();
	
	//~ Player events
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerAuditSuccess = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerAuditFail = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerAuditTimeouted = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerAuditRevived = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerConnected = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerId> m_OnPlayerRegistered = new ScriptInvokerBase<SCR_BaseGameMode_PlayerId>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected> m_OnPlayerDisconnected = new ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected> m_OnPostCompPlayerDisconnected = new ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected>(); //~ Called after the GameMode Components are notified that a player was disconnected
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity> m_OnPlayerSpawned = new ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_OnPlayerKilled> m_OnPlayerKilled = new ScriptInvokerBase<SCR_BaseGameMode_OnPlayerKilled>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity> m_OnPlayerDeleted = new ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity>();
	protected ref ScriptInvokerBase<SCR_BaseGameMode_OnPlayerRoleChanged> m_OnPlayerRoleChange = new ScriptInvokerBase<SCR_BaseGameMode_OnPlayerRoleChanged>();
	
	protected ref ScriptInvoker m_OnWorldPostProcess = new ScriptInvoker();
	protected ref ScriptInvoker m_OnControllableSpawned = new ScriptInvoker();
	protected ref ScriptInvoker m_OnControllableDestroyed = new ScriptInvoker();
	protected ref ScriptInvoker m_OnControllableDeleted = new ScriptInvoker();
	protected ref ScriptInvoker m_OnGameModeEnd = new ScriptInvoker();
	
	protected ref ScriptInvokerBase<SCR_BaseGameMode_OnResourceEnabledChanged> m_OnResourceTypeEnabledChanged;

	//-----------------------------------------
	//
	// World Editor attributes meant for Debug or Testing
	//
	[Attribute("0", uiwidget: UIWidgets.Flags, "Test Game Flags for when you run mission via WE.", "", ParamEnumArray.FromEnum(EGameFlags), WB_GAME_MODE_CATEGORY)]
	protected EGameFlags m_eTestGameFlags;

	[Attribute("1", uiwidget: UIWidgets.CheckBox, "When false, then Game mode need to handle its very own spawning. If true, then simple default logic is used to spawn and respawn automatically.", category: WB_GAME_MODE_CATEGORY)]
	protected bool m_bAutoPlayerRespawn;
	
	[Attribute("1", uiwidget: UIWidgets.CheckBox, "When true, allows players to freely swap their faction after initial assignment.", category: WB_GAME_MODE_CATEGORY)]
	protected bool m_bAllowFactionChange;
	
	[Attribute("30", UIWidgets.Slider, params: "0 600 1", desc: "Time in seconds after which the mission is reloaded upon completion or 0 to disable it.", category: WB_GAME_MODE_CATEGORY)]
	private float m_fAutoReloadTime;
	
	//-----------------------------------------
	//
	// Game End Screen States info
	//

	//! Current state of this gamemode.
	[RplProp(onRplName: "OnGameStateChanged")]
	private SCR_EGameModeState m_eGameState = SCR_EGameModeState.PREGAME;

	//! End data provided by the server when game ends.
	[RplProp()]
	private ref SCR_GameModeEndData m_pGameEndData = new SCR_GameModeEndData();

	[Attribute("1", uiwidget: UIWidgets.CheckBox, "If checked the elapsed time will only advance if at least one player is present on the server.", category: WB_GAME_MODE_CATEGORY)]
	protected bool m_bAdvanceTimeRequiresPlayers;

	/*!
		Elapsed time from the beginning of the game mode in seconds.
		NOTE: This does not correspond to the length of session but rather to the length of current game mode loop.
	*/
	[RplProp(condition: RplCondition.NoOwner)]
	protected float m_fTimeElapsed;

	//! If false, controls are disable for the time being.
	[RplProp()]
	protected bool m_bAllowControls = true;

	//! Interval of time synchronization in seconds
	protected float m_fTimeCorrectionInterval = 10.0;

	//! Last timestamp of sent time correction for the server.
	protected float m_fLastTimeCorrection;
	
	//! Is the session hosted by a player?
	[RplProp()]
	protected bool m_bIsHosted;

	//-----------------------------------------
	//
	// Required components
	//
	protected RplComponent m_RplComponent;
	protected SCR_GameModeHealthSettings m_pGameModeHealthSettings;
	protected SCR_RespawnSystemComponent m_pRespawnSystemComponent;
	protected SCR_BaseScoringSystemComponent m_ScoringSystemComponent;
	protected SCR_RespawnTimerComponent m_RespawnTimerComponent;

	/*!
		Additional game mode components attached to this gamemode where we dispatch all our game mode related events to.
	*/
	protected ref array<SCR_BaseGameModeComponent> m_aAdditionalGamemodeComponents = new array<SCR_BaseGameModeComponent>();

	//! Used on server to respawn player on their original position after reconnecting.
	protected ref map<int, vector> m_mPlayerSpawnPosition = new map<int, vector>();

	//! Map of components per state.
	protected ref map<SCR_EGameModeState, SCR_BaseGameModeStateComponent> m_mStateComponents = new map<SCR_EGameModeState, SCR_BaseGameModeStateComponent>();

	//! Spawn location preload
	[Attribute("1", category: WB_GAME_MODE_CATEGORY)]
	protected bool m_bUseSpawnPreload;

	protected ref SCR_SpawnPreload m_SpawnPreload;
	protected ref OnPreloadFinishedInvoker m_OnPreloadFinished;
	
	//~ Any Resource types that is set here is a disabled resource type
	[Attribute(desc: "List of disabled Resource Types in the GameMode.", uiwidget: UIWidgets.SearchComboBox, enums: ParamEnumArray.FromEnum(EResourceType), category: "Game Mode"), RplProp(onRplName: "OnResourceTypeEnabledChanged")]
	protected ref array<EResourceType> m_aDisabledResourceTypes;
	
	//~ Time stamp when end game was called
	protected WorldTimestamp m_GameEndTimeStamp = null;

	//------------------------------------------------------------------------------------------------
	//! \return If resource are enabled or not
	bool IsResourceTypeEnabled(EResourceType resourceType = EResourceType.SUPPLIES)
	{
		return !m_aDisabledResourceTypes.Contains(resourceType);
	}
	
	//------------------------------------------------------------------------------------------------
	//! \param[out] disabledResourceTypes Disabled resource types
	//! \return the count of all disabled resource types
	int GetDisabledResourceTypes(inout notnull array<EResourceType> disabledResourceTypes)
	{
		disabledResourceTypes.Copy(m_aDisabledResourceTypes);
		return disabledResourceTypes.Count();;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Change if supplies are enabled or not
	//! \param[in] enable Set true to enable supplies, set false to disable
	//! \param[in] resourceType Type of resource to enable
	//! \param[in] Player ID of player that enabled supplies for the game mode (For notifications)
	void SetResourceTypeEnabled(bool enable, EResourceType resourceType = EResourceType.SUPPLIES, int playerID = -1)
	{
		int index = m_aDisabledResourceTypes.Find(resourceType);
		
		//~ Already Enabled/Disabled
		if ((index < 0) == enable)
			return;
		
		if (!enable)
			m_aDisabledResourceTypes.Insert(resourceType);
		else 
			m_aDisabledResourceTypes.Remove(index);
		
		Replication.BumpMe();
		
		//~ Call on resourceType changed for server as well
		if (IsMaster())
			OnResourceTypeEnabledChanged();
		
		if (resourceType == EResourceType.SUPPLIES)
		{
			if (enable)
				SCR_NotificationsComponent.SendToEveryone(ENotification.EDITOR_ATTRIBUTES_ENABLE_GLOBAL_SUPPLY_USAGE, playerID);
			else 
				SCR_NotificationsComponent.SendToEveryone(ENotification.EDITOR_ATTRIBUTES_DISABLE_GLOBAL_SUPPLY_USAGE, playerID);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	//! \Called when Global Supplies is set to enabled or disabled (Server and client)
	protected void OnResourceTypeEnabledChanged()
	{
		if (m_OnResourceTypeEnabledChanged)
			m_OnResourceTypeEnabledChanged.Invoke(m_aDisabledResourceTypes);
	}
	
	//------------------------------------------------------------------------------------------------
	//! \return ScriptInvoker OnSupplies Enabled
	ScriptInvokerBase<SCR_BaseGameMode_OnResourceEnabledChanged> GetOnResourceTypeEnabledChanged()
	{
		if (!m_OnResourceTypeEnabledChanged)
			m_OnResourceTypeEnabledChanged = new ScriptInvokerBase<SCR_BaseGameMode_OnResourceEnabledChanged>();
		
		return m_OnResourceTypeEnabledChanged;
	}
	
	//------------------------------------------------------------------------------------------------
	bool CanStartSpawnPreload()
	{
		ChimeraWorld world = GetGame().GetWorld();
		return (!world.IsGameTimePaused() && m_bUseSpawnPreload);
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns whether current game mode is running its game loop.
		\return True in case gamemode is running, false if it is over.
	*/
	bool IsRunning()
	{
		return m_eGameState == SCR_EGameModeState.GAME;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns current game state. See SCR_GameModeState for more details.
		\return Current game state.
	*/
	SCR_EGameModeState GetState()
	{
		return m_eGameState;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns whether the owner of this instance is authority or not.
	*/
	sealed bool IsMaster()
	{
		return (!m_RplComponent || m_RplComponent.IsMaster());
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns elapsed time from the start of this game in seconds.
		\return Elapsed time in seconds.
	*/
	float GetElapsedTime()
	{
		return m_fTimeElapsed;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns the game duration from the start of game in seconds or 0 if none.
		\return Maximum time in seconds or 0 if infinite.
	*/
	float GetTimeLimit()
	{
		SCR_BaseGameModeStateComponent stateComponent = GetStateComponent(SCR_EGameModeState.GAME);
		if (stateComponent)
			return stateComponent.GetDuration();

		return 0.0;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns the remaining time of this game in seconds or -1 if none or undefined.
		\return Remaining time of this game.
	*/
	float GetRemainingTime()
	{
		float timeLimit = GetTimeLimit();
		if (!IsRunning() || timeLimit <= 0)
			return -1;

		float time = timeLimit - GetElapsedTime();
		if (time < 0)
			time = 0;

		return time;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		\return True if the game is hosted by a player (i.e., not dedicated server)
	*/
	bool IsHosted()
	{
		return m_bIsHosted;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called when game state changed, i.e. when game ends. Can be used to display post-scenario scoreboard
		and prepare for next match if any. See IsRunning() for new state.
	*/
	protected void OnGameStateChanged()
	{
		SCR_EGameModeState newState = GetState();
		Print("SCR_BaseGameMode::OnGameStateChanged = " + SCR_Enum.GetEnumName(SCR_EGameModeState, newState));

		//We handle the OnGameModeEnd and OnGameModeStart events here
		//Because this is the only replicated call.
		//And we propagate it to the components here for robusty in case of overriding those methods
		switch (newState)
		{
			case SCR_EGameModeState.POSTGAME:
				OnGameModeEnd(m_pGameEndData);
				foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
				{
					component.OnGameModeEnd(m_pGameEndData);
				}
				break;
			
			case SCR_EGameModeState.GAME:
				OnGameModeStart();
				foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
				{
					component.OnGameModeStart();
				}
				break;
		}

		// Dispatch events
		foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
		{
			component.OnGameStateChanged(newState);
		}
	}

	//------------------------------------------------------------------------------------------------
	void CachePlayerSpawnPosition(int playerID, vector position)
	{
		m_mPlayerSpawnPosition.Set(playerID, position);
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns respawn system component attached to this gamemode.
		\return Returns attached respawn system instance or null if none.
	*/
	SCR_RespawnSystemComponent GetRespawnSystemComponent()
	{
		return m_pRespawnSystemComponent;
	}
	
	//------------------------------------------------------------------------------------------------
	SCR_GameModeHealthSettings GetGameModeHealthSettings()
	{
		return m_pGameModeHealthSettings;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Returns scoring system component attached to this game mode or null if none.
		\return Returns attached scoring system instance or null if none.
	*/
	SCR_BaseScoringSystemComponent GetScoringSystemComponent()
	{
		return m_ScoringSystemComponent;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Server-only call that starts current session.
		Replicates state to all clients, see GetState() and IsRunning().
	*/
	void StartGameMode()
	{
		if (!IsMaster())
			return;

		if (IsRunning())
		{
			Print("Trying to start a gamemode that is already running.", LogLevel.WARNING);
			return;
		}

		m_fTimeElapsed = 0.0;
		m_eGameState = SCR_EGameModeState.GAME;
		Replication.BumpMe();

		// Raise event for authority
		OnGameStateChanged();
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Server-only logic that implements whether we can transition from pre-game (if enabled)
		to game loop. Requires an attached SCR_BaseGameModeStateComponent affiliated to the
		SCR_EGameModeState.PREGAME state.

		Does not apply to manual StartGameMode() call from the authority!

		\return Returns true in case pre-game can end and game can start or false otherwise.
	*/
	protected bool CanStartGameMode()
	{
		SCR_BaseGameModeStateComponent pregame = GetStateComponent(SCR_EGameModeState.PREGAME);
		if (!pregame)
			return true;

		return pregame.CanAdvanceState(SCR_EGameModeState.GAME);
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Server-only call that marks current session as complete.
		Replicates state to all clients, see GetState() and IsRunning().

		\param SCR_GameModeEndData the data this game mode ends with. When null, fallback to default is used.
		All machines can reach this data in the OnGameModeEnd(SCR_GameModeEndData) method.
	*/
	void EndGameMode(SCR_GameModeEndData endData)
	{
		if (!IsMaster())
			return;

		// Already over
		if (!IsRunning())
		{
			Print("Trying to end a gamemode that is not running.", LogLevel.WARNING);
			return;
		}

		if (endData == null)
			endData = SCR_GameModeEndData.CreateSimple();

		m_pGameEndData = endData;
		m_eGameState = SCR_EGameModeState.POSTGAME;
		Replication.BumpMe();

		// Raise event for authority
		OnGameStateChanged();
	}

	/*!
		Get end game data will return null if game has not ended
		\return SCR_GameModeEndData m_pGameEndData the data this game mode ends with
	*/
	SCR_GameModeEndData GetEndGameData()
	{
		if (m_eGameState != SCR_EGameModeState.POSTGAME)
			return null;
		else
			return m_pGameEndData;
	}

	ScriptInvoker GetOnGameStart()
	{
		return Event_OnGameStart;
	}
	ScriptInvoker GetOnGameEnd()
	{
		return m_OnGameEnd;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerAuditSuccess()
	{
		return m_OnPlayerAuditSuccess;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerAuditFail()
	{
		return m_OnPlayerAuditFail;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerAuditTimeouted()
	{
		return m_OnPlayerAuditTimeouted;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerAuditRevived()
	{
		return m_OnPlayerAuditRevived;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerConnected()
	{
		return m_OnPlayerConnected;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerId> GetOnPlayerRegistered()
	{
		return m_OnPlayerRegistered;
	}
	ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected> GetOnPlayerDisconnected()
	{
		return m_OnPlayerDisconnected;
	}
	/*!
	Called on player disconnect and after Gamemode components are notified
	\return Script invoker
	*/
	ScriptInvokerBase<SCR_BaseGameMode_OnPlayerDisconnected> GetOnPostCompPlayerDisconnected()
	{
		return m_OnPostCompPlayerDisconnected;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity> GetOnPlayerSpawned()
	{
		return m_OnPlayerSpawned;
	}
	ScriptInvokerBase<SCR_BaseGameMode_OnPlayerKilled> GetOnPlayerKilled()
	{
		return m_OnPlayerKilled;
	}
	ScriptInvokerBase<SCR_BaseGameMode_PlayerIdAndEntity> GetOnPlayerDeleted()
	{
		return m_OnPlayerDeleted;
	}
	ScriptInvokerBase<SCR_BaseGameMode_OnPlayerRoleChanged> GetOnPlayerRoleChange()
	{
		return m_OnPlayerRoleChange;
	}
	ScriptInvoker GetOnWorldPostProcess()
	{
		return m_OnWorldPostProcess;
	}
	ScriptInvoker GetOnControllableSpawned()
	{
		return m_OnControllableSpawned;
	}
	ScriptInvoker GetOnControllableDestroyed()
	{
		return m_OnControllableDestroyed;
	}
	ScriptInvoker GetOnControllableDeleted()
	{
		return m_OnControllableDeleted;
	}
	/*!
	Get on game end script invoker called on every machine
	\return ScriptInvoker Event_OnGameModeEnd
	*/
	ScriptInvoker GetOnGameModeEnd()
	{
		return m_OnGameModeEnd;
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called on every machine when game mode starts by OnGameStateChanged.
		This can be immediate (if no pre-game period is set) or can happen after
		a certain delay, as deemed appropriate by the authority.
	*/
	protected void OnGameModeStart()
	{
		#ifdef GM_AI_STATS
		if (IsGameModeStatisticsEnabled())
		{
			if (!SCR_GameModeStatistics.IsRecording())
				SCR_GameModeStatistics.StartRecording();
		}
		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called on every machine when game mode ends by OnGameStateChanged.
		This can be based on time limit or as deemed appropriate by the authority,
		e.g. after reaching certain score threshold and similar.
		\param SCR_GameModeEndData optional game mode end data received from the server.
	*/
	protected void OnGameModeEnd(SCR_GameModeEndData endData)
	{
		ChimeraWorld world = GetGame().GetWorld();
		if (world)
			m_GameEndTimeStamp = world.GetLocalTimestamp();
		
		m_OnGameModeEnd.Invoke(endData);

		#ifdef GM_AI_STATS
		if (IsGameModeStatisticsEnabled())
		{
			if (SCR_GameModeStatistics.IsRecording())
				SCR_GameModeStatistics.StopRecording();
		}
		#endif
		
		// Automatically restart the session on game mode end if enabled
		float reloadTime = GetAutoReloadDelay();
		if (reloadTime > 0)
			GetGame().GetCallqueue().CallLater(RestartSession, reloadTime * 1000.0, false);	
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Returns local timestamp when endgame was called
	*/
	WorldTimestamp GetGameEndTimeStamp()
	{
		return m_GameEndTimeStamp;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Returns delay of mission reload (if enabled) in seconds or else 0 if disabled.
	*/
	float GetAutoReloadDelay()
	{
		// Allow the server owner to override it via -autoReload=TIME
		string autoReloadTimeString;
		if (System.GetCLIParam("autoreload", autoReloadTimeString))
			return Math.Clamp(autoReloadTimeString.ToFloat(), 0.0, 600.0);

		return m_fAutoReloadTime;
	}

	/*!
		Reloads current session (authority only).
	*/
	protected void RestartSession()
	{
		if (!IsMaster())
			return;
		
		Print("SCR_BaseGameMode::RequestScenarioRestart()", LogLevel.DEBUG);
		if (GameStateTransitions.RequestScenarioRestart())
		{
			Print("SCR_BaseGameMode::RequestScenarioRestart() successfull server reload requested!", LogLevel.DEBUG);
		}
	}

	//------------------------------------------------------------------------------------------------
	protected override void OnGameStart()
	{
		BackendApi backendApi = GetGame().GetBackendApi();
		
		if (IsMaster())
			backendApi.NewSession();

		super.OnGameStart();
		Event_OnGameStart.Invoke();
	}

	//------------------------------------------------------------------------------------------------
	void OnGameEnd()
	{
		m_OnGameEnd.Invoke();

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnGameEnd();
		}
	}
	
	//------------------------------------------------------------------------------------------------
	bool IsFactionChangeAllowed()
	{
		return m_bAllowFactionChange;
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerAuditSuccess(int iPlayerID)
	{
	#ifdef RESPAWN_COMPONENT_VERBOSE
			Print("SCR_BaseGameMode::OnPlayerAuditSuccess - playerId: " + iPlayerID, LogLevel.DEBUG);
	#endif

		super.OnPlayerAuditSuccess(iPlayerID);
		m_OnPlayerAuditSuccess.Invoke(iPlayerID);
		
		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerAuditSuccess(iPlayerID);
		}
	}

	//------------------------------------------------------------------------------------------------
	override void OnPlayerAuditFail(int iPlayerID)
	{
	#ifdef RESPAWN_COMPONENT_VERBOSE
		Print("SCR_BaseGameMode::OnPlayerAuditFail - playerId: " + iPlayerID, LogLevel.DEBUG);
	#endif

		super.OnPlayerAuditFail(iPlayerID);
		m_OnPlayerAuditFail.Invoke(iPlayerID);
		
		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerAuditFail(iPlayerID);
		}
	}
	
	//------------------------------------------------------------------------------------------------
	override void OnPlayerAuditTimeouted( int iPlayerID) 
	{
		super.OnPlayerAuditTimeouted(iPlayerID);
		m_OnPlayerAuditTimeouted.Invoke(iPlayerID);
		
		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerAuditTimeouted(iPlayerID);
		}
	};

	//------------------------------------------------------------------------------------------------
	override void OnPlayerAuditRevived( int iPlayerID) 
	{
		super.OnPlayerAuditRevived(iPlayerID);
		m_OnPlayerAuditRevived.Invoke(iPlayerID);
		
		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerAuditRevived(iPlayerID);
		}
	};

	//------------------------------------------------------------------------------------------------
	/*!
		Called after a player is connected. Server-only.
		\param playerId PlayerId of connected player.
	*/
	override void OnPlayerConnected(int playerId)
	{
		if (!m_bIsHosted)
		{
			m_bIsHosted = GetGame().GetPlayerManager().GetPlayerController(playerId).GetRplIdentity() == RplIdentity.Local();
			Replication.BumpMe();
		}
		
		super.OnPlayerConnected(playerId);
		m_OnPlayerConnected.Invoke(playerId);
		
		// TODO: Please revisit and adjust this, this check results in some nasty branching and possible oversights/errors
		// Wait for backend response if dedicated server is used and is not run without backend functionality
		if (m_pRespawnSystemComponent && (RplSession.Mode() != RplMode.Dedicated || System.IsCLIParam("nobackend")))
		{
		#ifdef RESPAWN_COMPONENT_VERBOSE
			Print("SCR_BaseGameMode::OnPlayerConnected - playerId: " + playerId, LogLevel.DEBUG);
		#endif
		}

		// Dispatch event to child components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerConnected(playerId);
		}

		// DON'T! Leave that up to game mode respawning
		// SetPlayerRandomLoadout(playerId);

		#ifdef TREE_DESTRUCTION
		int count = SCR_DestructibleTree.DestructibleTrees.Count();
		for (int i = 0; i < count; i++)
		{
			SCR_DestructibleTree.DestructibleTrees[i].OnPlayerConnected();
		}
		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called after a player is disconnected.
		\param playerId PlayerId of disconnected player.
	*/
	protected override void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout)
	{
		super.OnPlayerDisconnected(playerId, cause, timeout);
		m_OnPlayerDisconnected.Invoke(playerId, cause, timeout);
		
		// RespawnSystemComponent is not a SCR_BaseGameModeComponent, so for now we have to
		// propagate these events manually. 
		if (IsMaster())
			m_pRespawnSystemComponent.OnPlayerDisconnected_S(playerId, cause, timeout);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerDisconnected(playerId, cause, timeout);
		}
		
		m_OnPostCompPlayerDisconnected.Invoke(playerId, cause, timeout);

		if (IsMaster())
		{
			IEntity controlledEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
			if (controlledEntity)
			{
				if (SCR_ReconnectComponent.GetInstance() && SCR_ReconnectComponent.GetInstance().IsReconnectEnabled())
				{
					if (SCR_ReconnectComponent.GetInstance().OnPlayerDC(playerId, cause))	// if conditions to allow reconnect pass, skip the entity delete  
					{
						CharacterControllerComponent charController = CharacterControllerComponent.Cast(controlledEntity.FindComponent(CharacterControllerComponent));
						if (charController)
						{
							charController.SetMovement(0, vector.Forward);
						}
						
						CompartmentAccessComponent compAccess = CompartmentAccessComponent.Cast(controlledEntity.FindComponent(CompartmentAccessComponent)); // TODO nullcheck
						if (compAccess)
						{
							BaseCompartmentSlot compartment = compAccess.GetCompartment();
							if (compartment)
							{
								if(GetGame().GetIsClientAuthority())
								{
									CarControllerComponent carController = CarControllerComponent.Cast(compartment.GetVehicle().FindComponent(CarControllerComponent));
									if (carController)
									{
										carController.Shutdown();
										carController.StopEngine(false);
									}
								}
								else
								{
									CarControllerComponent_SA carController = CarControllerComponent_SA.Cast(compartment.GetVehicle().FindComponent(CarControllerComponent_SA));
									if (carController)
									{
										carController.Shutdown();
										carController.StopEngine(false);
									}
								}
							}
						}
						
						return;
					}
				}
				
				//------------------------------------------------------------------------------------------------
				//!	ACE hard override: Clean up later since otherwise the carried player gets deleted as well...
				GetGame().GetCallqueue().CallLater(SCR_EntityHelper.DeleteEntityAndChildren, 1000, false, controlledEntity);
				//------------------------------------------------------------------------------------------------
			}
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called on every machine after a player is registered (identity, name etc.). Always called after OnPlayerConnected.
		\param playerId PlayerId of registered player.
	*/
	protected override void OnPlayerRegistered(int playerId)
	{
		super.OnPlayerRegistered(playerId);
		m_OnPlayerRegistered.Invoke(playerId);
		
		// RespawnSystemComponent is not a SCR_BaseGameModeComponent, so for now we have to
		// propagate these events manually. 
		if (IsMaster())
			m_pRespawnSystemComponent.OnPlayerRegistered_S(playerId);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerRegistered(playerId);
		}

		#ifdef GMSTATS
		if (IsGameModeStatisticsEnabled())
			SCR_GameModeStatistics.RecordConnection(playerId, GetGame().GetPlayerManager().GetPlayerName(playerId));
		#endif
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called after a player is spawned.
		\param playerId PlayerId of spawned player.
		\param controlledEntity Spawned entity for this player.
	*/
	[Obsolete("Use SCR_BaseGameMode.OnPlayerSpawnFinalize_S")]
	protected override void OnPlayerSpawned(int playerId, IEntity controlledEntity)
	{
		super.OnPlayerSpawned(playerId, controlledEntity);
		m_OnPlayerSpawned.Invoke(playerId, controlledEntity);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerSpawned(playerId, controlledEntity);
		}

		#ifdef GMSTATS
		if (IsGameModeStatisticsEnabled())
		{
			FactionAffiliationComponent fac = FactionAffiliationComponent.Cast(controlledEntity.FindComponent(FactionAffiliationComponent));
			SCR_GameModeStatistics.RecordSpawn(playerId, fac.GetAffiliatedFaction().GetFactionColor().PackToInt());
		}
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	protected override bool HandlePlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{		
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			// First component that returns false overrides the behaviour and can handle
			// the kill based on their respective needs
			if (!comp.HandlePlayerKilled(playerId, playerEntity, killerEntity, killer))
			{
				OnPlayerKilledHandled(playerId, playerEntity, killerEntity, killer);
				return false;
			}
		}
		
		// Handle automatically
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Default player kill behaviour. Called when a player is killed (and HandlePlayerKilled returns true).
	protected override void OnPlayerKilled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		super.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		
		m_OnPlayerKilled.Invoke(playerId, playerEntity, killerEntity, killer);
		
		// RespawnSystemComponent is not a SCR_BaseGameModeComponent, so for now we have to
		// propagate these events manually. 
		if (IsMaster())
			m_pRespawnSystemComponent.OnPlayerKilled_S(playerId, playerEntity, killerEntity, killer);

		// Dispatch events to children components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerKilled(playerId, playerEntity, killerEntity, killer);
		}

		#ifdef GMSTATS
		if (IsGameModeStatisticsEnabled())
			SCR_GameModeStatistics.RecordDeath(playerId, playerEntity, killerEntity, killer);
		#endif
	}
	
	//------------------------------------------------------------------------------------------------
	//! Called after player kill behaviour is handled by a component overriding the generic logic.
	protected void OnPlayerKilledHandled(int playerId, IEntity playerEntity, IEntity killerEntity, notnull Instigator killer)
	{
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
			comp.OnPlayerKilledHandled(playerId, playerEntity, killerEntity, killer);
	}
	
	//------------------------------------------------------------------------------------------------
	protected /*override*/ void OnPlayerDeleted(int playerId, IEntity player)
	{
		// RespawnSystemComponent is not a SCR_BaseGameModeComponent, so for now we have to
		// propagate these events manually. 
		if (IsMaster())
			m_pRespawnSystemComponent.OnPlayerDeleted_S(playerId);

		//super.OnPlayerDeleted(playerId, player);
		m_OnPlayerDeleted.Invoke(playerId, player);

		// Dispatch events to children components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerDeleted(playerId, player);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called when player role changes.
		\param playerId Player whose role is being changed.
		\param roleFlags Roles as a flags
	*/
	protected override void OnPlayerRoleChange(int playerId, EPlayerRole roleFlags)
	{
		super.OnPlayerRoleChange(playerId, roleFlags);
		m_OnPlayerRoleChange.Invoke(playerId, roleFlags);

		// Dispatch events to children components
		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnPlayerRoleChange(playerId, roleFlags);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*!
		Called once loading of all entities of the world have been finished (still within the loading)
		\param world Loaded world
	*/
	override event void OnWorldPostProcess(World world)
	{
		super.OnWorldPostProcess(world);
		m_OnWorldPostProcess.Invoke(world);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnWorldPostProcess(world);
		}
	};

	//------------------------------------------------------------------------------------------------
	/*
		When a controllable entity is spawned, this event is raised.
		\param entity Spawned entity that raised this event
	*/
	protected override void OnControllableSpawned(IEntity entity)
	{
		super.OnControllableSpawned(entity);
		m_OnControllableSpawned.Invoke(entity);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnControllableSpawned(entity);
		}
	}

	//------------------------------------------------------------------------------------------------
	/*
		When a controllable entity is destroyed, this event is raised.
		\param entity Destroyed entity that raised this event
		\param killerEntity Entity that killed the entity (can be null)
		\param killer Instigator entity that destroyed our victim
	*/
	protected override void OnControllableDestroyed(IEntity entity, IEntity killerEntity, notnull Instigator instigator)
	{
		super.OnControllableDestroyed(entity, killerEntity, instigator);
		m_OnControllableDestroyed.Invoke(entity, killerEntity, instigator);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnControllableDestroyed(entity, killerEntity, instigator);	
		}
	}

	//------------------------------------------------------------------------------------------------
	/*
		Prior to a controllable entity being DELETED, this event is raised.
		Controllable entity is such that has BaseControllerComponent and can be
		possessed either by a player, an AI or stay unpossessed.
		\param entity Entity about to be deleted
	*/
	protected override void OnControllableDeleted(IEntity entity)
	{
		super.OnControllableDeleted(entity);
		m_OnControllableDeleted.Invoke(entity);

		foreach (SCR_BaseGameModeComponent comp : m_aAdditionalGamemodeComponents)
		{
			comp.OnControllableDeleted(entity);
		}
		
		// Ensure that controlled entity is a player
		int playerId = GetGame().GetPlayerManager().GetPlayerIdFromControlledEntity(entity);
		if (playerId > 0)
			OnPlayerDeleted(playerId, entity);
	}
	
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Callback raised when the spawn process failed.
			In cases where an entity is spawned, it might be desirable to dispose of it or handle specific
			related game logic if it e.g. cannot be prepared, or assigned to the target player.
			\param entity The entity that should be spawned, but could not be given over (prepared, assigned, ..)
	*/
	void OnSpawnPlayerEntityFailure_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, IEntity entity, SCR_SpawnData data, SCR_ESpawnResult reason)
	{
		foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
			component.OnSpawnPlayerEntityFailure_S(requestComponent, handlerComponent, entity, data, reason);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Callback raised when the spawn system causes provided player's controlled entity to change.
	*/
	void OnPlayerEntityChanged_S(int playerId, IEntity previousEntity, IEntity newEntity)
	{
	}
	
	/*!
		Authority only:
			During the spawn process (after validation pass), the SCR_SpawnHandlerComponent can opt to prepare
			spawned entity. This process first happens on affiliated SCR_SpawnHandlerComponent and if it succeeds,
			it additionally raises this method, which can prepare entity on a global scale. (E.g. game mode logic)
			Preparation can still fail (e.g. desire to seat a character, but an error occurs) and by returning false
			the sender is informed of such failure and can respond accordingly.
			\param requestComponent Instigator of the request.
			\param handlerComponent Handler that processed the request.
			\param data The payload of the request.
			\param entity Spawned (or generally assigned) entity to be prepared.
			\return True on success (continue to next step), fail on failure (terminate spawn process).
	*/
	bool PreparePlayerEntity_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		// Example(s):
		//	- load some data from storage, fill inventory with stored items
		//  - apply loaded data by setting provided entity to bleed for a certain amount
		//  - drop character unconscious?
		
		foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
		{
			if (!component.PreparePlayerEntity_S(requestComponent, handlerComponent, data, entity))
				return false;
		}
		
		// Entity preparation has succeeded
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority only:
			Whenever a SCR_SpawnHandlerComponent processes a spawn request and finished the finalization stage
			(awaits finalization, passes control to client) this method is called. This is the final step in the respawn
			process and after this point the owner of SCR_SpawnRequestComponent is spawned.
			\param requestComponent Instigator of the request.
			\param handlerComponent Handler that processed the request.
			\param data The payload of the request.
			\param entity Spawned (or generally assigned) entity.
	*/
	void OnPlayerSpawnFinalize_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, IEntity entity)
	{
		SCR_SpawnPointSpawnData spawnPointData = SCR_SpawnPointSpawnData.Cast(data);
		if (spawnPointData)
		{
			OnPlayerSpawnOnPoint_S(requestComponent, handlerComponent, entity, spawnPointData);
		}
		
		foreach (SCR_BaseGameModeComponent component : m_aAdditionalGamemodeComponents)
			component.OnPlayerSpawnFinalize_S(requestComponent, handlerComponent, data, entity);

		SCR_PlayerLoadoutComponent loadoutComp = SCR_PlayerLoadoutComponent.Cast(requestComponent.GetPlayerController().FindComponent(SCR_PlayerLoadoutComponent));
		if (!SCR_PossessSpawnData.Cast(data) // hotfix when player is spawned through game master, their loadout would change to the saved loadout
			&& loadoutComp && loadoutComp.GetLoadout())
			loadoutComp.GetLoadout().OnLoadoutSpawned(GenericEntity.Cast(entity), requestComponent.GetPlayerId());		
		
		m_OnPlayerSpawned.Invoke(requestComponent.GetPlayerId(), entity);
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Callback raised when the spawn system causes provided player to spawn with an entity at given spawn point.
			See: SCR_BaseGameMode.OnPlayerSpawnFinalize_S
	*/
	void OnPlayerSpawnOnPoint_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, IEntity entity, SCR_SpawnPointSpawnData spawnPointData)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Callback raised when provided player (SCR_PlayerLoadoutComponent.GetPlayerController()) has their loadout set.
	*/
	void OnPlayerLoadoutSet_S(SCR_PlayerLoadoutComponent loadoutComponent, SCR_BasePlayerLoadout loadout)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Callback raised when provided player (SCR_PlayerFactionAffiliationComponent.GetPlayerController()) has their faction set.
	*/
	void OnPlayerFactionSet_S(SCR_PlayerFactionAffiliationComponent factionComponent, Faction faction)
	{
	}
	
	//------------------------------------------------------------------------------------------------
	/*!
		Authority:
			Override and implement logic for whether provided player can spawn.
			\param requestComponent The player request component instigating this spawn.
			\param handlerComponent The spawn handler component handling this spawn.
			\param data The request payload.
			\param[out] result Reason why respawn is disabled. Note that if returns true the reason will always be OK
			\return True when spawn is allowed, false otherwise. 
	*/
	bool CanPlayerSpawn_S(SCR_SpawnRequestComponent requestComponent, SCR_SpawnHandlerComponent handlerComponent, SCR_SpawnData data, out SCR_ESpawnResult result = SCR_ESpawnResult.SPAWN_NOT_ALLOWED)
	{
		if (m_RespawnTimerComponent) 
		{
			int playerId = requestComponent.GetPlayerId();
			
			// If player has not been enqueued yet, ignore the spawn timer
			if (m_RespawnTimerComponent.IsPlayerEnqueued(playerId))
			{
				float spawnPointTime = 0;
				if (data.IsInherited(SCR_SpawnPointSpawnData))
				{
					SCR_SpawnPointSpawnData spData = SCR_SpawnPointSpawnData.Cast(data);
					spawnPointTime = spData.GetSpawnPoint().GetRespawnTime();
				}

				if (!m_RespawnTimerComponent.GetCanPlayerSpawn(playerId, spawnPointTime))
				{
					result = SCR_ESpawnResult.NOT_ALLOWED_TIMER;
					return false;
				}
			}
		}
		
		result = SCR_ESpawnResult.OK;
		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! Returns remaining respawn time in seconds for given player
	int GetPlayerRemainingRespawnTime(int playerID)
	{
		// Respawn timers
		if (m_RespawnTimerComponent)
			return m_RespawnTimerComponent.GetPlayerRemainingTime(playerID);

		return 0;
	}

	//------------------------------------------------------------------------------------------------
	override bool RplSave(ScriptBitWriter writer)
	{
		EGameFlags gameFlags = GetGame().GetGameFlags();
		writer.WriteIntRange(gameFlags, 0, (EGameFlags.Last<<1)-1);

		return true;
	}

	//------------------------------------------------------------------------------------------------
	override bool RplLoad(ScriptBitReader reader)
	{
		EGameFlags gameFlags;
		reader.ReadIntRange(gameFlags, 0, (EGameFlags.Last<<1)-1);

		GetGame().SetGameFlags(gameFlags, true);

		return true;
	}

	#ifdef GAME_MODE_DEBUG
	
	//------------------------------------------------------------------------------------------------
	//! Draw diagnostic information about all available players.
	void Diag_DrawPlayersWindow()
	{
		//DbgUI.Begin("SCR_BaseGameMode: Players Diag");
		array<int> connectedPlayers = {};		
		GetGame().GetPlayerManager().GetPlayers(connectedPlayers);
		
		array<int> disconnectedPlayers = {};
		GetGame().GetPlayerManager().GetDisconnectedPlayers(disconnectedPlayers);	

		DbgUI.Begin("SCR_BaseGameMode: Connected players");
		{
			for (int i = 0, cnt = connectedPlayers.Count(); i < cnt; i++)
			{
				int playerId = connectedPlayers[i];
				Diag_DrawPlayerInfo(playerId);
			}
		}
		DbgUI.End();
		DbgUI.Begin("SCR_BaseGameMode: Disconnected players");
		{
			for (int i = 0, cnt = disconnectedPlayers.Count(); i < cnt; i++)
			{
				int playerId = disconnectedPlayers[i];
				Diag_DrawPlayerInfo(playerId);
			}
		}
		DbgUI.End();		
	}

	//------------------------------------------------------------------------------------------------
	//! Draw information about provided player (if valid).
	//! \param playerId
	void Diag_DrawPlayerInfo(int playerId)
	{
		string tmp = string.Format("Player %1: (name: %2)", playerId, GetGame().GetPlayerManager().GetPlayerName(playerId));
		array<string> extra = {};		
		
		// Fetch faction info (if present)
		SCR_FactionManager factionManager = SCR_FactionManager.Cast(GetGame().GetFactionManager());
		if (factionManager)
		{
			string factionKey = "None";
			Faction faction = factionManager.GetPlayerFaction(playerId);
			if (faction)
			{
				factionKey = faction.GetFactionKey();
			}
			
			extra.Insert(string.Format("Faction: %1", factionKey));
		}
		
		// Fetch loadout info (if present)
		SCR_LoadoutManager loadoutManager = SCR_LoadoutManager.Cast(GetGame().GetLoadoutManager());
		if (loadoutManager)
		{
			string loadoutKey = "None";
			SCR_BasePlayerLoadout loadout = loadoutManager.GetPlayerLoadout(playerId);
			if (loadout)
			{
				loadoutKey = loadout.GetLoadoutName();
			}
			
			extra.Insert(string.Format("Loadout: %1", loadoutKey));
		}
		
		// If any additional info is present, start formatting
		if (!extra.IsEmpty())
		{
			const string separator = "\n   ";
			for (int i = 0, count = extra.Count(); i < count; i++)
			{
				tmp += string.Format("%1%2", separator, extra[i] );
			}
		}
		
		DbgUI.Text(tmp);
	}

	//------------------------------------------------------------------------------------------------
	//! Draws information about controlled entities.
	void Diag_DrawControlledEntitiesWindow()
	{
		array<int> allPlayers = {};
		GetGame().GetPlayerManager().GetAllPlayers(allPlayers);
	
		DbgUI.Begin("SCR_BaseGameMode: Controlled Entities");	
		{
			for (int i = 0, cnt = allPlayers.Count(); i < cnt; i++)
			{
				int playerId = allPlayers[i];
				Diag_DrawControlledEntityInfo(playerId);
			}
		}
		DbgUI.End();
	}

	//------------------------------------------------------------------------------------------------
	//! Draws information about controlled entity of provided player (if any).
	//! \param playerId
	void Diag_DrawControlledEntityInfo(int playerId)
	{
		string tmp = string.Format("Player %1: (name: %2)", playerId, GetGame().GetPlayerManager().GetPlayerName(playerId));
		array<string> extra = {};
		
		// IsConnected?
		if (GetGame().GetPlayerManager().IsPlayerConnected(playerId))
		{
			extra.Insert("IsConnected: True");
		}
		else
			extra.Insert("IsConnected: False");
		
		// Controlled entity
		IEntity controlledEntity = GetGame().GetPlayerManager().GetPlayerControlledEntity(playerId);
		if (controlledEntity)
		{
			extra.Insert(string.Format("Entity: %1", controlledEntity));
			
			RplComponent rplComponent = RplComponent.Cast(controlledEntity.FindComponent(RplComponent));
			if (rplComponent)
			{
				extra.Insert(string.Format("RplId: %1", rplComponent.Id()));
			}
		}
		else
		{
			extra.Insert("Entity: None");
		}
		
		
		// If any additional info is present, start formatting
		if (!extra.IsEmpty())
		{
			const string separator = "\n   ";
			for (int i = 0, count = extra.Count(); i < count; i++)
			{
				tmp += string.Format("%1%2", separator, extra[i] );
			}
		}
		
		DbgUI.Text(tmp);
	}

	//------------------------------------------------------------------------------------------------
	//! Draws information about game mode component(s)
	void Diag_DrawComponentsWindow()
	{
		DbgUI.Begin("SCR_BaseGameMode: Components");
		{
			int numComponents = m_aAdditionalGamemodeComponents.Count();
			DbgUI.Text(string.Format("NumComponents: %1", numComponents));
			
			for (int i = 0; i < numComponents; i++)
			{
				Diag_DrawComponentInfo(i, m_aAdditionalGamemodeComponents[i]);
			}
		}
		DbgUI.End();
	}

	//------------------------------------------------------------------------------------------------
	//! Draw diagnostics for provided component (if any).
	void Diag_DrawComponentInfo(int index, SCR_BaseGameModeComponent component)
	{
		DbgUI.Text(string.Format("%1: %2", index, component));
	}

	//------------------------------------------------------------------------------------------------
	//!
	void Diag_DrawGameModeWindow()
	{
		DbgUI.Begin("SCR_BaseGameMode");
		{
			string elapsedTime = string.Format("Elapsed time: %1 s", GetElapsedTime());
			DbgUI.Text(elapsedTime);
			
			string running = string.Format("IsRunning: %1", IsRunning());
			DbgUI.Text(running);
			
			string state = string.Format("State: %1", SCR_Enum.GetEnumName(SCR_EGameModeState, GetState()));
			DbgUI.Text(state);
			
			string flags = string.Format("TestFlags: %1 (%2)", SCR_Enum.FlagsToString(EGameFlags, m_eGameState), m_eGameState);
			DbgUI.Text(flags);
			
			if (IsMaster())
			{
				DbgUI.Spacer(16);
				DbgUI.Text("Authority details:");
				
				float autoReloadDelay = GetAutoReloadDelay();
				if (autoReloadDelay > 0)
				{
					DbgUI.Text(string.Format("Auto reload: %1 seconds.", autoReloadDelay));
				}
				else
				{
					DbgUI.Text("Auto reload: Disabled.");
				}
				
				DbgUI.Spacer(16);
				
				if (DbgUI.Button("End Session"))
					EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.FACTION_DRAW));
				
				if (DbgUI.Button("Restart Session"))
					RestartSession();
			}
		}
		DbgUI.End();
	}
	#endif
	
	//------------------------------------------------------------------------------------------------
	//! Called once tasks are initialized
	void HandleOnTasksInitialized();

	//------------------------------------------------------------------------------------------------
	//! Enable or disable controls for the local client.
	//! \param enabled True to enable controls, false to disable controls over controlled entity.
	protected void SetLocalControls(bool enabled)
	{
		PlayerController playerController = GetGame().GetPlayerController();
		if (playerController)
		{
			IEntity controlledEntity = playerController.GetControlledEntity();
			if (controlledEntity)
			{
				ChimeraCharacter character = ChimeraCharacter.Cast(controlledEntity);
				if (character)
				{
					CharacterControllerComponent controller = character.GetCharacterController();
					if (controller)
					{
						bool doDisable = !enabled;
						controller.SetDisableWeaponControls(doDisable);
						controller.SetDisableMovementControls(doDisable);
					}
				}
			}
		}
	}

	#ifdef GM_AI_STATS
	private float m_fLastRecordTime;

	private float m_fLastFlushTime;
	private float m_fFlushRecordInterval = 10000; // 10s

	//------------------------------------------------------------------------------------------------
	private void UpdateStatistics(IEntity owner)
	{
		// Create new records only ever so often
		float timeStamp = owner.GetWorld().GetWorldTime();
		if (timeStamp > m_fLastRecordTime + SCR_GameModeStatistics.RECORD_INTERVAL_MS)
		{
			m_fLastRecordTime = timeStamp;

			#ifdef GMSTATS
			PlayerManager pm = GetGame().GetPlayerManager();
			array<int> players = {};
			pm.GetPlayers(players);

			foreach (int pid : players)
			{
				IEntity ctrlEnt = pm.GetPlayerControlledEntity(pid);
				if (!ctrlEnt)
					continue;

				SCR_GameModeStatistics.RecordMovement(ctrlEnt, pid);
			}
			#endif
			#ifdef AISTATS
			auto aiWorld = SCR_AIWorld.Cast(GetGame().GetAIWorld());
			if (aiWorld)
			{
				array<AIAgent> aiAgents = {};
				aiWorld.GetAIAgents(aiAgents);
				foreach (AIAgent agent : aiAgents)
				{		
					SCR_GameModeStatistics.RecordAILOD(agent);
				}
			}
			#endif
		}

		// Flush data if recording in smaller intervals
		if (timeStamp > m_fLastFlushTime + m_fFlushRecordInterval)
		{
			m_fLastFlushTime = timeStamp;
			if (IsGameModeStatisticsEnabled())
			{
				if (SCR_GameModeStatistics.IsRecording())
					SCR_GameModeStatistics.Flush();
			}
		}
	}
	#endif

	//------------------------------------------------------------------------------------------------
	//!
	//! \return true if controls for local player should be disabled.
	bool GetAllowControls()
	{
		return m_bAllowControls;
	}

	//------------------------------------------------------------------------------------------------
	//!
	//! \return the desired target for the authority of whether controls should be disabled or not, based on the current state, if any.
	protected bool GetAllowControlsTarget()
	{
		SCR_BaseGameModeStateComponent stateComponent = GetStateComponent(GetState());
		if (stateComponent)
			return stateComponent.GetAllowControls();

		return true;
	}

	//------------------------------------------------------------------------------------------------
	//! \return state component for provided state or null if none.
	SCR_BaseGameModeStateComponent GetStateComponent(SCR_EGameModeState state)
	{
		SCR_BaseGameModeStateComponent stateComponent;
		if (m_mStateComponents.Find(state, stateComponent))
			return stateComponent;

		return null;
	}

	//------------------------------------------------------------------------------------------------
	// TODO@AS: Small thing, but get rid of m_fTimeElapsed and use
	// some sort of m_fStartTime and Replication.Time() instead!
	override void EOnFrame(IEntity owner, float timeSlice)
	{		
		#ifdef GM_AI_STATS
		if (IsGameModeStatisticsEnabled())
			UpdateStatistics(owner);
		#endif
		
		if (m_bUseSpawnPreload && m_SpawnPreload)
			HandleSpawnPreload(timeSlice);

		// Allow to accumulate time in pre-game too.
 		SCR_BaseGameModeStateComponent pregameComponent = GetStateComponent(SCR_EGameModeState.PREGAME);
		bool isPregame = GetState() == SCR_EGameModeState.PREGAME;
		// Increment elapsed time on every machine
		bool isRunning = IsRunning();
		if (isRunning || isPregame)
		{
			bool canAdvanceTime = true;

			// Check if any players are present; if using "advance time requires players"
			// we will only advance the time if at least one player is present;
			// this is fairly easy to do, because dedicated (headless) server does not
			// count as a player in the PlayerManager
			if (m_bAdvanceTimeRequiresPlayers)
			{
				int playerCount = GetGame().GetPlayerManager().GetPlayerCount();
				if (playerCount == 0)
					canAdvanceTime = false;
			}

			if (canAdvanceTime)
				m_fTimeElapsed += timeSlice;
		}

		// As the authority make corrections as needed
		if (IsMaster())
		{
			if (m_fTimeElapsed >= m_fLastTimeCorrection + m_fTimeCorrectionInterval)
			{
				Replication.BumpMe();
				m_fLastTimeCorrection = m_fTimeElapsed;
			}

			// Transition from pre-game to game if possible
			// Either fully automatic transition (no component)
			// or state-driven transition based on component logic
			if (isPregame && (!pregameComponent || pregameComponent.CanAdvanceState(SCR_EGameModeState.GAME)))
			{
				if (CanStartGameMode())
					StartGameMode();
			}
			else
			{
				// Time limit end game transition to post-game if possible
				if (IsRunning())
				{
					SCR_BaseGameModeStateComponent gameState = GetStateComponent(SCR_EGameModeState.GAME);
					if (gameState && gameState.CanAdvanceState(SCR_EGameModeState.POSTGAME))
					{
						// Clamp time to maximum
						m_fTimeElapsed = Math.Clamp(m_fTimeElapsed, 0, gameState.GetDuration());

						// Terminate session
						SCR_GameModeEndData data = SCR_GameModeEndData.CreateSimple(EGameOverTypes.EDITOR_FACTION_DRAW); // TODO: Once FACTION_DRAW or TIME_LIMIT works..
						EndGameMode(data);
					}
				}
			}

			// Update controls state
			bool shouldAllowControls = GetAllowControlsTarget();
			if (shouldAllowControls != m_bAllowControls)
			{
				m_bAllowControls = shouldAllowControls;
				Replication.BumpMe();
			}
		}


		// Should we disable local player controls?
		bool allowControls = GetAllowControls();
		SetLocalControls(allowControls);

		#ifdef GAME_MODE_DEBUG
		if (DiagMenu.GetBool(SCR_DebugMenuID.DEBUGUI_GAME_MODE))
		{
			Diag_DrawGameModeWindow();
			Diag_DrawComponentsWindow();
			Diag_DrawPlayersWindow();
			Diag_DrawControlledEntitiesWindow();			
		}
		#endif
	}

	//------------------------------------------------------------------------------------------------
	override void EOnInit(IEntity owner)
	{
		// Set Test Game Flags
		#ifdef WORKBENCH
			if (GetGame().GetWorldEntity() && !GetGame().AreGameFlagsObtained())
			{
				GetGame().SetGameFlags(m_eTestGameFlags, false);
			}
		#endif
		
		//~ Remove any duplicate entries
		if (m_aDisabledResourceTypes.IsEmpty())
		{
			//~ TODO: Make this cleaner
			
			set<EResourceType> duplicateRemoveSet = new set<EResourceType>();
			
			foreach (EResourceType resourceType : m_aDisabledResourceTypes)
			{
				duplicateRemoveSet.Insert(resourceType);
			}
			
			m_aDisabledResourceTypes.Clear();
			foreach (EResourceType resourceType : duplicateRemoveSet)
			{
				m_aDisabledResourceTypes.Insert(resourceType);
			}
		}

		// Find required components
        m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
		m_pRespawnSystemComponent = SCR_RespawnSystemComponent.Cast(owner.FindComponent(SCR_RespawnSystemComponent));		
		m_RespawnTimerComponent = SCR_RespawnTimerComponent.Cast(owner.FindComponent(SCR_RespawnTimerComponent));
		m_ScoringSystemComponent = SCR_BaseScoringSystemComponent.Cast(owner.FindComponent(SCR_BaseScoringSystemComponent));
		m_pGameModeHealthSettings = SCR_GameModeHealthSettings.Cast(owner.FindComponent(SCR_GameModeHealthSettings));

		if (!m_RplComponent)
			Print("SCR_BaseGameMode is missing RplComponent!", LogLevel.ERROR);
		if (!m_pRespawnSystemComponent)
			Print("SCR_BaseGameMode is missing SCR_RespawnSystemComponent!", LogLevel.WARNING);
		
		if (!m_aAdditionalGamemodeComponents)
			m_aAdditionalGamemodeComponents = new array<SCR_BaseGameModeComponent>();

		array<Managed> additionalComponents = new array<Managed>();
		int count = owner.FindComponents(SCR_BaseGameModeComponent, additionalComponents);

		m_aAdditionalGamemodeComponents.Clear();
		for (int i = 0; i < count; i++)
		{
			SCR_BaseGameModeComponent comp = SCR_BaseGameModeComponent.Cast(additionalComponents[i]);
			m_aAdditionalGamemodeComponents.Insert(comp);
		}

		// Find and sort state components
		array<Managed> stateComponents = new array<Managed>();
		int stateCount = owner.FindComponents(SCR_BaseGameModeStateComponent, stateComponents);
		for (int i = 0; i < stateCount; i++)
		{
			SCR_BaseGameModeStateComponent stateComponent = SCR_BaseGameModeStateComponent.Cast(stateComponents[i]);
			SCR_EGameModeState state = stateComponent.GetAffiliatedState();
			// Invalid state
			if (state < 0)
			{
				Print("Skipping one of SCR_BaseGameStateComponent(s), invalid affiliated state!", LogLevel.ERROR);
				continue;
			}

			if (m_mStateComponents.Contains(state))
			{
				string stateName = SCR_Enum.GetEnumName(SCR_EGameModeState, state);
				Print("Skipping one of SCR_BaseGameStateComponent(s), duplicate component for state: " + stateName + "!", LogLevel.ERROR);
				continue;
			}

			m_mStateComponents.Insert(state, stateComponent);
		}
	}
	
	/*
		Preload handling
	*/

	//------------------------------------------------------------------------------------------------
	OnPreloadFinishedInvoker GetOnPreloadFinished()
	{
		if (!m_OnPreloadFinished)
			m_OnPreloadFinished = new OnPreloadFinishedInvoker();
		
		return m_OnPreloadFinished;
	}

	//------------------------------------------------------------------------------------------------
	//!
	//! \param position
	void StartSpawnPreload(vector position)
	{
		m_SpawnPreload = SCR_SpawnPreload.PreloadSpawnPosition(position);
		if (!m_SpawnPreload && m_OnPreloadFinished)
			m_OnPreloadFinished.Invoke();

	}

	//------------------------------------------------------------------------------------------------
	protected void HandleSpawnPreload(float timeSlice)
	{
		bool finished = m_SpawnPreload.Update(timeSlice);
		if (finished)
		{
			m_SpawnPreload = null;
			if (m_OnPreloadFinished)
				m_OnPreloadFinished.Invoke();
		}
	}	
	
	//------------------------------------------------------------------------------------------------
	// constructor
	void SCR_BaseGameMode(IEntitySource src, IEntity parent)
	{
		#ifdef GAME_MODE_DEBUG
		if (!s_DebugRegistered)
		{
			DiagMenu.RegisterMenu(SCR_DebugMenuID.DEBUGUI_GAME_MODE_MENU, "GameMode", "");
			DiagMenu.RegisterBool(SCR_DebugMenuID.DEBUGUI_GAME_MODE, "", "Game Mode", "GameMode");
			s_DebugRegistered = true;
		}
		#endif

		Activate();
		SetEventMask(EntityEvent.INIT | EntityEvent.FRAME);
	}

	//------------------------------------------------------------------------------------------------
	// destructor
	void ~SCR_BaseGameMode()
	{
		#ifdef GAME_MODE_DEBUG
			DiagMenu.Unregister(SCR_DebugMenuID.DEBUGUI_GAME_MODE);
			s_DebugRegistered = false;
		#endif

		#ifdef GM_AI_STATS
		if (SCR_GameModeStatistics.IsRecording())
			SCR_GameModeStatistics.StopRecording();
		#endif
	}

	#ifdef GM_AI_STATS
	//------------------------------------------------------------------------------------------------
	//! Should gamemode diagnostic statistics be enabled?
	private bool IsGameModeStatisticsEnabled()
	{
		// not authority
		if (m_RplComponent && !m_RplComponent.IsMaster())
			return false;

		return GetGame().InPlayMode();
	}
	#endif
}