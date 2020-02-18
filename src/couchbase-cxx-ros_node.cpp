#include <couchbase-cxx-ros/couchbase-cxx-ros_node.h>



CouchbaseNode::CouchbaseNode(ros::NodeHandle node_handle):
   node_(node_handle)
{
    sub_cam_ = node_.subscribe("/camera/rgb/image_raw", 10, &CouchbaseNode::callbackCamera, this);
}

bool init = false;

Couchbase::Client couchbase_client_("couchbase://127.0.0.1/image_test_bucket", "275koMMa&", "Administrator");
Couchbase::Status rv = couchbase_client_.connect();



int counter_ = 0;

void CouchbaseNode::callbackCamera(sensor_msgs::Image::ConstPtr msg)
{
  if (init == false)
  {
    init =  connectServer();
  }
  else
  {

    /*
     * Command: "UPSERT"
     *
     * Used to insert a new record or update an existing one.
     * If the document doesn’t exist it will be created.
     * UPSERT is a combination of INSERT and UPDATE
     *
     */

    cv::Mat image(msg->width, msg->height, CV_8UC3);
    image = cv_bridge::toCvShare(msg, "bgr8")->image;
    cv::namedWindow("Display window", cv::WINDOW_AUTOSIZE);// Create a window for display.
    cv::imshow("Display window", image);
    cv::waitKey(1);

  // opencv mat to array
  std::vector<uchar> array(image.rows*image.cols);
  if (image.isContinuous())
    {
      array.assign(image.data, image.data + image.total());
    } else {
      for (int i = 0; i < image.rows; ++i) {
        array.insert(array.end(), image.ptr<uchar>(i), image.ptr<uchar>(i)+image.cols);
    }
  }


    std::string name ("image_");
    std::stringstream ss_name;
    ss_name << counter_;
    string str = ss_name.str();
    name.append(str);
    counter_ ++;

/*
    // create an empty structure (null)
    json j;

    // add a number that is stored as double (note the implicit conversion of j to an object)
    j["pi"] = 3.141;

    // add a Boolean that is stored as bool
    j["happy"] = true;

    // add a string that is stored as std::string
    j["name"] = "Niels";

    // add another null object by passing nullptr
    j["nothing"] = nullptr;

    // add an object inside the object
    j["answer"]["everything"] = 42;

    // add an array that is stored as std::vector (using an initializer list)
    j["list"] = { 1, 0, 2 };

    // add another object (using an initializer list of pairs)
    j["object"] = { {"currency", "USD"}, {"value", 42.99} };
*/
    // instead, you could also write (which looks very similar to the JSON above)
    json j2 = {
      {"pi", 3.141},
      {"happy", true},
      {"name", "Niels"},
      {"nothing", nullptr},
      {"answer", {
        {"everything", 0}
      }},
      {"list", {1, 0, 2}},
      {"object", {
        {"currency", "USD"},
        {"value", 42.99}
      }}
    };

    string json_string = j2.dump();
    //std::cout << j2.dump() << std::endl;
    auto cres = couchbase_client_.upsert(name, json_string);
    std::cout << "Got status for store. " << name << " .. Cas = " << std::hex << cres.cas() << endl;


    auto gres = couchbase_client_.get(name);
    json j3 = gres.value();

    std::vector<uchar> array_out(image.rows*image.cols);
    //std::cout << j3.dump() << std::endl;

    //auto pi = j3["pi"].get<double>();
    //std::cout << pi << std::endl;



  }
}

bool CouchbaseNode::connectServer()
{
  //! Connect to the client
  string connstr("couchbase://localhost/image_test_bucket");
  //string connstr("couchbase://10.0.0.50/image_test_bucket");
  string username("Administrator");
  string passwd("275koMMa&");

  Couchbase::Client couchbase_client_(connstr, passwd, username);
  Couchbase::Status rv = couchbase_client_.connect();
  if (!rv.success()) {
      std::cout << "Couldn't connect to '" << connstr << "'. " << "Reason: " << rv << endl;
      return false;
  }
  else
  {
    std::cout << "successfully connected to server" << std::endl;
    couchbase_ = couchbase_client_;
    return true;
  }
}

  /*
    gres = couchbase_client_.get("non-exist-key");
    std::cout << "Got status for non-existent key: " << gres.status() << endl;

    Couchbase::BatchCommand<Couchbase::UpsertCommand, Couchbase::StoreResponse> bulkStore(couchbase_client_);
    for (size_t ii = 0; ii < 10; ii++) {
        bulkStore.add("Key", "Value");
    }
    bulkStore.submit();
    couchbase_client_.wait();

    Couchbase::BatchCommand<Couchbase::GetCommand, Couchbase::GetResponse> bulkGet(couchbase_client_);
    for (size_t ii = 0; ii < 10; ii++) {
        bulkGet.add("Key");
    }
    bulkGet.submit();
    couchbase_client_.wait();

    Couchbase::CallbackCommand<Couchbase::GetCommand, Couchbase::GetResponse> cbGet(couchbase_client_, [](Couchbase::GetResponse& resp) {
        std::cout << "Got response for: " << resp.key() << endl;
        std::cout << "  Status: " << resp.status() << endl;
        std::cout << "  Value: " << resp.value() << endl;
    });

    cbGet.add("Key");
    cbGet.submit();
    couchbase_client_.wait();

    couchbase_client_.upsert("foo", "FOOVALUE");
    couchbase_client_.upsert("bar", "BARVALUE");
    couchbase_client_.upsert("baz", "BAZVALUE");

    //! Use durability requirements
    sres = couchbase_client_.upsert("toEndure", "toEndure");
    std::cout << "Endure Status: "
            << couchbase_client_.endure(Couchbase::EndureCommand("toEndure", sres.cas()),
                Couchbase::DurabilityOptions(Couchbase::PersistTo::MASTER)).status()
                << endl;

    //! Remove the items we just created
    couchbase_client_.remove("foo");
    couchbase_client_.remove("bar");
    couchbase_client_.remove("baz");

    string connstr2("couchbase://localhost/beer-sample");
    Couchbase::Client h1{connstr2, "275koMMa&", "Administrator"};
    Couchbase::Status rv1 = h1.connect();
    if (!rv1.success()) {
        std::cout << "Couldn't connect to '" << connstr2 << "'. " << "Reason: " << rv1 << endl;
        exit(EXIT_FAILURE);
    }
    Couchbase::Status status;
    Couchbase::ViewCommand vCmd("beer", "brewery_beers");
    vCmd.include_docs();
    vCmd.limit(10);
    vCmd.skip(10);
    vCmd.descending(true);
    vCmd.reduce(false);

    std::cout << "using options: " << vCmd.get_options() << endl;

    Couchbase::ViewQuery query(h1, vCmd, status);
    if (!status) {
        cerr << "Error with view command: " << status << endl;
    }

    size_t numRows = 0;
    for (auto ii : query) {
            std::cout << "Key: " << ii.key() << endl;
            std::cout << "Value: " << ii.value() << endl;
            std::cout << "DocID: " << ii.docid() << endl;

            if (ii.has_document()) {
                string value = ii.document().value();
                std::cout << "Document: " << value << endl;
            } else {
                std::cout << "NO DOCUMENT!" << endl;
            }
        numRows ++;
    }
    std::cout << "Got: " << std::dec << numRows << " rows" << endl;
    if (!query.status()) {
        cerr << "Problem with query: " << query.status() << endl;
    }

    std::cout << "Using async query.." << endl;
    // Async
    Couchbase::CallbackViewQuery asyncQuery(h1, vCmd, status,
        [](Couchbase::ViewRow&& row, Couchbase::CallbackViewQuery*) {
        std::cout << "Key: " << row.key() << endl;
        std::cout << "Value: " << row.value() << endl;
        std::cout << "DocID: " << row.docid() << endl;

        if (row.has_document()) {
            string value = row.document().value();
            std::cout << "Document: " << value << endl;
        } else {
            std::cout << "NO DOCUMENT!" << endl;
        }
    },[](Couchbase::ViewMeta&& meta, Couchbase::CallbackViewQuery*) {
        std::cout << "View complete: " << endl;
        std::cout << "  Status: " << meta.status() << endl;
        std::cout << "  Body: " << meta.body() << endl;
    });
    couchbase_client_.wait();

    // Issue the N1QL request:
    auto m = Couchbase::Query::execute(couchbase_client_, "CREATE PRIMARY INDEX ON `travel-sample`");
    std::cout << "Index creation: " << endl
         << "  Status: " << m.status() << endl
         << "  Body: " << m.body() << endl;

    // Get the count of airports per country:
    Couchbase::QueryCommand qcmd(
        "SELECT t.country, COUNT(t.country)"
        "FROM `travel-sample` t GROUP BY t.country");

    Couchbase::Query q(couchbase_client_, qcmd, status);
    if (!status) {
        std::cout << "Couldn't issue query: " << status;
    }
    for (auto row : q) {
        std::cout << "Row: " << row.json() << endl;
    }
    if (!q.status()) {
        std::cout << "Couldn't execute query: " << q.status() << endl;
        std::cout << "Body is: " << q.meta().body() << endl;
    }



}
*/

int main(int argc, char **argv)
{
  ros::init(argc, argv, "couchbase_node");

  ros::NodeHandle node_handle;
  CouchbaseNode couchbase(node_handle);

  ROS_INFO("Node is spinning...");
  ros::spin();

  return 0;
}
