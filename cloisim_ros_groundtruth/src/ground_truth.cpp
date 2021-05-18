/**
 *  @file   ground_truth.cpp
 *  @date   2021-05-16
 *  @author Hyunseok Yang
 *  @brief
 *        ROS2 node for controlling unity simulation
 *  @remark
 *        Gazebonity
 *  @copyright
 *      LGE Advanced Robotics Laboratory
 *      Copyright (c) 2020 LG Electronics Inc., LTD., Seoul, Korea
 *      All Rights are Reserved.
 *
 *      SPDX-License-Identifier: MIT
 */

#include "cloisim_ros_groundtruth/ground_truth.hpp"
#include <cloisim_ros_base/helper.h>

using namespace std;
using namespace cloisim;
using namespace cloisim_ros;

GroundTruth::GroundTruth(const rclcpp::NodeOptions &options_, const std::string node_name)
  : Base(node_name, options_)
{
  Start();
}

GroundTruth::GroundTruth()
  : GroundTruth(rclcpp::NodeOptions(), "cloisim_ros_groundtruth")
{
}

GroundTruth::~GroundTruth()
{
  Stop();
}

void GroundTruth::Initialize()
{
  uint16_t portData;
  get_parameter_or("bridge.Data", portData, uint16_t(0));

  const auto hashKey = GetModelName() + GetPartsName() + "Data";
  DBG_SIM_INFO("hash Key: %s", hashKey.c_str());

  pub_ = create_publisher<perception_msgs::msg::ObjectArray>("/ground_truth", rclcpp::QoS(rclcpp::KeepLast(10)).transient_local());

  auto pBridgeData = CreateBridge(hashKey);
  if (pBridgeData != nullptr)
  {
    pBridgeData->Connect(zmq::Bridge::Mode::SUB, portData, hashKey);
    CreatePublisherThread(pBridgeData);
  }
}

void GroundTruth::UpdatePublishingData(const string &buffer)
{
  if (!pb_buf_.ParseFromString(buffer))
  {
    DBG_SIM_ERR("Parsing error, size(%d)", buffer.length());
    return;
  }

  SetSimTime(pb_buf_.header().stamp());

  UpdatePerceptionData();

  pub_->publish(msg_);
}

void GroundTruth::UpdatePerceptionData()
{
  msg_.header.stamp = GetSimTime();

  msg_.objects.clear();

  for (auto i = 0; i < pb_buf_.perception_size(); i++)
  {
    const auto perception = pb_buf_.perception(i);

    auto object_pose_msg = perception_msgs::msg::ObjectPose();
    object_pose_msg.tracking_id = perception.tracking_id();
    object_pose_msg.class_id = perception.class_id();

    SetVector3MessageToGeometry(perception.position(), object_pose_msg.position);
    SetVector3MessageToGeometry(perception.velocity(), object_pose_msg.velocity);

    for (auto it = perception.footprints().begin(); it < perception.footprints().end(); ++it)
    {
      const auto point = *it;
      geometry_msgs::msg::Point32 point32;
      SetVector3MessageToGeometry(point, point32);
      object_pose_msg.footprints.push_back(point32);
    }

    msg_.objects.push_back(object_pose_msg);
  }
}