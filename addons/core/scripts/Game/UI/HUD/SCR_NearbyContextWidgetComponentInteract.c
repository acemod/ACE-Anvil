//------------------------------------------------------------------------------------------------
//! Copy of vanilla, but IsHealingAction calls replaced with SCR_NearbyContextWidgetComponentInteract.IsHealingAction
class SCR_NearbyContextWidgetComponentInteract : SCR_ScriptedWidgetComponent
{	
	protected ref SCR_ContextBasicInteractionBlipWidgets m_Widgets = new SCR_ContextBasicInteractionBlipWidgets();
	
	protected const float ENABLED_NEARBY_INTERACTION_GLOW_OPACITY = 0.6;
	protected const float DISABLED_NEARBY_INTERACTION_OPACITY = 0.4;

	protected BaseUserAction m_FirstAction;

	// Invokes when a new Context is assigned to this Widget
	// Can be used to detect if this Widget is not assigned to the needed context anymore
	protected ref ScriptInvokerVoid m_OnContextAssigned;
	protected ref UserActionContext m_AssignedContext;
	
	//! Holds all the different cached id with the enum as key
	protected ref map<SCR_ENearbyInteractionContextColors, ref SCR_NearbyContextColorsComponentInteract> m_mCachedActionColors;
	
	protected Color m_IconColor;
	protected Color m_GlowColor;
	protected Color m_BackgroundColor;

	//------------------------------------------------------------------------------------------------
	override void HandlerAttached(Widget w)
	{
		super.HandlerAttached(w);

		m_Widgets.Init(w);
	}

	//------------------------------------------------------------------------------------------------
	//! Will be called everytime the Widget gets assigned to a new context
	//! This happens everytime the context is in line of sight again
	//! \param[in] UIInfo containing all the information for the widget Icon, etc. Can be null
	//! \param[in] UserActionContext Context the Widget gets assigned to
	void OnAssigned(SCR_ActionContextUIInfo info, UserActionContext context)
	{
		m_AssignedContext = context;
		
		array<BaseUserAction> allActions = {};
		context.GetActionsList(allActions);
		
		if (!allActions.IsEmpty())
			m_FirstAction = allActions[0];
		
		if (m_OnContextAssigned)
			m_OnContextAssigned.Invoke();

		if (info)
			info.OnWidgetAssigned(m_wRoot);
		
		SetIconFromAction(info);
	}

	//------------------------------------------------------------------------------------------------
	//! Will be called everytime the Widget gets assigned to a new context
	//! This happens everytime the context is in line of sight again
	//! \param[in] info containing all the information for the widget Icon, etc. Can be null
	//! \param[in] context the Widget gets assigned to
	void OnAssigned(SCR_ActionUIInfo info, BaseUserAction context)
	{
		m_FirstAction = context;
		
		if (m_OnContextAssigned)
			m_OnContextAssigned.Invoke();
		
		SetIconFromAction(info);
	}
	
	//------------------------------------------------------------------------------------------------
	bool HasColorData()
	{
		return m_mCachedActionColors != null;
	}
	
	//------------------------------------------------------------------------------------------------
	void SetColorsData(map<SCR_ENearbyInteractionContextColors, ref SCR_NearbyContextColorsComponentInteract> cachedActionColors)
	{
		m_mCachedActionColors = cachedActionColors;
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateColors(UserActionContext ctx, SCR_ENearbyInteractionContextColors nearbyColorState = UIConstants.NEARBY_INTERACTION_DEFAULT_STATE)
	{
		bool showOuterBackground;
		array<BaseUserAction> outActions = {};
		
		if (ctx.GetActionsList(outActions) > 0)
		{
			foreach(BaseUserAction userAct: outActions)
			{
				if (SCR_NearbyContextWidgetComponentInteract.IsHealingAction(userAct, showOuterBackground, nearbyColorState))
					break;
			}
		}
		
		SCR_NearbyContextColorsComponentInteract nearbyColors;
		if (m_mCachedActionColors.Find(nearbyColorState, nearbyColors))
			SetColors(nearbyColors, showOuterBackground);
	}
	
	//------------------------------------------------------------------------------------------------
	void UpdateColors(BaseUserAction userAct, SCR_ENearbyInteractionContextColors nearbyColorState = UIConstants.NEARBY_INTERACTION_DEFAULT_STATE)
	{
		bool showOuterBackground;

		SCR_NearbyContextWidgetComponentInteract.IsHealingAction(userAct, showOuterBackground, nearbyColorState);
		
		SCR_NearbyContextColorsComponentInteract nearbyColors;
		if (m_mCachedActionColors.Find(nearbyColorState, nearbyColors))
			SetColors(nearbyColors, showOuterBackground);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsHealingAction(BaseUserAction userAct)
	{
		bool showOuterBackground;
		int nearbyColorState;
		return SCR_NearbyContextWidgetComponentInteract.IsHealingAction(userAct, showOuterBackground, nearbyColorState);
	}
	
	//------------------------------------------------------------------------------------------------
	static bool IsHealingAction(BaseUserAction userAct, out bool showOuterBackground, out int nearbyColorState)
	{
		SCR_BaseDamageHealSupportStationAction supportHealAction = SCR_BaseDamageHealSupportStationAction.Cast(userAct);
		if (!supportHealAction)		
			return false;
		
		nearbyColorState = supportHealAction.GetHealthStatus();
		SCR_HealSupportStationAction medAction = SCR_HealSupportStationAction.Cast(userAct);
		if (medAction)
			showOuterBackground = true;
		
		return true;
	}
	
	//------------------------------------------------------------------------------------------------
	//! Try to set the Icon defined in the UIInfo from action first
	//! return True if icon was able to be set from Action or from UIInfo
	protected void SetIconFromAction(SCR_ActionUIInfo info)
	{
		if (m_FirstAction)
		{
			SCR_ActionUIInfo actionUIInfo = SCR_ActionUIInfo.Cast(m_FirstAction.GetUIInfo());			
			if (actionUIInfo && actionUIInfo.SetIconTo(m_Widgets.m_wIcon, m_Widgets.m_wIconGlow))
				return;
		}
		
		if (info && info.SetIconTo(m_Widgets.m_wIcon, m_Widgets.m_wIconGlow))
			return;
		
		m_Widgets.m_wIcon.LoadImageFromSet(0, UIConstants.ICONS_IMAGE_SET, UIConstants.ICON_INTERACT_DEFAULT);
		m_Widgets.m_wIconGlow.LoadImageFromSet(0, UIConstants.ICONS_GLOW_IMAGE_SET, UIConstants.ICON_INTERACT_DEFAULT);
	}

	//------------------------------------------------------------------------------------------------
	//! Set the colors from the given colorset
	//! \param[in] colorset The set of colors to apply 
	//! \param[in] showOuterBackground Should the outer shadow/background effect be displayed
	void SetColors(notnull SCR_NearbyContextColorsComponentInteract colorset, bool showOuterBackground)
	{
		m_IconColor 		= colorset.m_IconColor;
		m_GlowColor 		= colorset.m_IconGlowColor;
		m_BackgroundColor 	= colorset.m_BackgroundColor;
		
		m_Widgets.m_wIcon.SetColor(m_IconColor);
		m_Widgets.m_wIconGlow.SetColor(m_GlowColor);

		m_Widgets.m_wBackground.SetColor(m_BackgroundColor);
		m_Widgets.m_wBackgroundOuter.SetColor(colorset.m_OuterBackgroundColor);
		
		m_Widgets.m_wBackgroundShadowOuter.SetColor(colorset.m_OuterBackgroundColor);
		m_Widgets.m_wBackgroundShadowInner.SetColor(colorset.m_OuterBackgroundColor);
 		
		m_Widgets.m_wBackgroundShadowOuter.SetVisible(showOuterBackground);
		m_Widgets.m_wBackgroundShadowInner.SetVisible(showOuterBackground);
		m_Widgets.m_wBackgroundOuter.SetVisible(showOuterBackground);
	}	

	//------------------------------------------------------------------------------------------------
	//! Changes the color of the root to make the whole widget darker / lighter to indicate if it's on line of sight or not
	//! \param[in] bool Is in line of sight
	void ChangeVisibility(bool isInLineOfSight)
	{		
		if (isInLineOfSight)
		{
			m_Widgets.m_wOverlayBackground.SetOpacity(UIConstants.ENABLED_WIDGET_OPACITY);
			m_Widgets.m_wIconGlow.SetOpacity(UIConstants.DISABLED_WIDGET_OPACITY);
			m_Widgets.m_wIcon.SetOpacity(UIConstants.ENABLED_WIDGET_OPACITY);
			
			m_Widgets.m_wIcon.SetColor(m_IconColor);
			m_Widgets.m_wIconGlow.SetColor(m_GlowColor);
		}
		else
		{
			m_Widgets.m_wOverlayBackground.SetOpacity(UIConstants.DISABLED_WIDGET_OPACITY);
			m_Widgets.m_wIconGlow.SetOpacity(ENABLED_NEARBY_INTERACTION_GLOW_OPACITY);
			m_Widgets.m_wIcon.SetOpacity(DISABLED_NEARBY_INTERACTION_OPACITY);
			
			m_Widgets.m_wIcon.SetColor(Color.Black);
			m_Widgets.m_wIconGlow.SetColor(m_IconColor);
		}
 	}

	//------------------------------------------------------------------------------------------------
	//! Get the context that currently uses this Widget
	UserActionContext GetAssignedContext()
	{
		return m_AssignedContext;
	}

	//------------------------------------------------------------------------------------------------
	ScriptInvokerVoid GetOnContextAssigned()
	{
		if (!m_OnContextAssigned)
			m_OnContextAssigned = new ScriptInvokerVoid();

		return m_OnContextAssigned;
	}
}
