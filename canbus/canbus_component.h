/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

/**
 * @file
 */

#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "cyber/common/macros.h"
#include "cyber/component/timer_component.h"
#include "cyber/cyber.h"
#include "cyber/timer/timer.h"

#include "modules/canbus/proto/chassis_detail.pb.h"
#include "modules/drivers/canbus/proto/can_card_parameter.pb.h"
#include "modules/guardian/proto/guardian.pb.h"

#include "modules/canbus/vehicle/vehicle_controller.h"
#include "modules/common/monitor_log/monitor_log_buffer.h"
#include "modules/common/status/status.h"
#include "modules/control/proto/control_cmd.pb.h"
#include "modules/drivers/canbus/can_client/can_client.h"
#include "modules/drivers/canbus/can_comm/can_receiver.h"
#include "modules/drivers/canbus/can_comm/can_sender.h"
#include "modules/drivers/canbus/can_comm/message_manager.h"

/**
 * @namespace apollo::canbus
 * @brief apollo::canbus
 */
namespace apollo {
namespace canbus {

/**
 * @class Canbus
 *
 * @brief canbus module main class.
 * canbus模块主类。
 * It processes the control data to send protocol messages to can card.
 * 它处理控制数据以将协议消息发送到can卡
 */
class CanbusComponent final : public apollo::cyber::TimerComponent {
 public:
  CanbusComponent(); //构造函数
  /**
   * @brief obtain module name  得到模块名字
   * @return module name  返回模块名字
   */
  std::string Name() const; //返回模块名字

 private:
  /**
   * @brief module initialization function  模块初始化函数
   * @return initialization status  返回初始化的结果，成功初始化为true
   */
  bool Init() override; //初始化

  /**
   * @brief module on_time function 模块开启时间功能
   */
  bool Proc() override; //如果成功返回true

  /**
   * @brief module cleanup function 模块清理函数
   */
  void Clear() override; //清除，重置

  void PublishChassis(); //发布底盘
  void PublishChassisDetail(); //发布底盘信息
  void OnControlCommand(const apollo::control::ControlCommand &control_command); //控制命令来的时机、延迟时间、更新控制命令、can发送者信息
  void OnGuardianCommand(
      const apollo::guardian::GuardianCommand &guardian_command); //同上，不过是导航命令
  apollo::common::Status OnError(const std::string &error_msg); //返回canbus故障码及故障信息
  void RegisterCanClients(); //注册can从站

  CanbusConf canbus_conf_;
  std::shared_ptr<cyber::Reader<apollo::guardian::GuardianCommand>>
      guardian_cmd_reader_;  //导航命令阅读者
  std::shared_ptr<cyber::Reader<apollo::control::ControlCommand>>
      control_command_reader_; //控制命令阅读者
  std::unique_ptr<apollo::drivers::canbus::CanClient> can_client_;  //can客户端
  CanSender<ChassisDetail> can_sender_;  //can发送者
  apollo::drivers::canbus::CanReceiver<ChassisDetail> can_receiver_;  //can接受者
  std::unique_ptr<MessageManager<ChassisDetail>> message_manager_;  //消息管理者
  std::unique_ptr<VehicleController> vehicle_controller_;  //车辆控制器
  int64_t last_timestamp_ = 0;  //上一个时间戳
  ::apollo::common::monitor::MonitorLogBuffer monitor_logger_buffer_;  //监视器记录缓存区
  std::shared_ptr<cyber::Writer<Chassis>> chassis_writer_;  //底盘写的人
  std::shared_ptr<cyber::Writer<ChassisDetail>> chassis_detail_writer_;  //底盘信息写的人
};

CYBER_REGISTER_COMPONENT(CanbusComponent) //Cyber

}  // namespace canbus
}  // namespace apollo
