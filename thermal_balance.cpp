// thermal_balance.cpp — исправленный (тонкостенный шток)
#include <iostream>
#include <cmath>
#include <iomanip>

int main() {
    const double SIGMA = 5.670374419e-8;
    const double PI = 3.14159265358979323846;
    
    const double L_cold = 0.130;
    const double D_cold = 0.042;
    const double A_cold = PI * D_cold * L_cold;
    const double T_hot = 300.0;
    const double T_cold = 40.0;
    const double N_MLI = 20;
    
    std::cout << "=== THERMAL BALANCE: Cryo-Maxwell v3.0 (Optimized) ===\n\n";
    
    // 1. Излучение
    double eps_eff = 1.0 / (N_MLI + 1);
    double Q_rad = eps_eff * SIGMA * A_cold * (pow(T_hot,4) - pow(T_cold,4));
    std::cout << "1. Q_rad   = " << std::fixed << std::setprecision(3) << Q_rad << " W\n";
    
    // 2. Теплопроводность по штоку (ТОНКОСТЕННЫЙ)
    const double L_shaft = 0.050;
    const double D_shaft_out = 0.035;
    const double D_shaft_in  = 0.033;   // СТЕНКА 1 мм
    const double A_shaft = PI/4.0 * (D_shaft_out*D_shaft_out - D_shaft_in*D_shaft_in);
    const double k_316L_40K = 8.0;
    const double dT_shaft = 80.0 - 40.0;
    double Q_shaft = k_316L_40K * A_shaft * dT_shaft / L_shaft;
    std::cout << "2. Q_shaft = " << Q_shaft << " W (thin wall: 1 mm)\n";
    
    // 3. Провода (УМЕНЬШЕНО)
    const int N_wires = 6;
    const double D_wire = 0.1e-3;
    const double A_wire = PI/4.0 * D_wire*D_wire;
    const double L_wire = 0.080;
    const double k_Cu_40K = 500.0;
    double Q_wires = N_wires * k_Cu_40K * A_wire * (T_hot - T_cold) / L_wire;
    std::cout << "3. Q_wires = " << Q_wires << " W\n";
    
    // 4. Демпфер
    double Q_damper = 3.0 * 1.5*1.5 * 0.2 * 0.05;
    std::cout << "4. Q_damper = " << Q_damper << " W\n";
    
    // 5. Остаточный газ
    double P_vac = 1.0e-6;
    double alpha = 0.5, gamma = 1.4;
    double Q_gas = alpha * (gamma+1)/(gamma-1) * 
                   sqrt(1.38e-23/(28.0*1.66e-27*PI)) * P_vac * A_cold * (T_hot - T_cold);
    std::cout << "5. Q_gas   = " << Q_gas << " W\n";
    
    // Итого
    double Q_total = Q_rad + Q_shaft + Q_wires + Q_damper + Q_gas;
    double Q_cooler = 2.0;
    double margin = (Q_cooler - Q_total) / Q_cooler * 100.0;
    
    std::cout << "\n==========================================\n";
    std::cout << " Q_total  = " << std::setw(8) << Q_total << " W\n";
    std::cout << " Q_cooler = " << std::setw(8) << Q_cooler << " W\n";
    std::cout << " Margin   = " << std::setw(8) << std::setprecision(1) << margin << "%\n";
    std::cout << "==========================================\n";
    std::cout << " >>> " << (margin > 20 ? "PASS" : "FAIL") << " <<<\n";
    
    return 0;
}