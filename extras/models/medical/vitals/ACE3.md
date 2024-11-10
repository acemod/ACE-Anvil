# ACE3 Medical
## Mathematical Functions

$$\mathrm{Clamp}(x,\ x_{min},\ x_{max}) :=
\begin{cases}
    x_{max} & \text{for } x \ge x_{max} \\
    x_{min} & \text{for } x_{min} \ge x \\
    x & \text{otherwise}
\end{cases}$$

$$\mathrm{Map}(x,\ x_0,\ x_1,\ y_0,\ y_1) := \frac{y_1 - y_0}{x_1 - x_0} \cdot (x - x_0) + y_0$$

$$\mathrm{Lerp}(y_0,\ y_1,\ x) := \mathrm{Map}(x,\ 0,\ 1,\ y_0,\ y_1)$$

$$\mathrm{Lerp}^{-1}(y_0,\ y_1,\ x) := \mathrm{Map}(x,\ y_0,\ y_1,\ 0,\ 1)$$

$$\mathcal{U}(x_{min},\ x_{max}) \text{: RNG from uniform distribution}$$

$$\mathcal{N}(\bar{x},\ \sigma^2) \text{: RNG from normal distribution}$$

## Vitals
### Systolic pressure (SP):

$$SP = 9.4736842 \cdot CO \cdot SVR$$

- $SP\ [Pa]$: Systolic pressure
- $CO\ [m^3/s]$: Cardiac output
- $SVR\ [Pa \cdot s/m^3]$: Systemic vascular resistance

### Diastolic pressure (DP):

$$DP = 6.3157894 \cdot CO \cdot SVR$$

- $DP\ [Pa]$: Diastolic pressure

### Mean arterial pressure (MAP):

$$MAP = \frac{1}{3} SP + \frac{2}{3} DP = 7.368421 \cdot CO \cdot SVR$$

- $MAP\ [Pa]$: Mean arterial pressure

### Pulse pressure (PP):

$$PP = SP - DP = 3.1578948 \cdot CO \cdot SVR$$

- $PP [Pa]$: Pulse pressure (_Not calculated in ACE3_)

### Cardiac output (CO):

$$CO = HR \cdot SV$$

- $HR\ [s^{-1}]$: Heart rate
- $SV\ [m^3]$: Stroke volume

$$SV = SV_0 \cdot \mathrm{Lerp}^{-1}\Big(0.5,\ 1,\ \mathrm{Clamp}\big(BVR,\ 0.5,\ 1\big)\Big)$$

- $SV_0 = 9.5 \cdot 10^{-2}\ l$
- $BVR\ [\cdot]$: Blood volume ratio

### Blood volume ratio (BVR):

$$BVR = \frac{BV}{BV_0}$$

- $BV_0 = 6.0\ l$

### Blood volume (BV):

$$BV(t + \Delta t) = BV(t) - BR \cdot \Delta t + \Delta f(med.)$$

- $BV\ [m^3]$: Blood volume
- $BR\ [m^3/s]$: Bleeding rate
- $\Delta t\ [s]$: Time step ($\approx 1 s$)
- $\Delta f(med.)$: Some function to account for adjustments through medication

$$BR = WBP \cdot BC \cdot \max(CO,\ CO_{g})$$

- $WBP\ [\cdot]$: Wound bleeding percentage (How much of CO gets lost through wounds)
- $BC\ [\cdot]$: Bleeding coefficient (param, default = 1)
- $CO_{g} = 0.05\ l/min$ : Cardiac output due to gravity

### Systemic vascular resistance (SVR):

$$SVR = SVR_0 + \Delta f(med.)$$

- $SVR_0 = 100\ mmHg \cdot min / l$

### Heart rate (HR):

$$HR(t + \Delta t) =
\begin{cases}
    \begin{cases}
        \mathrm{Clamp}\big(25,\ 35,\ \mathcal{N}(30,\ ?)\big) & \text{| CPR} \\
        0 & \text{| otherwise}
    \end{cases} & \bigg|\text{ cardiac arrest} \\
    \\
    \begin{cases}
        \min\big(HR_{tgt.},\ HR(t) + HRA(t) \cdot \Delta t \big) & |\ HRA \ge 0 \\
        \max\big(HR_{tgt.},\ HR(t) + HRA(t) \cdot \Delta t \big) & |\ HRA < 0
    \end{cases} & \bigg|\text{ otherwise}
\end{cases}$$

- $HR_{tgt.}\ [s^{-1}]$: Target heart rate
- $HRA\ [s^{-2}]$: Heart rate acceleration

$$HRA(t) =
\begin{cases}
    -\mathrm{round}\Big(\frac{HR(t)}{10}\Big) & \text{| class 4 hemorrhage} \\
    \frac{1}{2} \cdot \mathrm{round}\big(HR_{tgt.} - HR(t)\big) & \text{| otherwise}
\end{cases}$$

$$HR_{tgt.} = \max\Big(0,\ \max\big(f(MAP,\ BV),\ f(PL)\big) + \Delta f(SpO_2,\ \Delta OD) + \Delta f(med.)\Big)$$

$$f(MAP,\ BV) =
\begin{cases}
    HR(t) \cdot \frac{107 \cdot BVR}{\max(45,\ MAP)} & \text{| at least class 3 hemorrhage} \\
    HR_0 & \text{| otherwise}
\end{cases}$$

$$f(PL) = HR_0 +
\begin{cases}
    50 \cdot PL & |\ PL > 0.2 \\
    0 & \text{| otherwise}
\end{cases}$$

- $PL\ [\cdot]$: Pain level
- $HR_0 = 80\ min^{-1}$

$$\Delta f(SpO_2,\ \Delta OD) = 2 \cdot \big(97 - SpO_2\big) - 1000 \cdot \Delta OD$$

- $SpO_2$ [%]: Peripheral oxygen saturation
- $\Delta OD\ [?]$: Additional oxygen demand

### Peripheral oxygen saturation ($SpO_2$):

$$SpO_2(t + \Delta t) = \mathrm{Clamp}\bigg(0,\ 100,\ SpO_2(t) + \frac{dSpO_2}{dt}(t) \cdot \Delta t \bigg)$$

- $\frac{dSpO_2}{dt}$ [%/s]: Rate of change of $SpO_2$

$$\frac{dSpO_2}{dt}(t) = OD_0 + \Delta OD + 0.00368 \cdot HR(t) \cdot OS$$

- $OD_0 = -0.25\ ?$ : Base oxygen demand
- $OS\ [\cdot]$: Oxygen saturation

$$OS = \bigg(\frac{p_{O_2}}{p_{O_2, 0}}\bigg)^{1-3 \cdot p_{O_2}}$$

- $p_{O_2}\ [Pa]$: Partial pressure of oxygen (capped at $p_{O_2, 0}$)
- $p_{O_2, 0} = 0.255\ bar$

### Oxygen demand (OD):

$$\Delta OD =
\begin{cases}
    \big((1 - AER) \cdot 0.1 \big) - \big((1 - ANR) \cdot 0.05 \big) & \text{| advanced fatigue} \\
    0 & \text{| otherwise}
\end{cases}$$

- $AER\ [\cdot]$: Aerobic reserve
- $ANR\ [\cdot]$: Anaerobic reserve

## State Machine
### Events
#### Bleedout

- `BV < BLOOD_VOLUME_FATAL`

#### Fatal Vitals

- `BV < BLOOD_VOLUME_CLASS_4_HEMORRHAGE`
- `HR < 20 || HR > 220`
- `SP < 50 && DP < 40 && HR < 40`
- `DP > 190`
- `HR < 30 && RNG`

#### Critical Vitals

- `WBL > BLOOD_LOSS_KNOCK_OUT_THRESHOLD`

#### Lowered Vitals

- `WBL > 0`
- `InPain`
