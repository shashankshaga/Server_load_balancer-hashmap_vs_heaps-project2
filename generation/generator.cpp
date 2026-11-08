//IMPORTANT!!!
// WE GENERATED BOTH THE generator.cpp and this File Using claude code.
// These both files help generate the datapoints and store in the postgresql database thats it
// The prompt we used was "Help use generate 100000 data points and store them in the PostgreSQL database" using claude code

#include <libpq-fe.h>

#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <random>
#include <string>

const uint64_t SEED = 42;

const double BASE_RATE = 1000.0;     // requests per second
const double SPIKE_RATE = 10000.0;   // requests per second during a spike
const long long SPIKE_EVERY_MS = 60000;
const long long SPIKE_LENGTH_MS = 5000;

enum Kind { UNIFORM, SPIKE, HEAVY };

struct Workload {
    const char* name;
    const char* description;
    Kind kind;
};

const Workload WORKLOADS[] = {
    {"Uniform Traffic",
     "Steady ~1,000 req/s; every request takes 50-250 ms", UNIFORM},
    {"Traffic Spikes",
     "~1,000 req/s with a 10x burst for 5 s every 60 s; requests take 50-250 ms", SPIKE},
    {"Heavy-Tailed",
     "Steady ~1,000 req/s; 90% short (10-50 ms), 9% medium (100-500 ms), 1% heavy (1-5 s)", HEAVY},
};

// Random numbers built directly on mt19937_64, whose output is identical everywhere.
// (std::uniform_int_distribution etc. give different numbers on MSVC vs g++.)
class Rng {
    std::mt19937_64 gen;
public:
    explicit Rng(uint64_t seed) : gen(seed) {}

    double uniform01() {  // [0, 1)
        return (gen() >> 11) * (1.0 / 9007199254740992.0);
    }
    int randint(int lo, int hi) {  // [lo, hi]
        return lo + static_cast<int>(gen() % static_cast<uint64_t>(hi - lo + 1));
    }
    double exponential(double rate) {  // random gap with average 1/rate
        return -std::log(1.0 - uniform01()) / rate;
    }
};

double rateAt(Kind kind, double tMs) {
    if (kind == SPIKE && static_cast<long long>(tMs) % SPIKE_EVERY_MS < SPIKE_LENGTH_MS)
        return SPIKE_RATE;
    return BASE_RATE;
}

int processingTime(Kind kind, Rng& rng) {
    if (kind == HEAVY) {
        double r = rng.uniform01();
        if (r < 0.90) return rng.randint(10, 50);
        if (r < 0.99) return rng.randint(100, 500);
        return rng.randint(1000, 5000);
    }
    return rng.randint(50, 250);
}

// ---------- small libpq helpers ----------

void fail(PGconn* conn, const std::string& what) {
    std::cerr << "ERROR (" << what << "): " << PQerrorMessage(conn);
    PQfinish(conn);
    std::exit(1);
}

void run(PGconn* conn, const char* sql) {
    PGresult* res = PQexec(conn, sql);
    ExecStatusType s = PQresultStatus(res);
    if (s != PGRES_COMMAND_OK && s != PGRES_TUPLES_OK) {
        PQclear(res);
        fail(conn, sql);
    }
    PQclear(res);
}

std::string quoteConnValue(const std::string& v) {  // for password=... in the connection string
    std::string out = "'";
    for (char c : v) {
        if (c == '\'' || c == '\\') out += '\\';
        out += c;
    }
    return out + "'";
}

// ---------- main ----------

int main(int argc, char* argv[]) {
    long long count = 1000000;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--count" && i + 1 < argc) count = std::atoll(argv[++i]);
    }

    std::string conninfo = "host=localhost port=5432 dbname=load_balancer user=postgres";
    if (!std::getenv("PGPASSWORD")) {
        std::string password;
        std::cout << "PostgreSQL password: ";
        std::getline(std::cin, password);
        conninfo += " password=" + quoteConnValue(password);
    }

    PGconn* conn = PQconnectdb(conninfo.c_str());
    if (PQstatus(conn) != CONNECTION_OK) fail(conn, "connect");

    int index = 0;
    for (const Workload& w : WORKLOADS) {
        auto start = std::chrono::steady_clock::now();
        Rng rng(SEED + index++);

        run(conn, "BEGIN");

        // Create the workload, or reuse it if it already exists
        const char* params[2] = {w.name, w.description};
        PGresult* res = PQexecParams(conn,
            "INSERT INTO workloads (name, description) VALUES ($1, $2) "
            "ON CONFLICT (name) DO UPDATE SET description = EXCLUDED.description "
            "RETURNING workload_id",
            2, nullptr, params, nullptr, nullptr, 0);
        if (PQresultStatus(res) != PGRES_TUPLES_OK) { PQclear(res); fail(conn, "insert workload"); }
        std::string workloadId = PQgetvalue(res, 0, 0);
        PQclear(res);

        // Clear old requests so re-running doesn't hit duplicate keys
        run(conn, ("DELETE FROM requests WHERE workload_id = " + workloadId).c_str());

        // COPY streams all rows in one go: far faster than 1M separate INSERTs
        res = PQexec(conn,
            "COPY requests (workload_id, request_id, arrival_time_ms, processing_time_ms, priority) "
            "FROM STDIN");
        if (PQresultStatus(res) != PGRES_COPY_IN) { PQclear(res); fail(conn, "start COPY"); }
        PQclear(res);

        std::string buffer;
        buffer.reserve(1 << 20);
        double t = 0.0;
        for (long long requestId = 1; requestId <= count; ++requestId) {
            t += rng.exponential(rateAt(w.kind, t) / 1000.0);  // Poisson arrivals
            int proc = processingTime(w.kind, rng);
            int priority = rng.randint(1, 3);

            // One row = tab-separated values + newline (PostgreSQL COPY text format)
            buffer += workloadId + '\t' + std::to_string(requestId) + '\t' +
                      std::to_string(static_cast<long long>(t)) + '\t' +
                      std::to_string(proc) + '\t' + std::to_string(priority) + '\n';

            if (buffer.size() > (1 << 20) - 100) {  // send roughly every 1 MB
                if (PQputCopyData(conn, buffer.data(), static_cast<int>(buffer.size())) != 1)
                    fail(conn, "send rows");
                buffer.clear();
            }
        }
        if (!buffer.empty() &&
            PQputCopyData(conn, buffer.data(), static_cast<int>(buffer.size())) != 1)
            fail(conn, "send rows");
        if (PQputCopyEnd(conn, nullptr) != 1) fail(conn, "finish COPY");

        res = PQgetResult(conn);
        if (PQresultStatus(res) != PGRES_COMMAND_OK) { PQclear(res); fail(conn, "COPY"); }
        PQclear(res);

        run(conn, "COMMIT");

        double secs = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
        std::cout << "[" << workloadId << "] " << w.name << ": " << count
                << " requests in " << secs << "s\n";
    }

    // Quick summary to sanity-check the data
    PGresult* res = PQexec(conn,
        "SELECT w.workload_id, w.name, COUNT(r.request_id), "
        "       ROUND(MAX(r.arrival_time_ms) / 1000.0, 1), ROUND(AVG(r.processing_time_ms), 1) "
        "FROM workloads w LEFT JOIN requests r USING (workload_id) "
        "GROUP BY w.workload_id, w.name ORDER BY w.workload_id");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) { PQclear(res); fail(conn, "summary"); }

    std::cout << "\nworkload_id | name | requests | last arrival (s) | avg processing (ms)\n";
    for (int row = 0; row < PQntuples(res); ++row) {
        for (int col = 0; col < PQnfields(res); ++col)
            std::cout << (col ? " | " : "") << PQgetvalue(res, row, col);
        std::cout << "\n";
    }
    PQclear(res);
    PQfinish(conn);
    return 0;
}