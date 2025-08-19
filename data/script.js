// ESP32 LED Controller JavaScript

// Constants
const colorOrderNames = ["RGB", "GRB", "BRG", "BGR", "RBG", "GBR"];

// Initialize event listeners when page loads
document.addEventListener('DOMContentLoaded', function() {
    setupEventListeners();
    loadCurrentConfig();
});

// Setup all event listeners
function setupEventListeners() {
    // Color preview updates
    document.getElementById('color1').addEventListener('input', function() {
        document.getElementById('color1-preview').style.backgroundColor = this.value;
    });
    
    document.getElementById('color2').addEventListener('input', function() {
        document.getElementById('color2-preview').style.backgroundColor = this.value;
    });
    
    document.getElementById('color3').addEventListener('input', function() {
        document.getElementById('color3-preview').style.backgroundColor = this.value;
    });
}

// Utility Functions
function hexToRgb(hex) {
    const result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function rgbToHex(r, g, b) {
    return "#" + ((1 << 24) + (r << 16) + (g << 8) + b).toString(16).slice(1);
}

function showStatus(message, type = 'success') {
    const statusDiv = document.getElementById('status');
    statusDiv.innerHTML = `<div class="status ${type}">${message}</div>`;
    
    // Auto-hide status after 5 seconds
    setTimeout(() => {
        statusDiv.innerHTML = '';
    }, 5000);
}

function showError(message) {
    showStatus(message, 'error');
}

// API Communication Functions
async function apiRequest(url, method = 'GET', data = null) {
    try {
        const options = {
            method: method,
            headers: {
                'Content-Type': 'application/json'
            }
        };
        
        if (data) {
            options.body = JSON.stringify(data);
        }
        
        const response = await fetch(url, options);
        
        if (!response.ok) {
            throw new Error(`HTTP error! status: ${response.status}`);
        }
        
        return response;
    } catch (error) {
        console.error('API request failed:', error);
        throw error;
    }
}

// Color Testing Functions
async function testRed() {
    await testColor(255, 0, 0, "RED");
}

async function testGreen() {
    await testColor(0, 255, 0, "GREEN");
}

async function testBlue() {
    await testColor(0, 0, 255, "BLUE");
}

async function testColor(r, g, b, colorName) {
    const config = {
        testColor: true,
        color_r: r,
        color_g: g,
        color_b: b
    };

    try {
        const response = await apiRequest('/test-color', 'POST', config);
        const result = await response.text();
        showStatus(`Testing ${colorName} - ${result}`);
    } catch (error) {
        showError(`Error testing color: ${error.message}`);
    }
}

// Configuration Management
async function updateConfig(event) {
    event.preventDefault();
    
    try {
        // Gather form data
        const delay = document.getElementById('delay').value;
        const colorOrder = document.getElementById('colorOrder').value;
        const color1 = hexToRgb(document.getElementById('color1').value);
        const color2 = hexToRgb(document.getElementById('color2').value);
        const color3 = hexToRgb(document.getElementById('color3').value);

        // Validate inputs
        if (!color1 || !color2 || !color3) {
            throw new Error('Invalid color values');
        }

        if (delay < 100 || delay > 10000) {
            throw new Error('Delay must be between 100 and 10000 milliseconds');
        }

        const config = {
            delayMs: parseInt(delay),
            colorOrder: parseInt(colorOrder),
            color1_r: color1.r, color1_g: color1.g, color1_b: color1.b,
            color2_r: color2.r, color2_g: color2.g, color2_b: color2.b,
            color3_r: color3.r, color3_g: color3.g, color3_b: color3.b
        };

        const response = await apiRequest('/update', 'POST', config);
        const result = await response.text();
        showStatus(result);
        
        // Reload configuration to show updated values
        await loadCurrentConfig();
        
    } catch (error) {
        showError(`Error updating configuration: ${error.message}`);
    }
}

async function toggleLED() {
    try {
        const response = await apiRequest('/toggle', 'POST');
        const result = await response.text();
        showStatus(result);
        
        // Reload configuration to show updated status
        await loadCurrentConfig();
        
    } catch (error) {
        showError(`Error toggling LED: ${error.message}`);
    }
}

async function loadCurrentConfig() {
    try {
        // Show loading state
        document.getElementById('currentDelay').textContent = 'Loading...';
        document.getElementById('currentColorOrder').textContent = 'Loading...';
        document.getElementById('currentStatus').textContent = 'Loading...';
        
        const response = await apiRequest('/config');
        const config = await response.json();
        
        // Update form fields
        document.getElementById('delay').value = config.delayMs;
        document.getElementById('colorOrder').value = config.colorOrder;
        
        // Update status display
        document.getElementById('currentDelay').textContent = config.delayMs;
        document.getElementById('currentColorOrder').textContent = colorOrderNames[config.colorOrder] || 'Unknown';
        document.getElementById('currentStatus').textContent = config.enabled ? 'Running' : 'Stopped';
        
        // Update color pickers and previews
        const color1Hex = rgbToHex(config.color1_r, config.color1_g, config.color1_b);
        const color2Hex = rgbToHex(config.color2_r, config.color2_g, config.color2_b);
        const color3Hex = rgbToHex(config.color3_r, config.color3_g, config.color3_b);
        
        document.getElementById('color1').value = color1Hex;
        document.getElementById('color2').value = color2Hex;
        document.getElementById('color3').value = color3Hex;
        
        // Update all previews
        updateColorPreviews(color1Hex, color2Hex, color3Hex);
        
    } catch (error) {
        console.error('Error loading config:', error);
        showError('Failed to load configuration');
        
        // Reset loading state
        document.getElementById('currentDelay').textContent = 'Error';
        document.getElementById('currentColorOrder').textContent = 'Error';
        document.getElementById('currentStatus').textContent = 'Error';
    }
}

function updateColorPreviews(color1Hex, color2Hex, color3Hex) {
    // Update form previews
    document.getElementById('color1-preview').style.backgroundColor = color1Hex;
    document.getElementById('color2-preview').style.backgroundColor = color2Hex;
    document.getElementById('color3-preview').style.backgroundColor = color3Hex;
    
    // Update status section previews
    document.getElementById('preview1').style.backgroundColor = color1Hex;
    document.getElementById('preview2').style.backgroundColor = color2Hex;
    document.getElementById('preview3').style.backgroundColor = color3Hex;
}

// Keyboard shortcuts
document.addEventListener('keydown', function(event) {
    // Ctrl+S to save configuration
    if (event.ctrlKey && event.key === 's') {
        event.preventDefault();
        document.querySelector('form').dispatchEvent(new Event('submit'));
    }
    
    // Ctrl+R to refresh (allow default browser behavior)
    if (event.ctrlKey && event.key === 'r') {
        loadCurrentConfig();
    }
    
    // Space to toggle LED
    if (event.code === 'Space' && event.target.tagName !== 'INPUT') {
        event.preventDefault();
        toggleLED();
    }
});

// Auto-refresh configuration every 30 seconds
setInterval(loadCurrentConfig, 30000);

// Export functions for HTML onclick handlers
window.testRed = testRed;
window.testGreen = testGreen;
window.testBlue = testBlue;
window.updateConfig = updateConfig;
window.toggleLED = toggleLED;
window.loadCurrentConfig = loadCurrentConfig;