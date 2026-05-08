Nice — I’ll produce a complete, step-by-step EMI **justification** for the filter choices you described. I will **not** propose any changes (you said the build is fixed) — only explain why the selected parts and topology make sense from an EMI point of view, using simple formulas and numeric examples at key frequencies. I keep the math explicit so you can verify every step.

---

# Executive summary

* Your input EMI chain (Line → Y caps + X cap + R || C damper → Common-Mode Choke (CMC) PA2750NL → more Y caps + X cap → converter) forms the standard **CM + DM split filter** and damping network.
* **Common-mode (CM)** suppression is dominated by the CMC (400 µH) working against the Y capacitors (2.2 nF). The net CM cutoff / resonance is in the few 10⁵ Hz region (≈170 kHz) — which means **CM attenuation at the LLC fundamental switching (60–150 kHz) is modest**, but CM attenuation increases rapidly at harmonics in the 0.5–10 MHz range (where conducted EMI limits typically matter).
* **Differential-mode (DM)** suppression relies principally on the leakage inductance (1.4 µH) together with the X (100 nF) shunt capacitance. The DM LC cutoff is ≈425 kHz — i.e. **above** the LLC switching fundamental band: the DM filter therefore provides **little attenuation at the switching fundamental** but **strong attenuation at higher harmonics**.
* The R-C damper (R = 27 Ω, C = 2.2 nF) near the CMC reduces Q and damps the L–C resonance (so you avoid large peaking around the CM resonance frequency). That is exactly the correct role for such a damper in your topology.

Below I show the step-by-step formulas, numeric calculations at representative frequencies (60 kHz, 100 kHz, 150 kHz, 1 MHz, 10 MHz), and simple transfer/attenuation estimates so you can see how the parts behave in the important bands.

---

# Notation and given values

* Common-mode choke (PA2750NL):
  
  * CM inductance (L_{CM} = 400\ \mu\text{H} = 400\times10^{-6}\ \text{H})
  * Leakage inductance (L_\ell = 1.4\ \mu\text{H} = 1.4\times10^{-6}\ \text{H})

* Y capacitors (each) (C_Y = 2.2\ \text{nF} = 2.2\times10^{-9}\ \text{F}) (you have two before and two after the CMC as stated)

* X capacitor (C_X = 100\ \text{nF} = 100\times10^{-9}\ \text{F})

* Damper: (R_d = 27\ \Omega,; C_d = 2.2\ \text{nF}) (same C as Y in your note)

* Frequency range of switching: (f_{SW}\in[60\ \text{kHz},150\ \text{kHz}])

* Useful formulas used:
  
  1. Capacitive reactance: (X_C = \dfrac{1}{2\pi f C})
  2. Inductive reactance: (X_L = 2\pi f L)
  3. LC cutoff (resonant) frequency: (f_0 = \dfrac{1}{2\pi\sqrt{L,C}})
  4. Simple two-element divider (series L then shunt C to ground) transfer to the shunt capacitor: ( \displaystyle H(f)=\frac{|Z_C|}{|Z_L|+|Z_C|}=\frac{X_C}{X_L+X_C}). (Used as a conservative magnitude estimate.)

---

# 1) Key reactances — numeric examples (step-by-step)

I compute (X_C) and (X_L) at the following frequencies: 60 kHz, 100 kHz, 150 kHz, 1 MHz, 10 MHz. (These frequencies show behaviour inside your switching band and well into the EMI test ranges.)

### A. Capacitor reactance (X_C)

Formula: (X_C = \dfrac{1}{2\pi f C}).

**For Y cap (C_Y=2.2\ \text{nF}):**

* At (f=60{,}000\ \text{Hz}):
  
  * (2\pi f = 2\pi\times60{,}000 = 376{,}991.118\ \text{rad/s})
  * (X_{C_Y}=1/(2\pi f C_Y)=1/(376{,}991.118\times2.2\times10^{-9}))
  * (X_{C_Y}\approx 1{,}205.72\ \Omega)

* At (f=100{,}000\ \text{Hz}): (X_{C_Y}\approx 723.43\ \Omega)

* At (f=150{,}000\ \text{Hz}): (X_{C_Y}\approx 482.29\ \Omega)

* At (f=1{,}000{,}000\ \text{Hz}): (X_{C_Y}\approx 72.34\ \Omega)

* At (f=10{,}000{,}000\ \text{Hz}): (X_{C_Y}\approx 7.23\ \Omega)

**For X cap (C_X=100\ \text{nF}):**

* At 60 kHz: (X_{C_X}=1/(2\pi\times60{,}000\times100\times10^{-9})\approx 26.53\ \Omega)
* At 100 kHz: (X_{C_X}\approx 15.92\ \Omega)
* At 150 kHz: (X_{C_X}\approx 10.61\ \Omega)
* At 1 MHz: (X_{C_X}\approx 1.59\ \Omega)
* At 10 MHz: (X_{C_X}\approx 0.16\ \Omega)

(These are straightforward inverses — you can re-plug the numbers into (1/(2\pi f C)) to confirm.)

---

### B. Inductor reactance (X_L)

Formula: (X_L = 2\pi f L).

**For common-mode inductance (L_{CM}=400\ \mu\text{H}):**

* At 60 kHz: (X_{L_{CM}}=2\pi\times60{,}000\times400\times10^{-6}\approx 150.80\ \Omega)
* At 100 kHz: (X_{L_{CM}}\approx 251.33\ \Omega)
* At 150 kHz: (X_{L_{CM}}\approx 376.99\ \Omega)
* At 1 MHz: (X_{L_{CM}}\approx 2{,}513.27\ \Omega)
* At 10 MHz: (X_{L_{CM}}\approx 25{,}132.74\ \Omega)

**For leakage inductance (L_\ell=1.4\ \mu\text{H}):**

* At 60 kHz: (X_{L_\ell}=2\pi\times60{,}000\times1.4\times10^{-6}\approx 0.5278\ \Omega)
* At 100 kHz: (X_{L_\ell}\approx 0.8796\ \Omega)
* At 150 kHz: (X_{L_\ell}\approx 1.3195\ \Omega)
* At 1 MHz: (X_{L_\ell}\approx 8.7965\ \Omega)
* At 10 MHz: (X_{L_\ell}\approx 87.9646\ \Omega)

---

# 2) Resonant / cutoff frequencies (LC)

Use (f_0 = 1/(2\pi\sqrt{L C})).

* **CM L–C (with (L_{CM}=400\ \mu\text{H}) and (C_Y=2.2\ \text{nF}))**
  
  (f_{CM}=\dfrac{1}{2\pi\sqrt{400\times10^{-6}\times2.2\times10^{-9}}})
  
  Compute the root: (400\times10^{-6}\times2.2\times10^{-9}=8.8\times10^{-13}).
  
  (\sqrt{8.8\times10^{-13}}=9.381\times10^{-7}).
  
  Then (1/(2\pi\times9.381\times10^{-7})\approx 169{,}660\ \text{Hz}) (≈ **170 kHz**).
  
  → **CM resonance/cutoff ≈ 170 kHz.**

* **DM (leakage L) with X cap (L_\ell=1.4\ \mu\text{H}, C_X=100\ \text{nF})**
  
  (f_{DM}=\dfrac{1}{2\pi\sqrt{1.4\times10^{-6}\times100\times10^{-9}}})
  
  Multiply: (1.4\times10^{-6}\times100\times10^{-9}=1.4\times10^{-13}).
  
  (\sqrt{1.4\times10^{-13}}=3.7417\times10^{-7}).
  
  (1/(2\pi\times3.7417\times10^{-7})\approx 425{,}359\ \text{Hz}) (≈ **425 kHz**).
  
  → **DM cutoff ≈ 425 kHz.**

**Interpretation:**

* CM resonance (~170 kHz) lies **within/near** the upper end of your switching range (150 kHz) — so the CMC+Y network will have resonant behavior near that band. That is why damping (the R||C damper) is important (see §4).
* DM cutoff (~425 kHz) is **well above** the switching fundamental (60–150 kHz), so the basic DM LC is **not** strongly attenuating the fundamental switching frequency — it becomes effective for higher harmonics (>400 kHz).

---

# 3) Simple attenuation estimates (magnitude) — how much attenuation occurs at representative frequencies

We use the two-element divider (H(f)=X_C/(X_L+X_C)) as a conservative magnitude estimate for the shunt cap transfer (this is a standard, simple way to see whether the shunt cap shorts the disturbance or not).

### A — Common-mode attenuation (CMC vs Y cap), using (X_{L_{CM}}) and (X_{C_Y})

Attenuation in dB approximated as (20\log_{10}!\left(\dfrac{X_{C_Y}}{X_{L_{CM}}+X_{C_Y}}\right)).

Computed values:

* **60 kHz:** (X_{C_Y}=1205.72\ \Omega,; X_{L_{CM}}=150.80\ \Omega)
  Ratio = (1205.72/(1205.72+150.80)=1205.72/1356.52\approx0.889)
  Attenuation ≈ (20\log_{10}(0.889)\approx -1.02\ \text{dB}).

* **100 kHz:** (X_{C_Y}=723.43\ \Omega,; X_{L_{CM}}=251.33\ \Omega)
  Ratio ≈ (723.43/(723.43+251.33)=0.805) → Att ≈ **-2.6 dB**.

* **150 kHz:** (X_{C_Y}=482.29\ \Omega,; X_{L_{CM}}=376.99\ \Omega)
  Ratio ≈ (482.29/(482.29+376.99)=0.561) → Att ≈ **-5.0 dB**.

* **1 MHz:** (X_{C_Y}=72.34\ \Omega,; X_{L_{CM}}=2513.27\ \Omega)
  Ratio ≈ (72.34/(72.34+2513.27)=0.02894) → Att ≈ **-31.1 dB**.

* **10 MHz:** Att ≈ **-70.8 dB** (very strong attenuation).

**Meaning (CM):** at the LLC switching fundamental (60–150 kHz) the **CM attenuation is modest** (≈1–5 dB), but at higher harmonic/EMI test frequencies (1 MHz and above) the attenuation becomes large (tens of dB). This is typical: CMC + Y caps are most potent at RF frequencies; at lower switching fundamental the Y caps are relatively high impedance and the CMC reactance is still moderate, so the pair’s effect at the fundamental is smaller.

Note: you have Y caps on **both** sides of the CMC. Having Y caps on both sides creates additional paths and tends to **increase effective attenuation at higher frequencies** (the simple single-cap model above is conservative).

### B — Differential-mode attenuation (leakage L vs X cap)

Use the same divider formula with (L_\ell) and (C_X) (series L then shunt X cap).

* **60 kHz:** (X_{L_\ell}=0.5278\ \Omega,; X_{C_X}=26.53\ \Omega) → Ratio (=\dfrac{26.53}{0.5278+26.53}=0.984) → Att ≈ **-0.17 dB**.
* **100 kHz:** Att ≈ **-0.47 dB**.
* **150 kHz:** Att ≈ **-1.02 dB**.
* **1 MHz:** Att ≈ **-16.3 dB**.
* **10 MHz:** Att gets large (many dB) because (X_{L_\ell}) grows and (X_{C_X}) shrinks.

**Meaning (DM):** for your switching band (60–150 kHz) the DM filter is **not strongly attenuating** (close to 0 dB attenuation), because DM cutoff is ≈425 kHz. The DM network becomes effective against higher frequency harmonics (≥ a few 100 kHz and up). That is standard in many power stages: single-stage DM filters are more effective against high-frequency harmonics than against the fundamental.

---

# 4) Role and justification of the damper (R = 27 Ω, C = 2.2 nF)

* With (L_{CM}=400\ \mu\text{H}) and (C_d=2.2\ \text{nF}), the undamped resonance is ~170 kHz (see §2). Left undamped, that resonance could make a **peak** in CM transfer (peaking = more EMI radiated or conducted at/near that f). In practice this shows as a passband or peaking near (f_{CM}) in conducted EMI scans.
* The series/parallel (R_d) with the C introduces loss at and around the resonance so Q is reduced. From a practical EMI viewpoint this prevents a sharp CM peak near the switching edge frequencies and spreads/damps the energy over a broader band where the CMC and Y caps are more effective in combination.
* Why 27 Ω? It is a moderate damping value: large enough to introduce significant loss at the resonance (reduce Q) but small enough to avoid dissipating enormous power during normal operation of the filter at RF currents. At resonance the reactive impedances cancel and the resistor sees the RF current; R = 27 Ω gives a practical compromise between damping and dissipation for the small RF currents typical of conducted EMI compared with mains currents. (You already built it — your R is in the typical range used for CM damping networks.)

**Quantitative sense:** the undamped CM reactances at 150 kHz are (X_{L_{CM}}\approx377\ \Omega) and (X_{C_d}\approx482\ \Omega) so without R the network can resonate. With R=27 Ω the peak amplitude is reduced by roughly the damping factor; the R is much smaller than those reactances only at the switching fundamental, so the resistor effectively introduces loss at the resonance while not dominating the low-frequency mains path. This is exactly the intended EMI damping behavior.

---

# 5) Topology & placement justification (your exact order)

You described: input → (Y, X, R||C damper) → CMC → (Y, X) → converter. That ordering is standard and justified:

1. **Y caps on the line side (before CMC):** provide a low-impedance path for CM currents back to protective earth at higher frequencies — they begin to be effective above a few hundred kHz as their reactance drops. They also form part of the CM resonant network with the CMC. Having Y caps on the source side captures some CM energy early.

2. **X cap (100 nF) near the inlet:** addresses differential transients and high-frequency common differential components; it also forms the DM shunt for the leakage inductance. Placing an X cap before the choke is conventional to reduce DM voltage stress and HF DM emissions.

3. **R||C damper (2.2 nF / 27 Ω) around the CMC:** as explained, damps the CM resonance and prevents peaking. Being around the CMC allows it to directly damp the L–C formed by the CMC and the Y caps.

4. **CMC (PA2750NL):** large CM inductance (400 µH) gives strong impedance rise with frequency (note (X_{L_{CM}}) is already thousands of ohms at MHz), so it’s very effective against the MHz EMI components. It does not block differential noise (which passes through the transformer’s leakage), which is why the leakage inductance + X cap pair handles DM higher harmonics.

5. **Y caps on the converter side:** these provide a local HF reference for the converter to earth, trap any residual CM current on the converter side, and if symmetric, reduce CM feed-through. Two sets (both sides) reduce the net feedthrough across the CMC (series/parallel combinations improve HF attenuation in practice).

**Net effect:** this ordered splitting (Y/X + damper → CMC → Y/X) is a textbook conducted-EMI filter for switching supplies. It maximises CM attenuation where the CMC is strong (MHz) while damping the resonance and gives DM suppression at higher harmonics through the leakage L + 100 nF X cap.

---

# 6) Practical takeaways (what your measurements will likely show)

* On a **conducted EMI test**, expect:
  
  * Little attenuation of low-order DM noise inside the switching fundamental range (60–150 kHz) — so if your converter generates strong differential energy at the fundamental it may appear unless the converter’s own layout/slew control already minimizes it.
  * **Significant attenuation at the usual EMI test bands (150 kHz — 30 MHz)** for CM noise, because the CMC + multiple Y caps produce strong CM impedance at MHz and the X caps short DM harmonics above the DM cutoff. The damper reduces the chance of a large resonant peak around ≈170 kHz.
  * A broadened, damped response near ~170 kHz (instead of a sharp spike) because of the 27 Ω damper.

* On **near-field scans / probe** you will likely see:
  
  * Lower common-mode emissions at MHz bands from the input harness (CMC effective).
  * If you see residual emissions near 170 kHz, they will be **damped** (reduced Q) rather than a sharp spike — consistent with an R‖C damper.

---

# 7) Closing justification (concise)

* The **PA2750NL (400 µH)** provides very large CM impedance at EMI frequencies (MHz range), which is exactly what you want for conducted EMI suppression — its impedance rises quickly with frequency (×10 per decade in reactance).
* The **2.2 nF Y caps** are small enough that they don’t create dangerously high leakage currents but large enough to provide an HF return path and to form the L–C resonance at a frequency (≈170 kHz) that’s near the top of your switching range — this gives a practical balance between attenuation at harmonics and safety leakage current limits.
* The **100 nF X cap** is a sensible DM shunt for higher harmonics (its cutoff with the leakage L is ≈425 kHz), so it targets harmonic content above your fundamental switching band.
* The **27 Ω / 2.2 nF damper** is the correct approach to prevent peaking at the CM resonance and to reduce Q — it damps the resonant energy without adding excessive dissipation at mains frequencies.

All of the above is consistent with standard EMI design practice for an LLC operating in 60–150 kHz: **CM attenuation is mainly at MHz (good), DM attenuation is mainly above the switching fundamental (good for harmonics), and the damper prevents resonant peaking near the high end of the switching range.**

---

If you want, I can (a) produce a short one-page printable PDF of this report, or (b) produce graphs (|X| vs frequency, and approximate attenuation vs frequency) using the numbers above — but I won’t change any hardware or make recommendations unless you ask.
