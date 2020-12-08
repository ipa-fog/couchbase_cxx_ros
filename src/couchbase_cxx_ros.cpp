#include <couchbase_cxx_ros/couchbase_cxx_ros.h>


#include <cassert>
#include <iostream>
#include <vector>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::vector;

using namespace Couchbase;

int main(int argc, const char **argv)
{
  // argument vector
  std::string username = "Administrator";
  std::string password = "275koMMa6";

  // Couchbase connect option
  std::string connstr = "couchbase://localhost/beer-sample";       // bucket name
  Couchbase::Client h1(connstr, password, username);


    Status rv1 = h1.connect();
    if (!rv1.success()) {
        cout << "Couldn't connect to '" << connstr << "'. " << "Reason: " << rv1 << endl;
        exit(EXIT_FAILURE);
    }
    Status status;
    ViewCommand vCmd("corsendonk", "brewery_beers");
    vCmd.include_docs();
    vCmd.limit(10);
    vCmd.skip(10);
    vCmd.descending(true);
    vCmd.reduce(false);

    cout << "using options: " << vCmd.get_options() << endl;

    ViewQuery query(h1, vCmd, status);
    if (!status) {
        cerr << "Error with view command: " << status << endl;
    }

    size_t numRows = 0;
    for (auto ii : query) {
            cout << "Key: " << ii.key() << endl;
            cout << "Value: " << ii.value() << endl;
            cout << "DocID: " << ii.docid() << endl;

            if (ii.has_document()) {
                string value = ii.document().value();
                cout << "Document: " << value << endl;
            } else {
                cout << "NO DOCUMENT!" << endl;
            }
        numRows ++;
    }
    cout << "Got: " << std::dec << numRows << " rows" << endl;
    if (!query.status()) {
        cerr << "Problem with query: " << query.status() << endl;
    }

    cout << "Using async query.." << endl;
    // Async
    CallbackViewQuery asyncQuery(h1, vCmd, status,
        [](ViewRow&& row, CallbackViewQuery*) {
        cout << "Key: " << row.key() << endl;
        cout << "Value: " << row.value() << endl;
        cout << "DocID: " << row.docid() << endl;

        if (row.has_document()) {
            string value = row.document().value();
            cout << "Document: " << value << endl;
        } else {
            cout << "NO DOCUMENT!" << endl;
        }
    },[](ViewMeta&& meta, CallbackViewQuery*) {
        cout << "View complete: " << endl;
        cout << "  Status: " << meta.status() << endl;
        cout << "  Body: " << meta.body() << endl;
    });

    return 0;
}
