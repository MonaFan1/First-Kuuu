#include <iostream>
#include <cstdio> 
class IMotor {
public:
    virtual void SetOutput(int current) = 0;
    virtual int GetSpeed() const = 0;
    virtual void Update() = 0;

    virtual ~IMotor() {
        std::cout << "IMotor 析构函数调用" << std::endl;
    }
};

class DJIMotor : public IMotor {
public:
    void SetOutput(int current) override {
        printf("[CAN BUS], Send Current: %d mA\n", current);
        this->current_ = current;
    }

    int GetSpeed() const override {
        return 1000;
    }

    void Update() override {
        std::cout << "[DJIMotor] 刷新CAN总线状态，读取反馈..." << std::endl;
    }

    ~DJIMotor() override {
        std::cout << "DJIMotor 析构函数调用" << std::endl;
    }

private:
    int current_ = 0;
};

class SimMotor : public IMotor {
public:
    SimMotor() : velocity_(0), resistance_(50), input_current_(0) {}

    void SetOutput(int current) override {
        std::cout << "SimMotor 设置输入电流: " << current << std::endl;
        input_current_ = current;
    }

    int GetSpeed() const override {
        return velocity_;
    }

    void Update() override {
        int delta = input_current_ - resistance_;
        velocity_ += delta;
        std::cout << "SimMotor 物理仿真刷新 - 速度变化: " << delta 
                  << " | 当前速度: " << velocity_ << " rpm" << std::endl;
    }

    ~SimMotor() override {
        std::cout << "SimMotor 析构函数调用" << std::endl;
    }

private:
    int velocity_;
    const int resistance_;
    int input_current_;
};

void TestMotor(IMotor* motor) {
    std::cout << "\n测试电机" << std::endl;
    
    motor->SetOutput(800);

    std::cout << "当前电机转速: " << motor->GetSpeed() << " rpm" << std::endl;
    
    motor->SetOutput(300);
    for (int i = 0; i < 2; ++i) {
        motor->Update();
    }
    std::cout << "调整后电机转速: " << motor->GetSpeed() << " rpm" << std::endl;
    
    std::cout << "电机测试结束\n" << std::endl;
}

int main() {
    IMotor* dji_motor = new DJIMotor();
    IMotor* sim_motor = new SimMotor();

    TestMotor(dji_motor);
    TestMotor(sim_motor);

    return 0;
}
