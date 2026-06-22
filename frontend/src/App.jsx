import { useState, useEffect } from 'react'
import './App.css'

const API_BASE = 'http://localhost:3000/api';

const TYPE_NAMES = ['CAR', 'BIKE', 'EV', 'HANDICAPPED'];

function App() {
  const [slots, setSlots] = useState([]);
  const [activeTab, setActiveTab] = useState('park');
  const [parkType, setParkType] = useState('0');
  const [parkPlate, setParkPlate] = useState('');
  const [removePlate, setRemovePlate] = useState('');
  const [notification, setNotification] = useState(null);

  const fetchSlots = async () => {
    try {
      const res = await fetch(`${API_BASE}/slots`);
      const data = await res.json();
      if (data.status === 'success') {
        setSlots(data.data);
      }
    } catch (err) {
      console.error('Failed to fetch slots', err);
    }
  };

  useEffect(() => {
    fetchSlots();
    const interval = setInterval(fetchSlots, 2000);
    return () => clearInterval(interval);
  }, []);

  const handlePark = async (e) => {
    e.preventDefault();
    try {
      const res = await fetch(`${API_BASE}/park`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ type: parseInt(parkType), plateNumber: parkPlate })
      });
      const data = await res.json();
      if (data.status === 'success') {
        showNotification(`Success! Allocated Slot #${data.slotID}`, 'success');
        setParkPlate('');
        fetchSlots();
      } else {
        showNotification(`Error: ${data.message}`, 'error');
      }
    } catch (err) {
      showNotification('Network error connecting to server.', 'error');
    }
  };

  const handleRemove = async (e) => {
    e.preventDefault();
    try {
      const res = await fetch(`${API_BASE}/remove`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ plateNumber: removePlate })
      });
      const data = await res.json();
      if (data.status === 'success') {
        showNotification(`Success! Fee: ₹${data.fee.toFixed(2)}. Freed Slot #${data.slotID}`, 'success');
        setRemovePlate('');
        fetchSlots();
      } else {
        showNotification(`Error: ${data.message}`, 'error');
      }
    } catch (err) {
      showNotification('Network error connecting to server.', 'error');
    }
  };

  const showNotification = (msg, type) => {
    setNotification({ msg, type });
    setTimeout(() => setNotification(null), 5000);
  };

  const occupiedCount = slots.filter(s => s.isOccupied).length;
  const occupancyRate = slots.length > 0 ? Math.round((occupiedCount / slots.length) * 100) : 0;

  return (
    <div className="dashboard">
      <header className="header">
        <h1>Smart Parking Console</h1>
      </header>

      <div className="stats-grid">
        <div className="stat-card">
          <h3>Total Slots</h3>
          <div className="value">{slots.length || 20}</div>
        </div>
        <div className="stat-card">
          <h3>Occupied</h3>
          <div className="value" style={{color: occupiedCount > 0 ? '#ef4444' : 'inherit'}}>{occupiedCount}</div>
        </div>
        <div className="stat-card">
          <h3>Occupancy Rate</h3>
          <div className="value">{occupancyRate}%</div>
        </div>
      </div>

      <div className="main-content">
        <div className="slots-container">
          <div className="slots-grid">
            {slots.map(slot => (
              <div key={slot.slotID} className={`slot ${slot.isOccupied ? 'occupied' : 'free'}`}>
                <div className="slot-id">#{slot.slotID}</div>
                <div className="slot-type">{TYPE_NAMES[slot.allowedType]}</div>
                <div className="slot-status">{slot.isOccupied ? 'Occupied' : 'Free'}</div>
                {slot.isOccupied && (
                  <div className="slot-plate">{slot.plateNumber}</div>
                )}
              </div>
            ))}
            {slots.length === 0 && <p>Loading slots from backend...</p>}
          </div>
        </div>

        <div className="action-panel">
          <div className="tabs">
            <button 
              className={`tab ${activeTab === 'park' ? 'active' : ''}`}
              onClick={() => setActiveTab('park')}
            >
              Park Vehicle
            </button>
            <button 
              className={`tab ${activeTab === 'remove' ? 'active' : ''}`}
              onClick={() => setActiveTab('remove')}
            >
              Remove Vehicle
            </button>
          </div>

          {activeTab === 'park' ? (
            <form onSubmit={handlePark}>
              <h2>Park a Vehicle</h2>
              <div className="form-group">
                <label>Vehicle Type</label>
                <select 
                  className="form-control" 
                  value={parkType} 
                  onChange={e => setParkType(e.target.value)}
                >
                  <option value="0">Car</option>
                  <option value="1">Bike</option>
                  <option value="2">Electric Vehicle (EV)</option>
                  <option value="3">Handicapped</option>
                </select>
              </div>
              <div className="form-group">
                <label>License Plate Number</label>
                <input 
                  type="text" 
                  className="form-control" 
                  placeholder="e.g. MH12AB1234" 
                  required
                  value={parkPlate}
                  onChange={e => setParkPlate(e.target.value.toUpperCase())}
                />
              </div>
              <button type="submit" className="btn btn-primary">Allocate Slot</button>
            </form>
          ) : (
            <form onSubmit={handleRemove}>
              <h2>Remove a Vehicle</h2>
              <div className="form-group">
                <label>License Plate Number</label>
                <input 
                  type="text" 
                  className="form-control" 
                  placeholder="e.g. MH12AB1234" 
                  required
                  value={removePlate}
                  onChange={e => setRemovePlate(e.target.value.toUpperCase())}
                />
              </div>
              <button type="submit" className="btn btn-danger">Process Exit & Bill</button>
            </form>
          )}

          {notification && (
            <div className={`notification ${notification.type}`}>
              {notification.msg}
            </div>
          )}
        </div>
      </div>
    </div>
  )
}

export default App
