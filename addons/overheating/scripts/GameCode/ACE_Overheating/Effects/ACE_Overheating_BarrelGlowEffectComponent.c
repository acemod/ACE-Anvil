//------------------------------------------------------------------------------------------------
//! How to configure a weapon for glowing:
//! --------------------------------------
//! 1) Add ACE_Overheating_BarrelGlowEffectComponent as a child of MuzzleComponent to weapon prefab
//! 2) Add emissive map to weapon emat and bind `Emissive` to `m_vColor`
//!    Example override for M249:
//!    ```
//!    MatPBRBasic : "{758946B2EC5F1439}Assets/Weapons/MachineGuns/M249/Data/M249_Body_1.emat" {
//!     Emissive 0 0 0 1
//!     EmissiveAbsoluteLV 0
//!     EmissiveMap "{3175FE13B1FC2769}Assets/Weapons/MachineGuns/M249/Data/M249_Body_1_EM.edds"
//!     Refs {
//!      "Emissive" "ACE_Overheating_BarrelGlowEffectComponent.m_vColor"
//!     }
//!    }
//!    ```
class ACE_Overheating_BarrelGlowEffectComponentClass : ScriptComponentClass
{
	[Attribute(defvalue: "0.188 0.025 0 1", desc: "Color for maximal glowing barrel.")]
	protected ref Color m_cMaxColor;
	
	[Attribute(defvalue: "40", desc: "Resolution for target intensity.", params: "1 inf")]
	protected int m_iMaxIntensityStep;
	
	[Attribute(defvalue: "0.03", desc: "Rate of change for approaching target intensity. [1/s]", params: "0 1")]
	protected float m_fIntensityRate;
	
	//------------------------------------------------------------------------------------------------
	Color GetMaxColor()
	{
		return m_cMaxColor;
	}
	
	//------------------------------------------------------------------------------------------------
	int GetMaxIntensityStep()
	{
		return m_iMaxIntensityStep;
	}
	
	//------------------------------------------------------------------------------------------------
	float GetIntensityRate()
	{
		return m_fIntensityRate;
	}
}

//------------------------------------------------------------------------------------------------
class ACE_Overheating_BarrelGlowEffectComponent : ScriptComponent
{
	[RplProp(onRplName: "OnIntensityStepChanged")]
	protected int m_iIntensityStep;
	
	protected float m_fTargetIntensity;
	protected float m_fIntensity;
	protected vector m_vColor;
	protected ACE_Overheating_BarrelGlowEffectComponentClass m_pData;
	
	//------------------------------------------------------------------------------------------------
	static ACE_Overheating_BarrelGlowEffectComponent FromMuzzle(BaseMuzzleComponent muzzle)
	{
		if (!muzzle)
			return null;
		
		return ACE_Overheating_BarrelGlowEffectComponent.Cast(muzzle.FindComponent(ACE_Overheating_BarrelGlowEffectComponent));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void OnPostInit(IEntity owner)
	{
		super.OnPostInit(owner);
		
		if (!GetGame().InPlayMode())
			return;
		
		m_pData = ACE_Overheating_BarrelGlowEffectComponentClass.Cast(GetComponentData(owner));
	}
	
	//------------------------------------------------------------------------------------------------
	override protected void EOnFrame(IEntity owner, float timeSlice)
	{
		if (float.AlmostEqual(m_fTargetIntensity, m_fIntensity))
		{
			ClearEventMask(owner, EntityEvent.FRAME);
			return;
		}
		
		float diff = m_fTargetIntensity - m_fIntensity;
		m_fIntensity += diff.Sign() * m_pData.GetIntensityRate() * timeSlice;
		UpdateColor();
	}
	
	//------------------------------------------------------------------------------------------------
	protected void UpdateColor()
	{
		Color color = m_pData.GetMaxColor();
		m_vColor = m_fIntensity * Vector(color.R(), color.G(), color.B()); 
	}
	
	//------------------------------------------------------------------------------------------------
	protected void OnIntensityStepChanged()
	{
		m_fTargetIntensity = (float)m_iIntensityStep / m_pData.GetMaxIntensityStep();
		SetEventMask(GetOwner(), EntityEvent.FRAME);
	}
	
	//------------------------------------------------------------------------------------------------
	//! Sets intensity of glow, with 0 for no glow and 1 for max glow. Glow changes smoothly.
	void SetIntensity(float intensity)
	{
		int step = Math.Round(intensity * m_pData.GetMaxIntensityStep());
		if (step == m_iIntensityStep)
			return;
		
		m_iIntensityStep = step;
		
		if (!System.IsConsoleApp())
			OnIntensityStepChanged();
		
		Replication.BumpMe();
	}
	
	//------------------------------------------------------------------------------------------------
	//! Same as SetIntensity, but change is done instantly
	void ForceIntensity(float intensity)
	{
		m_iIntensityStep = Math.Round(intensity * m_pData.GetMaxIntensityStep());
		
		if (!System.IsConsoleApp())
			RpcDo_ForceIntensityBroadcast(intensity);
		
		Rpc(RpcDo_ForceIntensityBroadcast, intensity);
	}
	
	//------------------------------------------------------------------------------------------------
	[RplRpc(RplChannel.Reliable, RplRcver.Broadcast)]
	protected void RpcDo_ForceIntensityBroadcast(float intensity)
	{
		m_fTargetIntensity = intensity;
		m_fIntensity = intensity;
		UpdateColor();
	}
}
