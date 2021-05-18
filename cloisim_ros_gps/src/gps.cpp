/**
 *  @file   gps.cpp
 *  @date   2021-01-14
 *  @author Hyunseok Yang
 *  @brief
 *        ROS2 GPS class for simulator
 *  @remark
 *  @copyright
 *      LGE Advanced Robotics Laboratory
 *      Copyright (c) 2020 LG Electronics Inc., LTD., Seoul, Korea
 *      All Rights are Reserved.
 *
 *      SPDX-License-Identifier: MIT
 */

#include "cloisim_ros_gps/gps.hpp"
#include <tf2/LinearMath/Quaternion.h>

using namespace std;
using namespace cloisim_ros;

Gps::Gps(const rclcpp::NodeOptions &options_, const string node_name, const string namespace_)
    : Base(node_name, namespace_, options_)
{
  topic_name_ = "navsatfix";

  Start();
}

Gps::Gps(const string namespace_)
    : Gps(rclcpp::NodeOptions(), "cloisim_ros_gps", namespace_)
{
}

Gps::~Gps()
{
  Stop();
}

void Gps::Initialize()
{
  uint16_t portInfo, portData;
  get_parameter_or("bridge.Data", portData, uint16_t(0));
  get_parameter_or("bridge.Info", portInfo, uint16_t(0));

  const auto hashKeyData = GetTargetHashKey("Data");
  const auto hashKeyInfo = GetTargetHashKey("Info");
  DBG_SIM_INFO("hash Key: data(%s), info(%s)", hashKeyData.c_str(), hashKeyInfo.c_str());

  auto pBridgeData = CreateBridge(hashKeyData);
  auto info_bridge_ptr = CreateBridge(hashKeyInfo);

  if (info_bridge_ptr != nullptr)
  {
    info_bridge_ptr->Connect(zmq::Bridge::Mode::CLIENT, portInfo, hashKeyInfo);

    GetRos2Parameter(info_bridge_ptr);

    // Get frame for message
    const auto frame_id = GetFrameId();
    msg_nav_.header.frame_id = frame_id;

    const auto transform = GetObjectTransform(info_bridge_ptr);
    SetupStaticTf2(transform, frame_id + "_link");
  }

  // Fill covariances
  // TODO: need to applying noise
  msg_nav_.position_covariance[0] = 0.0001f;
  msg_nav_.position_covariance[4] = 0.0001f;
  msg_nav_.position_covariance[8] = 0.0001f;
  msg_nav_.position_covariance_type = sensor_msgs::msg::NavSatFix::COVARIANCE_TYPE_DIAGONAL_KNOWN;

  // Fill gps status
  msg_nav_.status.status = sensor_msgs::msg::NavSatStatus::STATUS_FIX;
  msg_nav_.status.service = sensor_msgs::msg::NavSatStatus::SERVICE_GPS;

  // ROS2 Publisher
  pub_ = this->create_publisher<sensor_msgs::msg::NavSatFix>(topic_name_, rclcpp::SensorDataQoS());

  if (pBridgeData != nullptr)
  {
    pBridgeData->Connect(zmq::Bridge::Mode::SUB, portData, hashKeyData);
    CreatePublisherThread(pBridgeData);
  }
}

void Gps::UpdatePublishingData(const string &buffer)
{
  if (!pb_buf_.ParseFromString(buffer))
  {
    DBG_SIM_ERR("Parsing error, size(%d)", buffer.length());
    return;
  }

  SetSimTime(pb_buf_.time());

  // Fill message with latest sensor data
  msg_nav_.header.stamp = GetSimTime();
  msg_nav_.latitude = pb_buf_.latitude_deg();
  msg_nav_.longitude = pb_buf_.longitude_deg();
  msg_nav_.altitude = pb_buf_.altitude();

  pub_->publish(msg_nav_);
}