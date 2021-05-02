#pragma once

#include <vector>

#include <sensor_msgs/msg/image.hpp>
#include <image_geometry/pinhole_camera_model.h>
#include <geometry_msgs/msg/polygon_stamped.hpp>

namespace cliff_detector {

/**
 * @brief The CliffDetector class detect cliff based on depth image
 */
class CliffDetector {
 public:
  CliffDetector();
  /**
   * @brief detectCliff detects descending stairs based on the information in a depth image
   *
   * This function detects descending stairs based on the information
   * in the depth encoded image. It requires the synchronized
   * Image/CameraInfo pair associated with the image.
   *
   * @param image UInt16 encoded depth image.
   * @param info CameraInfo associated with depth_msg
   */
  geometry_msgs::msg::PolygonStamped detectCliff(
    const sensor_msgs::msg::Image::ConstSharedPtr& image,
    const sensor_msgs::msg::CameraInfo::ConstSharedPtr& info);
  /**
   * @brief setMinRange sets depth sensor min range
   *
   * @param rmin Minimum sensor range (below it is death zone) in meters.
   */

  void setMinRange(const float rmin);
  /**
   * @brief setMaxRange sets depth sensor max range
   *
   * @param rmax Maximum sensor range in meters.
   */
  void setMaxRange(const float rmax);
  /**
   * @brief setSensorMountHeight sets the height of sensor mount (in meters) from ground
   *
   * @param height Value of sensor mount height (in meters).
   */
  void setSensorMountHeight (const float height);
  /**
   * @brief getSensorMountHeight gets sensor mount height
   *
   * @return Return sensor mount height in meters
   */
  float getSensorMountHeight() { return sensor_mount_height_; }
  /**
   * @brief setSensorTiltAngle sets the sensor tilt angle (in degrees)
   * @param angle
   */
  void setSensorTiltAngle(const float angle);
  /**
   * @brief getSensorTiltAngle gets sensor tilt angle in degrees
   *
   * @return Return sensor tilt angle in degrees
   */
  float getSensorTiltAngle() { return sensor_tilt_angle_; }
  /**
   * @brief setPublishDepthEnable
   * @param enable
   */
  void setPublishDepthEnable(const bool enable) { publish_depth_enable_ = enable; }
  /**
   * @brief getPublishDepthEnable
   * @return
   */
  bool getPublishDepthEnable() const { return publish_depth_enable_; }
  /**
   * @brief setCamModelUpdate
   */
  void setCamModelUpdate(const bool u) { cam_model_update_ = u; }
  /**
   * @brief setUsedDepthHeight
   * @param height
   */
  void setUsedDepthHeight(const unsigned int height);
  /**
   * @brief setBlockSize sets size of square block (subimage) used in cliff detector
   *
   * @param size Size of square block in pixels
   */
  void setBlockSize(const int size);
  /**
   * @brief setBlockPointsThresh sets threshold value of pixels in block to set block as stairs
   *
   * @param thresh Value of threshold in pixels.
   */
  void setBlockPointsThresh(const int thresh);
  /**
   * @brief setDepthImgStepRow
   * @param step
   */
  void setDepthImgStepRow(const int step);
  /**
   * @brief setDepthImgStepCol
   * @param step
   */
  void setDepthImgStepCol(const int step);
  /**
   * @brief setGroundMargin sets the floor margin (in meters)
   * @param margin
   */
  void setGroundMargin (const float margin);
  /**
   * @brief setParametersConfigurated
   * @param u
   */
  void setParametersConfigurated (const bool u) { depth_sensor_params_update = u; }

 protected:
  /**
   * @brief lengthOfVector calculates length of 3D vector.
   *
   * Method calculates the length of 3D vector assumed to be a vector with start at the (0,0,0).
   *
   * @param vec Vector 3D which lenght will be calculated.
   * @return Returns the length of 3D vector.
   */
  double lengthOfVector(const cv::Point3d& vec) const;
  /**
    * Computes the angle between two cv::Point3d
    *
    * Computes the angle of two cv::Point3d assumed to be vectors starting
    * at the origin (0,0,0). Uses the following equation:
    * angle = arccos(a*b/(|a||b|)) where a = ray1 and b = ray2.
    *
    * @param ray1 The first ray
    * @param ray2 The second ray
    * @return The angle between the two rays (in radians)
    */
  double angleBetweenRays(const cv::Point3d& ray1, const cv::Point3d& ray2) const;
  /**
    * Find 2D points relative to robots where stairs are detected
    *
    * This uses a method
    *
    * @param depth_msg The UInt16 encoded depth message.
    */
  template<typename T>
  void findCliffInDepthImage(const sensor_msgs::msg::Image::ConstSharedPtr& depth_msg);
  /**
    * Calculate vertical angle_min and angle_max by measuring angles between
    * the top ray, bottom ray, and optical center ray
    *
    * @param camModel The image_geometry camera model for this image.
    * @param min_angle The minimum vertical angle
    * @param max_angle The maximum vertical angle
    */
  void fieldOfView(double & min, double & max, double x1, double y1,
                   double xc, double yc, double x2, double y2);
  /**
   * @brief calcDeltaAngleForImgRows
   * @param vertical_fov
   */
  void calcDeltaAngleForImgRows(double vertical_fov);
  /**
    * @brief Calculates distances to ground for every row of depth image
    *
    * Calculates distances to ground for rows of depth image based on known height of sensor
    * mount and tilt angle. It assume that sensor height and tilt angle in relative to ground
    * is constant.
    *
    * Calculated values are placed in vector dist_to_ground_.
    */
  void calcGroundDistancesForImgRows(double vertical_fov);
  /**
   * @brief calcTiltCompensationFactorsForImgRows calculate factors used in tilt compensation
   */
  void calcTiltCompensationFactorsForImgRows();

 private:
  float    range_min_;            ///< Stores the current minimum range to use
  float    range_max_;            ///< Stores the current maximum range to use
  float    sensor_mount_height_;  ///< Height of sensor mount from ground
  float    sensor_tilt_angle_;    ///< Sensor tilt angle (degrees)
  bool     publish_depth_enable_; ///< Determines if depth should be republished
  bool     cam_model_update_;     ///< Determines if continuously cam model update required
  unsigned used_depth_height_;    ///< Used depth height from img bottom (px)
  unsigned block_size_;           ///< Square block (subimage) size (px).
  unsigned block_points_thresh_;  ///< Threshold value of points in block to admit stairs
  unsigned depth_image_step_row_; ///< Rows step in depth processing (px).
  unsigned depth_image_step_col_; ///< Columns step in depth processing (px).
  float    ground_margin_;        ///< Margin for ground points feature detector (m)

  bool depth_sensor_params_update;
  /// Class for managing sensor_msgs/CameraInfo messages
  image_geometry::PinholeCameraModel camera_model_;
  /// Calculated distances to ground for every row of depth image in mm
  std::vector<unsigned int> dist_to_ground_;
  std::vector<double> tilt_compensation_factor_;
  std::vector<double> delta_row_;

 public:
  sensor_msgs::msg::Image new_depth_msg_;

 private:
  /// Store points which contain obstacle
  geometry_msgs::msg::PolygonStamped stairs_points_msg_;
};

} // namespace cliff_detector
