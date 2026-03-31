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
  - Motor  # 匹配你模块清单里的依赖名称
=== END MANIFEST === */
// clang-format on

#include "app_framework.hpp"
// 引入你的Motor模块头文件（路径和你工程一致）
#include "../Motor/Motor.hpp"
// LibXR核心组件（PID/日志/时间）
#include "LibXR/Control/PID.hpp"
#include "LibXR/Common/Log/Log.hpp"
#include "LibXR/Common/Time/Time.hpp"
#include <scoped_allocator>

namespace User {

class Action : public LibXR::Application {
public:
  // 构造函数声明（和实现代码匹配）
  Action(LibXR::HardwareContainer &hw, LibXR::ApplicationManager &app,
         int motor_id = 1, bool reverse = false);

  // 核心接口声明
  void SetTargetAngle(float angle);  // 设置目标角度（启动位置闭环）
  void StopLoop();                   // 停止位置闭环

  // 框架自动调用的监控函数
  void OnMonitor() override;

private:
  // 私有成员变量声明（和实现代码匹配）
  int motor_id_;
  bool reverse_;
  GM6020Motor* motor_ = nullptr;  // 你的Motor模块指针
  LibXR::PID<float> pos_pid_;     // 位置闭环PID控制器
  float target_angle_;            // 目标角度（0-360°）
  bool is_loop_running_;          // 闭环运行标志
};


} // namespace User


