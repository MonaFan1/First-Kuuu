#include <iostream>
template <typename T>
class LimitValue {
public:
    T current_value;
    T min_value;
    T max_value;

    void fun() {
        if (current_value < min_value) current_value = min_value;
        else if (current_value > max_value) current_value = max_value;
    }
    
    LimitValue(T t, T m_min, T m_max) : current_value(t), min_value(m_min), max_value(m_max) {
        fun();
    }
    void operator=(T val) {
        current_value = val;
        fun();
    }
    void operator+=(T val) {
        current_value += val;
        fun();
    }
    T val() { return current_value; }
};

int main() {
    LimitValue<int> pwm(50, 0, 100);
    pwm += 1000000;
    std::cout << "PWM=" << pwm.val() << std::endl;

    pwm = -1000000;
    std::cout << "PWM=" << pwm.val() << std::endl;

    return 0;
}
