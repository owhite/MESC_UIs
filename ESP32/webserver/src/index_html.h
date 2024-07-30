
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
      /* Existing styles... */
      #container {
          width: 400px;
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
          word-wrap: break-word;
      }
      .message {
          padding: 5px;
          border-bottom: 1px solid #ddd;
          word-wrap: break-word;
          white-space: pre-wrap;
      }
      .parameter {
          margin-bottom: 10px;
      }
      .parameter textarea {
          width: 100%;
          box-sizing: border-box;
      }
      table {
          border-collapse: collapse;
          width: 100%;
          margin-bottom: 20px;
          border: 3px solid #000;
      }
      th, td {
          border: 1px solid #ddd;
          text-align: left;
          width: 120px;
      }
      th {
          background-color: #f2f2f2;
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
          <td>pole_pairs</td>
          <td id="pole_pairs_value">28</td>
        </tr>
        <tr>
          <td>curr_max</td>
          <td id="curr_max_value">40.0</td>
        </tr>
        <tr>
          <td>p_max</td>
          <td id="p_max_value">12000</td>
        </tr>
        <tr>
          <td>fw_curr</td>
          <td id="fw_curr_value">36.000000</td>
        </tr>
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
        <tr>
          <td>vbus</td>
          <td id="vbus_value">54.530788</td>
        </tr>
        <tr>
          <td>adc1_max</td>
          <td id="adc1_max_value">2700</td>
        </tr>
        <tr>
          <td>adc1_min</td>
          <td id="adc1_min_value">600</td>
        </tr>
      </table>
      <br>
      <button onclick="sendCommand()">Send Command</button>
      <input type="text" id="command_input" placeholder="Enter command">
      <br>
      <br>
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
              updateTable(json);
          } catch (e) {
              console.error('Error parsing JSON:', e);
          }
      }
      function updateTable(data) {
          for (const [key, value] of Object.entries(data)) {
              var valueElement = document.getElementById(key + "_value");
              if (valueElement) {
                  valueElement.textContent = value;
              } else {
                  console.warn('No element found for key:', key);
              }
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
      function sendCommand() {
          var command = document.getElementById('command_input').value;
          if (command) {
              websocket.send(command);
              console.log('Sent command:', command);
          } else {
              console.warn('No command entered');
          }
      }
    </script>
  </body>
</html>

    )rawliteral";

    #endif
    