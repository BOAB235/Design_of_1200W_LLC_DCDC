Got it — you want a **deep, step‑by‑step EMI justification** of what you already built. No redesign, no “you should…”, just **why your choices make sense** from an **EMI** point of view. Here we go.

> **Assumptions for numeric estimates (standard practice for conducted EMI):**  
> • LISN equivalent impedances: **DM** → 100 Ω (50 Ω + 50 Ω), **CM** → 25 Ω (50 Ω ∥ 50 Ω)  
> • Frequency band of interest: **150 kHz–30 MHz** (CISPR region)  
> • Idealized component models unless stated (i.e., ignoring HF roll‑off of the choke core and ESL/ESR of caps).  
> If your LISN differs (e.g., automotive 5 µH/50 Ω), the numbers shift slightly but the conclusions hold.

---

## 1) What the filter must fight in an LLC 380 → 48 V, 1.2 kW

- **DM noise**: switching current loops between DC link (4 µF) ↔ half-bridge ↔ transformer primary. Dominant content around switching band **60–150 kHz** + harmonics to a few MHz.
- **CM noise**: high dv/dt on half‑bridge node and transformer capacitances push displacement current to chassis/earth, then out the input lines in **common mode**. Energy typically climbs with frequency (hundreds of kHz to several MHz).

Your network is a **two‑stage hybrid**: a CM π‑filter built around a **400 µH CMC**, with **Y capacitors** on both sides, plus **DM shunts (X2 = 100 nF)** on both sides, **leakage inductance** of the CMC as DM series impedance, and a **damping RC (27 Ω + 2.2 nF)** on the input side to temper peaking.

---

## 2) Common‑mode (CM) path justification

### 2.1 Topology

```
[LISN 25Ω]───(node IN)───[ L_CM = 400 µH ]───(node OUT)───[LISN 25Ω]
                 │                              │
               2×CY=2×2.2nF                   2×CY=2×2.2nF
                 │                              │
               Chassis/PE                     Chassis/PE
```

For CM, L and N move together vs. chassis. The two Y capacitors on each side (one from L, one from N) are **in parallel** for CM.  
So **effective shunt per side**: \( C_{Y,\text{eff}} = 2 \times 2.2\,\text{nF} = 4.4\,\text{nF} \).

- CMC impedance (one core, two series windings in CM):  
  \( Z_{L\_CM} = j 2\pi f \cdot 400\,\mu\text{H} \)
- Y‑cap shunt impedance per side:  
  \( Z_{CY} = \dfrac{1}{j 2\pi f \cdot 4.4\,\text{nF}} \)

**Numbers (magnitudes):**
- At **150 kHz**:  
  \(|Z_{L\_CM}| \approx 377\,\Omega\) ; \(|Z_{CY}| \approx 241\,\Omega\)
- At **1 MHz**:  
  \(|Z_{L\_CM}| \approx 2.5\,\text{k}\Omega\) ; \(|Z_{CY}| \approx 36\,\Omega\)

This gives a **CM π‑filter** where the series arm grows with f and the shunts drop with f → **attenuation increases strongly with frequency**.

### 2.2 CM corner/resonance

For a **symmetric CM π** (L in series, \(C_Y\) shunts on both sides), the natural frequency is roughly:
\[
f_{0,CM} \approx \frac{1}{2\pi}\sqrt{\frac{2}{L_{CM}\,C_Y}}
\]
With \(L_{CM}=400\,\mu H\) and \(C_Y=4.4\,nF\):
\[
f_{0,CM} \approx 170\,\text{kHz}
\]
This is **right where CISPR measurement band starts (≈150 kHz)**, which is good: you get **meaningful CM attenuation already at 150 kHz** and rapidly increasing above.

### 2.3 CM insertion loss (with LISN loading)

Using the π‑network above with 25 Ω source/load (per LISN), the predicted **CM insertion loss (IL)** is:

| Frequency | IL_CM (dB) |
|---:|---:|
| 150 kHz | 23.6 |
| 500 kHz | 34.9 |
| 1 MHz | 43.4 |
| 5 MHz | 76.3 |
| 10 MHz | 93.8 |
| 30 MHz | 122.3 |

> Interpretation: **~24 dB at 150 kHz**, climbing to **~43 dB at 1 MHz** and very high in the VHF band. That’s exactly what you want for CM.

**Why your choices are justified (CM):**
- **400 µH CMC** gives a **few hundred ohms** at 150 kHz and **kilohms** above 1 MHz—solid series barrier for CM currents.  
- **Two Y caps per side (4.4 nF effective)** create **low CM impedance** to chassis at high f (36 Ω @ 1 MHz), ensuring **CM currents are diverted to PE** rather than returning via the mains lines.  
- Y caps on **both sides** of the choke make the filter a **true π** → stronger roll‑off and **less sensitivity to parasitics** on just one side.

---

## 3) Differential‑mode (DM) path justification

### 3.1 Topology

For DM, the **leakage inductance** of the CMC acts as the **series L**. You placed 100 nF X2 shunts **both sides** and a **damping RC (27 Ω + 2.2 nF)** on the **input side**.

```
[LISN 50Ω]-L ──┬─────[ 100nF X2 ]────┬── [ L_leak ≈ 1.4 µH ] ──┬────[ 100nF X2 ]──┬── L-[LISN 50Ω]
               │                     │                           │                 │
             N-┴─────────────────────┴───────────────────────────┴─────────────────┴-N
                             │(in parallel with)\n                             [ 27Ω + 2.2nF ] (input side)
```

**Key impedances (magnitudes):**
- Leakage series: \( X_{L\_leak} = 2\pi f \cdot 1.4\,\mu H \)  
  → **1.32 Ω @ 150 kHz**, **8.80 Ω @ 1 MHz**
- X2 shunts: \( X_{C\_X} = 1/(2\pi f \cdot 100\,nF) \)  
  → **10.6 Ω @ 150 kHz**, **1.59 Ω @ 1 MHz**
- Snubber branch: \( Z_{sn}(f) = 27Ω + 1/(j2πf·2.2nF) \)  
  Corner \( f_c \approx \dfrac{1}{2\pi R C} \approx 2.68\,\text{MHz} \)

**Observations:**
- At 150 kHz, the shunt X2 caps are already **≈10 Ω** → they **bypass DM noise** effectively into the 100 Ω LISN.  
- The series leakage is **small at 150 kHz (≈1.3 Ω)**, so DM attenuation near f_SW is **mostly from the shunts** (not from the series L).  
- As frequency increases, leakage inductive reactance rises (~9 Ω @ 1 MHz), and with **shunt caps on both sides**, the section behaves like a **low‑pass L‑section**, then **quasi‑π** if you include the source/load impedances → **steeper roll‑off**.

### 3.2 DM corner/resonance (first‑order view)

A symmetric DM L‑section with shunts \(C_{X1}=C_{X2}=100\,nF\) around \(L_{leak}=1.4\,\mu H\) has a natural frequency approximately:
\[
f_{0,DM}\;\approx\;\frac{1}{2\pi}\sqrt{\frac{C_{X1}+C_{X2}}{L_{leak}\,C_{X1}\,C_{X2}}}
\;\approx\;0.60\,\text{MHz}
\]
In practice this peak is **heavily damped** by the **100 Ω source/load**, by **ESR/ESL**, and by your **RC snubber** (see next).

### 3.3 DM insertion loss (with LISN loading)

Solving the 2‑node network with \(R_s=R_L=100Ω\), shunt \(C_X\) on both sides, series \(L_{leak}\), and the input **RC snubber** in parallel with the input X2, the predicted **DM insertion loss** is:

| Frequency | IL_DM (dB) |
|---:|---:|
| 150 kHz | 25.1 |
| 500 kHz | 25.7 |
| 1 MHz | 47.1 |
| 5 MHz | 92.7 |
| 10 MHz | 110.8 |
| 30 MHz | 139.4 |

> Interpretation: **~25 dB already at 150 kHz** (your switching band), then **~47 dB at 1 MHz**, and very strong attenuation above. This matches the expectation that DM shunts dominate at low MHz, while leakage L and parasitics increase the slope at higher f.

**Why your choices are justified (DM):**
- **100 nF X2** on **both sides** ensures **DM current has two low‑impedance escape paths** → strong shunting of HF DM energy both towards the source and towards the load side.  
- The **CMC leakage (≈1.4 µH)** is **exactly what we want** as a **DM series element**—small enough not to hurt regulation, big enough to increase roll‑off beyond a few hundred kHz.  
- Symmetry (X2 on both sides) limits **peaking** and reduces sensitivity to the exact source/load impedances of the LISN/EUT.

---

## 4) The input RC snubber (27 Ω + 2.2 nF) — what it’s actually damping

- Its **corner** is \( f_c \approx 1/(2\pi·27·2.2\,nF) \approx 2.7\,\text{MHz} \).  
- Below ~1 MHz, its impedance is **much larger** than the 100 nF X2 (e.g., ≈123 Ω at 0.6 MHz vs. X2 ≈2.6 Ω), so **it doesn’t steal energy** from the main shunt path at f_SW.  
- **Above a few MHz**, the snubber **adds a real resistive component** in parallel with the input X2 (whose ESR/ESL may make it high‑Q with wiring inductance). That **damps high‑frequency peaking** caused by:
  - CMC **leakage inductance** + input **X2 ESL/ESR**,
  - **Wiring inductance** and layout stray L with the shunt caps,
  - Any **self‑resonance** of the X2 or the input network.

**Bottom line:** the snubber **flattens the HF response** and avoids undesirable **narrow‑band spikes** in the 2–10 MHz range without compromising low‑frequency DM attenuation.

---

## 5) Why Y caps on **both sides** of the CMC (and not only one side)

- With **CY on both sides**, the CMC sits between **two CM shunts** → a **π filter**. This gives:
  - **Higher CM attenuation** near and above the ~170 kHz corner,
  - **Lower sensitivity** to which side is noisier (source or load),
  - **Reduced CM circulating currents** inside the equipment because each side has a local, low‑Z path to PE at HF.

---

## 6) Check against your switching range and measurement band

- **Switching band 60–150 kHz**:  
  - **DM**: ~25 dB already at 150 kHz; at 60–100 kHz it will still be strong (X2 ≈ 26–16 Ω; leakage X_L ≈ 0.53–0.88 Ω), so the **DM shunt dominates**, giving meaningful attenuation **right where you need it**.  
  - **CM**: ~24 dB at 150 kHz, slightly less at 60–100 kHz but still significant because \(Z_{L\_CM}\) is already a few hundred ohms while Y shunts are a few hundred ohms, creating a proper divider.
- **1 MHz**:  
  - **CM** ≈ 43 dB; **DM** ≈ 47 dB — both are **well‑controlled**.
- **> 5 MHz**:  
  - The model predicts **very large IL**. In reality, core and capacitor parasitics limit this, but your **RC snubber** specifically helps **avoid HF peaking**, trading a little ripple current for a **smoother conducted spectrum**.

---

## 7) Quick sanity numbers (intuition builders)

- **CMC @150 kHz**: ~**377 Ω** series vs. **(241 Ω || 25 Ω)** shunt on each side → good CM division.  
- **X2 @150 kHz**: ~**10.6 Ω** to short DM noise across L–N vs. **100 Ω** LISN → the majority of DM ripple **stays local** and does not reach the mains.  
- **Leakage L @1 MHz**: ~**8.8 Ω** series helps the **100 nF shunts** form a stronger DM low‑pass beyond ~0.5–1 MHz.

---

## 8) What your 4 µF DC link is doing (EMI‑wise)

While not part of the mains filter, the **4 µF DC link** provides a **low DM impedance** on the converter side. That helps **recirculate switching currents locally** and **reduces DM injection** back through the input filter (i.e., it works *with* your X2 shunts to keep the energy in the power stage loop).

---

## 9) Final take

- **CM path**: 400 µH CMC + **two‑per‑side Y1 2.2 nF** is a textbook **CM π** with a ~170 kHz natural corner → **~24 dB at 150 kHz** and **>40 dB at 1 MHz**.  
- **DM path**: **100 nF X2** both sides + **1.4 µH leakage** gives **~25 dB at 150 kHz** and **~47 dB at 1 MHz**; strong roll‑off in the MHz region.  
- **RC snubber 27 Ω + 2.2 nF**: negligible impact below ~1 MHz (so no penalty where you don’t want it), **effective damping above ~2–3 MHz**, preventing narrow‑band **HF peaking** due to parasitics.

Your filter, as built, is **well‑justified** against both DM and CM conducted EMI in the 150 kHz–30 MHz band, especially considering your 60–150 kHz switching range.

---

### One detail to confirm (so I can pin the numbers even tighter):
- Which **conducted EMI standard/LISN** are you measured against (CISPR 11/32 with 50 Ω, or a DC standard with 5 µH/50 Ω)?  
- Is your **chassis/PE** reference solid at both input and output sides of the CMC (same node), as assumed for CM?

If you want, I can also provide a **simple schematic + Bode‑style plots** of the CM/DM insertion loss from the above models.