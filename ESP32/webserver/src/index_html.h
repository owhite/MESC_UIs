
    #ifndef INDEX_HTML_H
    #define INDEX_HTML_H

    #include <pgmspace.h>

    const char index_html[] PROGMEM = R"rawliteral(
    <!DOCTYPE html>
<html lang="en">
  <head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>MESC Web Cal</title>
    <style>
      body {
          font-family: Arial, sans-serif;
          background-color: #f0f0f0;
          margin: 0;
          padding: 20px;
          display: flex;
          justify-content: center; /* Center the container horizontally */
      }
      #container {
          width: 500px; /* Set the container width to 300px */
          background-color: #fff;
          padding: 20px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      }
      #messages {
          background-color: #fff;
          border: 1px solid #ccc;
          padding: 10px;
          height: 300px;
          overflow-y: scroll;
      }
      .message {
          padding: 5px;
          border-bottom: 1px solid #ddd;
      }
      .parameter {
          margin-bottom: 10px;
      }
      .parameter textarea {
          width: 100%;
          box-sizing: border-box;
      }
      table {
          border-collapse: collapse; /* Optional: makes borders collapse into a single border */
          width: 100%; /* Ensure table width matches container width */
          margin-bottom: 20px; /* Add some space between tables */
          border: 1px solid #ddd;
      }
      th, td {
          border: 1px solid #ddd; /* Border around table cells */
          text-align: left; /* Align text to the left */
          width: 120px; /* Set fixed width for columns */
      }
      th {
          background-color: #f2f2f2; /* Background color for table headers */
      }
    </style>
  </head>
  <body>
    <div id="container">
      <h1>MESC Web Cal</h1>
      <table>
        <tr>
          <td>error</td>
          <td id="error_value">0</td>
        </tr>
        <tr>
          <td>error_all</td>
          <td id="error_all_value">134217744</td>
        </tr>
      </table>
      <table>
        <tr>
          <td>flux</td>
          <td id="flux_value">0.004500</td>
        </tr>
        <tr>
          <td>r_phase</td>
          <td id="r_phase_value">0.070000</td>
        </tr>
        <tr>
          <td>ld_phase</td>
          <td id="ld_phase_value">0.000043</td>
        </tr>
        <tr>
          <td>lq_phase</td>
          <td id="lq_phase_value">0.000064</td>
        </tr>
        <tr>
          <td>motor_pp</td>
          <td id="motor_pp_value">28</td>
        </tr>
        <tr>
          <td>pole_pairs</td>
          <td id="pole_pairs_value">28</td>
        </tr>
      </table>
      <table>
        <tr>
          <td>curr_max</td>
          <td id="curr_max_value">40.0</td>
        </tr>
        <tr>
          <td>curr_min</td>
          <td id="curr_min_value">-10.000000</td>
        </tr>
        <tr>
          <td>i_max</td>
          <td id="i_max_value">500.</td>
        </tr>
        <tr>
          <td>p_max</td>
          <td id="p_max_value">12000</td>
        </tr>
        <tr>
          <td>v_max</td>
          <td id="v_max_value">0.000000</td>
        </tr>
        <tr>
          <td>fw_curr</td>
          <td id="fw_curr_value">36.000000</td>
        </tr>
        <tr>
          <td>fw_ehz</td>
          <td id="fw_ehz_value">0.000000</td>
        </tr>
      </table>
      <table>
        <tr>
          <td>SL_sensor</td>
          <td id="SL_sensor_value">0</td>
        </tr>
        <tr>
          <td>motor_sensor</td>
          <td id="motor_sensor_value">0</td>
        </tr>
        <tr>
          <td>input_opt</td>
          <td id="input_opt_value">1</td>
        </tr>
        <tr>
          <td>ol_step</td>
          <td id="ol_step_value">0</td>
        </tr>
        <tr>
          <td>uart_req</td>
          <td id="uart_req_value">0.000000</td>
        </tr>
      </table>
      <table>
        <tr>
          <td>vbus</td>
          <td id="vbus_value">54.530788</td>
        </tr>
        <tr>
          <td>adc1</td>
          <td id="adc1_value">0</td>
        </tr>
        <tr>
          <td>Vd</td>
          <td id="Vd_value">0.017626</td>
        </tr>
        <tr>
          <td>Vq</td>
          <td id="Vq_value">0.024023</td>
        </tr>
        <tr>
          <td>TMOS</td>
          <td id="TMOS_value">298.987518</td>
        </tr>
        <tr>
          <td>TMOT</td>
          <td id="TMOT_value">210.618149</td>
        </tr>
      </table>
      <table>
        <tr>
          <td>adc1_max</td>
          <td id="adc1_max_value">2700</td>
        </tr>
        <tr>
          <td>adc1_min</td>
          <td id="adc1_min_value">600</td>
        </tr>
        <tr>
          <td>adc2_max</td>
          <td id="adc2_max_value">4095</td>
        </tr>
        <tr>
          <td>adc2_min</td>
          <td id="adc2_min_value">1200</td>
        </tr>
      </table>
      <br>
      <button onclick="sendButtonPress()">UPDATE</button>
      <br>
      <div id="messages"></div>
    </div>
    <script>
      var gateway = `ws://${window.location.hostname}/ws`;
      var websocket;
      window.addEventListener('load', onLoad);
      function initWebSocket() {
          console.log('Trying to open a WebSocket connection...');
          websocket = new WebSocket(gateway);
          websocket.onopen = onOpen;
          websocket.onclose = onClose;
          websocket.onerror = onError;
          websocket.onmessage = onMessage;
      }
      function onOpen(event) {
          console.log('Log message: Connection opened');
      }
      function onClose(event) {
          console.log('Connection closed', event);
          // Comment out the reload to diagnose the issue
          setTimeout(function() {
              location.reload();
          }, 2000);
      }
      function onError(event) {
          console.error('WebSocket error:', event);
      }
      function onMessage(event) {
          console.log('Message received:', event.data);
          var messagesDiv = document.getElementById('messages');
          var message = document.createElement('div');
          message.className = 'message';
          message.textContent = event.data;
          messagesDiv.appendChild(message);
          messagesDiv.scrollTop = messagesDiv.scrollHeight;
          try {
              var json = JSON.parse(event.data);
              updateParameter(json.parameter, json.value);
          } catch (e) {
              console.error('Error parsing JSON:', e);
          }
      }
      function updateParameter(parameter, value) {
          var valueElement = document.getElementById(parameter + "_value");
          if (valueElement) {
              valueElement.textContent = value;
          } else {
              console.warn('No element found for parameter:', parameter);
          }
      }
      function onLoad(event) {
          initWebSocket();
      }
      function sendButtonPress() {
          fetch('/button')
              .then(response => response.text())
              .then(data => console.log(data));
      }
    </script>
  </body>
</html>

    )rawliteral";

    #endif
    