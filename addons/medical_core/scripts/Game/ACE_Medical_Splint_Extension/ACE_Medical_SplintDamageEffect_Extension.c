modded class ACE_Medical_SplintDamageEffect
{
    protected float m_fACE_Ext_SplintDurability = 1.0;

    float ACE_Ext_GetMovementPenaltyReduction()
    {
        return 0.5;
    }

    float ACE_Ext_GetSwayPenaltyReduction()
    {
        return 0.5;
    }

    float ACE_Ext_GetPainPenaltyReduction()
    {
        return 0.4;
    }

    float ACE_Ext_GetMovementSpeedMultiplier()
    {
        return 0.85;
    }

    float ACE_Ext_GetSplintDurability()
    {
        return m_fACE_Ext_SplintDurability;
    }

    void ACE_Ext_DamageSplint(float damage)
    {
        if (damage <= 0)
            return;

        m_fACE_Ext_SplintDurability -= damage;

        if (m_fACE_Ext_SplintDurability < 0)
            m_fACE_Ext_SplintDurability = 0;
    }

    bool ACE_Ext_IsSplintBroken()
    {
        return m_fACE_Ext_SplintDurability <= 0;
    }
}