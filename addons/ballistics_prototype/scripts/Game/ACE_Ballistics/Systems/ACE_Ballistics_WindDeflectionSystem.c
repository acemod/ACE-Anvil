//------------------------------------------------------------------------------------------------
class ACE_Ballistics_WindDeflectionSystem : GameSystem
{
	protected TimeAndWeatherManagerEntity m_pWeatherManager;
	protected ref array<ProjectileMoveComponent> m_aProjectiles = {};
	protected static const float MIN_PROJECTILE_SPEED_MPS_SQ = 10000;
	
	//------------------------------------------------------------------------------------------------
	override protected void OnInit()
	{
		super.OnInit();
		
		ChimeraWorld world = GetGame().GetWorld();
		if (world)
			m_pWeatherManager = world.GetTimeAndWeatherManager();
		
		SCR_BaseGameMode gameMode = SCR_BaseGameMode.Cast(GetGame().GetGameMode());
		if (gameMode)
			gameMode.GetOnPlayerSpawned().Insert(RegisterPlayerCharacter);
		
		if (m_aProjectiles.IsEmpty())
			Enable(false);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void RegisterPlayerCharacter(int playerId, IEntity controlledEntity)
	{
		EventHandlerManagerComponent eventHandlerManager = EventHandlerManagerComponent.Cast(controlledEntity.FindComponent(EventHandlerManagerComponent));
		if (!eventHandlerManager)
			return;

		eventHandlerManager.RegisterScriptHandler("OnProjectileShot", this, OnProjectileShot);
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnProjectileShot(int playerID, BaseWeaponComponent weapon, IEntity entity)
	{
		if (!entity)
			return;
		
		ShellMoveComponent projectile = ShellMoveComponent.Cast(entity.FindComponent(ShellMoveComponent));
		if (!projectile)
			return;
		
		RegisterProjectile(projectile);
	}
	
	//------------------------------------------------------------------------------------------------
	void RegisterProjectile(notnull ProjectileMoveComponent projectile)
	{
		m_aProjectiles.Insert(projectile);
		
		if (!IsEnabled())
			Enable(true);
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnUpdate(ESystemPoint point)
	{
		super.OnUpdate(point);
		
		float timeSlice = GetGame().GetWorld().GetTimeSlice();
		vector windVelocity = m_pWeatherManager.GetWindSpeed() * vector.FromYaw(m_pWeatherManager.GetWindDirection());
		float airFriction = 0.0012588; // --- To Do: Calculate from attributes in ProjectileMoveComponent
		
		for (int i = m_aProjectiles.Count() - 1; i >= 0; i--)
		{
			ProjectileMoveComponent projectile = m_aProjectiles[i];
			if (!projectile)
			{
				m_aProjectiles.Remove(i);
				continue;
			}
			
			vector velocity = projectile.GetVelocity();
			if (velocity.LengthSq() < MIN_PROJECTILE_SPEED_MPS_SQ)
			{
				m_aProjectiles.Remove(i);
				continue;
			}
			
			vector relVelocity = velocity - windVelocity;
			vector dragForce = -0.5 * airFriction * relVelocity.LengthSq() * relVelocity.Normalized();
			//projectile.SetVelocity(velocity + timeSlice * dragForce / mass);
		}
		
		if (m_aProjectiles.IsEmpty())
			Enable(false);
	}
}
