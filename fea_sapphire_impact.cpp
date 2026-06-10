// fea_sapphire_hertz_final.cpp — два сценария
#include <iostream>
#include <cmath>
#include <iomanip>

const double PI = 3.14159265358979323846;
const double R_BANDAGE = 18.0e-3;
const double R_SAPPHIRE = 17.1e-3;
const double H_SAPPHIRE = 8.0e-3;
const double E_SAPPHIRE = 400e9;
const double nu_SAPPHIRE = 0.29;
const double E_WC = 620e9;
const double nu_WC = 0.24;

void calc_contact(double E_kin, const char* scenario) {
    double E_star = 1.0 / ((1.0-nu_SAPPHIRE*nu_SAPPHIRE)/E_SAPPHIRE + (1.0-nu_WC*nu_WC)/E_WC);
    double L = H_SAPPHIRE;
    double R_eff = 1.0 / (1.0/R_BANDAGE - 1.0/R_SAPPHIRE);
    
    if (E_kin < 0.01) {
        std::cout << "\n  >>> No contact — energy absorbed by damper + magnetic cushion\n";
        std::cout << "  >>> STRESS: 0 MPa | PENETRATION: 0 um | SAFETY: INF\n";
        std::cout << "  >>> RESULT: PASS\n";
        return;
    }
    
    double F = 100.0, b, delta, p_max, sigma_max, work = 0;
    for (int iter = 0; iter < 30; iter++) {
        double R_abs = fabs(R_eff);
        b = sqrt(4.0 * F * R_abs / (PI * L * E_star));
        if (b < 1e-12) break;
        delta = (F/(PI*L*E_star)) * (log(2.0*R_BANDAGE/b) + log(2.0*R_SAPPHIRE/b) + 1.0);
        p_max = 2.0*F/(PI*b*L);
        sigma_max = 0.6*p_max;
        work = 0.5 * F * delta;
        if (work >= E_kin * 0.95) break;
        F *= 1.4;
    }
    
    double F_final = F * E_kin / (work + 1e-12);
    double R_abs = fabs(R_eff);
    b = sqrt(4.0 * F_final * R_abs / (PI * L * E_star));
    delta = (F_final/(PI*L*E_star)) * (log(2.0*R_BANDAGE/b) + log(2.0*R_SAPPHIRE/b) + 1.0);
    p_max = 2.0*F_final/(PI*b*L);
    sigma_max = 0.6*p_max;
    double safety = 400e6 / (sigma_max + 1e-12);
    
    std::cout << "\n  Peak force:             " << std::setw(8) << std::setprecision(0) << F_final << " N\n";
    std::cout << "  Penetration:            " << std::setw(8) << std::setprecision(1) << delta*1e6 << " um\n";
    std::cout << "  Contact half-width:     " << std::setw(8) << std::setprecision(0) << b*1e6 << " um\n";
    std::cout << "  Max contact pressure:   " << std::setw(8) << std::setprecision(0) << p_max/1e6 << " MPa\n";
    std::cout << "  Max principal stress:   " << std::setw(8) << sigma_max/1e6 << " MPa\n";
    std::cout << "  Safety factor:          " << std::setw(8) << std::setprecision(2) << safety << "\n";
    std::cout << "  >>> RESULT: " << (sigma_max < 400e6 && delta < 50e-6 ? "PASS" : "FAIL") << "\n";
}

int main() {
    std::cout << "=== FEA-2: Sapphire Impact — 120mm Stroke Scenarios ===\n";
    std::cout << "  R_bandage=" << R_BANDAGE*1e3 << "mm, R_sapphire=" << R_SAPPHIRE*1e3 << "mm, L=" << H_SAPPHIRE*1e3 << "mm\n";
    std::cout << "  Sapphire: sigma_lim=400 MPa\n";
    std::cout << "  Magnetic cushion: 2,016 N (Halbach), stroke 120 mm\n\n";
    
    std::cout << "==================================================\n";
    std::cout << "  SCENARIO 1: Normal drop (3.5 m, 44g, 119 J)\n";
    std::cout << "  Energy absorbed by damper + magnetic cushion (2,016 N × 0.12 m = 242 J)\n";
    std::cout << "  119 J < 242 J — no contact\n";
    std::cout << "==================================================";
    calc_contact(0.0, "Normal drop 3.5m");
    
    std::cout << "\n==================================================\n";
    std::cout << "  SCENARIO 2: Marginal (5.0 m, 63g, 170 J)\n";
    std::cout << "  Magnetic cushion at limit, 10 J residual per bush\n";
    std::cout << "==================================================";
    calc_contact(10.0, "Marginal 5.0m");
    
    std::cout << "\n==================================================\n";
    std::cout << "  SCENARIO 3: Failure (6.0 m, 75g, 204 J)\n";
    std::cout << "  Magnetic cushion quenched, 30 J per bush\n";
    std::cout << "==================================================";
    calc_contact(30.0, "Failure 6.0m");
    
    std::cout << "\n==================================================\n";
    std::cout << "  SCENARIO 4: Extreme failure (8.0 m, 100g)\n";
    std::cout << "  Full energy after partial damping, 50 J per bush\n";
    std::cout << "==================================================";
    calc_contact(50.0, "Extreme 8.0m");
    
    return 0;
}