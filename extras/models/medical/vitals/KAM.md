# KAT Advanced Medical
## Mathematical Functions

$$\mathrm{Clamp}(x,\ x_{min},\ x_{max}) :=
\begin{cases}
    x_{max} & \text{for } x \ge x_{max} \\
    x_{min} & \text{for } x_{min} \ge x \\
    x & \text{otherwise}
\end{cases}$$


## Respiratory Vitals
### Tidal volume (TV)

$$TV = 400 - 75 \cdot PTX$$

- $TV\ [\mathrm{ml}]$: Tidal volume
- $PTX\ [\cdot]$: Pneumothorax factor

### Ventilation (V):

$$V_D = 
\begin{cases}
    V_{min} & \text{| cardiac arrest}
    \\
    60 \cdot \frac{HR}{ANP} + \max\Big(200 \cdot \big(PaCO2(t) - PaCO2_0\big),\ V_{min}\Big) & \Big|\text{otherwise}
\end{cases}$$

- $V_D\ [\mathrm{ml/min}]$: Demanded ventilation
- $V_{min} = 2000\ \mathrm{ml/min}$ : Minimum ventilation demand
- $ANP\ [\cdot]$: Anaerobic pressure
- $PaCO2\ [\mathrm{mmHg}]$: Partial pressure of $\mathrm{CO_2}$

$$ANP = \min\bigg(\frac{0.8}{BVR},\ 1.2\bigg)$$

- $BVR\ [\cdot]$: Blood volume ratio

$$BVR = \frac{BV}{BV_0}$$

- $BV\ [\mathrm{ml}]$: Blood volume
- $BV_0 = 6000\ \mathrm{ml}$

$$V_A = 
\begin{cases}
    1 & \text{| cardiac arrest}
    \\
    TV \cdot RR & \text{| otherwise}
\end{cases}$$

- $V_A\ [\mathrm{ml/min}]$: Actual ventilation

### Respiratory rate (RR):

$$RR = f(V_D,\ TV,\ med.) + f\big(PaCO2(t)\big)$$

$$f(V_D,\ TV,\ med.) =
\begin{cases}
    0 & \text{| cardiac arrest}
    \\
    20 & \text{| BVM in use}
    \\
    \min\Big(\frac{V_D}{TV} - 5 \cdot f(med.),\ RR_{max}\Big) & \Big|\text{ otherwise}
\end{cases}$$

$$f\big(PaCO2(t)\big) =
\begin{cases}
    0 & \text{| cardiac arrest}
    \\
    \min\big(0.2 \cdot (PaCO2(t) - 50),\ RR_{max}\big) & \big|\ PaCO2(t) > 50
    \\
    0 & \text{| otherwise}
\end{cases}$$

- $RR\ [\mathrm{min^{-1}}]$: Respiratory rate
- $RR_{max} = 40\ \mathrm{min^{-1}}$
- $f(med.)$: Some function to account for adjustments through medication (_e.g._ opioid depression)

### Partial pressure of $\mathrm{CO_2}$ (PaCO2)

$$PaCO2(t + \Delta t) = 
\begin{cases}
    PaCO2(t) + \frac{dPACO2}{dt}(t) \cdot \Delta t & \text{|\ PaCO2 active setting enabled}
    \\
    PaCO2_0 & \text{| otherwise}
\end{cases}$$

$$\frac{dPaCO2}{dt}(t) =
\begin{cases}
    \mathrm{Clamp}\Big(PaCO2_0 - PaCO2(t) + 150 \cdot \max(ANP - 1,\ 0),\ -\big[\frac{dPaCO2}{dt}\big]\_{max},\ +\big[\frac{dPaCO2}{dt}\big]\_{max}\Big) & \Big|\ \frac{V_D}{V_A} \approx 1
    \\
    + \big[\frac{dPaCO2}{dt}\big]\_{max} & \big|\ \frac{V_D}{V_A} > 1
    \\
    - \big[\frac{dPaCO2}{dt}\big]\_{max} & \text{| otherwise}
\end{cases}$$

- $\frac{dPaCO2}{dt}\ [\mathrm{mmHg/s}]$: Rate of change of partial pressure of $\mathrm{CO_2}$
- $\big[\frac{dPaCO2}{dt}\big]_{max} = 0.05\ \mathrm{mmHg/s}$ : Maximum rate of change of partial pressure of $\mathrm{CO_2}$
- $PaCO2_0 = 40 \ \mathrm{mmHg}$ : Default partial pressure of $\mathrm{CO_2}$
- $\Delta t\ [s]$: Time step

### End-tidal $\mathrm{CO_2}$ (ETCO2)

$$ETCO2 = 
\begin{cases}
    0 & \text{| cardiac arrest}
    \\
    PaCO2 - 3 - \max\big(-0.0416667 \cdot RR^2 + 3.09167 \cdot RR - ETCO2_0,\ 10\big) & \text{| otherwise}
\end{cases}$$

- $ETCO2\ [\mathrm{mmHg}]$: End-tidal $\mathrm{CO_2}$
- $ETCO2_0 = 37\ \mathrm{mmHg}$

### Blood pH

$$pH = 
\begin{cases}
    pK_a + \log_{10}\bigg(\frac{\mathrm{[HCO_3^-]}}{K_H \cdot PaCO2 - \frac{\max(\Delta pH_{ext},\ 1)}{2000}}\bigg) & \bigg|\text{ kidney action setting enabled}
    \\
    pH_0 & \text{| otherwise}
\end{cases}$$

$$\mathrm{[HCO_3^-]} = \mathrm{[HCO_3^-]}_0$$

$$K_H = 0.03 - 0.001 \cdot (T - T_0)$$

$$T =\ ?$$

- $pH\ [\cdot]$: pH of blood
- $pH_0 = 7.4$: Default pH of blood
- $pK_a = 6.1$ : $pK_a$ of $\mathrm{H_2CO_3}$
- $\mathrm{[HCO_3^-]}\ [\mathrm{mM}]$: Concentration of $\mathrm{HCO_3^-}$ in blood
- $\mathrm{[HCO_3^-]_0} = 24\ \mathrm{mM}$
- $K_H\ [\mathrm{mM/mmHg}]$: Henry constant for $\mathrm{CO_2}$
- $\Delta pH_{ext}\ [\cdot]$: External pH shift due to saline, etc.
- $T\ [\mathrm{°C}]$: Body temperature
- $T_0 = 37\ \mathrm{°C}$

### Fraction of inspired $\mathrm{O_2}$ (FiO2)

$$FiO2 =
\begin{cases}
    \begin{cases}
    FiO2_0 & \text{| in recovery or overstreched} \\
    0 & \text{| otherwise} \\
    \end{cases} & \Bigg|\text{ airways occluded or obstructed}
    \\
    0 & \text{| tension pneumothorax or hemothorax}
    \\
    1 & \text{| oxygen tank connected}
    \\
    FiO2_0 & \text{| otherwise}
\end{cases}$$

- $FiO2\ [\cdot]$: Fraction of inspired $\mathrm{O_2}$
- $FiO2_0 = 0.21$

### Alveolar partial pressure of $\mathrm{O_2}$ (PalvO2)

$$PalvO2(t + \Delta t) = \max\bigg(FiO2 \cdot (p_{atm} - 47) - \frac{PaCO2(t + \Delta t)}{ANP},\ 1\bigg)$$

$$p_{atm} = \begin{cases}
p_{atm,0} \cdot \exp\big(-\frac{h}{8400}\big) & \big| \text{ hypothermia active setting enabled}
\\
p_{atm,0} & \text{| otherwise}
\end{cases}$$

- $PalvO2\ [\mathrm{mmHg}]$: Alveolar partial pressure of $\mathrm{O_2}$
- $p_{atm}\ [\mathrm{mmHg}]$: Atmospheric pressure
- $p_{atm,0} = 760\ \mathrm{mmHg}$
- $h\ [\mathrm{m}]$: Altitude

### Partial pressure of $\mathrm{O_2}$ (PaO2)

$$PaO2(t + \Delta t) = PaO2(t) + \frac{dPaO2}{dt}(t) \cdot \Delta t$$

- $PaO2\ [\mathrm{mmHg}]$: Partial pressure of $\mathrm{O_2}$
- $\frac{dPaO2}{dt}\ [\mathrm{mmHg/s}]$: Rate of change of partial pressure of $\mathrm{O_2}$

$$\frac{dPaO2}{dt}(t) = 
\begin{cases}
    \big[\frac{dPaO2}{dt}\big]\_{max}^+ & |\ PaO2_{tgt.}(t + \Delta t) > PaO2(t)
    \\
    \big[\frac{dPaO2}{dt}\big]\_{max}^- & |\ PaO2_{tgt.}(t + \Delta t) < PaO2(t)
    \\
    0 & \text{| otherwise}
\end{cases}$$

- $\big[\frac{dPaO2}{dt}\big]_{max}^+ = 0.1\ \mathrm{mmHg/s}$ : Maximum rate of increase of partial pressure of $\mathrm{O_2}$
- $\big[\frac{dPaO2}{dt}\big]_{max}^- = -0.1\ \mathrm{mmHg/s}$ : Maximum rate of decrease of partial pressure of $\mathrm{O_2}$
- $PaO2_{tgt.}\ [ \mathrm{mmHg/s}]$: Target partial pressure of $\mathrm{O_2}$

$$PaO2_{tgt.}(t + \Delta t) = \min\bigg(PaO2_0 - \frac{ECB_0}{\max(ECB,\ 500)} \cdot \frac{V_D - V_A}{120},\ PalvO2(t + \Delta t)\bigg)$$

- $ECB\ [\mathrm{ml}]$: Extracellular blood volume
- $ECB_0 = 2700\ \mathrm{ml}$ : Default extracellular blood volume

### Peripheral oxygen saturation (SpO2)

$$SpO2 =
\begin{cases}
    100 \cdot OS & \text{| breathing setting enabled}
    \\
    97 & \text{| otherwise}
\end{cases}$$

- $SpO2$ [%]: Peripheral oxygen saturation
- $OS\ [\cdot]$: Oxygen saturation

$$OS = \min\Bigg(\frac{\max\big((PaO2)^{2.7},\ 1\big)}{\Big(25 - 150 \cdot \big(\frac{pH}{pH_0} - 1\big)\Big)^{2.7} +\ (PaO2)^{2.7}},\ OS_{max}\Bigg)$$

- $OS_{max} = 0.999$

## Respiratory Vitals
### Heart rate (HR)

$$HR(t + \Delta t) =
\begin{cases}
    \begin{cases}
        \mathrm{Clamp}\big(\mathcal{N}(110,\ ?),\ 100,\ 120 \big) & \text{| CPR} \\
        0 & \text{| otherwise}
    \end{cases} & \bigg|\text{ cardiac arrest} \\
    \\
    \begin{cases}
        \min\big(HR_{tgt.},\ HR(t) + \big[\frac{dHR}{dt}\big]\_{max} \cdot \Delta t \big) & |\ HR_{tgt.}(t + \Delta t) \ge HR(t) \\
        \max\big(HR_{tgt.},\ HR(t) -  \big[\frac{dHR}{dt}\big]\_{max} \cdot \Delta t \big) & |\ HR_{tgt.}(t + \Delta t) < HR(t)
    \end{cases} & \bigg|\text{ otherwise}
\end{cases}$$

- $HR\ [\mathrm{min^{-1}}]$: Heart rate
- $HR_{tgt.}\ [\mathrm{min^{-1}}]$: Target heart rate
- $\big[\frac{dHR}{dt}\big]_{max} = 1\ \mathrm{min^{-1}s^{-1}}$ : Maximum heart rate acceleration

$$HR_{tgt.}(t + \Delta t) = 40 + \Delta f\big(SV(t + \Delta t)\big) + \Delta f(PL) + \Delta f(med.)$$

$$\Delta f(SV) = \frac{DR}{2 \cdot SV} + HR_0 \cdot (SVD - 1)$$

$$\Delta f(PL) = 10 \cdot PL$$

- $SV\ [\mathrm{l}]$: Stroke volume
- $DR\ [\cdot]$: Demand return
- $SVD\ [\cdot]$: Stroke volume difference
- $PL\ [\cdot]$: Pain level


$$DR(t + \Delta t) = 0.00002638888 \cdot PaCO2(t)$$

$$PaCO2(t) = 60 \cdot \Big(40 + \Delta f\big(SV(t)\big)\Big)$$

$$SVD(t + \Delta t) = \begin{cases}
    \frac{SV_0}{SV(t)} & \Big|\ \frac{SV_0}{SV(t)} < 1.22 \\
    \frac{SV(t)}{0.66 \cdot SV_0} & \Big|\text{ otherwise}
\end{cases}$$

- $SV_0 = 0.001583333323\ \mathrm{l}$ : Default stroke volume

$$SV(t + \Delta t) = 0.00026388888 \cdot SVD(t + \Delta t) \cdot BV$$

- $BV\ [\mathrm{l}]$: Blood volume

## State Machine
### Events
#### Fatal Vitals

- `SpO2 < 65`

#### Critical Vitals

- `SpO2 < 75`
