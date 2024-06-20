from flask import Flask, render_template, jsonify
from threading import Thread
import time
import logging

class MyFlaskApp:
    def __init__(self, config_file="config.ini"):
        self.app = Flask(__name__)
        self.upload_status_str = "Thread not started"
        self.worker_thread = None
        self._setup_routes()
        self._setup_logging()

    def _setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('upload.html')

        @self.app.route('/upload_thread', methods=['POST'])
        def upload_thread():
            if self.worker_thread is None or not self.worker_thread.is_alive():
                self.worker_thread = Thread(target=self.upload_thread)
                self.worker_thread.start()
            return jsonify(status=self.upload_status_str)

        @self.app.route('/upload_status', methods=['GET'])
        def upload_status_route():
            return jsonify(status=self.upload_status_str)

    def upload_thread(self):
        self.upload_status_str = "Upload started"
        time.sleep(2)
        self.upload_status_str = "Upload finished"

    def run(self):
        self.app.run(debug=True)

    def stop(self):
        pass  # No need to stop anything as we're using the Flask development server

    def _setup_logging(self):
        class NoStatusFilter(logging.Filter):
            def filter(self, record):
                return not record.getMessage().startswith('127.0.0.1 - - [') or 'GET /upload_status' not in record.getMessage()

        log = logging.getLogger('werkzeug')
        log.addFilter(NoStatusFilter())

if __name__ == '__main__':
    app = MyFlaskApp(config_file="config.ini")
    try:
        app.run()
    except KeyboardInterrupt:
        app.stop()
