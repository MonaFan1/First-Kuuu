#include "Action.hpp"
// 确保Motor模块的命名空间和类名匹配
using namespace User;

// 构造函数实现（LibXR框架自动注入硬件和应用管理器）
Action::Action(LibXR::HardwareContainer &hw, LibXR::ApplicationManager &app,
               int motor_id, bool reverse)
  : LibXR::Application("ActionModule", hw, app),  // 框架基类初始化
    motor_id_(motor_id),
    reverse_(reverse),
    // 位置环PID参数
    pos_pid_(4.0f, 0.05f, 0.02f, 100.0f, 5.0f),
    target_angle_(0.0f),
    is_loop_running_(false) {

  // 1. 从LibXR硬件容器获取Motor模块（框架自动管理生命周期）
  motor_ = new GM6020Motor(hw, app, motor_id, reverse);
  if (!motor_) {
    LOG_ERROR("Action模块初始化失败：无法创建GM6020Motor实例");
    return;
  }

  LOG_INFO("Action模块初始化完成（电机ID：{}，反向：{}）", motor_id_, reverse_);
}

// 设置目标角度（启动位置闭环）
void Action::SetTargetAngle(float angle) {
  if (!motor_) {
    LOG_WARN("Motor模块未初始化，无法设置目标角度");
    return;
  }
 
  // 角度限幅（0-360°）
  target_angle_ = LibXR::CycleValue<float>(0.0f, 360.0f).Get(angle);
  is_loop_running_ = true;  // 启动闭环
  LOG_INFO("设置GM6020目标角度：{:.1f}°", target_angle_);
}

// 停止位置闭环
void Action::StopLoop() {
  is_loop_running_ = false;
  // 停止电机（归零电流）
  if (motor_) {
    motor_->SetTarget(0.0f);
  }
  LOG_INFO("位置闭环已停止，电机归零");
}

// 框架自动调用的监控函数（10ms/次，无需手动轮询）
void Action::OnMonitor() override {
  // 非闭环状态直接返回
  if (!is_loop_running_ || !motor_) {
    return;
  }

  // 获取电机当前反馈
  const auto& feedback = motor_->GetFeedback();
  if (!feedback.is_valid) {
    LOG_WARN("电机无反馈数据，暂停闭环");
    return;
  }

  // 计算角度误差（处理360°循环，避免359°→0°的超大误差）
  float current_angle = feedback.angle;
  float angle_error = LibXR::CycleValue<float>(0.0f, 360.0f).GetDiff(current_angle, target_angle_);

  // 误差小于0.5°时停止闭环（精度控制）
  if (fabs(angle_error) < 0.5f) {
    StopLoop();
    LOG_INFO("角度到位！目标：{:.1f}°，当前：{:.1f}°，误差：{:.2f}°",
             target_angle_, current_angle, angle_error);
    return;
  }

  // PID计算输出（角度误差→电流指令）
  float current_cmd = pos_pid_.Update(target_angle_, current_angle, 0.01f);
  // 方向反转处理
  current_cmd = reverse_ ? -current_cmd : current_cmd;

  // 发送控制指令给电机
  motor_->SetTarget(current_cmd);

  // 调试日志（可选，发布时可注释）
  LOG_DEBUG("闭环中：目标{:.1f}° → 当前{:.1f}°，误差{:.2f}°，输出电流{:.2f}A",
            target_angle_, current_angle, angle_error, current_cmd);
}
