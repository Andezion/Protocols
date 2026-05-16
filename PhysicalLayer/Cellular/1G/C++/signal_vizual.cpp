#include <raylib.h>
#include <cmath>

#define Ac  1.0f    // амплитуда несущей
#define fc  1000.0f // частота несущей (Гц)
#define fic 0.0f    // начальная фаза несущей (рад)
#define kf  100.0f  // коэффициент частотной модуляции (Гц/В)

#define Am  0.5f    // амплитуда модулирующего сигнала (голос)
#define fm  300.0f  // частота модулирующего сигнала (Гц), нижняя граница речи

#define DT  0.0001f // шаг интегрирования (сек)

// модулирующий сигнал m(t) - упрощённая модель голоса (синус)
// s_FM = Ac * cos(2pi*fc*t + 2pi*kf * integral(m(tau), 0, t))
float m_signal(float t) {
    return Am * sinf(2.0f * M_PI * fm * t);
}

// численный интеграл от func от low до high методом прямоугольников
float integral(float low, float high, float (*func)(float)) {
    float sum = 0.0f;
    for (float tau = low; tau <= high; tau += DT) {
        sum += func(tau) * DT;
    }
    return sum;
}

// математическая несущая: s_c(t) = Ac * cos(2pi*fc*t + fic)
float mathematically_load_bearing(float t) {
    return Ac * cosf(2.0f * M_PI * fc * t + fic);
}

// FM-модулированный сигнал: s_FM(t) = Ac * cos(2pi*fc*t + 2pi*kf * ∫m(τ)dτ)
float fm_modulate_signal(float t) {
    return Ac * cosf(2.0f * M_PI * fc * t + 2.0f * M_PI * kf * integral(0.0f, t, m_signal));
}

// правило Карсона: BW ≈ 2*(delta_f + fm), где delta_f = kf * max(m(t)) = kf * Am
float rule_of_karson() {
    float delta_f = kf * Am;
    return 2.0f * (delta_f + fm);
}

// тут мы будем собственно генерировать сигнал
void generator_of_signal() {

}

// тут мы будем описывать схему смещения
void sheme_of_offset() {

}

// потери свободного пространства по Фриссу: PL(d) = 20*log10(4*pi*d*f/c)
// возвращает потери в дБ
float formula_friss(float d, float f) {
    const float c = 3e8f;
    return 20.0f * log10f((4.0f * M_PI * d * f) / c);
}

// принятая мощность по детерминированной модели Фрисса:
// Pr = Pt * Gt * Gr * (lambda / (4*pi*d))^2
float determinated_model_of_friss(float Pt, float Gt, float Gr, float d, float f) {
    const float c = 3e8f;
    float lambda = c / f;
    float factor = lambda / (4.0f * M_PI * d);
    return Pt * Gt * Gr * factor * factor;
}

// доплеровский сдвиг частоты: delta_f = (v/c) * fc * cos(theta)
// v - скорость абонента (м/с), theta - угол между вектором движения и направлением на БС
float doppler_effect(float v, float theta) {
    const float c = 3e8f;
    return (v / c) * fc * cosf(theta);
}

void draw_mesh() {
    for (int i = 0; i <= 40; i++) {
        DrawLine(200, 100 + i * 10, 900, 100 + i * 10, GREEN);
    }
    for (int i = 0; i <= 70; i++) {
        DrawLine(200 + i * 10, 100, 200 + i * 10, 500, GREEN);
    }
}

int main() {
    const int screenWidth = 1000;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "1G Signal Visualization");

    SetTargetFPS(120); 

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // то есть от 200 до 900 по горизонтали и от 100 до 500 по вертикали 
            DrawRectangle(200, 100, 700, 400, LIGHTGRAY);

            draw_mesh();

            DrawLine(210, 490, 890, 490, BLACK);
            DrawLine(210, 490, 210, 110, BLACK);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}