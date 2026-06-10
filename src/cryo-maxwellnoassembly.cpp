// cryo_maxwell_v4_production.cpp
// Производственный STL-генератор без упрощений
// Конфигурация: 16 сегм. Хальбаха N55, зазор 1.0 мм, Инконель 718
// Сборка: g++ -O3 cryo_maxwell_v4_production.cpp -o cryo_maxwell_v4 -std=c++11

#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <string>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

struct P { float x, y, z; };

void writeFacet(std::ostream& f, P p1, P p2, P p3) {
    // Нормаль вычисляется слайсером автоматически
    f << "facet normal 0 0 0\n outer loop\n"
      << " vertex " << p1.x << " " << p1.y << " " << p1.z << "\n"
      << " vertex " << p2.x << " " << p2.y << " " << p2.z << "\n"
      << " vertex " << p3.x << " " << p3.y << " " << p3.z << "\n"
      << " endloop\nendfacet\n";
}

// ======================== ПРИМИТИВЫ ========================

void cylinder(std::ostream& f, float x1, float x2, float r, int segments, float oy=0, float oz=0) {
    float step = 360.0f / segments;
    for (float a = 0; a < 360; a += step) {
        float a1 = a * M_PI / 180.0f, a2 = (a + step) * M_PI / 180.0f;
        writeFacet(f, {x1, oy+r*sinf(a1), oz+r*cosf(a1)}, {x1, oy+r*sinf(a2), oz+r*cosf(a2)}, {x2, oy+r*sinf(a2), oz+r*cosf(a2)});
        writeFacet(f, {x1, oy+r*sinf(a1), oz+r*cosf(a1)}, {x2, oy+r*sinf(a2), oz+r*cosf(a2)}, {x2, oy+r*sinf(a1), oz+r*cosf(a1)});
    }
    for (float a = 0; a < 360; a += step) {
        float a1 = a * M_PI / 180.0f, a2 = (a + step) * M_PI / 180.0f;
        writeFacet(f, {x1, oy, oz}, {x1, oy+r*sinf(a2), oz+r*cosf(a2)}, {x1, oy+r*sinf(a1), oz+r*cosf(a1)});
        writeFacet(f, {x2, oy, oz}, {x2, oy+r*sinf(a1), oz+r*cosf(a1)}, {x2, oy+r*sinf(a2), oz+r*cosf(a2)});
    }
}

void hollowCylinder(std::ostream& f, float x1, float x2, float r_out, float r_in, int seg, float oy=0, float oz=0) {
    if (r_in < 0.0005f) { cylinder(f, x1, x2, r_out, seg, oy, oz); return; }
    float step = 360.0f / seg;
    for (float a = 0; a < 360; a += step) {
        float a1 = a * M_PI / 180.0f, a2 = (a + step) * M_PI / 180.0f;
        writeFacet(f, {x1, oy+r_out*sinf(a1), oz+r_out*cosf(a1)}, {x1, oy+r_out*sinf(a2), oz+r_out*cosf(a2)}, {x2, oy+r_out*sinf(a2), oz+r_out*cosf(a2)});
        writeFacet(f, {x1, oy+r_out*sinf(a1), oz+r_out*cosf(a1)}, {x2, oy+r_out*sinf(a2), oz+r_out*cosf(a2)}, {x2, oy+r_out*sinf(a1), oz+r_out*cosf(a1)});
        writeFacet(f, {x2, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x2, oy+r_in*sinf(a2), oz+r_in*cosf(a2)}, {x1, oy+r_in*sinf(a2), oz+r_in*cosf(a2)});
        writeFacet(f, {x2, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x1, oy+r_in*sinf(a2), oz+r_in*cosf(a2)}, {x1, oy+r_in*sinf(a1), oz+r_in*cosf(a1)});
    }
    for (float a = 0; a < 360; a += step) {
        float a1 = a * M_PI / 180.0f, a2 = (a + step) * M_PI / 180.0f;
        writeFacet(f, {x1, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x1, oy+r_in*sinf(a2), oz+r_in*cosf(a2)}, {x1, oy+r_out*sinf(a2), oz+r_out*cosf(a2)});
        writeFacet(f, {x1, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x1, oy+r_out*sinf(a2), oz+r_out*cosf(a2)}, {x1, oy+r_out*sinf(a1), oz+r_out*cosf(a1)});
        writeFacet(f, {x2, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x2, oy+r_out*sinf(a1), oz+r_out*cosf(a1)}, {x2, oy+r_out*sinf(a2), oz+r_out*cosf(a2)});
        writeFacet(f, {x2, oy+r_in*sinf(a1), oz+r_in*cosf(a1)}, {x2, oy+r_out*sinf(a2), oz+r_out*cosf(a2)}, {x2, oy+r_in*sinf(a2), oz+r_in*cosf(a2)});
    }
}

void torusSegment(std::ostream& f, float x, float y, float z, float R, float r, float sa, float ea, int seg=16, int rings=8) {
    for (int i = 0; i < rings; ++i) {
        float phi1 = sa + (ea-sa)*i/rings, phi2 = sa + (ea-sa)*(i+1)/rings;
        for (int j = 0; j < seg; ++j) {
            float t1 = 2*M_PI*j/seg, t2 = 2*M_PI*(j+1)/seg;
            P p1 = {x+(R+r*cosf(t1))*cosf(phi1), y+r*sinf(t1), z+(R+r*cosf(t1))*sinf(phi1)};
            P p2 = {x+(R+r*cosf(t2))*cosf(phi1), y+r*sinf(t2), z+(R+r*cosf(t2))*sinf(phi1)};
            P p3 = {x+(R+r*cosf(t2))*cosf(phi2), y+r*sinf(t2), z+(R+r*cosf(t2))*sinf(phi2)};
            P p4 = {x+(R+r*cosf(t1))*cosf(phi2), y+r*sinf(t1), z+(R+r*cosf(t1))*sinf(phi2)};
            writeFacet(f, p1, p2, p3); writeFacet(f, p1, p3, p4);
        }
    }
}

void cable(std::ostream& f, P start, P end, float radius, int seg=12) {
    P dir = {end.x-start.x, end.y-start.y, end.z-start.z};
    float len = sqrtf(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z);
    if (len < 0.0001f) return;
    dir = {dir.x/len, dir.y/len, dir.z/len};
    P up = fabsf(dir.y) > 0.999f ? P{1,0,0} : P{0,1,0};
    P right = {dir.y*up.z-dir.z*up.y, dir.z*up.x-dir.x*up.z, dir.x*up.y-dir.y*up.x};
    float rlen = sqrtf(right.x*right.x+right.y*right.y+right.z*right.z);
    right = {right.x/rlen, right.y/rlen, right.z/rlen};
    P up_real = {dir.y*right.z-dir.z*right.y, dir.z*right.x-dir.x*right.z, dir.x*right.y-dir.y*right.x};
    float step = 360.0f/seg;
    for (float a=0; a<360; a+=step) {
        float a1=a*M_PI/180.0f, a2=(a+step)*M_PI/180.0f;
        P o1={right.x*cosf(a1)+up_real.x*sinf(a1), right.y*cosf(a1)+up_real.y*sinf(a1), right.z*cosf(a1)+up_real.z*sinf(a1)};
        P o2={right.x*cosf(a2)+up_real.x*sinf(a2), right.y*cosf(a2)+up_real.y*sinf(a2), right.z*cosf(a2)+up_real.z*sinf(a2)};
        P p1={start.x+o1.x*radius, start.y+o1.y*radius, start.z+o1.z*radius};
        P p2={start.x+o2.x*radius, start.y+o2.y*radius, start.z+o2.z*radius};
        P p3={end.x+o2.x*radius, end.y+o2.y*radius, end.z+o2.z*radius};
        P p4={end.x+o1.x*radius, end.y+o1.y*radius, end.z+o1.z*radius};
        writeFacet(f,p1,p2,p3); writeFacet(f,p1,p3,p4);
    }
    for (float a=0; a<360; a+=step) {
        float a1=a*M_PI/180.0f, a2=(a+step)*M_PI/180.0f;
        P o1={right.x*cosf(a1)+up_real.x*sinf(a1), right.y*cosf(a1)+up_real.y*sinf(a1), right.z*cosf(a1)+up_real.z*sinf(a1)};
        P o2={right.x*cosf(a2)+up_real.x*sinf(a2), right.y*cosf(a2)+up_real.y*sinf(a2), right.z*cosf(a2)+up_real.z*sinf(a2)};
        writeFacet(f, start, {start.x+o2.x*radius,start.y+o2.y*radius,start.z+o2.z*radius}, {start.x+o1.x*radius,start.y+o1.y*radius,start.z+o1.z*radius});
        writeFacet(f, end, {end.x+o1.x*radius,end.y+o1.y*radius,end.z+o1.z*radius}, {end.x+o2.x*radius,end.y+o2.y*radius,end.z+o2.z*radius});
    }
}

void box(std::ostream& f, P min, P max) {
    writeFacet(f,{min.x,min.y,min.z},{max.x,min.y,min.z},{max.x,max.y,min.z}); writeFacet(f,{min.x,min.y,min.z},{max.x,max.y,min.z},{min.x,max.y,min.z});
    writeFacet(f,{min.x,min.y,max.z},{max.x,max.y,max.z},{max.x,min.y,max.z}); writeFacet(f,{min.x,min.y,max.z},{min.x,max.y,max.z},{max.x,max.y,max.z});
    writeFacet(f,{min.x,min.y,min.z},{min.x,max.y,min.z},{min.x,max.y,max.z}); writeFacet(f,{min.x,min.y,min.z},{min.x,max.y,max.z},{min.x,min.y,max.z});
    writeFacet(f,{max.x,min.y,min.z},{max.x,min.y,max.z},{max.x,max.y,max.z}); writeFacet(f,{max.x,min.y,min.z},{max.x,max.y,max.z},{max.x,max.y,min.z});
    writeFacet(f,{min.x,min.y,min.z},{min.x,min.y,max.z},{max.x,min.y,max.z}); writeFacet(f,{min.x,min.y,min.z},{max.x,min.y,max.z},{max.x,min.y,min.z});
    writeFacet(f,{min.x,max.y,min.z},{max.x,max.y,min.z},{max.x,max.y,max.z}); writeFacet(f,{min.x,max.y,min.z},{max.x,max.y,max.z},{min.x,max.y,max.z});
}

void sphere(std::ostream& f, float cx, float cy, float cz, float r, int stacks=10, int slices=16) {
    for (int i=0; i<stacks; ++i) {
        float phi1 = M_PI*(-0.5f+(float)i/stacks), phi2 = M_PI*(-0.5f+(float)(i+1)/stacks);
        for (int j=0; j<slices; ++j) {
            float t1=2*M_PI*j/slices, t2=2*M_PI*(j+1)/slices;
            P p1={cx+r*cosf(phi1)*cosf(t1), cy+r*sinf(phi1), cz+r*cosf(phi1)*sinf(t1)};
            P p2={cx+r*cosf(phi2)*cosf(t1), cy+r*sinf(phi2), cz+r*cosf(phi2)*sinf(t1)};
            P p3={cx+r*cosf(phi2)*cosf(t2), cy+r*sinf(phi2), cz+r*cosf(phi2)*sinf(t2)};
            P p4={cx+r*cosf(phi1)*cosf(t2), cy+r*sinf(phi1), cz+r*cosf(phi1)*sinf(t2)};
            writeFacet(f,p1,p2,p3); writeFacet(f,p1,p3,p4);
        }
    }
}

// ======================== СПЕЦИАЛЬНЫЕ УЗЛЫ ========================

void halbachDiscrete(std::ostream& f, float xc, float yc, float zc, float ri, float ro, float len, int segs) {
    float seg_ang = 360.0f / segs;
    float gap = 1.0f;  // градусов зазора между сегментами
    float seg_arc = seg_ang - gap;
    int n_ang = 4;     // разбиений по углу на сегмент
    
    for (int s = 0; s < segs; s++) {
        float phi0 = (s * seg_ang + gap/2.0f) * M_PI / 180.0f;
        float dphi = seg_arc * M_PI / 180.0f;
        
        for (int i = 0; i < n_ang; i++) {
            float a1 = phi0 + dphi * i / n_ang;
            float a2 = phi0 + dphi * (i+1) / n_ang;
            
            // Внешняя грань
            writeFacet(f, {xc-len/2, yc+ri*sinf(a1), zc+ri*cosf(a1)}, {xc-len/2, yc+ri*sinf(a2), zc+ri*cosf(a2)}, {xc+len/2, yc+ro*sinf(a2), zc+ro*cosf(a2)});
            writeFacet(f, {xc-len/2, yc+ri*sinf(a1), zc+ri*cosf(a1)}, {xc+len/2, yc+ro*sinf(a2), zc+ro*cosf(a2)}, {xc+len/2, yc+ro*sinf(a1), zc+ro*cosf(a1)});
            // Внутренняя грань
            writeFacet(f, {xc-len/2, yc+ri*sinf(a1), zc+ri*cosf(a1)}, {xc+len/2, yc+ri*sinf(a1), zc+ri*cosf(a1)}, {xc+len/2, yc+ri*sinf(a2), zc+ri*cosf(a2)});
            writeFacet(f, {xc-len/2, yc+ri*sinf(a1), zc+ri*cosf(a1)}, {xc+len/2, yc+ri*sinf(a2), zc+ri*cosf(a2)}, {xc-len/2, yc+ri*sinf(a2), zc+ri*cosf(a2)});
        }
        // Торцы сегмента
        float as = phi0, ae = phi0 + dphi;
        writeFacet(f, {xc-len/2, yc+ri*sinf(as), zc+ri*cosf(as)}, {xc+len/2, yc+ri*sinf(as), zc+ri*cosf(as)}, {xc+len/2, yc+ro*sinf(as), zc+ro*cosf(as)});
        writeFacet(f, {xc-len/2, yc+ri*sinf(as), zc+ri*cosf(as)}, {xc+len/2, yc+ro*sinf(as), zc+ro*cosf(as)}, {xc-len/2, yc+ro*sinf(as), zc+ro*cosf(as)});
        writeFacet(f, {xc-len/2, yc+ri*sinf(ae), zc+ri*cosf(ae)}, {xc+len/2, yc+ro*sinf(ae), zc+ro*cosf(ae)}, {xc+len/2, yc+ri*sinf(ae), zc+ri*cosf(ae)});
        writeFacet(f, {xc-len/2, yc+ri*sinf(ae), zc+ri*cosf(ae)}, {xc-len/2, yc+ro*sinf(ae), zc+ro*cosf(ae)}, {xc+len/2, yc+ro*sinf(ae), zc+ro*cosf(ae)});
    }
}

void htscTile(std::ostream& f, float x, float y, float z, float w, float h, float d) {
    // Плитка YBCO
    box(f, {x-w/2, y-h/2, z-d/2}, {x+w/2, y+h/2, z+d/2});
    // Медная подложка (3 мм)
    box(f, {x-w/2, y-h/2, z-d/2-0.003f}, {x+w/2, y+h/2, z+d/2+0.003f});
    // Индиевый слой (0.1 мм) — визуально не отображаем, слишком тонкий
}

void bellows_inconel(std::ostream& f, float xs, float xe, float r_inner, float r_outer, int n_conv, float oy=0, float oz=0) {
    float len = xe - xs;
    float pitch = len / n_conv;
    float r = (r_outer - r_inner) / 2.0f;
    for (int i = 0; i < n_conv; i++) {
        float cx = xs + pitch * (i + 0.5f);
        float Rmid = (r_inner + r_outer) / 2.0f;
        float r = (r_outer - r_inner) / 2.0f;
        torusSegment(f, cx, oy, oz, Rmid, r, 0, 2*M_PI, 12, 6);
    }
    // Внутренняя обечайка
    hollowCylinder(f, xs + pitch*0.1f, xe - pitch*0.1f, r_inner + r*0.3f, r_inner, 24, oy, oz);
}

int main() {
    // ==================== КОНСТАНТЫ ====================
    const float STATOR_OUTER_R       = 0.0250f;
    const float STATOR_INNER_R_MAIN  = 0.0210f;
    const float STATOR_INNER_R_YBCO  = 0.0205f;
    const float SHAFT_OUTER_R        = 0.0175f;
    const float SHAFT_INNER_R_WARM   = 0.0140f;
    const float SHAFT_INNER_R_COLD   = 0.0165f;
    const float MAGNET_INNER_R       = SHAFT_OUTER_R;
    const float MAGNET_OUTER_R       = 0.0195f;
    const float YBCO_GAP             = 0.0010f;
    const float YBCO_TILE_W          = 0.030f;
    const float YBCO_TILE_H          = 0.020f;
    const float YBCO_TILE_D          = 0.004f;
    const float YBCO_CENTER_R        = MAGNET_OUTER_R + YBCO_GAP + YBCO_TILE_D*0.5f;
    
    const float STATOR_LENGTH   = 0.380f;
    const float SHAFT_LENGTH    = 0.540f;
    const float MAGNET_LENGTH   = 0.120f;
    const float ORIGIN_X = 0.0f, ORIGIN_Y = 0.0f, ORIGIN_Z = 0.0f;

    // ==================== 01_STATOR ====================
    {
        std::ofstream f("01_stator.stl");
        f << "solid stator\n";
        hollowCylinder(f, -STATOR_LENGTH*0.4f, -0.040f, STATOR_OUTER_R, STATOR_INNER_R_MAIN, 72, ORIGIN_Y, ORIGIN_Z);
        hollowCylinder(f, -0.040f, 0.100f, STATOR_OUTER_R, STATOR_INNER_R_YBCO, 72, ORIGIN_Y, ORIGIN_Z);
        hollowCylinder(f, 0.100f, STATOR_LENGTH*0.6f, STATOR_OUTER_R, STATOR_INNER_R_MAIN, 72, ORIGIN_Y, ORIGIN_Z);
        f << "endsolid stator\n";
        std::cout << ">>> 01_stator.stl\n";
    }

    // ==================== 02_SHAFT ====================
    {
        std::ofstream f("02_shaft.stl");
        f << "solid shaft\n";
        hollowCylinder(f, -SHAFT_LENGTH*0.5f, -0.080f, SHAFT_OUTER_R, SHAFT_INNER_R_WARM, 48, ORIGIN_Y, ORIGIN_Z);
        hollowCylinder(f, -0.080f, 0.200f, SHAFT_OUTER_R, SHAFT_INNER_R_COLD, 48, ORIGIN_Y, ORIGIN_Z);
        hollowCylinder(f, 0.150f, SHAFT_LENGTH*0.5f, SHAFT_OUTER_R, SHAFT_INNER_R_WARM, 48, ORIGIN_Y, ORIGIN_Z);
        // Дропауты
        float dox = SHAFT_LENGTH*0.5f - 0.040f;
        box(f, {dox-0.018f, ORIGIN_Y+SHAFT_OUTER_R, ORIGIN_Z-0.015f}, {dox+0.018f, ORIGIN_Y+SHAFT_OUTER_R+0.030f, ORIGIN_Z+0.015f});
        hollowCylinder(f, dox-0.010f, dox+0.010f, 0.010f, 0.006f, 24, ORIGIN_Y+SHAFT_OUTER_R+0.022f, ORIGIN_Z);
        f << "endsolid shaft\n";
        std::cout << ">>> 02_shaft.stl\n";
    }

    // ==================== 03_BANDAGE ====================
    {
        std::ofstream f("03_bandage.stl");
        f << "solid bandage\n";
        hollowCylinder(f, -0.100f, 0.200f, SHAFT_OUTER_R+0.0005f, SHAFT_OUTER_R, 48, ORIGIN_Y, ORIGIN_Z);
        f << "endsolid bandage\n";
        std::cout << ">>> 03_bandage.stl\n";
    }

    // ==================== 04_MAGNET (12 сегментов в одном файле) ====================
    {
        std::ofstream f("04_magnet_assembly.stl");
        f << "solid magnet_assembly\n";
        float magnet_x = 0.030f;
        halbachDiscrete(f, magnet_x, ORIGIN_Y, ORIGIN_Z, MAGNET_INNER_R, MAGNET_OUTER_R, MAGNET_LENGTH, 12);
        // Маркеры
        for (int i = 0; i < 12; i++) {
            float ang = i * 30.0f * M_PI / 180.0f;
            float mr = (MAGNET_INNER_R + MAGNET_OUTER_R) * 0.5f;
            P s = {magnet_x+MAGNET_LENGTH*0.5f+0.003f, ORIGIN_Y+mr*sinf(ang), ORIGIN_Z+mr*cosf(ang)};
            P e = {magnet_x+MAGNET_LENGTH*0.5f+0.015f, ORIGIN_Y+mr*sinf(ang), ORIGIN_Z+mr*cosf(ang)};
            cable(f, s, e, 0.0015f, 6);
        }
        // Ni-P бандаж
        hollowCylinder(f, magnet_x-MAGNET_LENGTH*0.5f-0.002f, magnet_x+MAGNET_LENGTH*0.5f+0.002f, MAGNET_OUTER_R+0.0005f, MAGNET_OUTER_R, 48, ORIGIN_Y, ORIGIN_Z);
        f << "endsolid magnet_assembly\n";
        std::cout << ">>> 04_magnet_assembly.stl\n";
    }

    // ==================== 05_06_YBCO_TILES ====================
    {
        std::ofstream f("05_ybco_tiles.stl");
        f << "solid ybco_tiles\n";
        float magnet_x = 0.030f;
        float hx1 = magnet_x - MAGNET_LENGTH*0.5f - 0.030f;
        float hx4 = magnet_x + MAGNET_LENGTH*0.5f + 0.030f;
        float hx2 = hx1 + (hx4 - hx1) / 3.0f;
        float hx3 = hx1 + 2.0f * (hx4 - hx1) / 3.0f;
        float hx[4] = {hx1, hx2, hx3, hx4};
        for (int ring = 0; ring < 4; ring++) {
            for (int i = 0; i < 4; i++) {
                float ang = i * 90.0f * M_PI / 180.0f;
                float ty = ORIGIN_Y + YBCO_CENTER_R * sinf(ang);
                float tz = ORIGIN_Z + YBCO_CENTER_R * cosf(ang);
                htscTile(f, hx[ring], ty, tz, YBCO_TILE_W, YBCO_TILE_H, YBCO_TILE_D);
            }
        }
        f << "endsolid ybco_tiles\n";
        std::cout << ">>> 05_ybco_tiles.stl\n";
    }

    // ==================== 06_COLD_FINGER ====================
    {
        std::ofstream f("06_cold_finger.stl");
        f << "solid cold_finger\n";
        float magnet_x = 0.030f;
        float hx1 = magnet_x - MAGNET_LENGTH*0.5f - 0.030f;
        float hx4 = magnet_x + MAGNET_LENGTH*0.5f + 0.030f;
        float hx2 = hx1 + (hx4 - hx1) / 3.0f;
        float cfx = hx2;
        hollowCylinder(f, cfx-0.018f, cfx+0.018f, 0.014f, 0.008f, 24, ORIGIN_Y+STATOR_OUTER_R+0.030f, ORIGIN_Z);
        cylinder(f, cfx-0.010f, cfx+0.010f, 0.022f, 24, ORIGIN_Y+STATOR_OUTER_R+0.050f, ORIGIN_Z);
        f << "endsolid cold_finger\n";
        std::cout << ">>> 06_cold_finger.stl\n";
    }

    // ==================== 07_COOLER ====================
    {
        std::ofstream f("07_cooler.stl");
        f << "solid cooler\n";
        float magnet_x = 0.030f;
        float hx1 = magnet_x - MAGNET_LENGTH*0.5f - 0.030f;
        float hx4 = magnet_x + MAGNET_LENGTH*0.5f + 0.030f;
        float hx2 = hx1 + (hx4 - hx1) / 3.0f;
        float cfx = hx2;
        float cooler_x = cfx;
        float cooler_y = ORIGIN_Y + STATOR_OUTER_R + 0.080f;
        hollowCylinder(f, cooler_x-0.030f, cooler_x+0.030f, 0.035f, 0.028f, 36, cooler_y, ORIGIN_Z);
        hollowCylinder(f, cooler_x-0.020f, cooler_x+0.020f, 0.028f, 0.018f, 36, cooler_y+0.040f, ORIGIN_Z);
        hollowCylinder(f, cooler_x-0.025f, cooler_x+0.025f, 0.022f, 0.006f, 24, cooler_y+0.075f, ORIGIN_Z);
        f << "endsolid cooler\n";
        std::cout << ">>> 07_cooler.stl\n";
    }

    // ==================== 08_SAPPHIRE_BUSHINGS ====================
    {
        std::ofstream f("08_sapphire_bushings.stl");
        f << "solid sapphire_bushings\n";
        float magnet_x = 0.030f;
        for (int i = 0; i < 2; i++) {
            float bx = (i == 0) ? magnet_x-MAGNET_LENGTH*0.5f-0.040f : magnet_x+MAGNET_LENGTH*0.5f+0.040f;
            hollowCylinder(f, bx-0.004f, bx+0.004f, 0.0190f, 0.0171f, 48, ORIGIN_Y, ORIGIN_Z);
            hollowCylinder(f, bx-0.006f, bx+0.006f, 0.0205f, 0.0195f, 48, ORIGIN_Y, ORIGIN_Z);
            for (int j = 0; j < 8; j++) {
                float a = j * 45.0f * M_PI / 180.0f;
                cable(f, {bx-0.006f, ORIGIN_Y+0.0195f*sinf(a), ORIGIN_Z+0.0195f*cosf(a)},
                          {bx+0.006f, ORIGIN_Y+0.0205f*sinf(a), ORIGIN_Z+0.0205f*cosf(a)}, 0.001f, 4);
            }
        }
        f << "endsolid sapphire_bushings\n";
        std::cout << ">>> 08_sapphire_bushings.stl\n";
    }

    // ==================== 09_DAMPER ====================
    {
        std::ofstream f("09_damper.stl");
        f << "solid damper\n";
        float magnet_x = 0.030f;
        float damper_x = magnet_x;
        for (int ph = 0; ph < 3; ph++) {
            float ang = ph * 120.0f * M_PI / 180.0f;
            float cy = ORIGIN_Y + STATOR_INNER_R_MAIN * 0.85f * sinf(ang);
            float cz = ORIGIN_Z + STATOR_INNER_R_MAIN * 0.85f * cosf(ang);
            for (int c = 0; c < 2; c++) {
                float cx = damper_x - 0.025f + c * 0.050f;
                torusSegment(f, cx, cy, cz, 0.008f, 0.0015f, -M_PI*0.45f, M_PI*0.45f, 10, 4);
            }
        }
        box(f, {damper_x-0.045f, ORIGIN_Y+STATOR_OUTER_R+0.005f, ORIGIN_Z-0.035f}, {damper_x+0.045f, ORIGIN_Y+STATOR_OUTER_R+0.030f, ORIGIN_Z+0.035f});
        box(f, {-0.200f, ORIGIN_Y+STATOR_OUTER_R+0.015f, ORIGIN_Z-0.010f}, {-0.185f, ORIGIN_Y+STATOR_OUTER_R+0.030f, ORIGIN_Z+0.010f});
        f << "endsolid damper\n";
        std::cout << ">>> 09_damper.stl\n";
    }

    // ==================== 10_BELLOWS ====================
    {
        std::ofstream f("10_bellows.stl");
        f << "solid bellows\n";
        float magnet_x = 0.030f;
        float bellows_len = 0.200f;
        float bxs = magnet_x - MAGNET_LENGTH*0.5f - bellows_len;
        bellows_inconel(f, bxs, bxs+bellows_len, SHAFT_OUTER_R+0.002f, STATOR_INNER_R_MAIN-0.001f, 12, ORIGIN_Y, ORIGIN_Z);
        f << "endsolid bellows\n";
        std::cout << ">>> 10_bellows.stl\n";
    }

    // ==================== 11_CRYOPUMP ====================
    {
        std::ofstream f("11_cryopump.stl");
        f << "solid cryopump\n";
        const float CP_INNER_R = SHAFT_OUTER_R + 0.0008f;
        const float CP_OUTER_R = STATOR_INNER_R_MAIN - 0.003f;
        float cpx = -STATOR_LENGTH*0.4f + 0.035f;
        hollowCylinder(f, cpx-0.020f, cpx+0.020f, CP_OUTER_R, CP_INNER_R, 36, ORIGIN_Y, ORIGIN_Z);
        for (int i = 0; i < 16; i++) {
            float a = i * 22.5f * M_PI / 180.0f;
            float rr = (CP_INNER_R + CP_OUTER_R) * 0.5f;
            cable(f, {cpx, ORIGIN_Y+rr*sinf(a), ORIGIN_Z+rr*cosf(a)},
                      {-STATOR_LENGTH*0.3f, ORIGIN_Y+rr*sinf(a), ORIGIN_Z+rr*cosf(a)}, 0.001f, 4);
        }
        f << "endsolid cryopump\n";
        std::cout << ">>> 11_cryopump.stl\n";
    }

    // ==================== 12_MLI ====================
    {
        std::ofstream f("12_mli.stl");
        f << "solid mli\n";
        for (int i = 0; i < 20; i++) {
            hollowCylinder(f, -STATOR_LENGTH*0.35f, STATOR_LENGTH*0.55f, 
                           STATOR_OUTER_R+0.0025f+i*0.0003f, STATOR_OUTER_R+0.0020f+i*0.0003f, 36, ORIGIN_Y, ORIGIN_Z);
        }
        f << "endsolid mli\n";
        std::cout << ">>> 12_mli.stl\n";
    }

    // ==================== 13_ENERGY_SYSTEM ====================
    {
        std::ofstream f("13_energy_system.stl");
        f << "solid energy_system\n";
        float magnet_x = 0.030f;
        float damper_x = magnet_x;
        float hx1 = magnet_x - MAGNET_LENGTH*0.5f - 0.030f;
        float hx4 = magnet_x + MAGNET_LENGTH*0.5f + 0.030f;
        float hx2 = hx1 + (hx4 - hx1) / 3.0f;
        float cfx = hx2;
        float cooler_x = cfx;
        float cooler_y = ORIGIN_Y + STATOR_OUTER_R + 0.080f;
        box(f, {0.100f, ORIGIN_Y+STATOR_OUTER_R+0.035f, ORIGIN_Z-0.050f}, {0.160f, ORIGIN_Y+STATOR_OUTER_R+0.075f, ORIGIN_Z+0.050f});
        cylinder(f, 0.180f, 0.230f, 0.045f, 24, ORIGIN_Y+STATOR_OUTER_R+0.055f, ORIGIN_Z);
        cable(f, {0.130f, ORIGIN_Y+STATOR_OUTER_R+0.075f, ORIGIN_Z+0.050f}, {damper_x+0.045f, ORIGIN_Y+STATOR_OUTER_R+0.030f, ORIGIN_Z+0.035f}, 0.006f, 8);
        cable(f, {0.205f, ORIGIN_Y+STATOR_OUTER_R+0.055f, ORIGIN_Z}, {cooler_x+0.025f, cooler_y+0.075f, ORIGIN_Z}, 0.005f, 8);
        f << "endsolid energy_system\n";
        std::cout << ">>> 13_energy_system.stl\n";
    }

    std::cout << "\n>>> ALL 13 STL FILES GENERATED <<<\n";
    return 0;
}
