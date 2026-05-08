# LLC

# DAB

### Architecture

Full bridge + L1 + Lm + Full bridge 

The energy is regulated by the AC inductor 

<img src = "https://passive-components.eu/wp-content/uploads/2024/02/dual-active-bridge-topology-example.jpg.webp">"

### Control

* Each leg dutyC is 50% with dead time

* If vin = vout : dutyC = 50% **phase shift** between prim and secondary to control the current 

* if vin > vout : make phase shifting to LegA LegB of **primary** to artificially decrease Vin, and make phase shift between prim and secondary to control the current

* if vin < vout : make phase shifting to LegA LegB of **secondary** to artificially decrease Vin, and make phase shift between prim and secondary to control the current

### Advantages / disadvantages

**Adv**
Simple to control
ZVS
range of the gain is large
Very effecient 
Bidirectional

**Disadv**
More complexe control

HW complexity and cost 

### Variants

* We can add big capacitor like LLC to block the DC current 
* Split L1 in both side for symetry L1/2 Lm L1/2

# Phase-Shifted full bridge

### Architecture

Full bridge + L1 + Lm + Rectifier diode + **DC inductance**

The energy is regulated by the DC inductor (Actually AC + DC inductor control the current sloop with DCind >> ACind)

<img src = "https://www.ti.com/ods/images/SLUP414/GUID-20240201-SS0I-TCGX-H4ZS-JQ34V2C6M6ZQ-low.jpg">

### Control

Each leg dutyC is 50% with dead time

Mainly phase shift between Leg A and Leg B of the primary 

by changing the phase between primary legs, we control the duty cycle so the transfered power 

No secondary control 

### Advantages / disadvantages

**Adv**
Simple to control 
ZVS is Load-Dependent (ZVS loses in low load)
Cost

**Disadv**

Need a large DC inductor 
One-way only (not bidirectional)
Voltage overshot in secondary rectifier due to the DC inductor, snubber is needed

### Variants

secondary rectifier can be synchrone mosfet rect (SR , smart diode)
