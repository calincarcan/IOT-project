from flask import Flask, jsonify, request
from datetime import datetime

app = Flask(__name__)

DATA = []

@app.route('/api/data', methods=['POST'])
def add_data():
    data = request.get_json()
    try:
        new_data = {
            'temperature': data.get('temperature'),
            'humidity': data.get('humidity'),
            'time': datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
        }
        print(new_data)
        DATA.append(new_data)
        return jsonify({"error": "Success!"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 400

@app.route('/api/data', methods=['GET'])
def get_data():
    return jsonify(DATA), 200

@app.route('/api/data', methods=['DELETE'])
def delete_data():
    DATA.clear()
    return jsonify({"error": "Success!"}), 200

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=9000)