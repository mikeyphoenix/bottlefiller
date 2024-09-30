#ifndef HOME_PAGE_H
#define HOME_PAGE_H

const char homepage_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE HTML>
    <html>
    <head>
    <title>Pressure Sensor Reading</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial; text-align: center; margin: 0px auto; padding-top: 30px; }
        .button { padding: 10px 20px; font-size: 24px; text-align: center; outline: none; color: #fff; background-color: #0f8b8d; border: none; border-radius: 5px; }
        .button:hover { background-color: #0f8b8d; }
        .input { width: 80%; padding: 12px 20px; margin: 8px 0; box-sizing: border-box; }
    </style>
    </head>
    <body>
    <h2>Pressure Sensor Reading</h2>
    <p>Pressure: <span id="pressure">%PRESSURE%</span> PSI</p>
    <p>
        <input type="number" class="input" id="calibrationFactor" placeholder="Enter calibration factor">
        <button class="button" onclick="calibrate()">Calibrate</button>
    </p>
    </body>
    <script>
    setInterval(function() {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        document.getElementById("pressure").innerHTML = this.responseText;
        }
    };
    xhttp.open("GET", "/pressure", true);
    xhttp.send();
    }, 1000);

    function calibrate() {
    var factor = document.getElementById("calibrationFactor").value;
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4 && this.status == 200) {
        alert("Calibration factor updated");
        }
    };
    xhttp.open("GET", "/calibrate?factor=" + factor, true);
    xhttp.send();
    }
    </script>
    </html>
    )rawliteral";

#endif