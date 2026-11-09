// We are running a server locally using Node.js express core

const express = require('express');
const cors = require('cors');
const path = require('path');
const fs = require('fs');
const {execFile} = require('child_process');

// Creates the server using express.js
const app = express();
app.use(cors());

app.use(express.json());

const PORT = 3001

// path to execute the load balancer executables
const HASHMAP_EXE = path.join(__dirname,'..','loadbalancer_hashmap.exe');
const HEAP_EXE = path.join(__dirname,"..", "loadbalancer_heap.exe");

function clamp(value, min, max) {
    return Math.min(Math.max(value, min), max);
}

// Executes the given executable with the provided arguments and returns the output as a promise
function runExe(exePath, args) {
    return new Promise((resolve, reject) => {
        execFile(exePath, args, {maxBuffer: 50 * 1024 * 1024, timeout: 120000}, (error, stdout, stderr) => {
            if (error) {
                return reject(error);
            }
            if (stderr) {
                console.error(stderr);
            }
            resolve(stdout);
        });
    });
}

app.post("/api/run", async (req, res) => {

    const args = [
        "--lookups", String(req.body.lookups ?? 0), 
        "--servers", String(req.body.numServers ?? 200), 
        "--limit", String(req.body.requests ?? 100000),
        "--workload", String(req.body.workloadId ?? 2),
        "--animation", String(req.body.animation ?? 30)
    ];

    try {
        // Parses the output as Json sent to the frontend for visualization
        const hashmap = JSON.parse(await runExe(HASHMAP_EXE, args))
        const heapReady = fs.existsSync(HEAP_EXE);
        let heap = hashmap;
        if (heapReady) {
            heap = JSON.parse(await runExe(HEAP_EXE, args))
        }
        res.json({
            hashmap,
            heap, heapReady
        });
    } catch (error) {
        console.error(error.message);
        res.status(500).json({error: "Failed to parse JSON output"});
    }
});

app.listen(PORT, () => {
    console.log(`Server is running on port ${PORT}`);
    if (!process.env.PGPASSWORD){
        console.warn("Warning: PGPASSWORD environment variable is not set.");

    }
});