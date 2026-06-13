# ❄ CRYO-MAXWELL V5.0

**A Quantum Pinning Fork Damper with Active Energy Recuperation**

![License](https://shields.io)
![Stroke](https://shields.io)
![Pinning Force](https://shields.io)
![Max Drop](https://shields.io)
![Operating Temp](https://shields.io)

---

## ABOUT THE PROJECT

**Cryo-Maxwell** is an inverted telescopic fork where the stanchion does not touch the walls. It levitates in the magnetic field of superconducting YBCO tiles at 40 K. There is zero friction. Upon impact, eddy currents generated in the copper coils decelerate the stanchion and return up to 70% of the kinetic energy back to the supercapacitor.

The project is fully simulated and calculated from magnetostatics to chemical compatibility. The geometry is generated using C++ source code, the control electronics are described in Verilog, and the technical documentation follows ESKD design standards.

---

## TECHNICAL SPECIFICATIONS

| Parameter | Value |
| :--- | :--- |
| **Type** | Inverted telescopic, Stirling cryogenic cycle |
| **Principle** | Quantum pinning of YBCO + eddy current damper |
| **Stroke** | 120 mm |
| **Pinning Force (Halbach)** | 2,016 N |
| **Maximum Drop (without breakthrough)** | 4.5 m |
| **Friction** | 0 N (levitation) |
| **Operating Temperature** | 40 K (−233°C) |
| **Cool-down Time** | ~15 min |
| **Weight** | ~4.5 kg |
| **Power Consumption** | ~50 W |
| **Recuperation Efficiency** | up to 70% |

---

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

---

## QUICK START (C++ Compilation)

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
   ```

2. **Compile the desired module:**

   * **Main fork simulation:**
     ```bash
     g++ -O3 CRYO-MAXWELL.cpp -o cryo_maxwell
     ```
   * **Thermal balance simulation (cryocooler/YBCO):**
     ```bash
     g++ -O3 thermal_balance.cpp -o thermal_balance
     ```
   * **Numerical matrix solver:**
     ```bash
     g++ -O3 xim_solv.cpp -o xim_solv
     ```
   * **Magnetic field calculation (Halbach FEA):**
     ```bash
     g++ -O3 fea_halbach_magnetostatic.cpp -o fea_magnetostatic
     ```
   * **Sapphire bushing strength/impact calculation:**
     ```bash
     g++ -O3 fea_sapphire_impact.cpp -o fea_impact
     ```
   * **Version without assembly (No Assembly):**
     ```bash
     g++ -O3 cryo-maxwellnoassembly.cpp -o cryo_no_assembly
     ```
   *(The `-O3` flag enables code optimization, which is critically important for heavy mathematical calculations).*

3. **Run the compiled application:**
   ```bash
   ./cryo_maxwell
   ```

---

## CONVERGENCE GRAPHS & FEA RESULTS

Numerical simulation results and convergence graphs for calculations are available in the `docs` folder:

| Study | Convergence Graph |
| :--- | :--- |
| **General FEA Convergence** | [![FEA Convergence](docs/convergence_FEA.png)](docs/convergence_FEA.png) |
| **Magnetic System (Halbach)** | [![Halbach Convergence](docs/convergence_halbah.png)](docs/convergence_halbah.png) |
| **Heat Gain** | [![Heat Gain Convergence](docs/convergence_heat_gain.png)](docs/convergence_heat_gain.png) |
| **Impact Loads** | [![Impact Convergence](docs/convergence_impact.png)](docs/convergence_impact.png) |
| **Superconductor (YBCO)** | [![YBCO Convergence](docs/convergence_ybco.png)](docs/convergence_ybco.png) |

*Click on any graph to open it in its original resolution.*

---

## HOW IT WORKS

* **Cryogenics:** A Stirling cryocooler (2 W @ 40 K) cools 16 single-domain YBCO tiles. 
* **Levitation:** A magnetic assembly of 12 NdFeB N55 segments (Halbach cylinder, $B_r$ = 1.45 T) levitates in the frozen magnetic field of the tiles with a stable 1.0 mm gap. Total holding force is 2,016 N.
* **Damping:** Upon compression, the eddy current damper (3 phases, 3×0.3 mm copper ribbon, controlled via Artix-7 FPGA at 250 kHz) shorts the coils, creating a braking torque. On rebound, the kinetic energy is rectified into a supercapacitor.
* **Safety & Sealing:** Two sapphire bushings (leukosapphire, $R_a \le 0.02\ \mu\text{m}$) in beryllium bronze cages secure the stanchion in case of a magnetic cushion breakthrough. An Inconel 718 bellows (12 convolutions) completely seals the vacuum cryostat.

---

## CALCULATION RESULTS

* **Magnetostatics (FEA 2.5D):** $B_{\text{max}}$ = 1.64 T, $B_{\text{ybco}} \approx$ 0.20 T, $dB/dr$ = 149 T/m, homogeneity within $\pm10\%$.
* **Sapphire-Shroud Contact (Hertz):** At a normal drop of 3.5 m — stress is 0 MPa (no contact). At a 5.0+ m breakthrough — mechanical stresses exceed 400 MPa, causing the sapphire to crack.
* **Thermal Balance:** Total heat gain is 1.20 W against a cryocooler capacity of 2.00 W. This provides a safe 40% thermal margin.
* **Chemistry:** Oxygen safety is confirmed ($0.0001\text{ J}$ impact energy threshold), Au-In intermetallics are thermodynamically stable, hydrogen embrittlement is below the risk threshold, and galvanic corrosion is absent.

---

## FAQ

**Why 40 K? Can it be warmer?**  
YBCO transitions to a superconducting state at 92 K. However, the pinning force increases as the temperature decreases. At 77 K (liquid nitrogen), the pinning force is only ~5 N/cm², which is too low. At 40 K, it reaches 15 N/cm². This temperature is an optimal compromise between the cryocooler's cooling capacity and the target holding force.

**Is it possible without a cryocooler? Just fill it with liquid nitrogen and go?**  
It is possible, but the pinning force will drop threefold (from 2016 N down to ~670 N). The maximum drop height before a mechanical breakthrough occurs will decrease from 4.5 m to ~1.5 m. For light trail riding — OK. For Downhill (DH) — absolutely no.

**What happens if the power goes out?**  
The YBCO will slowly heat up, superconductivity will cease, and the magnetic shaft will land safely on the backup sapphire bushings. The fork will become rigid but remains fully steerable and controllable. Once power is restored and the system cools down, levitation will resume.

**What about a breakthrough? Is it dangerous?**  
During a drop higher than 5 meters, the magnetic cushion is breached. The shaft strikes the sapphire bushings. The sapphire or the YBCO tiles may crack, requiring a complete fork overhaul. For the rider, the physical height of a 5+ meter drop itself is significantly more dangerous than the fork failure.

**Why is this not used in professional cycling?**  
Weight (4.5 kg vs 2.5 kg for a Fox 40), cool-down start-up time (15 min), power consumption (50 W), and prototype cost (~$15k). For an active DH race, it's uncompetitive. However, for space applications, cryogenic equipment, and precision vibration isolation — it is optimal.

**What is the service life? When should parts be replaced?**  
The service life of the main levitation unit is technically unlimited due to zero contact wear. The Inconel 718 bellows should be replaced at 70,000 cycles. Sapphire bushings require inspection every 10,000 cycles (replace if cracked). The Stirling cryocooler has a service life of ~10,000 hours (replace every 3-5 years).

**Can it be 3D printed?**  
The STL files are ready. However, for full operation, you need a cryocooler, YBCO tiles, N55 magnets, sapphire bushings, and a vacuum post. 3D printing is recommended only for visualization and assembly/fit checks. The CAD model includes an STL representation alongside an accompanying text drawing with Product Manufacturing Information (PMI).

---

## LICENSE

This project is licensed under the **GNU General Public License v3.0**. Commercial use is strictly prohibited without the prior written consent of the author.

**Copyright © Roman Chernyaev.** ([amerkj999@gmail.com](mailto:amerkj999@gmail.com))

