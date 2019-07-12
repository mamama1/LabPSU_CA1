# LabPSU_CA1
"CheapAss 1" Atmega 1284p controlled LabPSU with basic functionallity like CV and CC mode

Welcome to my "CheapAss 1" Lab PSU project, which is based on a TIP3055 power transistor and an Atmega 1284p microcontroller.<br /><br />

Specs:<br />
0 - 24V output voltage in 100 mV steps (CV mode)<br />
0 - 1A output current in 10 mA steps (CC mode)<br />
Around 1% accuracy<br />
Dual 12 Bit DAC (MCP4922)<br />
Precision 4.096V voltage reference for the DAC and Atmega-integrated ADC (MCP1541)<br />
High-side current measurement using the MAX4080T IC<br />
LM324 Quad opamp and LM358 Dual opamp<br />
Low Ripple, Low Noise<br />
One Board solution (transformer not onboard)<br />
Optional front panel for display, buttons, LED and piezo buzzer mounting<br />
Designed for I2C capable 2004 Character LCD