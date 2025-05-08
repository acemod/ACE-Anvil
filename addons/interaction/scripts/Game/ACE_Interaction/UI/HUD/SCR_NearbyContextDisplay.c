//------------------------------------------------------------------------------------------------
modded class SCR_NearbyContextDisplay : SCR_InfoDisplayExtended
{
	//------------------------------------------------------------------------------------------------
	//! Copy of vanilla method, but fetches overridden available context list if requested
	override void DisplayUpdate(IEntity owner, float timeSlice)
	{
		bool bCameraActive = m_CameraHandler && m_CameraHandler.IsCameraActive();
		// m_InteractionHandlerComponent.SetNearbyCollectionEnabled(true);
		if (!bCameraActive || !m_InteractionHandlerComponent.GetNearbyCollectionEnabled())
		{
			// if we are currently displaying something, clear it.
			if (!m_aCurrentlyUsedWidgets.IsEmpty())
			{
				foreach (Widget w : m_aCurrentlyUsedWidgets)
				{
					if (w)
						w.SetVisible(false);
				}

				m_aCurrentlyUsedWidgets.Clear();
			}

			return;
		}

		// Fetch nearby contexts
		array<UserActionContext> contexts = {};
		// <<<<<<< vanilla
		//	m_InteractionHandlerComponent.GetNearbyAvailableContextList(contexts);
		// =======
			if (m_InteractionHandlerComponent.ACE_GetManualNearbyAvailableContextOverride())
				m_InteractionHandlerComponent.ACE_GetNearbyAvailableContextOverrideList(contexts);
			else
				m_InteractionHandlerComponent.GetNearbyAvailableContextList(contexts);
		// >>>>>>> ACE
		
		#ifdef DEBUG_ACTIONICONS
		array<UserActionContext> debugContexts = {};
		m_InteractionHandlerComponent.GetNearbyUnavailableContextList(debugContexts);
		contexts.InsertAll(debugContexts);
		#endif

		// Get required data
		BaseWorld world = owner.GetWorld();
		int cameraIndex = world.GetCurrentCameraId();
		vector mat[4];
		world.GetCamera(cameraIndex, mat);
		vector referencePos = mat[3];

		// Iterate through individual contexts,
		// validate that they are visible,
		// and update their widget representation
		const float threshold = 0.25;
		const float fovBase = 100; // whatever fits

		// Field of view and screen resolution is necessary to compute proper position and size of widgets
		float zoom = 1; // world.GetCameraVerticalFOV(cameraIndex) - missing crucial getter
		CameraManager cameraManager = GetGame().GetCameraManager();
		if (cameraManager)
		{
			CameraBase camera = cameraManager.CurrentCamera();

			#ifdef DEBUG_NEARBY_CONTEXT_DISPLAY
			if (camera.GetProjType() == CameraType.NONE)
				PrintFormat("%1 [DisplayUpdate] None Projection", this);
			#endif

			if (camera && camera.GetProjType() != CameraType.NONE)
				zoom = fovBase / Math.Max(camera.GetVerticalFOV(), 1);
		}

		// Screen resolution is necessary to know how far away the widget is from screen center
		// or from cursor, if we ever allow player to use mouse cursor or eye tracking software to select the actions.
		float resX; float resY;
		GetGame().GetWorkspace().GetScreenSize(resX, resY);

		UserActionContext currentContext = m_InteractionHandlerComponent.GetCurrentContext();

		// If we draw something, hide it first
		if (!m_aCurrentlyUsedWidgets.IsEmpty())
		{
			foreach (Widget w : m_aCurrentlyUsedWidgets)
			{
				if (w)
					w.SetVisible(false);
			}

			m_aCurrentlyUsedWidgets.Clear();
		}

		bool isOverrideEnabled = m_InteractionHandlerComponent.GetManualCollectionOverride();
		SCR_NearbyContextColorsComponentInteract nearbyColors;
		SCR_HealSupportStationAction medAction;
		array<BaseUserAction> outActions = {};
		IEntity entAction;

		// Iterate through every available context and assign a Widget to it
		foreach (UserActionContext ctx : contexts)
		{
			// Do not draw currently select one
			if (currentContext == ctx)
				continue;

			vector position = ctx.GetOrigin();
			float posX, posY;
			GetWorldToScreenPosition(world, cameraIndex, position, posX, posY);
			
			if (ctx.GetActionsList(outActions) < 1)
				continue;
			
			entAction = outActions[0].GetOwner();
			if (!entAction)
				continue;
			
			#ifdef DEBUG_ACTIONICONS
			foreach(BaseUserAction baseAction: outActions)
			{
				SCR_ActionUIInfo actionUIInfo = SCR_ActionUIInfo.Cast(baseAction.GetUIInfo());
		
				if (!actionUIInfo)
					Print("DEBUG_ACTIONICONS:: No icons found for "+ baseAction);
			}
			#endif

			// Just ignore actions out of reach or out of screen, we will fade them out anyway
			if (IsOnScreen(resX, resY, posX, posY) && IsInLineOfSight(position, mat, threshold))
			{
				// Only do the raycast if there is no collection override and the context has UseRaycast enabled
				if (!isOverrideEnabled && ctx.ShouldCheckLineOfSight() && IsObstructed(position, referencePos, world, entAction))
					continue;
				
				//Get the widget array from map using the layout as key
				SCR_ActionContextUIInfo info = SCR_ActionContextUIInfo.Cast(ctx.GetUIInfo());
				Widget widget;
				array<Widget> mapWidgets;

				if (info)
				{
					string layout = info.GetLayout();
					string mapKey;

					// Get the GUID of the Layout as it's used as key in the map holding the array of different widgets
					if (layout)
						mapKey = layout;
					else
						mapKey = m_sDefaultMapKey;

					mapWidgets = m_mCachedWidgets.Get(mapKey);

					if (!mapWidgets || mapWidgets.IsEmpty())
						CacheWidget(mapKey, 1);

					mapWidgets = m_mCachedWidgets.Get(mapKey);

					foreach (Widget w : mapWidgets)
					{
						if (m_aCurrentlyUsedWidgets.Contains(w))
							continue;

						widget = w;
						m_aCurrentlyUsedWidgets.Insert(w);
						break;
					}

					//! If there is no widget available create a new one and use that
					if (!widget)
					{
						CacheWidget(mapKey, 1, true, widget);
						m_aCurrentlyUsedWidgets.Insert(widget);
					}

				}
				else
				{
					mapWidgets = m_mCachedWidgets.Get(m_sDefaultMapKey);

					if (!mapWidgets || mapWidgets.IsEmpty())
						CacheWidget(m_sDefaultMapKey, 1);

					mapWidgets = m_mCachedWidgets.Get(m_sDefaultMapKey);

					foreach (Widget w : mapWidgets)
					{
						if (m_aCurrentlyUsedWidgets.Contains(w))
							continue;

						widget = w;
						m_aCurrentlyUsedWidgets.Insert(w);
						break;
					}

					//! If there is no widget available create a new one and use that
					if (!widget)
					{
						CacheWidget(m_sDefaultMapKey, 1, true, widget);
						m_aCurrentlyUsedWidgets.Insert(widget);
					}
				}

				if (widget)
				{
					FrameSlot.SetPos(widget, posX, posY);

					bool visible = ctx.IsInVisibilityAngle(referencePos);
					float distanceLimit = m_fContextCollectDistance * zoom;
					float distanceSq = vector.DistanceSq(position, referencePos);
					float distance = Math.Sqrt(distanceSq);

					SetWidgetAlpha(widget, distance, distanceLimit);
					widget.SetZOrder(-(int)distance);

					SCR_NearbyContextWidgetComponentInteract widgetComp = SCR_NearbyContextWidgetComponentInteract.Cast(widget.FindHandler(SCR_NearbyContextWidgetComponentInteract));
					if (!widgetComp)
						continue;
					
					if (widgetComp.GetAssignedContext() != ctx)
					{
						widgetComp.OnAssigned(info, ctx);

						if (!widgetComp.HasColorData())
							widgetComp.SetColorsData(m_mCachedActionColors);
						
						widgetComp.UpdateColors(ctx);
					}

					widgetComp.ChangeVisibility(visible);
				}
			}
		}
		// Enable required amount of widgets
		SetVisibleWidgets();
	}
}