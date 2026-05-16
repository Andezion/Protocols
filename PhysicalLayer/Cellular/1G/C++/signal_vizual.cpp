#include <raylib.h>
#include <cmath>

#define Ac 1.0 // амплитуда несущей
#define fc 1000.0 // частота несущей
#define fic 0.0 // начальная фаза несущей

// тут мы будем описывать математическую несущую
float mathematically_load_bearing(int t) {
    return Ac * cos(2 * M_PI * fc * t + fic);
}

// тут мы будем описывать фм модулированный сигнал 
void fm_modulate_signal() {

}

// тут мы будем описывать правило Карсона для оценки ширины спектра фм сигнала
void rule_of_karson() {

}

// тут мы будем собственно генерировать сигнал
void generator_of_signal() {

}

// тут мы будем описывать схему смещения 
void sheme_of_offset() {

}

// тут мы будем описывать формулу Фрисса для модели свободного пространства потери
void formula_friss() {

}

// тут мы будем описывать детерминированную модель формулы Фрисса 
void determinated_model_of_friss() {

}

// тут мы будем оценивать допплеровский эффект для мобильных устройств
void doppler_effect() {

}

int main() {

}