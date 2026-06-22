const express = require('express');
const cors = require('cors');
const fs = require('fs');
const { exec } = require('child_process');
const path = require('path');

const app = express();
app.use(cors());
app.use(express.json());

const PORT = 3000;
const PARKING_APP_DIR = path.join(__dirname, '..'); // PC_PBL
const SLOTS_FILE = path.join(PARKING_APP_DIR, 'slots_data.txt');
// Use the .exe if on Windows, else without .exe
const BINARY = process.platform === 'win32' ? 'parking_system.exe' : './parking_system';

// Helper to parse slots_data.txt
function readSlotsData() {
    if (!fs.existsSync(SLOTS_FILE)) {
        return [];
    }
    const data = fs.readFileSync(SLOTS_FILE, 'utf8');
    const lines = data.split('\n').filter(line => line.trim() !== '');
    // Skip header
    lines.shift();
    
    return lines.map(line => {
        const [slotID, allowedType, isOccupied, plateNumber, entryTime] = line.split(',');
        return {
            slotID: parseInt(slotID),
            allowedType: parseInt(allowedType),
            isOccupied: parseInt(isOccupied) === 1,
            plateNumber: plateNumber || '',
            entryTime: parseInt(entryTime)
        };
    });
}

// GET all slots
app.get('/api/slots', (req, res) => {
    try {
        const slots = readSlotsData();
        res.json({ status: 'success', data: slots });
    } catch (err) {
        res.status(500).json({ status: 'error', message: err.message });
    }
});

// POST park a vehicle
app.post('/api/park', (req, res) => {
    const { type, plateNumber } = req.body; // type is 0-3
    if (type === undefined || !plateNumber) {
        return res.status(400).json({ status: 'error', message: 'Missing type or plateNumber' });
    }

    const cmd = `${BINARY} --park ${type} "${plateNumber}"`;
    exec(cmd, { cwd: PARKING_APP_DIR }, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).json({ status: 'error', message: 'Failed to execute backend process. Do you have the compiled binary?' });
        }
        try {
            const result = JSON.parse(stdout);
            res.json(result);
        } catch (e) {
            res.status(500).json({ status: 'error', message: 'Invalid response from backend', raw: stdout });
        }
    });
});

// POST remove a vehicle
app.post('/api/remove', (req, res) => {
    const { plateNumber } = req.body;
    if (!plateNumber) {
        return res.status(400).json({ status: 'error', message: 'Missing plateNumber' });
    }

    const cmd = `${BINARY} --remove "${plateNumber}"`;
    exec(cmd, { cwd: PARKING_APP_DIR }, (error, stdout, stderr) => {
        if (error) {
            console.error(`exec error: ${error}`);
            return res.status(500).json({ status: 'error', message: 'Failed to execute backend process. Do you have the compiled binary?' });
        }
        try {
            const result = JSON.parse(stdout);
            res.json(result);
        } catch (e) {
            res.status(500).json({ status: 'error', message: 'Invalid response from backend', raw: stdout });
        }
    });
});

app.listen(PORT, () => {
    console.log(`Smart Parking Server running on http://localhost:${PORT}`);
});
