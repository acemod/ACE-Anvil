# Anvil Medical
## Mathematical Functions

$$\mathrm{Hill}(x) := \frac{x}{1 + x}$$

$$\mathrm{AbsHill}(x) := \frac{x}{1 + |x|}$$

## Pharmacokinetics
### IV Bolus

$$\mathrm{[A]}(t) = \frac{k_a}{k_a-k_d} \cdot \Big(\exp\big(-k_d \cdot t \big) - \exp\big(-k_a \cdot t \big)\Big) \cdot \mathrm{[P]}_0$$

- $\mathrm{[A]}\ [\mathrm{nM}]$: Concentration of drug in active state
- $\mathrm{[P]}_0\ [\mathrm{nM}]$: Initial concentration of drug in precursor state
- $k_a\ [\mathrm{s^{-1}}]$: Activation rate constant
- $k_d\ \mathrm{[s^{-1}}]$: Deactivation rate constant

### IV Infusion (with steady-state approximation of precursor)

$$\mathrm{[A]}(t) = \frac{v_{IV,0}}{k_d} \cdot
\begin{dcases}
    \Big( 1 + \exp\big(-k_d \cdot t \big) \Big) & \Big|\ t \le \Delta t_{IV} \\
    \exp\big(-k_d \cdot t \big) & \big|\ t > \Delta t_{IV} \\
\end{dcases}$$

- $v_{IV}\ [\mathrm{nM/s}]$: Infusion rate
- $\Delta t_{IV}\ [\mathrm{s}]$: Infusion duration

### Additional notes

- Exponential tails can be dealt with by subtracting the curve by a constant ([see approaches to truncating LJ](https://computecanada.github.io/molmodsim-md-theory-lesson-novice/aio/index.html#problems-with-truncation-of-lennard-jones-interactions-and-how-to-avoid-them))

## Pharmacodynamics
### Hill with multiple drugs

#### Version 1:

$$E = \frac{1}{\mathcal{A}} \cdot\mathrm{Hill}\Bigg(\sum_i \mathcal{X}_i \Bigg)$$


#### Version 2:

$$E = \min\bigg(1,\ \sum_i\frac{\mathcal{X}_i / \mathcal{B}_i}{1 + \mathcal{X}_i}\bigg)$$

- $E\ [\cdot]$: Effect ($E \in [0, 1]$)
- $\mathcal{X}_i\ [\cdot]$: Term for drug $\mathrm{X}_i$
- $\mathcal{A}\ [\cdot]$: Global non-competitive inhibitions
- $\mathcal{B}_i\ [\cdot]$: Non-competitive inhibitions for drug $\mathrm{X}_i$
- $\mathcal{I}_i\ [\cdot]$: Competitive inhibitions for drug $\mathrm{X}_i$

$$\mathcal{X}_i =  \bigg(\frac{\mathrm{[X]}_i}{\mathcal{I}_i \cdot K_i}\bigg)^{\gamma_i}$$

$$\mathcal{A} = 1 + \sum_i \bigg(\frac{\mathrm{[A]}_i}{K_i}\bigg)^{\gamma_i}$$

$$\mathcal{B} = 1 + \sum_i \bigg(\frac{\mathrm{[B]}_i}{K_i}\bigg)^{\gamma_i}$$

$$\mathcal{I} = 1 + \sum_i \frac{\mathrm{[I]}_i}{K_i}$$

- $\mathrm{[X]}_i\ [\mathrm{nM}]$: Concentration of drug $\mathrm{X}_i$
- $K_i\ [\mathrm{nM}]$: Concentration of drug $\mathrm{X}_i$ for 50% effect in absence of others
- $\gamma_i\ [\cdot]$: Hill coefficient of drug $\mathrm{X}_i$

#### Version 3 (Hill with synergistic and antagonistic interactions):

$$E = \mathrm{Hill}\Bigg(\sum_i \mathcal{X}_i \Bigg)$$

$$\mathcal{X}_i = 
\begin{dcases}
    \Big(\frac{\mathcal{S}_i}{\mathcal{A}_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\Big)^{\gamma_i} & \Big|\ K_{ii} > 0 \\
     0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{S}_i = 1 + \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{K_{ij}} & \Big|\ K_{ij} > 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{A}_i = 1 - \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{K_{ij}} & \Big|\ K_{ij} < 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

- $E\ [\cdot]$: Effect ($E \in [0, 1]$)
- $\mathcal{X}_i\ [\cdot]$: Term for drug $\mathrm{X}_i$
- $\mathcal{S}_i\ [\cdot]$: Synergistic interactions for drug $\mathrm{X}_i$
- $\mathcal{A}_i\ [\cdot]$: Antagonistic interactions for drug $\mathrm{X}_i$
- $\mathrm{[X]}_i\ [\mathrm{nM}]$: Concentration of drug $\mathrm{X}_i$
- $K_{ii}\ [\mathrm{nM}]$: Concentration of drug $\mathrm{X}_i$ for 50% effect (EC50) in absence of others
- $\gamma_i\ [\cdot]$: Hill coefficient of drug $\mathrm{X}_i$
- $K_{ij}\ [\mathrm{nM}]$: Interaction constant for drug $\mathrm{X}_j$ to enhance (if positive) or inhibit (if negative) the effect of drug $\mathrm{X}_i$

#### Version 4 (Same as version 3, but with negative effects):


$$E = \mathrm{AbsHill}\Bigg(\sum_i \mathcal{X}_i \Bigg)$$

$$\mathcal{X}_i = 
\begin{dcases}
    \Big(\frac{\mathcal{S}_i}{\mathcal{A}_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\Big)^{\gamma_i} & \Big|\ K_{ii} \neq 0 \\
     0 & \text{| otherwise}
\end{dcases}$$

#### Version 5:

$$Q = \mathrm{AceHill}\big(\mathrm{\underline{x}};\ \mathrm{\underline{\underline{K}}},\ \mathrm{\underline{\underline{L}}},\ Q_{min},\ Q_0,\ Q_{max}\big) = (Q_{max} - Q_{min}) \cdot \ \mathrm{Hill}\Bigg(C_0 \cdot \frac{\big(1 + \sum_i \mathcal{X}^+_i \big)^{n^+}}{\big(1 + \sum_i \mathcal{X}^-_i \big)^{n^-}} \Bigg) + Q_{min}$$

$$C_0 = \frac{Q_0 + Q_{min}}{Q_{max} - Q_0 - 2 \cdot Q_{min}}$$

$$\mathcal{X}^+_i =
\begin{dcases}
    \bigg(\frac{\mathcal{J}^+_i}{\mathcal{J}^-_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\bigg)^{\gamma_i} & \bigg|\ K_{ii} > 0 \land L_{ii} = 0 \\
    \\
    L_{ii} \cdot \frac{\mathcal{A}^+_i}{\mathcal{A}^-_i} \cdot\mathrm{Hill}\Bigg(\bigg(\frac{\mathcal{J}^+_i}{\mathcal{J}^-_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\bigg)^{\gamma_i}\Bigg) & \Bigg|\ K_{ii} > 0 \land L_{ii} > 0 \\
    \\
     0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{X}^-_i =
\begin{dcases}
    \bigg(-\frac{\mathcal{J}^+_i}{\mathcal{J}^-_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\bigg)^{\gamma_i} & \bigg|\ K_{ii} < 0 \land L_{ii} = 0 \\
    \\
    L_{ii} \cdot \frac{\mathcal{A}^+_i}{\mathcal{A}^-_i} \cdot\mathrm{Hill}\Bigg(\bigg(-\frac{\mathcal{J}^+_i}{\mathcal{J}^-_i} \cdot \frac{\mathrm{[X]}_i}{K_{ii}}\bigg)^{\gamma_i}\Bigg) & \Bigg|\ K_{ii} < 0 \land L_{ii} > 0 \\
    \\
     0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{J}^+_i = 1 + \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{K_{ij}} & \bigg|\ K_{ij} > 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{J}^-_i = 1 - \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{K_{ij}} & \bigg|\ K_{ij} < 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{A}^+_i = 1 + \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{L_{ij}} & \bigg|\ L_{ij} > 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

$$\mathcal{A}^-_i = 1 - \sum_{j \neq i}
\begin{dcases}
    \frac{\mathrm{[X]}_j}{L_{ij}} & \bigg|\ L_{ij} < 0 \\
    0 & \text{| otherwise}
\end{dcases}$$

- $Q$: Some quantity $\big(Q \in [Q_{min}, Q_{max}]\big)$
- $Q_0$: Quantity's value in absence of drugs $\big(Q_0 \in (Q_{min}, Q_{max})\big)$
- $\mathcal{X}_i\ [\cdot]$: Term for drug $\mathrm{X}_i$ with either positive $(\mathcal{X}^+_i)$ or negative effect $(\mathcal{X}^-_i)$ on $Q$
- $\mathcal{J}_i\ [\cdot]$: Orthosterically synergistic $(\mathcal{J}^+_i)$ and antagonistic interactions $(\mathcal{J}^-_i)$ for drug $\mathrm{X}_i$
- $\mathcal{A}_i\ [\cdot]$: Allosterically synergistic $(\mathcal{A}^+_i)$ and antagonistic interactions $(\mathcal{A}^-_i)$ for drug $\mathrm{X}_i$
- $n\ [\cdot]$: Global Hill coefficients for positive ($n^+$) and negative effects ($n^-$)
- $\gamma_i\ [\cdot]$: Hill coefficient of drug $\mathrm{X}_i$
- $\mathrm{\underline{x}}\ [\mathrm{nM}]$: Vector containing drug concentrations $\big(x_i = \mathrm{[X]}_i\big)$
- $\mathrm{[X]}_i\ [\mathrm{nM}]$: Concentration of drug $\mathrm{X}_i$
- $\mathrm{\underline{\underline{K}}}\ [\mathrm{nM}]$: Matrix containing dissociation constants on diagonal and orthosteric interaction constants on off-diagonals
- $K_{ii}\ [\mathrm{nM}]$: Dissociation constant of drug $\mathrm{X}_i$ for $Q$-altering receptor
- $K_{ij}\ [\mathrm{nM}]$: Orthosteric interaction constant for drug $\mathrm{X}_j$ to enhance (if positive) or inhibit (if negative) the effect of drug $\mathrm{X}_i$
- $\mathrm{\underline{\underline{L}}}\ [\mathrm{nM}]$: Matrix containing effect limit on diagonals and allosteric interaction constants on off-diagonals
- $L_{ii}\ [\mathrm{nM}]$: Limit of $\mathcal{X}_i$ (Optional parameter to limit effect of drug $\mathrm{X}_i$)
- $L_{ij}\ [\mathrm{nM}]$: Allosteric interaction constant for drug $\mathrm{X}_j$ to enhance (if positive) or inhibit (if negative) the effect of drug $\mathrm{X}_i$

<!--
$$K_{ii} = EC_{50,\ i} \cdot
\begin{dcases}
    \frac{C_0 \cdot (Q_{max} - Q_{min})}{(1 - C_0) \cdot Q_{max} + (1 + C_0) \cdot Q_0 - 2 \cdot Q_{min}} & \bigg|\text{ positive effect} \\
    \\
    \frac{Q_0 - Q_{min}}{(C_0 - 1) \cdot Q_{min} + (C_0 + 1) \cdot Q_0 - 2 \cdot Q_{max} \cdot C_0} & \bigg|\text{ negative effect}
\end{dcases}$$
-->
