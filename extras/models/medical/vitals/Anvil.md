# Anvil Medical
## Mathematical Functions

$$\mathrm{Hill}(x) := \frac{x}{1 + x}$$


## Vitals
### Mean arterial pressure (MAP):

$$MAP = CO \cdot SVR$$

- $MAP\ [\mathrm{kPa}]$: Mean arterial pressure
- $CO\ [\mathrm{ml/s}]$: Cardiac output
- $SVR\ [\mathrm{kPa \cdot s/ml}]$: Systemic vascular resistance

### Pulse pressure (PP):

$$PP =\ ?$$

- $PP\ [\mathrm{kPa}]$: Pulse pressure

### Systolic pressure (SP):

$$SP = MAP + \frac{2}{3}PP$$

- $SP\ [\mathrm{kPa}]$: Systolic pressure

### Diastolic pressure (DP):

$$DP = MAP - \frac{1}{3}PP$$

- $DP\ [\mathrm{kPa}]$: Diastolic pressure

### Cardiac output (CO):

$$CO = HR \cdot SV$$

- $HR\ [\mathrm{Hz}]$: Heart rate
- $SV\ [\mathrm{ml}]$: Stroke volume

$$SV = SV_0 \cdot\ ?$$

- $SV_0 = 95\ \mathrm{ml}$

### Blood volume (BV):

$$\frac{dBV}{dt}(t) = \frac{dBT}{dt}(t) - WBP(t) \cdot \max\big(CO(t),\ CO_{g}\big)$$

- $BV\ [\mathrm{ml}]$: Blood volume
- $\frac{dBT}{dt}\ [\mathrm{ml/s}]$: Blood transfusion rate
- $WBP\ [\cdot]$: Wound bleeding percentage (How much of CO gets lost through wounds). Certain type of bleedings can be reduced through "alpha action" of drugs (see KAM)
- $CO_{g} = 0.05\ l/min$ : Cardiac output due to gravity

### Systemic vascular resistance (SVR):

$$SVR = \frac{8 \cdot L_{BV}}{\pi} \cdot \frac{\eta_B}{r_{BV}^4} \qquad\text{(based on Hagen–Poiseuille)}$$

- $L_{BV}\ [\mathrm{mm}]$: = Length of blood vessel
- $\eta_B\ [\mathrm{kPa \cdot s}]$: Dynamic viscosity of blood
- $r_{BV}\ [\mathrm{mm}]$: Radius of blood vessel

$$\eta_B = \eta_{B,0} + \Delta f(med.)$$

$$r_{BV} = r_{BV,0} + \Delta f(med.)$$

- $\Delta f(med.)$: Some function to account for adjustments through medication


### Heart rate (HR):

$$\frac{dHR}{dt}(t) = k_{HR} \cdot \big(HR_{tgt.}(t)  - HR(t) \big)$$

- $HR_{tgt.}\ [\mathrm{Hz}]$: Target heart rate
- $k_{HR}\ [\mathrm{s^{-1}}]$: HR rate constant (parameter for tweaking how fast adjustment to new target takes place)

$$HR_{tgt.}(t) = 
\begin{cases}
    \mathrm{HR_{max} \cdot Hill}\big(\Delta SPE(t) - SPE_0\big) & |\ \Delta SPE(t) > -SPE_0 \\
    0 & \text{| otherwise}
\end{cases}$$

$$SPE_0 = \frac{HR_0}{HR_{max} - HR_0}$$

- $\Delta SPE\ [\cdot]$: Sympathetic and parasympathetic effects
- $HR_{max} = \frac{10}{3}\ \mathrm{Hz}$
- $HR_0 = \frac{4}{3}\ \mathrm{Hz}$

$$\Delta SPE = f(MAP,\ BV) + f(PL) + f(SpO_2,\ \Delta OD) + f(med.)$$

- $PL\ [\cdot]$: Pain level
- $SpO_2$ [%]: Peripheral oxygen saturation
- $\Delta OD\ [?]$: Additional oxygen demand
- $f(med.)$: Some function to account for adjustments through medication

### Peripheral oxygen saturation ($SpO_2$):

$$OS = \mathrm{Hill}\bigg(\Big(\frac{p_{O_2}}{3.5\ \mathrm{kPa}}\Big)^{2.85} \bigg)$$

- $OS\ [\cdot]$: Oxygen saturation
- $p_{O_2}\ [\mathrm{kPa}]$: Partial pressure of oxygen
