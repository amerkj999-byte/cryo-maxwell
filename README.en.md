[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)]()
[![Verilog](https://img.shields.io/badge/FPGA-Verilog-green.svg)]()
[![FEA: PASS](https://img.shields.io/badge/FEA-PASS-brightgreen.svg)]()
[![Thermal: 40% margin](https://img.shields.io/badge/Thermal-40%25_margin-brightgreen.svg)]()
[![Chemistry: SAFE](https://img.shields.io/badge/Chemistry-SAFE-brightgreen.svg)]()
[![Stroke: 120mm](https://img.shields.io/badge/Stroke-120%20mm-orange.svg)]()
[![Force: 2016 N](https://img.shields.io/badge/Pinning_Force-2016%20N-red.svg)]()
[![Drop: 4.5m](https://img.shields.io/badge/Max_Drop-4.5%20m-critical.svg)]()
[![Temp: 40K](https://img.shields.io/badge/Operating-40%20K-blue.svg)]()
[![CAD: KOMPAS-3D](https://img.shields.io/badge/CAD-KOMPAS--3D%20v16-yellow.svg)]()

FAQ
<details> <summary><b>Why 40 K? Can it be warmer?</b></summary>
YBCO transitions to a superconducting state at 92 K. However, the pinning force increases as the temperature decreases. At 77 K (liquid nitrogen), the pinning force is ~5 N/cm² — too low. At 40 K, it's 15 N/cm². This is a compromise between the cryocooler's cooling capacity and the holding force.

</details><details> <summary><b>Is it possible without a cryocooler? Just fill it with liquid nitrogen and go?</b></summary>
It is possible, but the pinning force will drop threefold (from 2016 N to ~670 N). The maximum drop without breakthrough will decrease from 4.5 m to ~1.5 m. For trail riding — OK. For DH — no.

</details><details> <summary><b>What happens if the power goes out?</b></summary>
The YBCO will heat up, superconductivity will cease, and the magnet will land on the sapphire bushings. The fork will become rigid but controllable. After power is restored, levitation will resume.

</details><details> <summary><b>What about a breakthrough? Is it dangerous?</b></summary>
During a drop >5 m, the magnetic cushion is breached. The shaft strikes the sapphire bushings. The sapphire may crack. The YBCO may crack. The fork requires an overhaul. For the rider, the height of the drop itself is more dangerous than the fork failure.

</details><details> <summary><b>Why is this not used in cycling?</b></summary>
Weight (4.5 kg vs 2.5 kg for Fox 40), start-up time (15 min), power consumption (50 W), cost (~$15k). For a DH race, it's uncompetitive. For space, cryogenic equipment, and precision vibration isolation — it's optimal.

</details><details> <summary><b>What is the service life? When to replace?</b></summary>
The service life of the levitation unit is unlimited (no contact wear). The Inconel 718 bellows — replace at 70,000 cycles. Sapphire bushings — replace if cracked (inspect every 10,000 cycles). Cryocooler — service life ~10,000 hours (replace every 3-5 years).

</details><details> <summary><b>Can it be 3D printed?</b></summary>
The STL files are ready. However, for operation, you need a cryocooler, YBCO tiles, N55 magnets, sapphire bushings, and a vacuum post. 3D printing is only for visualization and assembly checks. Also, the work includes an STL model with an accompanying text drawing (PMI).

</details>

## Quick Start (C++ Compilation)

Instructions for building and running the simulation, thermal balance, and numerical solver modules from the `src` folder.

### Requirements
Compilation requires a compiler supporting at least the C++11 standard (GCC or Clang recommended):
* **Linux:** `sudo apt install build-essential`
* **macOS:** `xcode-select --install`
* **Windows:** Install [Visual Studio](https://microsoft.com) with the "Desktop development with C++" component.

### Building and Running

1. **Clone the repository and navigate to the source code folder:**
   ```bash
   git clone https://github.com
   cd cryo-maxwell/src
Compile the desired executable file:

Main fork generator:

bash
g++ -O3 CRYO-MAXWELL.cpp -o cryo_maxwell
Thermal balance simulation (cryocooler/YBCO):

bash
g++ -O3 thermal_balance.cpp -o thermal_balance
Numerical solver:

bash
g++ -O3 xim_solv.cpp -o xim_solv
Magnetic field calculation (Halbach FEA):

bash
g++ -O3 fea_halbach_magnetostatic.cpp -o fea_magnetostatic
Sapphire bushing strength/impact calculation:

bash
g++ -O3 fea_sapphire_impact.cpp -o fea_impact
Version without assembly (No Assembly):

bash
g++ -O3 cryo-maxwellnoassembly.cpp -o cryo_no_assembly
(The -O3 flag enables code optimization, which is critically important for heavy mathematical calculations).

Run the compiled application:

bash
./cryo_maxwell
Convergence Graphs and Simulation Results (FEA)
Numerical simulation results and convergence graphs for calculations are available in the docs folder:

Study	Convergence Graph
General FEA Convergence	https://docs/convergence_FEA.png
Magnetic System (Halbach)	https://docs/convergence_halbah.png
Heat Gain	https://docs/convergence_heat_gain.png
Impact Loads	https://docs/convergence_impact.png
Superconductor (YBCO)	https://docs/convergence_ybco.png
Click on any graph to open it in its original resolution.

❄️ CRYO-MAXWELL V5.0
A Quantum Pinning Fork Damper with Active Energy Recuperation

https://img.shields.io/badge/License-GPLv3-blue.svg
https://img.shields.io/badge/Stroke-120%2520mm-orange.svg
https://img.shields.io/badge/Pinning_Force-2016%2520N-red.svg
https://img.shields.io/badge/Max_Drop-4.5%2520m-critical.svg
https://img.shields.io/badge/Operating-40%2520K-blue.svg

ABOUT THE PROJECT
Cryo-Maxwell is an inverted telescopic fork where the stanchion does not touch the walls. It levitates in the magnetic field of superconducting YBCO tiles at 40 K. There is no friction. Upon impact, eddy currents in the copper coils decelerate the stanchion and return up to 70% of the energy to the supercapacitor.

The project is calculated from magnetostatics to chemical compatibility. The geometry is generated by C++ code, the control electronics are described in Verilog, and the documentation follows ESKD standards.

TECHNICAL SPECIFICATIONS
Parameter	Value
Type	Inverted telescopic, Stirling cryogenic cycle
Principle	Quantum pinning of YBCO + eddy current damper
Stroke	120 mm
Pinning Force (Halbach)	2,016 N
Maximum Drop (without breakthrough)	4.5 m
Friction	0 N (levitation)
Operating Temperature	40 K (−233°C)
Cool-down Time	~15 min
Weight	~4.5 kg
Power Consumption	~50 W
Recuperation	up to 70%
## REPOSITORY STRUCTURE

```text
.
├── cad/           # CAD drawings and PMI models (KOMPAS-3D)
├── docs/          # Specifications, test programs, and FEA convergence charts
├── hdl/           # FPGA controller hardware description source files (Verilog)
├── src/           # Core C++ modules, numeric solvers, and FEA simulation engines
├── LICENSE        # Project license (GNU GPL v3.0)
└── README.md      # Project documentation
```

HOW IT WORKS
A Stirling cryocooler (2 W @ 40 K) cools 16 single-domain YBCO tiles. A magnetic assembly of 12 NdFeB N55 segments (Halbach cylinder, Br = 1.45 T) levitates in the field of the tiles with a 1.0 mm gap. The holding force is 2,016 N.

Upon compression, the eddy current damper (3 phases, 3×0.3 mm ribbon, Artix-7 FPGA, 250 kHz) shorts the coils, creating a braking torque. On rebound, the energy is rectified into a supercapacitor.

Two sapphire bushings (leukosapphire, Ra ≤ 0.02 µm) in beryllium bronze cages secure the stanchion in case of a magnetic cushion breakthrough. An Inconel 718 bellows (12 convolutions) seals the cryostat.

CALCULATION RESULTS
Magnetostatics (FEA 2.5D): B_max = 1.64 T, B_ybco ≈ 0.20 T, dB/dr = 149 T/m, homogeneity ±10%.

Sapphire-Shroud Contact (Hertz): at a normal drop of 3.5 m — stress is 0 MPa (no contact). At a 5.0+ m breakthrough — stresses exceed 400 MPa, sapphire cracks.

Thermal Balance: total heat gain is 1.20 W against a cryocooler capacity of 2.00 W. A 40% margin.

Chemistry: oxygen safety is confirmed (0.0001 J), Au-In intermetallics are stable, hydrogen embrittlement is below the threshold, galvanic corrosion is absent.

LICENSE
GNU General Public License v3.0. Commercial use is only permitted with the written consent of the author.

Copyright © Roman Chernyaev. [amerkj999@gmail.com]
