from flask import Flask, send_file, render_template, jsonify

class MyFlaskApp:
    def __init__(self, config_file="config.ini"):
        self.app = Flask(__name__)
        self.image_visible = False
        self._setup_routes()

    def run(self):
        self.app.run(debug=True)

    def stop(self):
        # Any cleanup code if needed
        pass

    def _setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('pics.html', image_visible=self.image_visible)

        @self.app.route('/create_image', methods=['POST'])
        def create_image():
            # Since the file already exists, no need to create it
            self.image_visible = True
            print("CREATE", self.image_visible)
            return '', 204

        @self.app.route('/toggle_image', methods=['POST'])
        def toggle_image():
            self.image_visible = not self.image_visible
            print("TOGGLE", self.image_visible)
            return jsonify(image_visible=self.image_visible)

        @self.app.route('/get_image', methods=['GET'])
        def get_image():
            print("GET", self.image_visible)
            if self.image_visible:
                return send_file('static/Stempy2.jpg', mimetype='image/jpg')
            else:
                return '', 204

        @self.app.route('/image_status', methods=['GET'])
        def image_status():
            return jsonify(image_visible=self.image_visible)

if __name__ == '__main__':
    app = MyFlaskApp()
    try:
        app.run()
    finally:
        app.stop()
