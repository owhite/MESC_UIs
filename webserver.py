from flask import Flask, render_template, request, jsonify

class MyFlaskApp:
    def __init__(self):
        self.app = Flask(__name__)
        self.setup_routes()

    def setup_routes(self):
        @self.app.route('/')
        def index():
            return render_template('index.html')

        @self.app.route('/button1_click', methods=['POST'])
        def button1_click():
            button_id = request.json['button_id']
            # Perform the Python command for button 1
            print(f'Button 1 clicked! Button ID: {button_id}')
            return jsonify({'status': 'success', 'button_id': button_id})

        @self.app.route('/button2_click', methods=['POST'])
        def button2_click():
            button_id = request.json['button_id']
            # Perform the Python command for button 2
            print(f'Button 2 clicked! Button ID: {button_id}')
            return jsonify({'status': 'success', 'button_id': button_id})

        @self.app.route('/button3_click', methods=['POST'])
        def button3_click():
            button_id = request.json['button_id']
            # Perform the Python command for button 3
            print(f'Button 3 clicked! Button ID: {button_id}')
            return jsonify({'status': 'success', 'button_id': button_id})

    def run(self):
        self.app.run(debug=True)

if __name__ == '__main__':
    app = MyFlaskApp()
    app.run()
