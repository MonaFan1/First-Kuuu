#include <iostream>

template <typename T>
class LimitValue {
private:
    T current_value;
    T min_value;
    T max_value;

    void clamp() {
        if (current_value < min_value) current_value = min_value;
        else if (current_value > max_value) current_value = max_value;
    }

public:
    LimitValue(T init_val, T min_val, T max_val)
        : current_value(init_val), min_value(min_val), max_value(max_val) {
        clamp();
    }

    LimitValue<T>& operator=(const T& new_val) {
        current_value = new_val;
        clamp();
        return *this;
    }

    LimitValue<T>& operator+=(const T& add_val) {
        current_value += add_val;
        clamp();
        return *this;
    }

    T val() const {
        return current_value;
    }
};

int main() {
    LimitValue<int> pwm(50, 0, 100);
    pwm += 60;
    std::cout << "PWM值：" << pwm.val() << std::endl;

    pwm = -20;
    std::cout << "PWM值：" << pwm.val() << std::endl;

    return 0;
}