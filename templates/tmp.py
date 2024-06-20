from flask import Flask, render_template
from flask_socketio import SocketIO
import datetime

app = Flask(__name__)
socketio = SocketIO(app)

@app.route('/')
def index():
    return render_template('index.html')

def update_banner_with_time():
    current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    socketio.emit('update_banner', {'banner_text': current_time})

@socketio.on('connect')
def handle_connect():
    print('Client connected')

@socketio.on('disconnect')
def handle_disconnect():
    print('Client disconnected')

if __name__ == '__main__':
    # Emit the current time every 5 seconds for demonstration
    import threading

    def emit_time():
        while True:
            update_banner_with_time()
            socketio.sleep(5)

    threading.Thread(target=emit_time).start()
    socketio.run(app, host='0.0.0.0', port=5000)
