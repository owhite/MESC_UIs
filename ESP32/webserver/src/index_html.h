
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
      body {
          font-family: Arial, sans-serif;
	  Color: #000088;
      }
      .tab-container {
          width: 240px;
          align-items: center;
          width: 100%;

      }
      .tab-buttons {
          background-color: #fff;
	  border: none;
	  color: white;
	  padding: 10px 20px;
	  text-align: center;
	  text-decoration: none;
	  display: inline-block;
	  font-size: 16px;
	  margin: 4px 2px;
	  border-radius: 4px;
      }
      .tab-content {
          display: none;
          width: 100%;
      }
      .tab-content.active {
          display: block;
      }
      #container1, #container2 {
          width: 360px;
          background-color: #fff;
          padding: 20px;
          box-shadow: 0 0 10px rgba(0, 0, 0, 0.1);
      }
      #messages {
          background-color: #fff;
          border: 1px solid #ccc;
          padding: 10px;
          height: 400px;
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
      .table-container {
          width: 100%;
          overflow-x: auto;
          white-space: nowrap;
          margin-bottom: 2px;
      }
      table {
          border-collapse: collapse;
          width: 100%;
          margin-bottom: 2px;
          border: 3px solid #000;
          display: inline-block; /* Keeps tables in a single row */
      }
      th, td {
          border: 1px solid #ddd;
          text-align: left;
          width: auto;
      }
      #editableTable th:nth-child(1), #editableTable td:nth-child(1),
      #additionalTable th:nth-child(1), #additionalTable td:nth-child(1) {
          width: 100px; /* adjust as needed */
      }
      #editableTable th:nth-child(2), #editableTable td:nth-child(2),
      #additionalTable th:nth-child(2), #additionalTable td:nth-child(2) {
          width: 200px; /* adjust as needed */
      }
      td.editing {
          padding: 0;
      }
      td input {
          width: 100%;
          box-sizing: border-box;
          padding: 8px;
          border: none;
          background-color: #fff;
      }
      /* Styles specific to bannerTable */
      .bannerTable {
          width: 100%;
          border-collapse: collapse;
          text-align: center;
          margin-bottom: 20px;
      }
      .bannerTable td {
          /* border: 1px solid #ddd; */ 
          padding: 16px;
          font-size: 24px;
      }
      .bannerTable .number {
          font-size: 20px;
          font-weight: bold;
      }
      .bannerTable .unit {
          font-size: 12px;
          color: #555;
      }
      /* Update button styles */
      #updateButton {
          color: black;
          padding: 10px;
          border: none;
          cursor: pointer;
      }
      @keyframes colorCycle {
          0% { background-color: #007bff; }
          25% { background-color: #6610f2; }
          50% { background-color: #6f42c1; }
          75% { background-color: #e83e8c; }
          100% { background-color: #dc3545; }
      }

      #updateButton.loading {
          animation: colorCycle 2s infinite;
      }
    </style>
  </head>
  <body>
    <h1>MESC Web Cal</h1>
    <div class="tab-container">
      <div class="tab-buttons">
        <button onclick="openTab('tab1')">Settings</button>
        <button onclick="openTab('tab2')">Messages</button>
      </div>
      <div id="tab1" class="tab-content">
        <div id="container1">
          <table class="bannerTable">
            <tr>
              <td>
                <div class="number" id="error_value">-</div>
                <div class="unit">Error</div>
              </td>
              <td>
                <div class="number" id="vbus_value">-</div>
                <div class="unit">vbus</div>
              </td>
              <td>
                <div class="number" id="ehz_value">-</div>
                <div class="unit">eHz</div>
              </td>
              <td>
                <div class="number" id="TMOT_value">-</div>
                <div class="unit">TMOT</div>
              </td>
              <td>
                <div class="number" id="TMOS_value">-</div>
                <div class="unit">TMOS</div>
              </td>
            </tr>
          </table>
          <div style="display: flex; align-items: center;">
            <button id="updateButton">UPDATE</button>
          </div>
          <br>
          <br>
          <div class="table-container">
            <!-- First table -->
            <table id="editableTable">
              <tr>
                <td>pole_pairs</td>
                <td id="pole_pairs_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>curr_max</td>
                <td id="curr_max_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>p_max</td>
                <td id="p_max_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>fw_curr</td>
                <td id="fw_curr_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>SL_sensor</td>
                <td id="SL_sensor_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>motor_sensor</td>
                <td id="motor_sensor_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>input_opt</td>
                <td id="input_opt_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>ol_step</td>
                <td id="ol_step_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>uart_req</td>
                <td id="uart_req_value" class="editable">NaN</td>
              </tr>
            </table>
            <!-- Additional table -->
            <table id="additionalTable">
              <tr>
                <td>adc1_max</td>
                <td id="adc1_max_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>adc1_min</td>
                <td id="adc1_min_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>adc2_max</td>
                <td id="adc2_max_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>adc2_min</td>
                <td id="adc2_min_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>flux</td>
                <td id="flux_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>r_phase</td>
                <td id="r_phase_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>ld_phase</td>
                <td id="ld_phase_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>lq_phase</td>
                <td id="lq_phase_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>motor_pp</td>
                <td id="motor_pp_value" class="editable">NaN</td>
              </tr>
            </table>
            <!-- Additional table -->
            <table id="additionalTable">
              <tr>
                <td>error_all</td>
                <td id="error_all_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>curr_min</td>
                <td id="curr_min_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>p_min</td>
                <td id="p_min_value" class="editable">NaN</td>
              </tr>
              <tr>
                <td>pwm_freq</td>
                <td id="pwm_freq_value" class="editable">NaN</td>
              </tr>
	      <tr>
		<td>fw_ehz</td>
		<td id="fw_ehz_value" class="editable">NaN</td>
	      </tr>
	      <tr>
		<td>adc1</td>
		<td id="adc1_value" class="editable">NaN</td>
	      </tr>
	      <tr>
		<td>Vd</td>
		<td id="Vd_value" class="editable">NaN</td>
	      </tr>
	      <tr>
		<td>Vq</td>
		<td id="Vq_value" class="editable">NaN</td>
	      </tr>
            </table>
          </div>
	  <br>
	  <button onclick="sendCommand()">Send</button>
	  <input type="text" id="command_input" placeholder="MESC command">
	  <br>
	  <br>
	  <button onclick="logCommand()">Log</button>

        </div>
      </div>
      <div id="tab2" class="tab-content">
        <div id="container2">
          <div id="messages"></div>
        </div>
      </div>
    </div>

    <div id="chart2a" style='width: 100%; height: 400px; position: relative'></div> 				

    <script>
      function openTab(tabId) {
          const tabs = document.querySelectorAll('.tab-content');
          tabs.forEach(tab => {
              tab.classList.remove('active');
          });
          document.getElementById(tabId).classList.add('active');
      }
      document.addEventListener('DOMContentLoaded', () => {
          openTab('tab1'); // Open the first tab by default
      });
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

      // xxx
      function onMessage(event) {
	  try {
              // Parse the incoming JSON string
              const data = JSON.parse(event.data);

        // Create a new message element
        const messageDiv = document.createElement('div');
        messageDiv.classList.add('message');

        // Format the message
        messageDiv.textContent = JSON.stringify(data, null, 2); // Pretty-print JSON data

        // Append the message to the #messages div
        document.getElementById('messages').appendChild(messageDiv);

        // Scroll to the bottom of the #messages div to show the latest message
        const messagesDiv = document.getElementById('messages');
        messagesDiv.scrollTop = messagesDiv.scrollHeight;


              // Check if the JSON object contains the "time" key
              if ('time' in data) {
		  // If "time" is present, call createGraph()
		  createGraph(data);
              } else {
		  // If "time" is not present, call updateTable()
		  updateTable(data);
              }
	  } catch (error) {
              console.error("Error parsing JSON:", error);
	  }
      }

      function truncateNumber(value, decimalPlaces) {
          if (isNaN(value)) return value; // Return as is if it's not a number
          return parseFloat(parseFloat(value).toFixed(decimalPlaces));
      }

      async function updateTable(data) {
          const entries = Object.entries(data);
          const totalEntries = entries.length;
          let count = 0;

          for (const [key, value] of entries) {
              let truncatedValue = value;
              switch (key) {
              case "ld_phase":
                  // truncatedValue = truncateNumber(value, 6);
                  break;
              case "lq_phase":
                  // truncatedValue = truncateNumber(value, 6);
                  break;
              case "flux":
                  // truncatedValue = truncateNumber(value, 6);
                  break;
              case "r_phase":
                  // truncatedValue = truncateNumber(value, 6);
                  break;
              case "Vq":
                  break;
              default:
                  truncatedValue = truncateNumber(value, 1);
                  break;
              }

              // Update editable table cells
              var valueElement = document.getElementById(key + "_value");
              if (valueElement) {
                  valueElement.textContent = truncatedValue;
              }

              // Update non-editable elements
              var nonEditableElement = document.getElementById(key);
              if (nonEditableElement) {
                  nonEditableElement.textContent = truncatedValue;
              }

              count++;
              // Test for last element and hide the spinner
              if (count === totalEntries) {
                  hideLoadingSpinner();
              }
          }
      }

      function onLoad(event) {
          initWebSocket();
          document.getElementById('command_input').addEventListener('keydown', function(event) {
              if (event.key === 'Enter') {
                  sendCommand();
              }
          });
          const tables = document.querySelectorAll('table');
          tables.forEach(table => {
              table.addEventListener('click', function(event) {
                  const target = event.target;
                  if (target.classList.contains('editable')) {
                      editCell(target);
                  }
              });
          });

          // Ensure the button has the correct ID and event listener
          const updateButton = document.getElementById('updateButton');
          updateButton.addEventListener('click', updateButtonPress);
      }

      function sendCommand() {
          var inputBox = document.getElementById('command_input');
          var command = inputBox.value;
          if (command) {
              websocket.send(command);
              console.log('Sent command:', command);
              // Clear the input box and reset the placeholder
              inputBox.value = '';
              inputBox.placeholder = 'MESC command';
          } else {
              console.warn('No command entered');
          }
      }

      function logCommand() {
          websocket.send("log_request");
      }

      function editCell(cell) {
          const currentValue = cell.textContent;
          cell.classList.add('editing');
          cell.innerHTML = `<input type="text" value="${currentValue}">`;
          const input = cell.querySelector('input');
          input.focus();
          input.addEventListener('blur', function() {
              saveCell(cell, input.value);
          });
          input.addEventListener('keydown', function(event) {
              if (event.key === 'Enter') {
                  saveCell(cell, input.value);
              }
          });
      }

      function saveCell(cell, newValue) {
          cell.classList.remove('editing');
          cell.textContent = newValue;
          const cellID = cell.id.slice(0, -6);
          const command = "set " + cellID + " " + newValue;
          websocket.send(command);
          onCellEdit(cell, newValue);
      }

      function onCellEdit(cell, newValue) {
          console.log('Cell edited:', cell, 'New value:', newValue);
          // Perform additional actions, e.g., sending data to the server
      }

      async function updateButtonPress() {
          console.log('Update button pressed');
          showLoadingSpinner(); // Show loading state
          try {
              const response = await fetch('/button');
              const data = await response.text();
              console.log('Response from server:', data);
          } catch (error) {
              console.error('Error:', error);
          } finally {
              // hideLoadingSpinner(); // Hide loading state
          }
      }

      function showLoadingSpinner() {
          const updateButton = document.getElementById('updateButton');
          updateButton.classList.add('loading'); // Add class to change button color
      }

      function hideLoadingSpinner() {
          const updateButton = document.getElementById('updateButton');
          updateButton.classList.remove('loading'); // Remove class to reset button color
      }


      // shout out to https://github.com/SK-SpeedBit/js_chart for his cool graphing code
      function createGraph(data) {

          var mydata_1 = data['Iu.I_phase.y1'];
          var mydata_2 = data['Iv.I_phase.y1'];
          var mydata_3 = data['Iw.I_phase.y1'];

          // Assuming 'desc' is part of the data or static
          var desc = data['time'] || ["00:00", "00:15", "00:30", /* your time labels */ ];
	  desc = desc.map(function(value) {
	      return value * 10000;
	  });

          console.log('create graph', mydata_1);
          console.log('create graph', desc);

          // Chart styles
          var mystyle_L1 = ["type=line", "linecolor=rgba(0, 150, 255, 1)", "fillcolor=rgba(0, 0, 255, 0.3)", "linewidth=1", "beziercurve=false"];
          var mystyle_L2 = ["type=line", "linecolor=rgba(255, 0, 0, 1)", "fillcolor=rgba(255, 0, 0, 0.3)", "linewidth=1", "beziercurve=false"];
          var mystyle_L3 = ["type=line", "linecolor=rgba(0, 255, 0, 1)", "fillcolor=rgba(0, 255, 0, 0.3)", "linewidth=1", "beziercurve=false"];

          // Initialize and draw the chart
          var dr2a = new js_chart('chart2a', [mydata_1, mydata_2, mydata_3], [mystyle_L1, mystyle_L2, mystyle_L3], desc, "t", "Amps");
          // var dr2a = new js_chart('chart2a', [mydata_1], [mystyle_L1], desc, "t", "Amps");
          // Additional chart configuration
          dr2a.drawaxis = false;
          dr2a.margv = 0;
          dr2a.margh = 0;
          dr2a.marsize = 5;
          dr2a.marw = 1.5;
          dr2a.crHint = true;
          dr2a.crYjumpM = true;
          dr2a.crYjumpP = true;
          dr2a.crXjump = true;
          dr2a.hintfpx = 12;
          dr2a.canvasbkcol = "rgba(0,0,0,1)";
          dr2a.axcol = "rgba(155,155,155,1)";
          dr2a.axdesccol = "rgba(255,255, 0, 1)";
          dr2a.descol = "rgba(250,250,0,0.7)";
          dr2a.drwfracol = "rgba(150,150,150,0.5)";
          dr2a.drwfilcol = "rgba(50,50,50, 0.0)";
          dr2a.mshcol = "rgb(200,200,200, 0.5)";
          dr2a.crlinecol = "rgb(255, 255,255)";
          dr2a.crlinewidth = 0.5;
          dr2a.meshlw = 0.5;
          dr2a.descfpx = 12;
          dr2a.crXjump = false;
          dr2a.crYjumpM = false;
          dr2a.crYjumpP = false;
          dr2a.hintpointfill = "rgba(0, 150, 150, 0.3)";
          dr2a.hintlinecol = "rgba(250, 250, 250, 1)";
          dr2a.hintpointcolfromdraw = true;
          dr2a.hintpointw = 4;
          dr2a.hintpointlwdth = 1;
          dr2a.hintselectcol = "rgba(200, 200, 200, 0.3)";
          dr2a.hintzoomcol = "rgba(0, 250, 250, 0.3)";
          dr2a.hintwithctrl = false;
          dr2a.legend("left", ["Iu", "Iv", "Iw"]);
          dr2a.vzonXaxis = true;
          dr2a.zoom = true;
          dr2a.draw();

      }


      </script>

    <script>
class js_chart{constructor(container,Ydata,Ystyle,Xdesc,XaxisTxt,YaxisTxt){this.onmouseclick=true;this.onmousemove=true;this.ontouchmove=true;this.onmousedown=true;this.onmouseup=true;this.onkeydown=true;this.onkeyup=true;this.oncontext=true;this.moveCursor="pointer";this.pointerCursor="crosshair";this.autoCursor="auto";this.downloadChartEnable=true;this.download_png_fname="chart.png";this.download_csv_fname="data.csv";this.download_csv_listsep=";";this.axisXtxt="x";this.axisYtxt="y";this.Ymax=0;this.Ymin=0;this.axisdecimalX=0;this.axisdecimalY=2;this.margv=5;this.margh=5;this.canvasbkcol="rgba(250, 250, 170, 0.4)";this.canvasfrw=2;this.canvasfrcol="rgba(0, 0, 255, 1)";this.marsize=8;this.marw=2;this.drawaxis=true;this.drawarrow=true;this.al=18;this.aw=9;this.axcol="rgba(0,0,0,1)";this.axw=2;this.axisXdesc=true;this.axisYdesc=true;this.axdesccol="rgba(0,100,200,1)";this.drawXdesc=true;this.drawYdesc=true;this.dgroup=true;this.drawmark=true;this.descol="rgba(0,0,255,1)";this.descfpx=15;this.descfontmod=" italic ";this.descfont="px Courier New";this.rotdescX=true;this.drw0x=true;this.drw0y=true;this.hmarshift=false;this.Yzoom=false;this.YonlyVisiblescale=true;this.repeatMouseMode=true;this.repeatPerDelay=500;this.repeatPerSlow=200;this.repeatPerSpeed=50;this.drawmesh=true;this.meshframe=true;this.mshcol="rgb(200,200,200, 0.7)";this.meshlw=1;this.chartzone=false;this.chartzonecol="rgba(120, 120, 120, 1)";this.drawzone=false;this.drawzfrm=true;this.drwfracol="rgba(150,  0,  0, 0.5)";this.drwfilcol="rgba(250,250,250, 0.3)";this.crXYline=true;this.crXYlineOnButton=true;this.crXjump=true;this.crYjumpM=false;this.crYjumpP=true;this.crHint=true;this.crpointhint=true;this.crpointfill=true;this.crlinetomouse=false;this.crlinetosmallhint=true;this.crlinecol="rgb(0,0,255)";this.crlinewidth=1;this.crXlinedash=[7,7];this.crYlinedash=[7,7];this.smallhint=true;this.hintwithctrl=true;this.hintfillcol="rgba(150, 200, 150, 0.7)";this.hintfillhitcol="rgba(200, 200, 200, 1)";this.hintframewidth=3;this.hintrectcol="rgba(50, 50, 50, 0.7)";this.hintfpx=11;this.hintfontmod=" italic ";this.hintfont="px Courier New";this.hinttxtcol="rgb(0, 0, 50)";this.hintpointw=5;this.hintlinecol="rgba(50, 50, 50, 1)";this.hintpointfill="rgba(50, 50, 150, 0.5)";this.hintpointcolfromdraw=true;this.hintpointlwdth=1;this.hintcolfrwdth=.5;this.hintlinetoMcol="rgba(250, 0, 0, 1)";this.hintlinetoMwidth=.5;this.hintmaxalpha=false;this.hintdatashadow=true;this.hintshadowcol="rgba(150, 150, 150, 0.2)";this.linecol="rgba(250, 0, 0, 0.6)";this.linewidth=2;this.linepoints=false;this.linepointsize=4;this.linepointcol="rgba(250, 0, 0, 0.7)";this.arealine="rgb(250, 0, 250)";this.areafill="rgba(250, 0, 250, 0.3)";this.areawidth=2;this.areapoints=false;this.areapointsize=5;this.areapointcol="rgb(250, 0, 250)";this.beziercurve=true;this.beziercnst=3;this.bezierlvloff=3;this.barline="rgb(0, 250, 250)";this.barfill="rgba(0, 250, 250, 0.3)";this.barlw=1;this.barpoints=false;this.barpointsize=5;this.barpointcol="rgba(0, 250, 250, 0.3)";this.barperc=.75;this.boldonover=true;this.lineBoldAlpha=1;this.lineBoldWidth=3;this.barBoldLineWidth=3;this.barBoldAlpha=.8;this.areaBoldLineWidth=3;this.areaBoldLineAlpha=1;this.areaBoldAreaAlpha=.6;this.stairsBoldLineWidth=3;this.stairsBoldLineAlpha=1;this.hideonclick=true;this.inactivechartboxcol="rgb(100, 100, 100)";this.bigmax=1e4;this.allmaxmin=true;this.multimsdown=true;this.multimsdowncnt=3;this.multimstout=1e3;this.multimsdowncol="rgba(0, 0, 0, 0.15)";this.hintselectshadow=true;this.hintselectcol="rgba(0, 0, 0, 0.3)";this.mouseclickLR=true;this.LRsize=1/6;this.hintzoomcol="rgba(0, 250, 0, 0.15)";this.undozoompx=20;this.movezoombymouse=true;this.revarrowkeysinzoom=false;this.revmousemoveinzoom=false;this.vsOn=true;this.vzonXaxis=!this.drawaxis;this.vzcolor=this.axdesccol;this.vzalpha=.8;this.vzwdtsm=this.axw;this.vzwdtlg=this.aw;this.legframew=1;this.legframecol="rgba(250, 250, 250, 0.7)";this.legfillcol="rgba(150, 150, 150, 0.5)";this.legpx=12;this.legfontmod=" italic ";this.legfont="px Courier New";this.legtxtcol="rgba(  0, 250, 250, 1)";this.XaxisLiveValue=false;this.YaxisLiveValue=false;this.liveValueback="rgba(200, 200, 200, 0.8)";this.liveValueColor="rgba(0, 0, 0, 1)";this.showAllValues=false;this.textvalue=true;this.textvalueoffset=1;this.formattxtvalues=true;this.livevalfpx=12;this.livevalfontmod=" italic ";this.livevalfont="px Courier New";this.valuedecimal=2;this.zcorr=1;this.repeatMousePer=0;this.islegend=false;this.legmarg=5;this.legpos=0;this.legstr=[];this.legtop=0;this.legleft=0;this.legbottom=0;this.legright=0;if(typeof container=="undefined")return-1;if(typeof Ydata=="undefined")return-2;if(typeof Ystyle=="undefined")return-3;this.data=Ydata;this.style=Ystyle;this.desc=Xdesc;if(typeof XaxisTxt!="undefined")this.axisXtxt=XaxisTxt;if(typeof YaxisTxt!="undefined")this.axisYtxt=YaxisTxt;this.container=container;this.ctx=null;this.ctxl2=null;this.canvas=null;this.layer2=null;this.canvas=document.createElement("canvas");this.canvas.id="canvas";this.layer2=document.createElement("canvas");this.layer2.id="layer2";this.canvas.style.zindex=1;this.layer2.style.zindex=2;this.canvas.style.order="1";this.layer2.style.order="2";this.layer2.style.position="absolute";this.canvas.style.position="absolute";document.getElementById(container).appendChild(this.canvas);document.getElementById(container).appendChild(this.layer2);this.ctx=this.canvas.getContext("2d");this.ctxl2=this.layer2.getContext("2d");this.alldatalength=0;this.datalength=0;this.from=0;this.zoom=false;if(Array.isArray(this.data)){for(let i=0;i<this.data.length;i++)if(typeof this.data[i]!="undefined"&&this.alldatalength<this.data[i].length)this.alldatalength=this.data[i].length}this.datalength=this.alldatalength;this.to=this.alldatalength;this.eventsactivated=false;this.pointclicked=false;this.xdiv=0;this.wght=0;this.barcnt=0;this.barnr=0;this.marhpx=0;this.zlvl=0;this.lvlv=this.margh+this.marsize*2*this.drawmark+this.legleft*this.islegend;this.dcorr=1;this.zoffs=0;this.aYtxt="Y";this.lft=0;this.top=0;this.rgt=0;this.bot=0;this.zerox=0;this.drt=0;this.drb=0;this.drl=0;this.drr=0;this.drv=0;this.allpoints=false;this.marpx=0;this.marmx=0;this.marax=0;this.marpl=0;this.marmi=0;this.marall=0;this.oldMouseEv=null;this.marxdata={cnt:0,div:1};this.internal_mose_flag=false}legend(pos,leg){var marg=0;var txt="";var txtw=0;function roundRect(ctx,x,y,width,height,radius,fill,stroke){if(typeof stroke=="undefined")stroke=true;if(typeof radius=="undefined")radius=5;if(typeof radius=="number")radius={tl:radius,tr:radius,br:radius,bl:radius};else{var defaultRadius={tl:0,tr:0,br:0,bl:0};for(var side in defaultRadius)radius[side]=radius[side]||defaultRadius[side]}ctx.beginPath();ctx.moveTo(x+radius.tl,y);ctx.lineTo(x+width-radius.tr,y);ctx.quadraticCurveTo(x+width,y,x+width,y+radius.tr);ctx.lineTo(x+width,y+height-radius.br);ctx.quadraticCurveTo(x+width,y+height,x+width-radius.br,y+height);ctx.lineTo(x+radius.bl,y+height);ctx.quadraticCurveTo(x,y+height,x,y+height-radius.bl);ctx.lineTo(x,y+radius.tl);ctx.quadraticCurveTo(x,y,x+radius.tl,y);ctx.closePath();if(fill)ctx.fill();if(stroke)ctx.stroke()}function getStyle(style,name,defvalue){let s;if(typeof style=="undefined")return defvalue;for(let i=0;i<style.length;i++){if(typeof style[i]=="undefined")continue;s=style[i].split("=");if(s[0]==name)return s[1]}return defvalue}function changeRGBAalpha(rgba,alpha){if(typeof rgba=="undefined")return rgba;if(!rgba.includes("rgba"))return rgba;let str=rgba.split(",");str[3]=alpha+")";return str[0]+","+str[1]+","+str[2]+","+str[3]}function colRect(t,i,x,y){if(typeof t.style[i]=="undefined")return;t.ctx.save();t.ctx.beginPath();t.ctx.strokeStyle="rgba(0,0,0,1)";t.ctx.fillStyle=getStyle(t.style[i],"linecolor",getStyle(t.style[i],"fillcolor",t.areafill));t.ctx.lineWidth=t.hintcolfrwdth;if(getStyle(t.style[i],"visible","")=="NO")t.ctx.fillStyle=t.inactivechartboxcol;if(getStyle(t.style[i],"boldover","---")=="YES"){t.ctx.lineWidth=t.hintcolfrwdth+4;t.ctx.strokeStyle=changeRGBAalpha(t.ctx.fillStyle,1);t.ctx.fillStyle=changeRGBAalpha(t.ctx.fillStyle,1)}t.ctx.globalAlpha=1;t.ctx.fillRect(x,y-t.legpx,t.legpx,t.legpx);t.ctx.rect(x,y-t.legpx,t.legpx,t.legpx);t.ctx.closePath();t.ctx.stroke();t.ctx.restore()}if(typeof this.data=="undefined")return;if(typeof pos=="undefined"&&typeof leg=="undefined"&&this.islegend==false)return;var lw=0;if(typeof pos!="undefined"&&typeof leg!="undefined"){this.islegend=true;this.legstr=leg;this.legpos=pos;if(pos=="top")this.legpos=0;if(pos=="right")this.legpos=1;if(pos=="bottom")this.legpos=2;if(pos=="left")this.legpos=3;this.legtop=0;this.legright=0;this.legbottom=0;this.legleft=0;this.ctx.save();this.ctx.font=this.legfontmod+this.legpx+this.legfont;lw=0;txtw=0;if(this.legpos==1||this.legpos==3){for(let i=0;i<this.legstr.length;i++){if(typeof this.data[i]=="undefined")continue;lw=this.ctx.measureText(this.legstr[i]).width;if(lw>txtw)txtw=lw}}this.ctx.restore();if(this.legpos==0)this.legtop=this.legpx*2+this.legframew*2+this.legmarg;if(this.legpos==1)this.legright=txtw+this.legpx*2.2+this.legframew*2+this.legmarg;if(this.legpos==2)this.legbottom=this.legpx*2+this.legframew*2+this.legmarg;if(this.legpos==3)this.legleft=txtw+this.legpx*2.2+this.legframew*2+this.legmarg;for(let i=0;i<this.legstr.length;i++){if(typeof this.legstr[i]=="undefined"||this.legstr[i]==null||this.legstr[i]=="")this.legstr[i]=(i+1).toString()}return}this.ctx.save();this.ctx.beginPath();marg=this.legpx/2;this.ctx.lineWidth=this.legframew;this.ctx.strokeStyle=this.legframecol;this.ctx.fillStyle=this.legfillcol;this.ctx.font=this.legfontmod+this.legpx+this.legfont;var cl=0;var wd=0;var lf=0;var frmy=0;var recy=0;if(this.legpos==0||this.legpos==2){cl=0;for(let i=0;i<this.legstr.length;i++){if(typeof this.data[i]=="undefined")continue;cl++;txt+=this.legstr[i]}txtw=this.ctx.measureText(txt).width+cl*this.legpx*2.2-this.legpx;wd=txtw+marg*2;lf=(this.ctx.canvas.width-wd)/2;var txty=0;recy=0;frmy=0;if(this.legpos==0)frmy=this.top-this.legpx*2-this.legframew-this.legmarg;else frmy=this.bot+this.legframew*2+this.legmarg;recy=frmy+this.legpx/.7;txty=frmy+this.legpx/.8;let txtx=lf+marg;roundRect(this.ctx,lf,frmy,wd,this.legpx*1.8,8,true,true);this.ctx.font=this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=this.legtxtcol;for(let i=0;i<this.data.length;i++){if(this.data[i]==null||typeof this.data[i]=="undefined"||typeof this.legstr[i]=="undefined")continue;colRect(this,i,txtx,recy);txtx+=this.legpx*1.2;if(getStyle(this.style[i],"boldover","---")=="YES"){this.ctx.font="Bold "+this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=getStyle(this.style[i],"linecolor",getStyle(this.style[i],"fillcolor",this.areafill))}else{this.ctx.font=this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=this.legtxtcol}this.ctx.fillText(this.legstr[i],txtx,txty);txtx+=this.ctx.measureText(this.legstr[i]).width+this.legpx*1}}if(this.legpos==1||this.legpos==3){txtw=0;lw=0;cl=0;for(let i=0;i<this.legstr.length;i++){lw=this.ctx.measureText(this.legstr[i]).width;if(typeof this.data[i]=="undefined")continue;cl++;if(lw>txtw)txtw=lw}wd=txtw+this.legpx*2.2;lf=0;if(this.legpos==1)lf=this.rgt+this.legframew+this.legmarg;else lf=this.margh+this.legframew;frmy=(this.ctx.canvas.height-this.legpx*(this.legstr.length+1))/2;let txtx=lf+marg;roundRect(this.ctx,lf,frmy,wd,this.legpx*(cl+1),8,true,true);recy=frmy+this.legpx*1.5;this.ctx.font=this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=this.legtxtcol;for(let i=0;i<this.data.length;i++){if(this.data[i]==null||typeof this.data[i]=="undefined"||typeof this.legstr[i]=="undefined")continue;colRect(this,i,txtx,recy);if(getStyle(this.style[i],"boldover","---")=="YES"){this.ctx.font="Bold "+this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=getStyle(this.style[i],"linecolor",getStyle(this.style[i],"fillcolor",this.areafill))}else{this.ctx.font=this.legfontmod+this.legpx+this.legfont;this.ctx.fillStyle=this.legtxtcol}this.ctx.fillText(this.legstr[i],txtx+this.legpx*1.2,recy-this.legpx/5);recy+=this.legpx}}this.ctx.closePath();this.ctx.restore()}update(Ydata,Ystyle,Xdesc,XaxisTxt,YaxisTxt){if(typeof Ydata!="undefined"&&Ydata!=null)this.data=Ydata;if(typeof Ystyle!="undefined"&&Ystyle!=null)this.style=Ystyle;if(typeof Xdesc!="undefined"&&Xdesc!=null)this.desc=Xdesc;if(typeof XaxisTxt!="undefined"&&XaxisTxt!=null)this.axisXtxt=XaxisTxt;if(typeof YaxisTxt!="undefined"&&YaxisTxt!=null)this.axisYtxt=YaxisTxt;this.alldatalength=0;if(Array.isArray(this.data)){for(let i=0;i<this.data.length;i++)if(typeof this.data[i]!="undefined"&&this.alldatalength<this.data[i].length)this.alldatalength=this.data[i].length}if(this.datalength>this.alldatalength){this.from=0;this.to=this.alldatalength;this.zoom=false}this.allpoints=false;this.draw()}home(){if(!this.zoom)return;let size=this.to-this.from;this.from=0;this.to=size;this.draw()}end(){if(!this.zoom)return;let size=this.to-this.from;this.from=this.alldatalength-size;this.to=this.alldatalength;this.draw()}left(){if(!this.zoom)return;if(this.from==0)return;this.from--;this.to--;this.draw()}right(){if(!this.zoom)return;if(this.to>=this.alldatalength)return;this.from++;this.to++;this.draw()}draw_FromTo(from,to){if(from<0||from>=this.alldatalength)return-1;if(to<0||to>this.alldatalength)return-1;if(from>to||from==to)return-1;this.from=from;this.to=to;this.zoom=true;if(this.from==0&&this.to==this.alldatalength)this.zoom=false;this.ctxl2.canvas.style.cursor=this.pointerCursor;this.draw()}draw_FromCount(from,cnt){if(from<0||from>this.alldatalength-2)return-1;if(cnt<=1)return-1;if(from+cnt>this.alldatalength)cnt=this.alldatalength-from;this.from=from;this.to=from+cnt;this.zoom=true;if(this.from==0&&this.to==this.alldatalength)this.zoom=false;this.ctxl2.canvas.style.cursor=this.pointerCursor;this.draw()}undozoom(){this.from=0;this.to=this.alldatalength;this.datalength=this.alldatalength;this.zoom=false;this.ctxl2.canvas.style.cursor=this.autoCursor;this.draw()}clear(){this.ctx.clearRect(0,0,this.ctx.canvas.width,this.ctx.canvas.height);this.ctxl2.clearRect(0,0,this.ctxl2.canvas.width,this.ctxl2.canvas.height)}redraw(){this.draw()}draw(){let cv=document.getElementById(this.container);this.canvas.style.left=0;this.canvas.style.top=0;this.canvas.width=cv.offsetWidth;this.canvas.height=cv.offsetHeight;this.layer2.style.left=0;this.layer2.style.top=0;this.layer2.width=cv.offsetWidth;this.layer2.height=cv.offsetHeight;this.zlvl=0;this.lvlv=this.margh+this.marsize*2*this.drawmark+this.legleft*this.islegend;this.zlvl=this.drb-this.zerox;this.lft=this.margh+this.legleft*this.islegend;this.top=this.margv+this.legtop*this.islegend;this.rgt=this.ctx.canvas.clientWidth-this.margh-this.legright*this.islegend;this.bot=this.ctx.canvas.clientHeight-this.margv-this.legbottom*this.islegend;this.zerox=0;this.drt=this.top+(this.drawaxis?this.al*1.5:0);this.drb=this.bot-this.marsize*this.drawmark;this.drl=this.lvlv;this.drr=this.rgt-(this.drawaxis?this.al*1.5:0);this.drv=this.drb-this.drt;this.make_chart()}make_chart(){let self=this;let minv=0;let maxv=0;let rangev={minv:0,maxv:0};let marpldata={cnt:0,factor:1,weight:1};let marmidata={cnt:0,factor:1,weight:1};let maxhm=0;let marh=0;let vmp=0;let vmm=0;function getMinMax(arr){if(typeof arr=="undefined")return{min:0,max:0};let min=Infinity;let max=-Infinity;let start=0;let stop=arr.length;if(self.zoom&&!self.allmaxmin){start=self.from;stop=self.to}for(let i=start;i<stop;i++){if(typeof arr[i]=="undefined"||arr[i]==null)continue;min=arr[i]<min?arr[i]:min;max=arr[i]>max?arr[i]:max}return{min:min,max:max}}function getMaxMarkerValue(maxvalue,marcntmax){if(marcntmax==0||maxvalue==0)return{cnt:1,factor:0,weight:0,value:0};maxvalue=Math.abs(maxvalue);marcntmax=Math.abs(marcntmax);let weight=1;let factor=0;let marcntact=0;if(maxvalue>1){do{factor=.1;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=.2;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=.25;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=.5;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=1;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=2;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=2.5;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;factor=5;marcntact=Math.abs(maxvalue/(factor*weight));if(marcntact<=marcntmax)break;weight=weight*10}while(marcntact>marcntmax)}else{do{factor=8;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=7.5;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=6;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=5;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=2.5;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=2;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;factor=1;marcntact=Math.abs(maxvalue*weight/factor);if(marcntact>=marcntmax)break;weight=weight*10}while(marcntact<marcntmax);if(Math.abs(maxvalue*weight/factor)>marcntmax){if(factor==8){factor=1;weight=weight/10}if(factor==7.5){factor=8}if(factor==7){factor=7.5}if(factor==5){factor=7}if(factor==2.5){factor=5}if(factor==2){factor=2.5}if(factor==1){factor=2}marcntact=Math.abs(maxvalue*weight/factor)}weight=1/weight}return{cnt:marcntact,factor:factor,weight:weight,value:factor*weight}}function getXmarkersDiv(data_cnt,maxhm){let marcnt=data_cnt;let div=1;let weight=1;do{div=1;marcnt=Math.abs(data_cnt/(div*weight));if(marcnt<=maxhm)break;div=2;marcnt=Math.abs(data_cnt/(div*weight));if(marcnt<=maxhm)break;div=5;marcnt=Math.abs(data_cnt/(div*weight));if(marcnt<=maxhm)break;weight=weight*10}while(marcnt>maxhm);return{cnt:marcnt,div:div*weight}}function getStyle(style,name,defvalue){let s;if(typeof style=="undefined")return defvalue;for(let i=0;i<style.length;i++){if(typeof style[i]=="undefined")continue;s=style[i].split("=");if(s[0]==name)return s[1]}return defvalue}function changeRGBAalpha(rgba,alpha){if(typeof rgba=="undefined")return rgba;if(!rgba.includes("rgba"))return rgba;let str=rgba.split(",");str[3]=alpha+")";return str[0]+","+str[1]+","+str[2]+","+str[3]}function do_line_graph(data,style){if(typeof data=="undefined")return;let w1=0;let w2=0;if(maxv!=0){w1=Math.abs((self.zlvl-self.drt)/maxv)}else w1=0;if(minv!=0){w2=Math.abs((self.drb-self.zlvl)/minv)}else w2=0;self.wght=Math.max(w1,w2);let loclinecolor=getStyle(style,"linecolor",self.linecol);let loclinewidth=getStyle(style,"linewidth",self.linewidth);let loclinepoints=getStyle(style,"points",self.linepoints).toString()=="true";let locpointsize=getStyle(style,"pointsize",self.linepointsize);let locpointcolor=getStyle(style,"pointcolor",self.linepointcol);let locbeziercurve=getStyle(style,"beziercurve",self.beziercurve).toString()=="true";let locbezierconst=getStyle(style,"beziercnst",self.beziercnst);if(getStyle(style,"pointcolor",null)==null)locpointcolor=changeRGBAalpha(loclinecolor,.3);if(getStyle(style,"boldover","---")=="YES"){loclinewidth=self.lineBoldWidth;loclinecolor=changeRGBAalpha(loclinecolor,self.lineBoldAlpha)}let localshowAllValues=getStyle(style,"showallvalues",self.showAllValues).toString()=="true";let loctxtvaluescolor=getStyle(style,"txtvaluescolor",loclinecolor);let locdashlinestr=getStyle(style,"dash","[]");locdashlinestr=locdashlinestr.trimLeft();locdashlinestr=locdashlinestr.trimRight();locdashlinestr=locdashlinestr.slice(1,-1);let locdashline=[];if(locdashlinestr!=""){let params=locdashlinestr.split(",");self.ctx.setLineDash(params)}else self.ctx.setLineDash([]);if(self.bezierlvloff>=0&&self.datalength/maxhm>self.bezierlvloff)locbeziercurve=false;self.ctx.strokeStyle=loclinecolor;self.ctx.fillStyle=loclinecolor;self.ctx.lineWidth=loclinewidth;let lastdef=false;let x=0;let x1=0;let y=0;let y1=0;for(let i=0;i<self.datalength;i++){let x0def=typeof data[i+0+self.from*self.zoom]!="undefined"&&data[i+0+self.from*self.zoom]!=null;let x1def=typeof data[i+1+self.from*self.zoom]!="undefined"&&data[i+1+self.from*self.zoom]!=null;if(i==self.datalength-1)x1def=false;x=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);if(x0def)y=self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;else y=self.zlvl;if(x0def&&!lastdef){self.ctx.beginPath();if(x1def){self.ctx.moveTo(x,y);self.ctx.lineTo(x,y)}else{self.ctx.save();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill();self.ctx.restore()}}if(x0def&&x1def){if(locbeziercurve){x1=self.lvlv+(i+1)*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);y1=self.zlvl-(data[i+1+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;self.ctx.bezierCurveTo(x+self.marhpx/locbezierconst,y,x1-self.marhpx/locbezierconst,y1,x1,y1)}else self.ctx.lineTo(x,y)}if(x0def&&!x1def){self.ctx.lineTo(x,y);self.ctx.stroke();self.ctx.closePath();self.ctx.beginPath()}if(x0def&&!lastdef&&!x1def){if(i+1<self.datalength){self.ctx.lineTo(x+self.marhpx,y)}else{self.ctx.save();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill();self.ctx.restore()}}lastdef=x0def}self.ctx.setLineDash([]);if(loclinepoints||self.allpoints){self.ctx.beginPath();if(typeof data[0+self.from*self.zoom]!="undefined"&&data[0+self.from*self.zoom]!=null){self.ctx.arc(self.lvlv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[0+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}for(let i=1;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}self.ctx.closePath()}if(localshowAllValues){self.ctx.font=(getStyle(style,"boldover","---")=="YES"?"Bold":"")+self.descfontmod+self.descfpx+self.descfont;self.ctx.strokeStyle=loctxtvaluescolor;self.ctx.fillStyle=loctxtvaluescolor;self.ctx.textAlign="center";self.ctx.textBaseline="middle";self.ctx.beginPath();for(let i=0;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;let val=data[i+self.from*self.zoom];let valstr=val;if(self.formattxtvalues)valstr=val.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal});let w=self.ctx.measureText(valstr).width;if(w+self.descfpx>self.marhpx)break;let locy=-(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset+self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;if(locy<self.drt||locy>self.drb)locy+=2*(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;let locx=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);if(locx-w/2-self.descfpx/2<self.lvlv)locx=locx+w/2+self.descfpx/2;if(locx+w/2+self.descfpx/2>self.drr)locx=locx-w/2-self.descfpx/2;self.ctx.fillText(valstr,locx,locy);self.ctx.stroke();self.ctx.fill()}self.ctx.closePath()}}function do_bar_graph(data,style){if(typeof data=="undefined")return;let w1=0;let w2=0;if(maxv!=0){w1=Math.abs((self.zlvl-self.drt)/maxv)}else w1=0;if(minv!=0){w2=Math.abs((self.drb-self.zlvl)/minv)}else w2=0;self.wght=Math.max(w1,w2);self.ctx.strokeStyle=getStyle(style,"linecolor",self.barline);self.ctx.fillStyle=getStyle(style,"fillcolor",self.barfill);self.ctx.lineWidth=getStyle(style,"linewidth",self.barlw);let locpoints=getStyle(style,"points",self.barpoints).toString()=="true";let locpointsize=getStyle(style,"pointsize",self.barpointsize);let locpointcolor=getStyle(style,"pointcolor",self.barpointcol);if(getStyle(style,"pointcolor",null)==null)locpointcolor=changeRGBAalpha(self.ctx.fillStyle,.3);if(getStyle(style,"boldover","---")=="YES"){self.ctx.lineWidth=self.barBoldLineWidth;self.ctx.fillStyle=changeRGBAalpha(self.ctx.fillStyle,self.barBoldAlpha)}let localshowAllValues=getStyle(style,"showallvalues",self.showAllValues).toString()=="true";let loctxtvaluescolor=getStyle(style,"txtvaluescolor",self.ctx.strokeStyle);self.ctx.beginPath();let xdef=false;let wdh=self.marhpx*self.barperc;let wob=wdh/self.barcnt;if(self.marxdata.div>1){wob=wob/(self.marxdata.div*1)}let x=0;let y=0;for(let i=0;i<self.datalength;i++){xdef=typeof data[i+self.from*self.zoom]!="undefined"&&data[i+self.from*self.zoom]!=null;if(xdef){y=self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;x=self.lvlv+self.hmarshift*(self.marhpx/2)-wob/2*self.barcnt+i*self.marhpx/self.xdiv+wob*self.barnr;self.ctx.rect(x,y,wob,self.zlvl-y)}}self.ctx.stroke();self.ctx.fill();self.ctx.closePath();self.barnr++;if(locpoints||self.allpoints){self.ctx.beginPath();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;self.ctx.stroke();self.ctx.fill();for(let i=0;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;self.ctx.beginPath();x=self.lvlv+i*self.marhpx/self.xdiv+wob*self.barnr+self.hmarshift*(self.marhpx/2-wdh/2-wob/2);y=self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;self.ctx.arc(x,y,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}}if(localshowAllValues){self.ctx.font=(getStyle(style,"boldover","---")=="YES"?"Bold":"")+self.descfontmod+self.descfpx+self.descfont;self.ctx.strokeStyle=loctxtvaluescolor;self.ctx.fillStyle=loctxtvaluescolor;self.ctx.textAlign="center";self.ctx.textBaseline="middle";self.ctx.beginPath();for(let i=0;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;let val=data[i+self.from*self.zoom];let valstr=val;if(self.formattxtvalues)valstr=val.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal});let w=self.ctx.measureText(valstr).width;if(w+self.descfpx>self.marhpx)break;let locx=self.lvlv+i*self.marhpx/self.xdiv+wob*self.barnr+self.hmarshift*(self.marhpx/2-wdh/2-wob/2);let locy=-(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset+self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;if(locy<self.drt||locy>self.drb)locy+=2*(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;if(locx-w/2-self.descfpx/2<self.lvlv)locx=locx+w/2+self.descfpx/2;if(locx+w/2+self.descfpx/2>self.drr)locx=locx-w/2-self.descfpx/2;self.ctx.fillText(valstr,locx,locy);self.ctx.stroke();self.ctx.fill()}self.ctx.closePath()}}function do_area_graph(data,style){if(typeof data=="undefined")return;let w1=0;let w2=0;if(maxv!=0){w1=Math.abs((self.zlvl-self.drt)/maxv)}else w1=0;if(minv!=0){w2=Math.abs((self.drb-self.zlvl)/minv)}else w2=0;self.wght=Math.max(w1,w2);let loclinecolor=getStyle(style,"linecolor",self.arealine);let loclinewidth=getStyle(style,"linewidth",self.areawidth);let locfillcolor=getStyle(style,"fillcolor",self.areafill);let locpoints=getStyle(style,"points",self.areapoints).toString()=="true";let locpointsize=getStyle(style,"pointsize",self.areapointsize);let locpointcolor=getStyle(style,"pointcolor",self.areapointcol);let locbeziercurve=getStyle(style,"beziercurve",self.beziercurve).toString()=="true";let locbezierconst=getStyle(style,"beziercnst",self.beziercnst);if(getStyle(style,"pointcolor",null)==null)locpointcolor=changeRGBAalpha(locfillcolor,.3);if(getStyle(style,"boldover","---")=="YES"){loclinewidth=self.areaBoldLineWidth;loclinecolor=changeRGBAalpha(loclinecolor,self.areaBoldLineWidth);locfillcolor=changeRGBAalpha(locfillcolor,self.areaBoldAreaAlpha)}let localshowAllValues=getStyle(style,"showallvalues",self.showAllValues).toString()=="true";let loctxtvaluescolor=getStyle(style,"txtvaluescolor",loclinecolor);self.ctx.strokeStyle=loclinecolor;self.ctx.fillStyle=locfillcolor;self.ctx.lineWidth=loclinewidth;if(self.bezierlvloff>=0&&self.datalength/maxhm>self.bezierlvloff)locbeziercurve=false;let lastdef=false;let x=0;let x1=0;let y=0;let y1=0;for(let i=0;i<self.datalength;i++){let x0def=typeof data[i+0+self.from*self.zoom]!="undefined"&&data[i+0+self.from*self.zoom]!=null;let x1def=typeof data[i+1+self.from*self.zoom]!="undefined"&&data[i+1+self.from*self.zoom]!=null;if(i==self.datalength-1)x1def=false;x=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);y=0;if(x0def)y=self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;else y=self.zlvl;if(x0def&&!lastdef){self.ctx.beginPath();self.ctx.moveTo(x,self.zlvl);self.ctx.lineTo(x,y)}if(x0def&&x1def){if(locbeziercurve){x1=self.lvlv+(i+1)*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);y1=self.zlvl-(data[i+1+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;self.ctx.bezierCurveTo(x+self.marhpx/locbezierconst,y,x1-self.marhpx/locbezierconst,y1,x1,y1)}else self.ctx.lineTo(x,y)}if(x0def&&!x1def){self.ctx.lineTo(x,y);self.ctx.lineTo(x,self.zlvl);self.ctx.stroke();self.ctx.fill();self.ctx.closePath();self.ctx.beginPath()}lastdef=x0def}if(locpoints||self.allpoints){self.ctx.beginPath();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;if(typeof data[0+self.from*self.zoom]!="undefined"&&data[0+self.from*self.zoom]!=null){self.ctx.arc(self.lvlv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[0+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}for(let i=1;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}}if(localshowAllValues){self.ctx.font=(getStyle(style,"boldover","---")=="YES"?"Bold":"")+self.descfontmod+self.descfpx+self.descfont;self.ctx.strokeStyle=loctxtvaluescolor;self.ctx.fillStyle=loctxtvaluescolor;self.ctx.textAlign="center";self.ctx.textBaseline="middle";self.ctx.beginPath();for(let i=0;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;let val=data[i+self.from*self.zoom];let valstr=val;if(self.formattxtvalues)valstr=val.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal});let w=self.ctx.measureText(valstr).width;if(w+self.descfpx>self.marhpx)break;let locy=-(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset+self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;if(locy<self.drt||locy>self.drb)locy+=2*(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;let locx=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);if(locx-w/2-self.descfpx/2<self.lvlv)locx=locx+w/2+self.descfpx/2;if(locx+w/2+self.descfpx/2>self.drr)locx=locx-w/2-self.descfpx/2;self.ctx.fillText(valstr,locx,locy);self.ctx.stroke();self.ctx.fill()}self.ctx.closePath()}}function do_stairs_graph(data,style){if(typeof data=="undefined")return;let w1=0;let w2=0;if(maxv!=0){w1=Math.abs((self.zlvl-self.drt)/maxv)}else w1=0;if(minv!=0){w2=Math.abs((self.drb-self.zlvl)/minv)}else w2=0;self.wght=Math.max(w1,w2);let loclinecolor=getStyle(style,"linecolor",self.linecol);let loclinewidth=getStyle(style,"linewidth",self.linewidth);let loclinepoints=getStyle(style,"points",self.linepoints).toString()=="true";let locpointsize=getStyle(style,"pointsize",self.linepointsize);let locpointcolor=getStyle(style,"pointcolor",self.linepointcol);if(getStyle(style,"pointcolor",null)==null)locpointcolor=changeRGBAalpha(loclinecolor,.3);if(getStyle(style,"boldover","---")=="YES"){loclinewidth=self.stairsBoldLineWidth;loclinecolor=changeRGBAalpha(loclinecolor,self.stairsBoldLineAlpha)}let localshowAllValues=getStyle(style,"showallvalues",self.showAllValues).toString()=="true";let loctxtvaluescolor=getStyle(style,"txtvaluescolor",loclinecolor);self.ctx.strokeStyle=loclinecolor;self.ctx.fillStyle=loclinecolor;self.ctx.lineWidth=loclinewidth;let lastdef=false;let x=0;let x1=0;let y=0;let y1=0;for(let i=0;i<self.datalength;i++){let x0def=typeof data[i+0+self.from*self.zoom]!="undefined"&&data[i+0+self.from*self.zoom]!=null;let x1def=typeof data[i+1+self.from*self.zoom]!="undefined"&&data[i+1+self.from*self.zoom]!=null;if(i==self.datalength-1)x1def=false;x=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);if(x0def)y=self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;else y=self.zlvl;if(x1def)y1=self.zlvl-(data[i+1+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;else y1=y;if(x0def&&!lastdef&&x1def){self.ctx.beginPath();self.ctx.moveTo(x,y);self.ctx.lineTo(x+self.marhpx/self.xdiv,y)}if(x0def&&x1def){self.ctx.lineTo(x+self.marhpx/self.xdiv,y);self.ctx.lineTo(x+self.marhpx/self.xdiv,y1)}if(x0def&&!x1def){if(i+1<self.datalength){self.ctx.moveTo(x,y1);self.ctx.lineTo(x+self.marhpx/self.xdiv,y1)}self.ctx.stroke();self.ctx.closePath();self.ctx.beginPath()}if(x0def&&!lastdef&&!x1def){if(i+1<self.datalength){self.ctx.lineTo(x+self.marhpx/self.xdiv,y)}else{self.ctx.save();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill();self.ctx.restore()}}lastdef=x0def}if(loclinepoints||self.allpoints){self.ctx.beginPath();self.ctx.strokeStyle=changeRGBAalpha(locpointcolor,1);self.ctx.fillStyle=locpointcolor;if(typeof data[0+self.from*self.zoom]!="undefined"&&data[0+self.from*self.zoom]!=null){self.ctx.arc(self.lvlv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[0+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}for(let i=1;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;self.ctx.beginPath();self.ctx.arc(self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2),self.zlvl-(data[i+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr,locpointsize,0,2*Math.PI);self.ctx.stroke();self.ctx.fill()}}if(localshowAllValues){self.ctx.font=(getStyle(style,"boldover","---")=="YES"?"Bold":"")+self.descfontmod+self.descfpx+self.descfont;self.ctx.strokeStyle=loctxtvaluescolor;self.ctx.fillStyle=loctxtvaluescolor;self.ctx.textAlign="center";self.ctx.textBaseline="middle";self.ctx.beginPath();for(let i=0;i<self.datalength;i++){if(typeof data[i+self.from*self.zoom]=="undefined"||data[i+self.from*self.zoom]==null)continue;let val=data[i+self.from*self.zoom];let valstr=val;if(self.formattxtvalues)valstr=val.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal});let w=self.ctx.measureText(valstr).width;if(w+self.descfpx>self.marhpx)break;let locy=-(val>0?1:-1)*self.descfpx*self.textvalueoffset+self.zlvl-(val+self.zoffs)*self.wght*self.dcorr*self.zcorr;if(locy<self.drt||locy>self.drb)locy+=2*(data[i+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;let locx=self.lvlv+i*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);if(locx-w/2-self.descfpx/2<self.lvlv)locx=locx+w/2+self.descfpx/2;if(locx+w/2+self.descfpx/2>self.drr)locx=locx-w/2-self.descfpx/2;self.ctx.fillText(valstr,locx,locy);self.ctx.stroke();self.ctx.fill()}self.ctx.closePath()}}function do_graph(){if(typeof self.style=="undefined")return;for(let i=0;i<self.data.length;i++){if(typeof self.style[i]=="undefined")continue;if(getStyle(self.style[i],"visible","")=="NO"){if(isBar(i)){self.barnr++}continue}if(getStyle(self.style[i],"type","")=="line")do_line_graph(self.data[i],self.style[i]);else if(getStyle(self.style[i],"type","")=="area")do_area_graph(self.data[i],self.style[i]);else if(getStyle(self.style[i],"type","")=="bar")do_bar_graph(self.data[i],self.style[i]);else if(getStyle(self.style[i],"type","")=="stairs")do_stairs_graph(self.data[i],self.style[i]);else;}}function isBar(i){return getStyle(self.style[i],"type","")=="bar"}function isArea(i){return getStyle(self.style[i],"type","")=="area"}function isLine(i){return getStyle(self.style[i],"type","")=="line"}var lastX=0;var lastY=0;var Zfrom=0;var ZRfrom=0;var Zto=0;var mouseisdown=false;var mousebuttons=0;var noclick=false;var xmove=0;var oldind=-1;function on_mouseMove(ev){if(ev==null)ev=self.oldMouseEv;else self.oldMouseEv=ev;if(ev==null)return;if(self.internal_mose_flag)return;if(self.boldonover){var ind=isinrect(ev.offsetX,ev.offsetY);if(ind>=0&&ind!=oldind){if(ind!=oldind)setstyleparam(ind,"boldover",true)}else{for(let ind=0;ind<self.style.length;ind++)setstyleparam(ind,"boldover",false)}if(ind!=oldind){self.internal_mose_flag=true;self.draw()}oldind=ind}self.internal_mose_flag=false;if(self.zoom&&self.movezoombymouse){if(ev.buttons==2){self.ctxl2.canvas.style.cursor=self.moveCursor;xmove+=ev.movementX;if(Math.abs(xmove)>=self.marhpx){if(xmove<0){if(self.revmousemoveinzoom)self.left();else self.right()}else{if(self.revmousemoveinzoom)self.right();else self.left()}xmove=0}return}self.ctxl2.canvas.style.cursor=self.pointerCursor}let lastXshadow=lastX;if(ev.type!="keydown"&&ev.offsetX!=0&&ev.offsetY!=0){lastX=ev.offsetX;lastY=ev.offsetY}if(ev.type=="mousemove"&&ev.offsetX!=0&&ev.offsetY!=0){lastX=ev.offsetX;lastY=ev.offsetY}if(self.data.length<=0)return;if(self.pointclicked)return;let xnbr=0;let ynbr=0;let x=ev.offsetX;let y=ev.offsetY;if(ev.type!="mousemove"){x=lastX;y=lastY}self.ctxl2.clearRect(0,0,self.layer2.width,self.layer2.height);if(y>=self.drt-self.hintpointw*2&&y<=self.drb+self.hintpointw*2&&x>=self.drl-self.hintpointw*2&&x<=self.drr+self.hintpointw*2){xnbr=Math.round((x-self.lvlv-self.hmarshift*(self.marhpx/2))/(self.marhpx/self.xdiv));if(isNaN(xnbr)||xnbr<0||xnbr>=self.datalength)return;let xJump=self.lvlv+xnbr*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);let LRmarg=self.LRsize*self.drv;if(self.zoom&&self.mouseclickLR&&!ev.shiftKey){if(x>self.drl&&x<self.drl+LRmarg&&self.from>0){self.ctxl2.lineWidth=5;self.ctxl2.strokeStyle=changeRGBAalpha(self.hintzoomcol,1);self.ctxl2.fillStyle=self.hintzoomcol;self.ctxl2.beginPath();self.ctxl2.fillRect(self.drl,self.drt,LRmarg,self.drv);self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.beginPath();self.ctxl2.moveTo(self.drl+LRmarg*.8,self.drt+self.drv/2-self.drv*.1);self.ctxl2.lineTo(self.drl+LRmarg*.2,self.drt+(self.drb-self.drt)/2);self.ctxl2.lineTo(self.drl+LRmarg*.8,self.drt+self.drv/2+self.drv*.1);self.ctxl2.stroke();self.ctxl2.closePath()}if(x>self.drr-LRmarg&&x<self.drr&&self.to<self.alldatalength){self.ctxl2.lineWidth=5;self.ctxl2.strokeStyle=changeRGBAalpha(self.hintzoomcol,1);self.ctxl2.fillStyle=self.hintzoomcol;self.ctxl2.beginPath();self.ctxl2.fillRect(self.drr-LRmarg,self.drt,LRmarg,self.drv);self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.beginPath();self.ctxl2.moveTo(self.drr-LRmarg*.8,self.drt+self.drv/2-self.drv*.1);self.ctxl2.lineTo(self.drr-LRmarg*.2,self.drt+(self.drb-self.drt)/2);self.ctxl2.lineTo(self.drr-LRmarg*.8,self.drt+self.drv/2+self.drv*.1);self.ctxl2.stroke();self.ctxl2.closePath()}}if(self.hintselectshadow){if(ev.shiftKey&&mouseisdown&&mousebuttons==1){Zto=xnbr+self.from;let xFrom=self.lvlv+(Zfrom-self.from)*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);let selx=xJump-xFrom;if(selx<1)selx=self.marhpx;self.ctxl2.fillStyle=self.hintselectcol;let difx=self.marhpx/2;self.ctxl2.beginPath();if(xFrom+selx>self.drr)selx=self.drr-xFrom;if(Zfrom>Zto)selx=0;if(Math.abs(self.from-self.to)>2)self.ctxl2.fillRect(xFrom,self.drt,selx,self.drb-self.drt);self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.closePath();if(x-lastXshadow<-self.undozoompx){Zfrom=0;Zto=0;mouseisdown=false;self.undozoom()}return}}let wassmallhint=false;let hitpoint=false;let xa=0;let ya=0;let locbarnr=0;let sens=self.hintpointw*2;let wdh=self.marhpx*self.barperc;let wob=wdh/self.barcnt;let difx=self.marhpx*self.barperc/2;let xzero=self.lvlv+xnbr*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);for(let i=0;i<self.data.length;i++){if(typeof self.data[i]=="undefined")continue;ya=self.zlvl-(self.data[i][xnbr+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;xa=xzero;if(isBar(i)){locbarnr++;xa=xa-difx+wob*(locbarnr-1)}if(Math.abs(xa-lastX)<sens&&Math.abs(ya-lastY)<sens){hitpoint=true;break}else hitpoint=false}if(hitpoint&&self.smallhint){noclick=true;on_canvas_click(ev);noclick=false;wassmallhint=true}if(self.hintwithctrl&&!ev.ctrlKey)return;self.ctxl2.lineWidth=self.crlinewidth;self.ctxl2.setLineDash(self.crYlinedash);self.ctxl2.strokeStyle=self.crlinecol;if(self.hintdatashadow){if(self.barcnt>0){self.ctxl2.fillStyle=self.hintshadowcol;let difx=self.marhpx/2;self.ctxl2.beginPath();self.ctxl2.fillRect(xJump-difx,self.drt,difx*2,self.drb-self.drt);self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.closePath()}}if(self.crXjump)x=xJump;if(self.crXYline&&x>=self.drl&&x<=self.drr||self.crXYlineOnButton&&ev.buttons==1){self.ctxl2.beginPath();self.ctxl2.moveTo(x,self.drb);self.ctxl2.lineTo(x,self.drt);self.ctxl2.stroke();self.ctxl2.closePath()}self.ctxl2.setLineDash(self.crXlinedash);if(self.crYjumpM||self.crYjumpP){let y1=0;let y2=0;if(self.crYjumpM){ynbr=Math.round((y-self.zlvl)/self.marpx);y1=self.zlvl+ynbr*self.marpx;y=y1}let ydef=false;if(self.crYjumpP){let sens=self.marax;let oldydif=sens;for(let i=0;i<self.data.length;i++){ydef=typeof self.data[i]!="undefined"&&typeof self.data[i][xnbr+self.from*self.zoom]!="undefined"&&self.data[i][xnbr+self.from*self.zoom]!=null;if(ydef){y2=self.zlvl-(self.data[i][xnbr+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;let ydif=Math.abs(y2-y);if(!self.crYjumpM&&ydif<sens&&ydif<=oldydif){oldydif=ydif;y=y2}}else continue;if(self.crYjumpM&&self.crYjumpP)if(Math.abs(y2-y1)<sens){if(getStyle(self.style[i],"visible","")!="NO")y=y2}}}}if(self.crXYline&&y>=self.drt&&y<=self.drb||self.crXYlineOnButton&&ev.buttons==1){self.ctxl2.beginPath();self.ctxl2.moveTo(self.drl,y);self.ctxl2.lineTo(self.drr,y);self.ctxl2.stroke();self.ctxl2.closePath()}if(self.crpointhint||self.crlinetomouse||self.textvalue){let ydef;let y2;let bnr=0;let difx=0;self.ctxl2.setLineDash([]);for(let i=0;i<self.data.length;i++){ydef=typeof self.data[i]!="undefined"&&typeof self.data[i][xnbr+self.from*self.zoom]!="undefined"&&self.data[i][xnbr+self.from*self.zoom]!=null;if(ydef)y2=self.zlvl-(self.data[i][xnbr+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;else continue;difx=0;if(self.crpointhint){self.ctxl2.lineWidth=self.hintpointlwdth;self.ctxl2.strokeStyle=self.hintlinecol;if(isLine(i)){if(self.hintpointcolfromdraw){self.ctxl2.fillStyle=getStyle(self.style[i],"linecolor",self.hintpointfill);self.ctxl2.strokeStyle=changeRGBAalpha(self.ctxl2.fillStyle,1)}else{self.ctxl2.fillStyle=self.hintpointfill;self.ctxl2.strokeStyle=self.hintlinecol}}else{if(self.hintpointcolfromdraw){self.ctxl2.fillStyle=getStyle(self.style[i],"fillcolor",self.hintpointfill);self.ctxl2.strokeStyle=changeRGBAalpha(self.ctxl2.fillStyle,1)}else{self.ctxl2.fillStyle=self.hintpointfill;self.ctxl2.strokeStyle=self.hintlinecol}}if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);if(isBar(i)){let wdh=self.marhpx*self.barperc;let wob=wdh/self.barcnt;if(self.marxdata.div>1){wob=wob/(self.marxdata.div*1)}difx=-(wob/1*bnr)+(self.barcnt-1)*wob/2;bnr++}if(getStyle(self.style[i],"visible","---")=="NO")continue;self.ctxl2.beginPath();self.ctxl2.arc(xJump-difx,y2,self.hintpointw,0,2*Math.PI);self.ctxl2.stroke();if(self.crpointfill)self.ctxl2.fill()}if(self.textvalue){if(getStyle(self.style[i],"visible","---")=="NO")continue;self.ctxl2.fillStyle=getStyle(self.style[i],"txtvaluescolor",getStyle(self.style[i],"linecolor",self.hintpointfill));self.ctxl2.font=self.livevalfontmod+self.livevalfpx+self.livevalfont;self.ctxl2.beginPath();self.ctxl2.textAlign="center";self.ctxl2.textBaseline="middle";let val=self.data[i][xnbr+self.from*self.zoom];if(self.formattxtvalues)val=val.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal});let locy=y2-(self.data[i][xnbr+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;if(locy<self.drt||locy>self.drb)locy=y2+(self.data[i][xnbr+self.from*self.zoom]>0?1:-1)*self.descfpx*self.textvalueoffset;let locx=xJump-difx;let w=self.ctx.measureText(val).width;if(locx-w/2-self.descfpx/2<self.lvlv)locx=locx+w/2+self.descfpx/2;if(locx+w/2+self.descfpx/2>self.drr)locx=locx-w/2-self.descfpx/2;self.ctxl2.fillText(val,locx,locy)}if(self.crlinetomouse){self.ctxl2.lineWidth=self.hintlinetoMwidth;self.ctxl2.strokeStyle=self.hintlinetoMcol;self.ctxl2.beginPath();self.ctxl2.moveTo(xJump-difx,y2);self.ctxl2.lineTo(lastX,lastY);self.ctxl2.stroke()}}}if(self.crHint&&!wassmallhint){self.ctxl2.setLineDash([]);self.ctxl2.font=self.hintfontmod+self.hintfpx+self.hintfont;let s=[];let txtw=0;let txtwt=0;let txt="";if(typeof self.desc[xnbr+self.from*self.zoom]=="number")txt=self.axisXtxt+" = "+self.desc[xnbr+self.from*self.zoom].toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalX,maximumFractionDigits:self.axisdecimalX});else txt=self.axisXtxt+" = "+self.desc[xnbr+self.from*self.zoom];let txt2="";txtw=self.ctxl2.measureText(txt).width;for(let i=0;i<self.data.length;i++){if(getStyle(self.style[i],"visible","---")=="NO")continue;if(typeof self.data[i]!="undefined"&&typeof self.data[i][xnbr+self.from*self.zoom]!="undefined"&&self.data[i][xnbr+self.from*self.zoom]!=null)s.push((self.data[i][xnbr+self.from*self.zoom]*self.dcorr).toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal})+" "+self.aYtxt);else s.push("no data");txtwt=self.ctxl2.measureText(s[i]).width+self.hintfpx*3;if(txtwt>txtw)txtw=txtwt}while(self.ctxl2.measureText(txt2).width<txtw){txt2+=String.fromCharCode(9472);if(txt2.length>20)break}self.ctxl2.beginPath();if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);if(hitpoint)self.ctxl2.fillStyle=self.hintfillhitcol;else self.ctxl2.fillStyle=self.hintfillcol;self.ctxl2.textBaseline="top";self.ctxl2.textAlign="start";let mx;let my;if(lastX+txtw+self.hintfpx*3>self.drr)mx=lastX-self.hintfpx*3-txtw;else mx=lastX+self.hintfpx*2;if(lastY+self.hintfpx*(s.length+4+1)>self.drb)my=lastY-self.hintfpx*(s.length+4+1);else my=lastY+self.hintfpx*2;self.ctxl2.lineWidth=self.hintframewidth;self.ctxl2.strokeStyle=self.hintrectcol;if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);self.ctxl2.rect(mx,my,txtw+self.hintfpx,self.hintfpx*(s.length+2+1));self.ctxl2.stroke();self.ctxl2.fillRect(mx,my,txtw+self.hintfpx,self.hintfpx*(s.length+2+1));self.ctxl2.fillStyle=self.hinttxtcol;self.ctxl2.textAlign="end";self.ctxl2.fillText(txt,mx+self.hintfpx/2+txtw,my+self.hintfpx*.5+self.hintfpx*0);self.ctxl2.fillText(txt2,mx+self.hintfpx/2+txtw,my+self.hintfpx*.5+self.hintfpx*1);var v=0;for(let i=0;i<self.data.length;i++){if(getStyle(self.style[i],"visible","---")=="NO"){v++;continue}self.ctxl2.beginPath();if(isLine(i))self.ctxl2.fillStyle=getStyle(self.style[i],"linecolor",self.hintpointfill);else self.ctxl2.fillStyle=getStyle(self.style[i],"fillcolor",self.hintpointfill);if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);self.ctxl2.strokeStyle="rgba(0,0,0,1)";self.ctxl2.lineWidth=self.hintcolfrwdth;self.ctxl2.rect(mx+self.hintfpx/2,my+self.hintfpx*.5+self.hintfpx*(i-v+2),self.hintfpx,self.hintfpx);self.ctxl2.stroke();self.ctxl2.beginPath();self.ctxl2.fillRect(mx+self.hintfpx/2,my+self.hintfpx*.5+self.hintfpx*(i-v+2),self.hintfpx,self.hintfpx);self.ctxl2.stroke();self.ctxl2.fillStyle=self.hinttxtcol;self.ctxl2.textAlign="start";self.ctxl2.fillText(" ",mx+self.hintfpx*2,my+self.hintfpx*.5+self.hintfpx*(i-v+2));self.ctxl2.textAlign="end";self.ctxl2.fillText(s[i-v],mx+self.hintfpx/2+txtw,my+self.hintfpx*.5+self.hintfpx*(i-v+2))}}if(self.XaxisLiveValue||self.YaxisLiveValue){self.ctxl2.font=self.descfontmod+self.descfpx+self.descfont;let mar=2;let maxw=0;let w=0;let axtxt="";if(self.YaxisLiveValue){axtxt=((self.zlvl-y)/(self.wght*self.dcorr*self.zcorr)-self.zoffs).toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalY,maximumFractionDigits:self.axisdecimalY});w=self.ctxl2.measureText(axtxt).width;w=self.ctxl2.measureText(minv.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalY,maximumFractionDigits:self.axisdecimalY})).width;maxw=self.ctxl2.measureText(maxv.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalY,maximumFractionDigits:self.axisdecimalY})).width;if(maxw>w)w=maxw;self.ctxl2.beginPath();self.ctxl2.textAlign="end";self.ctxl2.textBaseline="middle";self.ctxl2.fillStyle=self.liveValueback;self.ctxl2.fillRect(self.lvlv-self.aw-w-mar,y-self.descfpx/2-mar,1+w+mar*2,self.descfpx+mar*2);self.ctxl2.fillStyle=self.liveValueColor;self.ctxl2.fillText(axtxt,self.lvlv-self.aw,y);self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.closePath()}if(self.XaxisLiveValue){let hor=x;let rhor=1;self.ctxl2.font=self.descfontmod+self.descfpx+self.descfont;w=maxw;if(self.zoom){w=self.ctxl2.measureText(self.desc[self.from]).width;maxw=self.ctxl2.measureText(self.desc[self.to-1]).width}else{w=self.ctxl2.measureText(self.desc[0]).width;maxw=self.ctxl2.measureText(self.desc[self.desc.length-1]).width}if(maxw>w)w=maxw;axtxt=self.desc[xnbr+self.from*self.zoom];self.ctxl2.beginPath();self.ctxl2.save();self.ctxl2.translate(self.drl,self.drb);self.ctxl2.rotate(-.5*Math.PI);rhor=x-self.lvlv;self.ctxl2.fillStyle=self.liveValueback;self.ctxl2.fillRect(self.drb-self.zlvl-self.marsize+mar,rhor-self.descfpx/2-mar+self.descfpx/2*(xnbr==0)*!self.hmarshift,-(1+w+mar*2),self.descfpx+mar*2);self.ctxl2.restore();self.ctxl2.fillStyle=self.liveValueColor;self.ctxl2.textAlign="end";self.ctxl2.textBaseline="middle";self.ctxl2.save();self.ctxl2.textBaseline="middle";self.ctxl2.textAlign="end";self.ctxl2.translate(self.drl,self.drb);self.ctxl2.rotate(-.5*Math.PI);if(typeof axtxt!="undefined")self.ctxl2.fillText(axtxt.toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalX,maximumFractionDigits:self.axisdecimalX}),self.drb-self.zlvl-self.marsize,rhor+self.descfpx/2*(xnbr==0)*!self.hmarshift);self.ctxl2.restore();self.ctxl2.stroke();self.ctxl2.fill();self.ctxl2.closePath()}}}self.ctxl2.setLineDash([])}function isinrect(x,y){var ind=-1;var tstx=false;var tsty=false;var marg=0;var txt="";var txtw=0;var txty=0;var frmy=0;var cl=0;var wd=0;var lf=0;if(!self.islegend)return-1;if(self.legpos==0||self.legpos==2){cl=0;for(let i=0;i<self.legstr.length;i++){if(typeof self.data[i]=="undefined")continue;cl++;txt+=self.legstr[i]}self.ctx.font=self.legfontmod+self.legpx+self.legfont;self.ctx.fillStyle=self.legtxtcol;txtw=self.ctx.measureText(txt).width+cl*self.legpx*2.2-self.legpx;wd=txtw+marg*2;lf=(self.ctx.canvas.width-wd)/2;txty=0;frmy=0;if(self.legpos==0)frmy=self.top-self.legpx*2-self.legframew-self.legmarg;else frmy=self.bot+self.legframew*2+self.legmarg;txty=frmy+self.legpx/.8;let txtx=lf+marg;for(let i=0;i<self.data.length;i++){tstx=x>=txtx&&x<=txtx+self.legpx*1.2+self.ctx.measureText(self.legstr[i]).width+1;tsty=y>=txty-self.legpx+1&&y<=txty+1;if(tstx&&tsty)ind=i;if(ind>=0)return ind;txtx+=self.legpx*1.2;txtx+=self.ctx.measureText(self.legstr[i]).width+self.legpx*1}}if(self.legpos==1||self.legpos==3){txtw=0;var lw=0;cl=0;for(let i=0;i<self.legstr.length;i++){lw=self.ctx.measureText(self.legstr[i]).width;if(typeof self.data[i]=="undefined")continue;cl++;if(lw>txtw)txtw=lw}wd=txtw+self.legpx*2.2;lf=0;if(self.legpos==1)lf=self.rgt+self.legframew+self.legmarg;else lf=self.margh+self.legframew;frmy=(self.ctx.canvas.height-self.legpx*(self.legstr.length+1))/2;let txtx=lf+marg;var recy=frmy+self.legpx*1.5;self.ctx.font=self.legfontmod+self.legpx+self.legfont;self.ctx.fillStyle=self.legtxtcol;for(let i=0;i<self.data.length;i++){self.ctx.fillStyle=self.legtxtcol;tstx=x>=txtx+self.legpx/2&&x<=txtx+self.legpx/2+txtw+self.legpx*1.2;tsty=y>recy-self.legpx&&y<recy;if(tstx&&tsty)ind=i;if(ind>=0)return ind;recy+=self.legpx}}return ind}function drawstr(s){var h=self.ctx.canvas.height;self.ctx.beginPath();self.ctx.fillStyle="rgba(0, 0, 0, 1)";self.ctx.fillRect(0,h-30,300,25);self.ctx.lineWidth=self.marw;self.ctx.strokeStyle=self.descol;self.ctx.fillStyle="rgb(255,255,0)";self.ctx.fillText(s,100,h-15);self.ctx.stroke();self.ctx.fill();self.ctx.closePath()}function setstyleparam(ind,par,state){if(self.style==null||typeof self.style=="undefined")return;if(ind>self.style.length)return;if(self.style[ind]==null||typeof self.style[ind]=="undefined")return;if(getStyle(self.style[ind],par,"---")=="---"){self.style[ind].push(par+"="+(state?"YES":"NO"));return}for(let i=0;i<self.style[ind].length;i++){if(self.style[ind][i].indexOf(par)>=0){self.style[ind][i]=par+"="+(state?"YES":"NO");break}}}function on_canvas_click(ev){if(self.data.length<=0)return;if(self.hideonclick){var ind=isinrect(ev.offsetX,ev.offsetY);if(ind>=0){var s=getStyle(self.style[ind],"visible","---");switch(s){case"---":{setstyleparam(ind,"visible",false);break}case"NO":{setstyleparam(ind,"visible",true);if(self.boldonover)setstyleparam(ind,"boldover",true);break}case"YES":{setstyleparam(ind,"visible",false);break}}self.redraw();return}}if(self.pointclicked==true){self.pointclicked=false;self.ctxl2.clearRect(0,0,self.layer2.width,self.layer2.height);if(self.onmousemove||self.ontouchmove){on_mouseMove(ev)}}if(self.zoom&&ev.shiftKey)return;let x=ev.offsetX;let y=ev.offsetY;let xnbr=0;let ynbr=0;if(y>=self.drt-self.hintpointw*2&&y<=self.drb+self.hintpointw*2&&x>=self.drl-self.hintpointw*2&&x<=self.drr+self.hintpointw*2){xnbr=Math.round((x-self.lvlv-self.hmarshift*(self.marhpx/2))/(self.marhpx/self.xdiv));if(isNaN(xnbr)||xnbr<0||xnbr>self.datalength)return;let sens=self.hintpointw*2;let xzero=self.lvlv+xnbr*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);let wdh=self.marhpx*self.barperc;let wob=wdh/self.barcnt;let difx=self.marhpx*self.barperc/2-wob/2;let locbarnr=0;let xa=0;let ya=0;for(let i=0;i<self.data.length;i++){if(typeof self.data[i]=="undefined")continue;ya=self.zlvl-(self.data[i][xnbr+self.from*self.zoom]+self.zoffs)*self.wght*self.dcorr*self.zcorr;xa=xzero;if(isBar(i)){locbarnr++;xa=xa-difx+wob*(locbarnr-1)}if(Math.abs(xa-x)<sens&&Math.abs(ya-y)<sens){if(getStyle(self.style[i],"visible","---")=="NO")continue;if(!noclick)self.pointclicked=true;let txt=self.axisXtxt+" ["+self.desc[xnbr+self.from*self.zoom].toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.axisdecimalX,maximumFractionDigits:self.axisdecimalX})+"] = "+(self.data[i][xnbr+self.from*self.zoom]*self.dcorr).toLocaleString(undefined,{useGrouping:self.dgroup,minimumFractionDigits:self.valuedecimal,maximumFractionDigits:self.valuedecimal})+" "+self.aYtxt;self.ctxl2.font=self.hintfontmod+self.hintfpx+self.hintfont;let txtw=self.ctxl2.measureText(txt).width;txtw=txtw+self.hintfpx*2;self.ctxl2.clearRect(0,0,self.layer2.width,self.layer2.height);self.ctxl2.setLineDash([]);self.ctxl2.beginPath();self.ctxl2.fillStyle=self.hintfillcol;self.ctxl2.textBaseline="top";self.ctxl2.textAlign="start";let xflipped=false;let yflipped=false;let mx=0;let my=0;if(ev.offsetX+txtw+self.hintfpx*2.5>self.drr){mx=ev.offsetX-self.hintfpx*3-txtw;xflipped=true}else mx=ev.offsetX+self.hintfpx*2;if(ev.offsetY+self.hintfpx*3+self.hintframewidth>self.drb){my=ev.offsetY-self.hintfpx*3;yflipped=true}else my=ev.offsetY+self.hintfpx*2;self.ctxl2.lineWidth=self.hintframewidth;self.ctxl2.strokeStyle=self.hintrectcol;self.ctxl2.rect(mx,my,txtw+self.hintfpx*.5,self.hintfpx*2);self.ctxl2.stroke();if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);self.ctxl2.fillRect(mx,my,txtw+self.hintfpx*.5,self.hintfpx*2);self.ctxl2.fillStyle=self.hinttxtcol;self.ctxl2.beginPath();if(isLine(i))self.ctxl2.fillStyle=getStyle(self.style[i],"linecolor",self.hintpointfill);else self.ctxl2.fillStyle=getStyle(self.style[i],"fillcolor",self.hintpointfill);if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);self.ctxl2.strokeStyle="rgba(0,0,0,1)";self.ctxl2.lineWidth=self.hintcolfrwdth;self.ctxl2.rect(mx+self.hintfpx/2,my+self.hintfpx/2,self.hintfpx,self.hintfpx);self.ctxl2.stroke();self.ctxl2.beginPath();self.ctxl2.fillRect(mx+self.hintfpx/2,my+self.hintfpx/2,self.hintfpx,self.hintfpx);self.ctxl2.stroke();self.ctxl2.strokeStyle=self.hinttxtcol;self.ctxl2.fillStyle=self.hinttxtcol;if(self.hintmaxalpha)self.ctxl2.fillStyle=changeRGBAalpha(self.ctxl2.fillStyle,1);self.ctxl2.textAlign="end";self.ctxl2.fillText(txt,mx+self.hintfpx*0+txtw,my+self.hintfpx*.5);self.ctxl2.lineWidth=self.hintlinetoMwidth;self.ctxl2.strokeStyle=self.hintlinetoMcol;self.ctxl2.beginPath();self.ctxl2.arc(xa,ya,self.hintpointw,0,2*Math.PI);if(self.crlinetosmallhint)self.ctxl2.moveTo(mx+(txtw+self.hintfpx*.5)*xflipped,my+self.hintfpx*2*yflipped);self.ctxl2.lineTo(xa,ya);self.ctxl2.stroke();break}}}}function on_mouseout(){self.ctxl2.clearRect(0,0,self.ctxl2.canvas.width,self.ctxl2.canvas.height);self.pointclicked=false;self.oldMouseEv=null;lastX=0;lastY=0;mouseisdown=false;mousebuttons=0;oldind=-1;noclick=false}let lastrepev=null;let intervalId=null;let CtrlKeyDown=false;let ShiftKeyDown=false;let inRepeat=false;function repeatMouse(ev){let x=lastX;let y=lastY;if(y>=self.drt&&y<=self.drb&&x>=self.drl&&x<=self.drr&&mousebuttons==1){if(CtrlKeyDown)self.repeatMousePer=self.repeatPerSpeed;else self.repeatMousePer=self.repeatPerSlow;intervalId=setTimeout(repeatMouse,self.repeatMousePer);let LRmarg=self.LRsize*self.drv;if(x-self.drl<LRmarg)self.left();if(self.drr-x<LRmarg)self.right();on_mouseMove(lastrepev);inRepeat=true}return false}var oldticks=0;var mdcnt=0;function on_mouseDown(ev){if(ev.buttons==3)self.hintwithctrl=!self.hintwithctrl;if(self.multimsdown&&ev.buttons==2&&ev.shiftKey&&!ev.ctrlKey&&!ev.altKey){console.log("ev.buttons="+ev.buttons);var now=new Date;var ticks=now.getTime();if(ticks-oldticks>self.multimstout){mdcnt=0;oldticks=ticks}mdcnt++;if(mdcnt>=self.multimsdowncnt){mdcnt=0;oldticks=0;self.allpoints=!self.allpoints;self.draw()}}mouseisdown=true;mousebuttons=ev.buttons;let x=ev.offsetX;let y=ev.offsetY;let xnbr=0;if(y>=self.drt&&y<=self.drb&&x>=self.drl&&x<=self.drr&&mousebuttons==1){xnbr=Math.round((x-self.lvlv-self.hmarshift*(self.marhpx/2))/(self.marhpx/self.xdiv));if(isNaN(xnbr)||xnbr<0||xnbr>=self.datalength)return false;if(self.mouseclickLR&&!ev.shiftKey){let sens=self.hintpointw;let xzero=self.lvlv+xnbr*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);let wdh=self.marhpx*self.barperc;let wob=wdh/self.barcnt;let difx=self.marhpx*self.barperc/2-wob/2;let locbarnr=0;let xa=0;let ya=0;for(let i=0;i<self.data.length;i++){if(typeof self.data[i]=="undefined")continue;ya=self.zlvl-self.data[i][xnbr+self.from*self.zoom]*self.wght*self.dcorr*self.zcorr;xa=xzero;if(isBar(i)){locbarnr++;xa=xa-difx+wob*(locbarnr-1)}if(Math.abs(xa-x)<sens&&Math.abs(ya-y)<sens)return}let LRmarg=self.LRsize*self.drv;if(x-self.drl<LRmarg)self.left();if(self.drr-x<LRmarg)self.right();if(self.repeatMouseMode){if(x-self.drl<LRmarg||self.drr-x<LRmarg){if(ev.offsetX!=0&&ev.offsetY!=0)lastrepev=ev;intervalId=setTimeout(repeatMouse,self.repeatPerDelay)}}}if(ev.shiftKey){let xJump=self.lvlv+xnbr*self.marhpx/self.xdiv+self.hmarshift*(self.marhpx/2);Zfrom=xnbr+self.from;Zto=Zfrom}}on_mouseMove(ev);return false}function on_mouseUp(ev){clearInterval(intervalId);inRepeat=false;lastrepev=null;mouseisdown=false;if(mousebuttons==1&&ev.shiftKey){if(Zfrom==Zto||Zto+1<=Zfrom);else{self.from=Zfrom;self.to=Zto+1;self.zoom=true;self.draw()}}mousebuttons=ev.buttons;on_mouseMove(ev);return false}function on_keyDown(ev){CtrlKeyDown=ev.ctrlKey;ShiftKeyDown=ev.shiftKey;switch(event.key){case"ArrowLeft":{if(self.revarrowkeysinzoom)self.right();else self.left();if(ev.preventDefault)ev.preventDefault();break}case"ArrowRight":{if(self.revarrowkeysinzoom)self.left();else self.right();if(ev.preventDefault)ev.preventDefault();break}case"ArrowUp":{self.home();if(ev.preventDefault)ev.preventDefault();break}case"ArrowDown":{self.end();if(ev.preventDefault)ev.preventDefault();break}case"Home":{self.home();if(ev.preventDefault)ev.preventDefault();break}case"End":{self.end();if(ev.preventDefault)ev.preventDefault();break}case"Escape":{self.undozoom();if(ev.preventDefault)ev.preventDefault();break}}on_mouseMove(ev);return false}function on_keyUp(ev){CtrlKeyDown=ev.ctrlKey;ShiftKeyDown=ev.shiftKey;if(!CtrlKeyDown&&self.hintwithctrl&&!inRepeat)self.ctxl2.clearRect(0,0,self.ctxl2.canvas.width,self.ctxl2.canvas.height);if(!ev.shiftKey){Zfrom=0;Zto=0}return false}function downloadChart(fname){var image=self.ctx.canvas.toDataURL("image/png",1).replace("image/png","image/octet-stream");var anchor=document.createElement("a");anchor.setAttribute("download",fname);anchor.setAttribute("href",image);anchor.click();anchor.remove()}function DecimalSeparator(){var n=1.1;n=n.toLocaleString().substring(1,2);return n}function savecsvfile(fname){var sep=self.download_csv_listsep;var csv=self.axisXtxt;for(let i=0;i<self.data.length;i++){if(typeof self.legstr[i]=="undefined"){csv+=sep+"series_"+(i+1).toString();continue}else csv+=sep+self.legstr[i]}csv+="\n";for(let x=0;x<self.desc.length;x++){csv+=self.desc[x];for(let i=0;i<self.data.length;i++){if(self.data[i]!=null&&typeof self.data[i]!="undefined"&&self.data[i][x]!=null&&typeof self.data[i][x]!="undefined")csv+=sep+self.data[i][x].toLocaleString();else csv+=sep}csv+="\n"}var anchor=document.createElement("a");anchor.setAttribute("href","data:text/csv;charset=utf-8,"+encodeURI(csv));anchor.setAttribute("download",fname);anchor.click();anchor.remove()}function onContextMenu(ev){ev=ev||window.ev;if(ev.stopPropagation)ev.stopPropagation();if(ev.preventDefault)ev.preventDefault();ev.cancelBubble=true;ev.returnValue=false;if(ev.altKey&&!ev.ctrlKey&&self.downloadChartEnable)downloadChart(self.download_png_fname);if(ev.altKey&&ev.ctrlKey)savecsvfile(self.download_csv_fname);return}if(this.eventsactivated==false){if(this.onmouseclick)this.ctxl2.canvas.addEventListener("click",on_canvas_click,true);if(this.onmousemove)this.ctxl2.canvas.addEventListener("mousemove",on_mouseMove,true);if(this.ontouchmove)this.ctxl2.canvas.addEventListener("touchmove",on_mouseMove,true);if(this.onmousedown)this.ctxl2.canvas.addEventListener("mousedown",on_mouseDown,true);if(this.onmouseup)this.ctxl2.canvas.addEventListener("mouseup",on_mouseUp,true);if(this.onkeydown)this.ctxl2.canvas.addEventListener("keydown",on_keyDown,true);if(this.onkeyup)this.ctxl2.canvas.addEventListener("keyup",on_keyUp,true);if(this.oncontext)this.ctxl2.canvas.addEventListener("contextmenu",onContextMenu,false);this.ctxl2.canvas.setAttribute("tabindex",0);this.ctxl2.canvas.focus();if(this.onmouseclick||this.onmousemove||this.ontouchmove)this.ctxl2.canvas.addEventListener("mouseout",on_mouseout,true);this.eventsactivated=true}this.datalength=this.alldatalength;this.wght=0;this.barcnt=0;this.barnr=0;this.marhpx=0;minv=Infinity;maxv=-Infinity;if(Array.isArray(this.data)){for(let i=0;i<this.data.length;i++){if(typeof this.data[i]=="undefined")continue;if(isBar(i)){self.barcnt++;self.hmarshift=true}if(this.Yzoom&this.YonlyVisiblescale&getStyle(this.style[i],"visible","---")=="NO")continue;rangev=getMinMax(this.data[i]);if(rangev.min<minv)minv=rangev.min;if(rangev.max>maxv)maxv=rangev.max}}if(minv==Infinity)minv=0;if(maxv==-Infinity)maxv=0;if(this.Yzoom&&this.Ymax==0&&this.Ymin!=0)this.Yzoom=false;if(this.Yzoom&&this.Ymax!=0&&this.Ymin==0)this.Yzoom=false;if(maxv>0&&minv>0&&!this.Yzoom)minv=0;if(maxv<0&&minv<0&&!this.Yzoom)maxv=0;if(this.Ymax<=this.Ymin){this.Ymax=0;this.Ymin=0}let checkok=true;if(this.from<0||this.from>this.alldatalength)checkok=false;if(this.to<0||this.to>this.alldatalength)checkok=false;if(this.to-this.from<2)checkok=false;if(!checkok){this.from=0;this.to=this.alldatalength;this.datalength=this.alldatalength;this.zoom=false}if(this.Ymax!=0)maxv=this.Ymax;if(this.Ymin!=0)minv=this.Ymin;this.zoffs=0;this.zcorr=1;if(this.Yzoom&&maxv>0&&minv>0){this.zoffs=-minv;this.zcorr=maxv/(maxv-minv)}if(this.Yzoom&&maxv<0&&minv<0){this.zoffs=-maxv;this.zcorr=Math.abs(minv)/(Math.abs(minv)-Math.abs(maxv))}if(this.axisdecimalX<0||this.axisdecimalX>3)this.axisdecimalX=1;if(this.axisdecimalY<0||this.axisdecimalY>3)this.axisdecimalY=1;if(this.valuedecimal<0||this.valuedecimal>5)this.valuedecimal=0;if(typeof this.desc=="undefined"||this.desc==null||this.desc==0||this.desc.length==0){this.desc=[];for(let i=0;i<this.datalength;i++)this.desc.push(i+1)}for(let i=0;i<this.desc.length;i++){if(typeof this.desc[i]=="undefined"||this.desc[i]==null)this.desc[i]=""}this.ctx.beginPath();this.ctx.clearRect(0,0,this.ctx.canvas.width,this.ctx.canvas.height);this.ctx.fillStyle=this.canvasbkcol;this.ctx.lineWidth=this.canvasfrw;this.ctx.fillRect(0,0,this.ctx.canvas.width,this.ctx.canvas.height);this.ctx.stroke();this.ctx.closePath();this.legend();this.ctx.strokeStyle=this.canvasfrcol;if(this.drawzfrm){this.ctx.beginPath();this.ctx.rect(0,0,this.ctx.canvas.width,this.ctx.canvas.height);this.ctx.stroke();this.ctx.closePath()}this.ctx.lineWidth=1;this.ctx.font=this.descfontmod+this.descfpx+this.descfont;this.ctx.lineCap="round";this.ctx.lineJoin="miter";this.ctx.miterLimit=1;this.ctxl2.lineWidth=1;this.ctxl2.font=this.descfontmod+this.descfpx+this.descfont;this.ctxl2.lineCap="round";this.ctxl2.lineJoin="miter";this.ctxl2.miterLimit=1;this.aYtxt=self.axisYtxt.toString();let tm=Math.abs(Math.max(Math.abs(maxv),Math.abs(minv)));if(tm*Math.pow(10,this.valuedecimal)<1){let tw=1;let ti=0;if(tm>0)while(tm*tw<10){tw*=10;ti++;if(ti>10)break}if(ti>0){this.aYtxt="*(10^-"+ti+")"+self.axisYtxt.toString();minv*=tw;maxv*=tw;this.dcorr=tw}if(this.valuedecimal==0)this.valuedecimal=2}tm=Math.abs(Math.max(Math.abs(maxv),Math.abs(minv)));if(tm>this.bigmax){let tw=1;let ti=0;while(tm/tw>this.bigmax){tw=tw*10;ti++;if(ti>10)break}if(ti>0){this.aYtxt="*(10^"+ti+")"+self.axisYtxt.toString();minv=minv/tw;maxv=maxv/tw;this.dcorr=1/tw}this.valuedecimal=0}if(!this.zoom)this.datalength=this.alldatalength;else this.datalength=this.to-this.from;if(maxv>=0&&minv==0)this.zerox=0;if(minv<0&&maxv==0)this.zerox=this.drb-this.drt;if(maxv>0&&minv<0)this.zerox=Math.abs(minv/(maxv-minv)*this.drv);this.zlvl=this.drb-this.zerox;if(this.datalength==0)this.zlvl=this.drb;let marwxdesc=0;let maxw=0;for(let i=this.from*this.zoom;i<this.desc.length;i++){maxw=this.ctx.measureText(this.desc[i].toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalX,maximumFractionDigits:this.axisdecimalX})).width;if(maxw>marwxdesc)marwxdesc=maxw}if(!this.drawXdesc)marwxdesc=0;if(this.drb-this.zlvl<marwxdesc){this.drb=this.bot-this.marsize/2-marwxdesc-this.descfpx/2;this.drv=this.drb-this.drt;if(maxv>0&&minv<0)this.zerox=Math.abs(minv/(maxv-minv)*this.drv);else if(maxv>=0)this.zerox=0;else if(minv<0)this.zerox=this.drb-this.drt;this.zlvl=this.drb-this.zerox}this.marpl=Math.round(Math.abs((this.zlvl-this.drt)/this.descfpx));this.marmi=Math.round(Math.abs((this.drb-this.zlvl)/this.descfpx));this.marall=this.marpl+this.marmi;marpldata=getMaxMarkerValue(maxv-(minv>0?minv*this.Yzoom:0),this.marpl);this.marpl=marpldata.cnt;vmp=marpldata.value;marmidata=getMaxMarkerValue(minv-(maxv<0?maxv*this.Yzoom:0),this.marmi);this.marmi=marmidata.cnt;vmm=marmidata.value;this.marpx=Math.abs((this.zlvl-this.drt)/this.marpl);this.marmx=Math.abs((this.drb-this.zlvl)/this.marmi);this.marax=Math.max(this.marpx,this.marmx);this.marpx=this.marax;this.marmx=this.marax;if(vmp>vmm)vmm=vmp;if(vmm>vmp)vmp=vmm;this.marpl=Math.floor(Math.abs((this.zlvl-this.drt)/this.marax));this.marmi=Math.floor(Math.abs((this.drb-this.zlvl)/this.marax));this.marall=this.marpl+this.marmi;let wp=this.ctx.measureText(maxv.toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalY,maximumFractionDigits:this.axisdecimalY})).width+this.descfpx;let wm=this.ctx.measureText(minv.toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalY,maximumFractionDigits:this.axisdecimalY})).width+this.descfpx;if(!this.drawYdesc){wp=0;wm=0}let lvlt=this.lft+this.marsize*this.drawmark/2+Math.max(wp,wm);if(lvlt>this.lvlv){this.lvlv=lvlt;this.drl=this.lvlv}if(this.rotdescX)maxhm=Math.round(Math.abs((this.lvlv-this.drr)/this.descfpx))+1;else maxhm=Math.round(Math.abs((this.lvlv-this.drr)/marwxdesc))+1;this.marxdata=getXmarkersDiv(this.datalength-0,maxhm);maxhm=this.marxdata.cnt;this.xdiv=this.marxdata.div;marh=Math.min(this.datalength-1,maxhm)+1*this.hmarshift;this.marhpx=(this.drr-this.lvlv)/marh;if(this.chartzone){this.ctx.strokeStyle=this.chartzonecol;this.ctx.rect(this.lft,this.top,this.rgt-this.lft,this.bot-this.top);this.ctx.stroke()}if(this.drawzone){this.ctx.strokeStyle=this.drwfracol;this.ctx.fillStyle=this.drwfilcol;this.ctx.beginPath();if(this.drawzfrm)this.ctx.rect(this.drl,this.drt,this.drr-this.drl,this.drb-this.drt);this.ctx.fillRect(this.drl,this.drt,this.drr-this.drl,this.drb-this.drt);this.ctx.stroke()}if(this.drawmesh){let zm=false;this.ctx.lineWidth=this.meshlw;this.ctx.strokeStyle=this.mshcol;this.ctx.fillStyle=this.mshcol;if(this.meshframe){this.ctx.rect(this.drl,this.drt,this.drr-this.drl,this.drb-this.drt);this.ctx.stroke()}let i=0;let lvl=this.zlvl-i*this.marpx;while(lvl>=this.drt){this.ctx.beginPath();this.ctx.moveTo(this.lvlv,this.zlvl-i*this.marpx);this.ctx.lineTo(this.drr,this.zlvl-i*this.marpx);this.ctx.stroke();i++;lvl=this.zlvl-i*this.marpx}i=1;lvl=this.zlvl+i*this.marmx;while(lvl<=this.drb){this.ctx.beginPath();this.ctx.moveTo(this.lvlv,this.zlvl+i*this.marmx);this.ctx.lineTo(this.drr,this.zlvl+i*this.marmx);i++;lvl=this.zlvl+i*this.marmx;this.ctx.stroke()}}if(this.drawmesh){this.ctx.strokeStyle=this.mshcol;this.ctx.fillStyle=this.mshcol;let i=0;i=1*this.hmarshift;if(this.meshframe)i=1;let hor=0;hor=this.lvlv+i*this.marhpx-this.hmarshift*(this.marhpx/2);while(hor<=this.drr){if(i>marh)break;this.ctx.beginPath();this.ctx.moveTo(hor,this.drt);this.ctx.lineTo(hor,this.drb);this.ctx.stroke();i++;hor=this.lvlv+i*this.marhpx-this.hmarshift*(this.marhpx/2)}}if(this.vsOn&&this.zoom){let zoomlvl=this.top+this.al;if(this.vzonXaxis)zoomlvl=this.zlvl;let c=this.ctx;c.lineCap="round";c.lineJoin="miter";c.miterLimit=1;c.strokeStyle=changeRGBAalpha(this.vzcolor,this.vzalpha);c.fillStyle=this.vzcolor;c.lineWidth=this.vzwdtsm;c.beginPath();c.moveTo(this.drl,zoomlvl);c.lineTo(this.drr,zoomlvl);c.stroke();let all=(this.drr-this.drl)/this.alldatalength;c.lineWidth=this.vzwdtlg;c.beginPath();c.moveTo(this.drl+this.from*all,zoomlvl);c.lineTo(this.drl+this.to*all,zoomlvl);c.stroke();c.closePath()}if(this.drawmark||this.drawYdesc){this.ctx.lineWidth=this.marw;this.ctx.strokeStyle=this.descol;this.ctx.fillStyle=this.descol;this.ctx.textBaseline="middle";this.ctx.textAlign="end";let i=0;let lvl=0;let zerodwd=false;if(maxv>0){i=1*!this.drw0y;zerodwd=true;lvl=this.zlvl-i*this.marpx;while(lvl>=this.drt){this.ctx.beginPath();if(this.drawmark){this.ctx.moveTo(this.lvlv-this.marsize/2,lvl);this.ctx.lineTo(this.lvlv+this.marsize/2,lvl)}if(this.drawYdesc){let ofs=minv>0&&this.Yzoom?minv:0;this.ctx.fillText((ofs+i*vmp).toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalY,maximumFractionDigits:this.axisdecimalY}),this.lvlv-this.marsize,lvl)}i++;lvl=this.zlvl-i*this.marpx;this.ctx.stroke()}}if(minv<0){if(!zerodwd)i=1*!this.drw0y;else i=1;lvl=this.zlvl+i*this.marmx;while(lvl<=this.drb){this.ctx.beginPath();this.ctx.moveTo(this.lvlv-this.marsize/2,lvl);this.ctx.lineTo(this.lvlv+this.marsize/2,lvl);if(this.drawYdesc){let ofs=maxv<0&&this.Yzoom?maxv:0;this.ctx.fillText((ofs-i*vmm).toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalY,maximumFractionDigits:this.axisdecimalY}),this.lvlv-this.marsize,lvl)}i++;lvl=this.zlvl+i*this.marmx;this.ctx.stroke()}}}if(this.drawmark||this.drawXdesc){this.ctx.lineWidth=this.marw;this.ctx.strokeStyle=this.descol;this.ctx.fillStyle=this.descol;this.ctx.textBaseline="top";this.ctx.textAlign="center";let i=1;if(maxhm>0){i=i+1*!this.drw0x-this.hmarshift;let hor=this.lvlv+i*this.marhpx+this.hmarshift*(this.marhpx/2);while(hor<=this.drr){if(i>marh)break;this.ctx.beginPath();if(this.drawmark){this.ctx.moveTo(hor,this.zlvl-this.marsize/2);this.ctx.lineTo(hor,this.zlvl+this.marsize/2)}if(this.drawXdesc){if(!this.rotdescX){if(typeof this.desc[i*this.xdiv]!="undefined")this.ctx.fillText(this.desc[i*this.xdiv+this.from*this.zoom],hor-(i==0)*this.descfpx/2,this.zlvl+this.marsize)}else{this.ctx.save();this.ctx.textBaseline="middle";this.ctx.textAlign="end";this.ctx.translate(this.drl,this.drb);this.ctx.rotate(-.5*Math.PI);let rhor=i*this.marhpx+this.hmarshift*(this.marhpx/2);if(typeof this.desc[i*this.xdiv+this.from*this.zoom]!="undefined")this.ctx.fillText(this.desc[i*this.xdiv+this.from*this.zoom].toLocaleString(undefined,{useGrouping:this.dgroup,minimumFractionDigits:this.axisdecimalX,maximumFractionDigits:this.axisdecimalX}),this.drb-this.zlvl-this.marsize,rhor+this.descfpx/2*(i==0)*!this.hmarshift);this.ctx.restore()}}i++;hor=this.lvlv+i*this.marhpx+this.hmarshift*(this.marhpx/2);this.ctx.stroke()}}}if(this.drawaxis){this.ctx.strokeStyle=this.axcol;this.ctx.fillStyle=this.axcol;this.ctx.lineWidth=this.axw;this.ctx.font=this.descfontmod+this.descfpx+this.descfont;this.ctx.beginPath();this.ctx.moveTo(this.lvlv,this.zlvl);this.ctx.lineTo(this.rgt-this.axw*3*this.drawarrow,this.zlvl);this.ctx.moveTo(this.lvlv,this.bot-(this.bot-this.drb)/2);this.ctx.lineTo(this.lvlv,this.top+this.axw*3*this.drawarrow);this.ctx.stroke();if(this.drawarrow){this.ctx.beginPath();this.ctx.lineTo(this.rgt,this.zlvl);this.ctx.lineTo(this.rgt-this.al,this.zlvl-this.aw/2);this.ctx.lineTo(this.rgt-this.al,this.zlvl+this.aw/2);this.ctx.lineTo(this.rgt,this.zlvl);this.ctx.fillStyle=this.axcol;this.ctx.moveTo(this.lvlv,this.top);this.ctx.lineTo(this.lvlv-this.aw/2,this.top+this.al);this.ctx.lineTo(this.lvlv+this.aw/2,this.top+this.al);this.ctx.lineTo(this.lvlv,this.top);this.ctx.fillStyle=this.axcol;this.ctx.fill()}this.ctx.beginPath();if(this.axisXdesc){this.ctx.fillStyle=this.axdesccol;let txw=this.ctx.measureText(self.axisXtxt).width;if(txw+this.drr+this.descfpx/2>this.rgt){this.ctx.textBaseline="top";this.ctx.textAlign="end";let wa=0;if(txw+this.descfpx>this.drb-this.zlvl){this.ctx.textAlign="start";wa=this.descfpx*2}this.ctx.save();this.ctx.translate(this.drr,this.zlvl);this.ctx.rotate(-.5*Math.PI);this.ctx.fillText(self.axisXtxt,-this.descfpx+wa,this.descfpx*.5);this.ctx.restore()}else{this.ctx.textAlign="start";this.ctx.textBaseline="top";this.ctx.fillText(self.axisXtxt,this.drr+this.descfpx/2,this.zlvl+this.descfpx/2)}}if(this.axisYdesc){this.ctx.textAlign="start";this.ctx.textBaseline="middle";this.ctx.fillStyle=this.axdesccol;this.ctx.fillText(this.aYtxt,this.lvlv+this.aw/1,this.top+this.al/2)}}do_graph();on_mouseMove(null)}}
      </script>

  </body>
</html>


    )rawliteral";

    #endif
    