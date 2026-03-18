cmake_minimum_required(VERSION 3.8)
project(px4_avoidance)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(px4_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(visualization_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)



include_directories(include)
# =========================
# VFH STAR NODE
# =========================

add_executable(vfh_star_node
  src/offboard_controll.cpp
  src/binary_histogram.cpp
  src/vfh_star_node.cpp
  src/histogram_grid.cpp
  src/lidar_processing.cpp
  src/polar_histogram.cpp
  src/candidate_search.cpp
  src/cost_function.cpp
)
ament_target_dependencies(vfh_star_node
  rclcpp
  px4_msgs
  sensor_msgs
  visualization_msgs
  nav_msgs
  geometry_msgs
  std_msgs
  tf2
)

# =========================
# ODOM TF NODE
# =========================

add_executable(odom_tf
  src/odom_tf.cpp
)
ament_target_dependencies(odom_tf
  rclcpp
  px4_msgs
  tf2_ros
  geometry_msgs
)

# =========================
# INSTALL
# =========================

install(TARGETS
  vfh_star_node
  odom_tf
  DESTINATION lib/${PROJECT_NAME}
)






if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  # the following line skips the linter which checks for copyrights
  # comment the line when a copyright and license is added to all source files
  set(ament_cmake_copyright_FOUND TRUE)
  # the following line skips cpplint (only works in a git repo)
  # comment the line when this package is in a git repo and when
  # a copyright and license is added to all source files
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()
