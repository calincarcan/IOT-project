from flask import Flask, request, jsonify, render_template
from flask_tinydb import TinyDB
from datetime import datetime
from flask_cors import CORS

app = Flask("server")
CORS(app)
db = TinyDB(app).get_db()

@app.route('/')
def home():
    # return render_template('index.html')
    return "Hello, World!"

@app.route('/api/data', methods=['POST'])
def add_data():
    data = request.get_json()
    try:
        new_data = {
            'sensor_id': data.get('sensor_id'),
            'temperature': data.get('temperature'),
            'humidity': data.get('humidity'),
            'time': datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
        }
        print(new_data)
        db.insert(new_data)
        return jsonify({"error": "Success!"}), 200
    except Exception as e:
        return jsonify({"error": str(e), "payload": data}), 400

@app.route('/api/data', methods=['GET'])
def get_data():
    data = db.all()
    return jsonify(data), 200

@app.route('/api/reset')
def delete_data():
    db.truncate()
    return jsonify({'message': 'Data deleted'})

if __name__ == "__main__":
    app.run(host='0.0.0.0', port=20388)