#include "Action.hpp"
// 包含抽象电机接口
#include "Motor.hpp"
// LibXR 工具
#include "libxr_log.hpp"
#include <cmath>

using namespace User;

Action::Action(LibXR::HardwareContainer &hw, LibXR::ApplicationManager &app,
               int motor_id, bool reverse)
  : LibXR::Application("ActionModule", hw, app),
    motor_id_(motor_id),
    reverse_(reverse),
    pos_pid_(4.0f, 0.05f, 0.02f, 100.0f, 5.0f),
    target_angle_(0.0f),
    is_loop_running_(false) {

  auto& module_mgr = app.GetModuleManager();
  motor_ = module_mgr.CreateModule<GM6020Motor>(hw, module_mgr, motor_id, reverse);

  if (!motor_) {
    LOG_ERROR("Action模块初始化失败：无法创建GM6020Motor实例");
    return;
  }

  motor_->Enable();
  LOG_INFO("Action模块初始化完成（电机ID：%d）", motor_id_);
}

void Action::SetTargetAngle(float angle) {
  if (!motor_) {
    LOG_WARN("Motor模块未初始化");
    return;
  }

  target_angle_ = LibXR::CycleValue<float>(0.0f, 360.0f).Get(angle);
  is_loop_running_ = true;
  LOG_INFO("设置目标角度：%.1f°", target_angle_);
}

void Action::StopLoop() {
  is_loop_running_ = false;

  if (motor_) {
    Motor::MotorCmd cmd;
    cmd.mode = Motor::MODE_CURRENT;
    cmd.torque = 0.0f;
    motor_->Control(cmd);
  }

  LOG_INFO("位置闭环停止");
}

void Action::OnMonitor() override {
  if (!is_loop_running_ || !motor_) return;

  motor_->Update();

  const auto& feedback = motor_->GetFeedback();
  if (!feedback.is_valid) return;

  float current_angle = feedback.position;

  LibXR::CycleValue<float> cycle(0.0f, 360.0f);
  float angle_error = cycle.GetDiff(current_angle, target_angle_);

  if (fabs(angle_error) < 0.5f) {
    StopLoop();
    LOG_INFO("已到达目标！当前：%.1f°", current_angle);
    return;
  }

  float current_cmd = pos_pid_.Update(target_angle_, current_angle, 0.01f);
  if (reverse_) current_cmd = -current_cmd;

  Motor::MotorCmd cmd;
  cmd.mode = Motor::MODE_CURRENT;
  cmd.torque = current_cmd;
  motor_->Control(cmd);
}
