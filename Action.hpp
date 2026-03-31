#pragma once

// clang-format off
/* === MODULE MANIFEST V2 ===
module_description: Robot action control module (call Motor for position loop)
constructor_args:
  - name: motor_id
    type: int
    desc: GM6020 motor ID (1-7)
  - name: reverse
    type: bool
    desc: Motor direction reverse (default: false)
template_args: []
required_hardware:
  - type: LibXR::CAN
    name: can1
depends:
  - Motor
=== END MANIFEST ===
*/
// clang-format on

#include "app_framework.hpp"
#include "LibXR/Control/PID.hpp"
#include "LibXR/Common/Log/Log.hpp"
#include "LibXR/Common/Time/Time.hpp"
#include "../Motor/Motor.hpp"

namespace User {

class Action : public LibXR::Application {
public:
  Action(LibXR::HardwareContainer &hw, LibXR::ApplicationManager &app,
         int motor_id = 1, bool reverse = false);

  void SetTargetAngle(float angle);  // 设置目标角度
  void StopLoop();                   // 停止闭环

  void OnMonitor() override;

private:
  int motor_id_;
  bool reverse_;

  //使用抽象电机接口 Motor*，而不是 GM6020Motor*
  Motor* motor_ = nullptr;

  LibXR::PID<float> pos_pid_;
  float target_angle_;
  bool is_loop_running_;
};

} // namespace User

