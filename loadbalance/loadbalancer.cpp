#include "loadbalancer.h"

#include <iostream>
#include <libpq-fe.h>
#include <cstdlib>
#include <string>
using namespace std;


vector<Request> loadRequests(int workloadId, int limit) {

    string connectInfo = "host=localhost port=5432 dbname=load_balancer user=postgres";
    if (!getenv("PGPASSWORD")) {
        string password;
        cout << "Enter PostgreSQL password: ";
        getline(cin, password);
        connectInfo += " password=" + password;
    }
    
    PGconn *conn = PQconnectdb(connectInfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) {
        cerr << "Connection to database failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
        exit(1);
        exit(1);
    }

    string query = "SELECT request_id, arrival_time_ms, processing_time_ms FROM requests WHERE workload_id = " + to_string(workloadId) + " ORDER BY arrival_time_ms, request_id LIMIT " + to_string(limit);
    PGresult *res = PQexec(conn, query.c_str());
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        cerr << "Query failed: " << PQerrorMessage(conn) << endl;
        PQclear(res);
        PQfinish(conn);
        exit(1);
    }

    vector<Request> requests;
    int rows = PQntuples(res);
    for (int i = 0; i < rows; ++i) {
        Request req;
        req.id = stoi(PQgetvalue(res, i, 0));
        req.arrivalTime = stoi(PQgetvalue(res, i, 1));
        req.processingTime = stoi(PQgetvalue(res, i, 2));
        requests.push_back(req);
    }

    PQclear(res);
    PQfinish(conn);
    return requests;
}