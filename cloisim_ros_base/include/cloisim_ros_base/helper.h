/**
 *  @file   helper.h
 *  @date   2021-01-14
 *  @author Hyunseok Yang
 *  @brief
 *        ROS2 CLOiSim-ROS helper
 *  @remark
 *  @copyright
 *      LGE Advanced Robotics Laboratory
 *      Copyright (c) 2020 LG Electronics Inc., LTD., Seoul, Korea
 *      All Rights are Reserved.
 *
 *      SPDX-License-Identifier: MIT
 */

#ifndef _CLOISIM_ROS_HELPER_H_
#define _CLOISIM_ROS_HELPER_H_

#include <geometry_msgs/msg/vector3.hpp>
#include <geometry_msgs/msg/point32.hpp>
#include <geometry_msgs/msg/point.hpp>
#include <geometry_msgs/msg/quaternion.hpp>
#include <tf2/LinearMath/Quaternion.h>

static void SetVector3MessageToGeometry(const cloisim::msgs::Vector3d &src, geometry_msgs::msg::Vector3 &dst)
{
  dst.x = src.x();
  dst.y = src.y();
  dst.z = src.z();
}

static void SetVector3MessageToGeometry(const cloisim::msgs::Vector3d &src, geometry_msgs::msg::Point32 &dst)
{
  dst.x = src.x();
  dst.y = src.y();
  dst.z = src.z();
}

static void SetVector3MessageToGeometry(const cloisim::msgs::Vector3d &src, geometry_msgs::msg::Point &dst)
{
  dst.x = src.x();
  dst.y = src.y();
  dst.z = src.z();
}

static void SetQuaternionMessageToGeometry(const cloisim::msgs::Quaternion &src, geometry_msgs::msg::Quaternion &dst)
{
  dst.x = src.x();
  dst.y = src.y();
  dst.z = src.z();
  dst.w = src.w();
}

static void SetQuaternionMessageToGeometry(const tf2::Quaternion &src, geometry_msgs::msg::Quaternion &dst)
{
  dst.x = src.x();
  dst.y = src.y();
  dst.z = src.z();
  dst.w = src.w();
}

#endif