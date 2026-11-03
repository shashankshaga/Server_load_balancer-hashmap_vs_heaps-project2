DROP TABLE IF EXISTS requests;
DROP TABLE IF EXISTS workloads;


CREATE TABLE workloads(

    workload_id SERIAL PRIMARY KEY, 
    name        VARCHAR(100) NOT NULL UNIQUE, 
    description TEXT,
    created_at  TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE requests(

    workload_id  INT REFERENCES workloads(workload_id),
    request_id   BIGINT NOT NULL, 
    arrival_time_ms BIGINT NOT NULL CHECK(arrival_time_ms >= 0 ),
    processing_time_ms INT NOT NULL CHECK(processing_time_ms >= 0),
    priority INT NOT NULL DEFAULT 1 CHECK (Priority BETWEEN 1 AND 3),

    PRIMARY KEY (workload_id, request_id)
);


CREATE INDEX index_requests_workload_priority 
    ON requests(workload_id, priority, request_id);


INSERT INTO workloads (name, description)
VALUES ('Clean Traffic', 'Clean and consistent processing times'),

INSERT INTO requests (workload_id, request_id, arrival_time_ms, processing_time_ms, priority)
VALUES
    (1, 1, 0, 120, 2),
    (1, 2, 10, 150, 1),
    (1, 3, 20, 100, 3);



SELECT request_id, arrival_time_ms, processing_time_ms, priority
FROM requests
WHERE workload_id = 1
ORDER BY arrival_time_ms, request_id;