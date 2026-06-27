### Relative Sun Power ($P_{sun}$):

$$
P_{sun} =
\begin{cases}
    1 - 4 \cdot \left(\mathrm{Lerp}^{-1}(t_{rise},\ t_{set},\ t) - 0.5 \right)^2 & |\ t_{rise} \le t \le t_{set} \\
    0 & \text{| otherwise}
\end{cases}
$$

### Wind Chill Index ($WCI$):

$$WCI =  (P_{WC} \cdot v_{wind})^{0.8} \cdot \mathrm{Lerp}\left(P_{WC},\ P_{IL},\ \max(S_{indoor}, S_{vehicle})\right)$$

### Outdoor Temperature ($T_{outdoor}$):

$$T_{outdoor} = \mathrm{Lerp}(T_{low},\ T_{high},\ P_{sun}) - \frac{h}{1000\ \mathrm{m}} \cdot 6.5\ \mathrm{K} - WCI$$

### Body Temperature ($T_{body}$):

$$NHF = (BVR - 0.5) \cdot\frac{NCH}{1000}$$
$$NCF = (BVR - 0.5) \cdot\frac{CTE}{1000} \cdot
\begin{cases}
+1 & |\ T_{body} \leq 307\ \mathrm{K}\\
-1 & \text{| otherwise}
\end{cases}
$$

$$\frac{dT_{body}}{dt}(t) = C_{IF} \cdot \left(T_{outdoor} - T_{body}(t)\right) + NHF + NCF$$
$$T_{body}(t + \Delta t) = T_{body}(t) + \Delta t \cdot \frac{dT_{body}}{dt}(t)$$


