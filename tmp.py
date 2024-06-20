from flask import Flask, render_template, Response
import threading
import time
import datetime

app = Flask(__name__)

def generate_time():
    while True:
        current_time = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
        yield f"data: {current_time}\n\n"
        time.sleep(1)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/time_stream')
def time_stream():
    return Response(generate_time(), mimetype='text/event-stream')

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
