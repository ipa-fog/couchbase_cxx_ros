#include <libcouchbase/couchbase++.h>
#include <libcouchbase/couchbase++/views.h>
#include <libcouchbase/couchbase++/query.h>
#include <libcouchbase/couchbase++/endure.h>
#include <libcouchbase/couchbase++/logging.h>
#include <cassert>
#include <iostream>
#include <vector>
#include <ros/package.h>


#include <cv_bridge/cv_bridge.h>
#include <image_geometry/pinhole_camera_model.h>
#include <ros/ros.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/PointCloud2.h>
#include <visualization_msgs/Marker.h>
#include "ros/package.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

using std::cerr;
using std::endl;
using std::string;
using std::vector;


#include "opencv2/core/version.hpp"
#if CV_MAJOR_VERSION == 2
// do opencv 2 code
#elif CV_MAJOR_VERSION == 3
  #include <opencv2/core.hpp>
  #include <opencv2/imgcodecs.hpp>
  #include <opencv2/highgui.hpp>
#endif

class CouchbaseNode
{
  public:
  CouchbaseNode(ros::NodeHandle node_handle);


  private:
  ros::NodeHandle node_;

  ros::Publisher pub_marker_;
  ros::Subscriber sub_cam_;
  bool connectServer();
  void callbackCamera(sensor_msgs::Image::ConstPtr msg);


  Couchbase::Client couchbase_;

};
