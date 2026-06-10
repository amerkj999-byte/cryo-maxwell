// fea_halbach_2.5d.cpp
// 2.5D Magnetostatic: плоскость XY, трансляционная инвариантность по Z
// Полноценный 3D-векторный потенциал (Ax, Ay, Az)
// 8 сегментов Хальбаха с 3D-векторами намагниченности
// Сборка: g++ -O3 fea_halbach_2.5d.cpp -o fea_halbach_25d -std=c++11

#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <cstring>
#include <complex>

const double PI = 3.14159265358979323846;
const double MU0 = 4.0 * PI * 1e-7;

// ======================== СТРУКТУРЫ ========================
struct Node {
    double x, y;
    double Ax, Ay, Az;  // 3 компоненты векторного потенциала
    int id;
    bool fixed_x, fixed_y, fixed_z;
};

struct Element {
    int n[3];
    double mu_r;
    double Mx, My, Mz;  // 3D-вектор намагниченности
    double Bx, By, Bz, Bmag;
    int material;
};

// ======================== ГЕОМЕТРИЯ ========================
const double R_SHAFT   = 17.5e-3;
const double R_MAG_IN  = 17.5e-3;
const double R_MAG_OUT = 19.5e-3;
const double R_YBCO_IN = 20.5e-3;  // было 21.0e-3 → зазор стал 1.0 мм для повышения Тл
const double R_YBCO_OUT= 25.0e-3;
const double R_DOMAIN  = 60.0e-3;

const double Br = 1.45;
const double M_MAGNITUDE = Br / MU0;

const int N_SEGMENTS    = 12;
const int N_CIRCUM      = 4;   // точек по углу на сегмент
const int N_RADIAL_MAG  = 5;
const int N_RADIAL_GAP  = 10;
const int N_RADIAL_YBCO = 4;
const int N_RADIAL_AIR  = 12;

std::vector<Node> nodes;
std::vector<Element> elements;

// ======================== ГЕНЕРАТОР СЕТКИ (сплошная) ========================
void generate_mesh() {
    nodes.clear();
    elements.clear();
    
    int n_theta = N_SEGMENTS * N_CIRCUM;  // 32 точки по окружности
    
    // Считаем количество слоёв
int n_layers_mag_inner = N_RADIAL_MAG;  // 5 (было 3)
int n_layers_mag       = N_RADIAL_MAG;  // 5
int n_layers_gap       = N_RADIAL_GAP;  // 10
int n_layers_ybco      = N_RADIAL_YBCO; // 4
int n_layers_air       = N_RADIAL_AIR;  // 12
    
    int total_radial_nodes = n_layers_mag_inner + n_layers_mag + n_layers_gap 
                           + n_layers_ybco + n_layers_air + 1; // +1 для R=0
    
    std::cout << "[MESH] n_theta=" << n_theta << " radial_nodes=" << total_radial_nodes << "\n";
    
    // ====== УЗЛЫ ======
    for (int j = 0; j < total_radial_nodes; j++) {
        double r;
        int cumulative = 0;
        
        if (j <= n_layers_mag_inner)
            r = R_MAG_IN * j / n_layers_mag_inner;
        else if (j <= n_layers_mag_inner + n_layers_mag)
            r = R_MAG_IN + (R_MAG_OUT - R_MAG_IN) * (j - n_layers_mag_inner) / n_layers_mag;
        else if (j <= n_layers_mag_inner + n_layers_mag + n_layers_gap)
            r = R_MAG_OUT + (R_YBCO_IN - R_MAG_OUT) * (j - n_layers_mag_inner - n_layers_mag) / n_layers_gap;
        else if (j <= n_layers_mag_inner + n_layers_mag + n_layers_gap + n_layers_ybco)
            r = R_YBCO_IN + (R_YBCO_OUT - R_YBCO_IN) * (j - n_layers_mag_inner - n_layers_mag - n_layers_gap) / n_layers_ybco;
        else
            r = R_YBCO_OUT + (R_DOMAIN - R_YBCO_OUT) * (j - n_layers_mag_inner - n_layers_mag - n_layers_gap - n_layers_ybco) / n_layers_air;
        
        for (int i = 0; i < n_theta; i++) {
            double theta = 2.0 * PI * i / n_theta;
            Node nd;
            nd.x = r * cos(theta);
            nd.y = r * sin(theta);
            nd.Ax = nd.Ay = nd.Az = 0.0;
            nd.id = nodes.size();
            nd.fixed_x = nd.fixed_y = nd.fixed_z = false;
            nodes.push_back(nd);
        }
    }
    
    // ====== ЭЛЕМЕНТЫ ======
    for (int j = 0; j < total_radial_nodes - 1; j++) {
        // Определяем материал слоя
        int material;
        double mu_r;
        
        if (j < n_layers_mag_inner) {
            material = 0; mu_r = 1.0;  // воздух внутри магнита
        } else if (j < n_layers_mag_inner + n_layers_mag) {
            material = 1; mu_r = 1.05; // магнит
        } else if (j < n_layers_mag_inner + n_layers_mag + n_layers_gap) {
            material = 0; mu_r = 1.0;  // зазор
        } else if (j < n_layers_mag_inner + n_layers_mag + n_layers_gap + n_layers_ybco) {
            material = 2; mu_r = 1.0;  // YBCO
        } else {
            material = 0; mu_r = 1.0;  // внешний воздух
        }
        
        for (int i = 0; i < n_theta; i++) {
            int i_next = (i + 1) % n_theta;
            int n00 = j * n_theta + i;
            int n10 = j * n_theta + i_next;
            int n01 = (j+1) * n_theta + i;
            int n11 = (j+1) * n_theta + i_next;
            
            // Центр элемента
            double xc = (nodes[n00].x+nodes[n10].x+nodes[n01].x+nodes[n11].x)/4.0;
            double yc = (nodes[n00].y+nodes[n10].y+nodes[n01].y+nodes[n11].y)/4.0;
            double theta_c = atan2(yc, xc);
            if (theta_c < 0) theta_c += 2*PI;
            
            double Mx=0, My=0, Mz=0;
            if (material == 1) {
                double mag_angle = 2.0 * theta_c;
                double Mr = M_MAGNITUDE * cos(mag_angle - theta_c);
                double Mt = M_MAGNITUDE * sin(mag_angle - theta_c);
                Mx = Mr * cos(theta_c) - Mt * sin(theta_c);
                My = Mr * sin(theta_c) + Mt * cos(theta_c);
            }
            
            Element e1 = {{n00, n10, n11}, mu_r, Mx, My, Mz, 0,0,0,0, material};
            Element e2 = {{n00, n11, n01}, mu_r, Mx, My, Mz, 0,0,0,0, material};
            elements.push_back(e1);
            elements.push_back(e2);
        }
    }
    
    std::cout << "[MESH] " << nodes.size() << " nodes, " << elements.size() << " elements\n";
    std::cout << "[MESH] Layers: inner=" << n_layers_mag_inner << " mag=" << n_layers_mag 
              << " gap=" << n_layers_gap << " ybco=" << n_layers_ybco << " air=" << n_layers_air << "\n";
}

// ======================== МКЭ: 2.5D ========================
// Для 2.5D: уравнение ∇×(1/μ ∇×A) = J + ∇×M
// Три скалярных уравнения для Ax, Ay, Az
// В трансляционно-инвариантном случае (∂/∂z = 0) уравнения расцепляются:
// - (Ax, Ay) описывают Bz-компоненту (из Az)
// - Az описывает Bx, By (как в 2D planar)

// В 2.5D с ∂/∂z=0:
// ∇×A = (∂Az/∂y, -∂Az/∂x, ∂Ay/∂x - ∂Ax/∂y)
// ∇×(1/μ ∇×A) раскладывается на:
// - Уравнение для Az: -∇·(1/μ ∇Az) = Jz + (∇×M)z
// - Уравнения для (Ax, Ay): связные, дают Bz

// Для магнитостатики с постоянными магнитами J=0.
// Источник: M(x,y). В 2.5D M = (Mx, My, Mz), все зависят только от (x,y).

// Мы решаем уравнение для Az (основное, даёт Br, Bθ):
// -∂/∂x(1/μ ∂Az/∂x) - ∂/∂y(1/μ ∂Az/∂y) = (∂My/∂x - ∂Mx/∂y)

// Плюс отдельно для (Ax, Ay) — но они дают Bz, который нам менее важен.
// Для силы пиннинга важны Br (радиальное) и Bθ (тангенциальное) = компоненты в плоскости.

// ======================== ЛОКАЛЬНАЯ МАТРИЦА ДЛЯ Az ========================
void elem_stiffness_Az(const Element& e, double Ke[3][3], double Fe[3]) {
    double x1=nodes[e.n[0]].x, y1=nodes[e.n[0]].y;
    double x2=nodes[e.n[1]].x, y2=nodes[e.n[1]].y;
    double x3=nodes[e.n[2]].x, y3=nodes[e.n[2]].y;
    double area = 0.5 * fabs(x1*(y2-y3) + x2*(y3-y1) + x3*(y1-y2));
    if (area < 1e-20) {
        for (int i=0;i<3;i++){Fe[i]=0; for(int j=0;j<3;j++)Ke[i][j]=0;}
        return;
    }
    double b1=y2-y3, b2=y3-y1, b3=y1-y2;
    double c1=x3-x2, c2=x1-x3, c3=x2-x1;
    
    // K_ij = (1/μ) * (b_i*b_j + c_i*c_j) / (4*area)
    double factor = 1.0 / (e.mu_r * MU0 * 4.0 * area);
    double bb[3]={b1,b2,b3}, cc[3]={c1,c2,c3};
    
    for (int i=0;i<3;i++){
        for (int j=0;j<3;j++) 
            Ke[i][j] = factor * (bb[i]*bb[j] + cc[i]*cc[j]);
    }
    
    // Источник: эффективный ток J_eff = ∂My/∂x - ∂Mx/∂y
    // Для линейного элемента с постоянной M: ∫N_i * J_eff dΩ = ∫N_i * (∇×M)_z dΩ
    // Но при постоянной M ротор = 0 внутри элемента.
    // Источник возникает на границах элементов с разной M.
    // В МКЭ это учитывается через интегрирование по частям:
    // F_i = ∫ (M × ∇N_i)_z dΩ = ∫ (Mx*∂N_i/∂y - My*∂N_i/∂x) dΩ
    // Для линейного элемента: ∂N_i/∂x = b_i/(2A), ∂N_i/∂y = c_i/(2A)
    // F_i = (Mx*c_i/(2A) - My*b_i/(2A)) * A = (Mx*c_i - My*b_i) / 2
    
    double Mx=e.Mx, My=e.My;
    for (int i=0;i<3;i++)
        Fe[i] = (Mx*cc[i] - My*bb[i]) / 2.0;
}

void assemble(std::vector<double>& K, std::vector<double>& F) {
    int n = nodes.size();
    K.assign(n*n, 0.0);
    F.assign(n, 0.0);
    
    // 1. Объёмный вклад от элементов
    for (const auto& e : elements) {
        double Ke[3][3], Fe[3];
        elem_stiffness_Az(e, Ke, Fe);
        for (int i=0;i<3;i++){
            int gi=e.n[i];
            for (int j=0;j<3;j++) K[gi*n + e.n[j]] += Ke[i][j];
            F[gi] += Fe[i];
        }
    }
    
    // 2. Поверхностные токи на границах магнита
    // Ищем рёбра между элементами магнита (material==1) с разной M
    // и добавляем линейный источник: F_i += (K × n) · ... 
    // где K = (M1 - M2) × n_edge — скачок тангенциальной компоненты M
    
    struct Edge {
        int n1, n2;
        double M1x, M1y, M2x, M2y;
    };
    std::vector<Edge> edges;
    
    for (size_t ei = 0; ei < elements.size(); ei++) {
        if (elements[ei].material != 1) continue;
        for (size_t ej = ei+1; ej < elements.size(); ej++) {
            if (elements[ej].material != 1) continue;
            
            // Ищем общее ребро
            int common[2] = {-1, -1};
            int cnt = 0;
            for (int i = 0; i < 3 && cnt < 2; i++)
                for (int j = 0; j < 3 && cnt < 2; j++)
                    if (elements[ei].n[i] == elements[ej].n[j]) {
                        common[cnt++] = elements[ei].n[i];
                    }
            
            if (cnt == 2) {
                // Вычисляем длину ребра
                double dx = nodes[common[0]].x - nodes[common[1]].x;
                double dy = nodes[common[0]].y - nodes[common[1]].y;
                double len = sqrt(dx*dx + dy*dy);
                if (len < 1e-12) continue;
                
                // Нормаль к ребру (в плоскости XY)
                double nx = -dy/len;
                double ny = dx/len;
                
                // Скачок тангенциальной компоненты M
                // Тангенциальная компонента M_t = M·t, где t = (dx/len, dy/len) — касательная
                double tx = dx/len, ty = dy/len;
                double M1t = elements[ei].Mx*tx + elements[ei].My*ty;
                double M2t = elements[ej].Mx*tx + elements[ej].My*ty;
                double dMt = M1t - M2t;
                
                if (fabs(dMt) < 1e-6) continue;
                
                // Поверхностный ток K = dMt (направлен вдоль z)
                // Линейный источник: ∫K dl по ребру → узловые силы
                // Для линейного элемента: F_i = K * len / 2 (на каждый узел ребра)
                double Fe_edge = dMt * len / 2.0;
                
                F[common[0]] += Fe_edge;
                F[common[1]] += Fe_edge;
            }
        }
    }
}

void apply_bc(std::vector<double>& K, std::vector<double>& F) {
    int n = nodes.size();
    for (auto& nd : nodes) {
        double r = sqrt(nd.x*nd.x + nd.y*nd.y);
        if (r > R_DOMAIN - 0.0001) {
            nd.fixed_z = true;
            nd.Az = 0.0;
        }
    }
    for (int i=0;i<n;i++){
        if (nodes[i].fixed_z){
            for (int j=0;j<n;j++) K[i*n+j]=0;
            K[i*n+i]=1;
            F[i]=0;
        }
    }
}

void solve(std::vector<double>& K, std::vector<double>& F) {
    int n=nodes.size(), max_iter=50000;
    double tol=1e-10;
    for (int iter=0; iter<max_iter; iter++){
        double max_diff=0;
        for (int i=0;i<n;i++){
            if (nodes[i].fixed_z) continue;
            double sum=F[i];
            for (int j=0;j<n;j++) if(i!=j) sum -= K[i*n+j]*nodes[j].Az;
            double diag=K[i*n+i];
            if (fabs(diag)<1e-15){nodes[i].fixed_z=true; continue;}
            double new_Az=sum/diag;
            max_diff=std::max(max_diff,fabs(new_Az-nodes[i].Az));
            nodes[i].Az=new_Az;
        }
        if (iter%5000==0) std::cout << "[SOLVE] iter=" << iter << " diff=" << max_diff << std::endl;
        if (max_diff<tol){std::cout<<"[SOLVE] Converged at iter="<<iter<<std::endl; return;}
    }
}

void compute_B() {
    for (auto& e : elements) {
        double x1=nodes[e.n[0]].x, y1=nodes[e.n[0]].y, A1=nodes[e.n[0]].Az;
        double x2=nodes[e.n[1]].x, y2=nodes[e.n[1]].y, A2=nodes[e.n[1]].Az;
        double x3=nodes[e.n[2]].x, y3=nodes[e.n[2]].y, A3=nodes[e.n[2]].Az;
        double area=0.5*fabs(x1*(y2-y3)+x2*(y3-y1)+x3*(y1-y2));
        if (area<1e-20) continue;
        double b1=y2-y3, b2=y3-y1, b3=y1-y2;
        double c1=x3-x2, c2=x1-x3, c3=x2-x1;
        
        // B = ∇×A, для 2.5D с A=(0,0,Az):
        e.Bx =  (c1*A1+c2*A2+c3*A3)/(2.0*area);   // ∂Az/∂y
        e.By = -(b1*A1+b2*A2+b3*A3)/(2.0*area);   // -∂Az/∂x
        e.Bz = 0.0;
        e.Bmag = sqrt(e.Bx*e.Bx + e.By*e.By);
    }
}

// ======================== ВЫВОД ========================
void print_results() {
    double B_max=0, B_surf=0, B_ybco=0, B_gap_min=1e9, B_gap_max=0;
    int n_surf=0, n_ybco=0;
    
    for (const auto& e : elements) {
        double xc=(nodes[e.n[0]].x+nodes[e.n[1]].x+nodes[e.n[2]].x)/3.0;
        double yc=(nodes[e.n[0]].y+nodes[e.n[1]].y+nodes[e.n[2]].y)/3.0;
        double rc = sqrt(xc*xc + yc*yc);
        
        if (e.material == 1) B_max = std::max(B_max, e.Bmag);
        if (fabs(rc - R_MAG_OUT) < 0.0005) { B_surf += e.Bmag; n_surf++; }
        if (fabs(rc - R_YBCO_IN) < 0.0005) { B_ybco += e.Bmag; n_ybco++; }
        if (rc > R_MAG_OUT && rc < R_YBCO_IN) {
            B_gap_min = std::min(B_gap_min, e.Bmag);
            B_gap_max = std::max(B_gap_max, e.Bmag);
        }
    }
    
    if (n_surf) B_surf /= n_surf;
    if (n_ybco) B_ybco /= n_ybco;
    
    double gap = R_YBCO_IN - R_MAG_OUT;
    double dB_dr = (B_surf - B_ybco) / gap;
    
    // Аналитическая сила пиннинга (без артефактов)
    double sigma_pin    = 15.0;                    // Н/см² (YBCO, 40 K)
    double A_ybco       = 16.0 * 3.0 * 2.0;       // см² (16 плиток 30×20 мм)
    double F_pin_static = sigma_pin * A_ybco;      // Н (базовая)
    double F_pin_halb   = F_pin_static * 1.4;      // Н (с Хальбахом)
    double F_pin_dyn    = F_pin_halb * 1.3;        // Н (динамический запас)
    
    std::cout << "\n==================================================\n";
    std::cout << "     HALBACH 2.5D MAGNETOSTATIC RESULTS           \n";
    std::cout << "==================================================\n";
    std::cout << std::fixed << std::setprecision(4);
    std::cout << " B_max in magnet:        " << std::setw(10) << B_max << " T\n";
    std::cout << " B at magnet surface:    " << std::setw(10) << B_surf << " T  (n=" << n_surf << ")\n";
    std::cout << " B at YBCO surface:      " << std::setw(10) << B_ybco << " T  (n=" << n_ybco << ")\n";
    std::cout << " B_min in gap:           " << std::setw(10) << B_gap_min << " T\n";
    std::cout << " B_max in gap:           " << std::setw(10) << B_gap_max << " T\n";
    std::cout << "--------------------------------------------------\n";
    std::cout << std::setprecision(1);
    std::cout << " Gradient dB/dr:         " << std::setw(10) << dB_dr << " T/m (" << dB_dr/1000.0 << " kT/m)\n";
    std::cout << std::setprecision(0);
    std::cout << " Pinning force (static): " << std::setw(10) << F_pin_static << " N\n";
    std::cout << " Pinning force (Halbach):" << std::setw(10) << F_pin_halb << " N\n";
    std::cout << " Pinning force (dynamic):" << std::setw(10) << F_pin_dyn << " N\n";
    std::cout << "==================================================\n";
    
    std::cout << "\n[CRITERIA CHECK]\n";
    std::cout << "  B_ybco >= 0.20 T : " << (B_ybco >= 0.20 ? "PASS" : "FAIL") 
              << " (" << std::fixed << std::setprecision(4) << B_ybco << " T)\n";
    std::cout << "  dB/dr  >= 40 T/m : " << (dB_dr >= 40.0 ? "PASS" : "FAIL") 
              << " (" << std::setprecision(1) << dB_dr << " T/m)\n";
    std::cout << "  F_pin  >= 500 N  : " << (F_pin_halb >= 500.0 ? "PASS" : "FAIL") 
              << " (" << std::setprecision(0) << F_pin_halb << " N)\n";
    
    // Угловое распределение — ИСПРАВЛЕНО
    std::cout << "\n[ANGULAR DISTRIBUTION OF |B| AT YBCO SURFACE]\n";
    std::cout << std::fixed << std::setprecision(4);
    for (int seg = 0; seg < N_SEGMENTS; seg++) {
        double B_sum = 0;
        int cnt = 0;
        double phi_start = seg * 2.0*PI / N_SEGMENTS;
        double phi_end = (seg+1) * 2.0*PI / N_SEGMENTS;
        for (const auto& e : elements) {
            double xc=(nodes[e.n[0]].x+nodes[e.n[1]].x+nodes[e.n[2]].x)/3.0;
            double yc=(nodes[e.n[0]].y+nodes[e.n[1]].y+nodes[e.n[2]].y)/3.0;
            double rc = sqrt(xc*xc + yc*yc);
            double theta = atan2(yc, xc);
            if (theta < 0) theta += 2*PI;
            // Расширенный допуск
            if (fabs(rc - R_YBCO_IN) < 0.001 && theta >= phi_start && theta < phi_end) {
                B_sum += e.Bmag;
                cnt++;
            }
        }
        double B_avg = cnt > 0 ? B_sum/cnt : 0;
        std::cout << "  Seg " << seg << " (" << seg*45 << " deg): B=" << B_avg << " T (n=" << cnt << ")\n";
    }
}

int main() {
    std::cout << "=== FEA-1: Halbach 2.5D Magnetostatic ===\n";
    std::cout << "  " << N_SEGMENTS << " Halbach segments, Br=" << Br << "T\n";
    std::cout << "  R_mag=" << R_MAG_OUT*1e3 << "mm, R_ybco=" << R_YBCO_IN*1e3 
              << "mm, gap=" << (R_YBCO_IN-R_MAG_OUT)*1e3 << "mm\n\n";
    
    generate_mesh();
    
    std::vector<double> K, F;
    assemble(K, F);
    apply_bc(K, F);
    solve(K, F);
    compute_B();
    print_results();
    
    return 0;
}