import { useState } from 'react'

const TYPES = ["hashmap", "heap"];
const NAMES = { hashmap: "Hashmap", heap: "Heap" }
  


export default function App() {
  const [servers, setServers] = useState(20);
  const [requests, setRequests] = useState(100000);
  const [counts, setCounts] = useState(null);
  const [results, setResults] = useState(null);

  async function run() {
    setResults(null);
    const res = await fetch("http://localhost:3001/api/run", {
      method: "POST",
      headers: {
        "Content-Type": "application/json"
      },
      body: JSON.stringify({
        numServers: servers,
        requests: requests,
        workloadId: 2,
        animation: 200
      })
    });
    const data = await res.json();
    if (!res.ok) return alert(data.error);
    const placed = { hashmap : new Array(data.hashmap.servers).fill(0), heap: new Array(data.heap.servers).fill(0) }
    for (let s = 0; s < data.hashmap.trace.length; s++){
      for (const type of TYPES) {
        const step = data[type].trace[s];
        if (step) placed[type][step.server]++;
      }
      setCounts({ hashmap: [...placed.hashmap], heap: [...placed.heap] })
      await new Promise((r) => setTimeout(r, 50));
    }

    setResults(data);
  }

  return (
    <div className="app">
      <h1>Server Load Balancer Simulation </h1>
      <h3>HashMap vs Heap</h3>
      <div className = "card">
        <label>Servers: <b>{servers}</b></label>
        <input type="range" min="1" max="100" value={servers} onChange={(e) => setServers(Number(e.target.value))} />
        <label>Requests: <b>{requests}</b></label>
        <input type="range" min="10" max="1000000" step="1000" value={requests} onChange={(e) => setRequests(Number(e.target.value))} />
        <button onClick={run}>Run Simulation</button>
      </div>
      {counts && TYPES.map((type) => <Blocks key = {type} title = {NAMES[type]} counts={counts[type]} />)}
      {results && <Comparison results={results} />}
      
    </div>  
  )
}

function Blocks({ title, counts }) {
  const most = Math.max(...counts, 1);
  return (
    <div className="card blocks">
      <h4>{title}</h4>
      <div className="blocks">
      {counts.map((c, i) => (
        <div key={i} className="block" title={"Server " + i + ": " + c}>
          <div className="fill" style={{ height: `${(c / most) * 100}%` }}></div>
        </div>
      ))}
      </div>
    </div>
  );
}

function Comparison({ results }) {

  const {hashmap, heap} = results;
  const faster = hashmap.routingMs < heap.routingMs ? "HashMap" : "Heap";
  const ratio = Math.max(hashmap.routingMs, heap.routingMs) / Math.min(hashmap.routingMs, heap.routingMs);
  const rows = [
    ["Routing time", (r) => r.routingMs.toFixed(2) + " ms"
    ],
    ["Average wait", (r) => Math.round(r.avgWait).toLocaleString() + " ms"
    ],
    ["Longest wait", (r) => r.maxWait.toLocaleString() + " ms"
    ],
  ]

  return (
    <div className="card">
      <h3>Results</h3>
      <table>
        <thead>
          <tr>
            <th>Metric</th>
            <th>HashMap</th>
            <th>Heap</th>
          </tr>
        </thead>
        <tbody>
          {rows.map(([label, show]) => (
            <tr key={label}>
              <td>{label}</td>
              <td>{show(hashmap)}</td>
              <td>{show(heap)}</td>
            </tr>
          ))}
        </tbody>
      </table>
      <p className="verdict">
        {faster} is faster by a ratio of {ratio.toFixed(2)}
      </p>
    </div>
  );
}
