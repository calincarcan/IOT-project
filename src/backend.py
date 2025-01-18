from flask import Flask, request, jsonify, render_template
from flask_tinydb import TinyDB
from datetime import datetime
from flask_cors import CORS
import smtplib, os
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart

app = Flask("server")
CORS(app)
db = TinyDB(app).get_db()

ALERT_EMAIL = "calincarcan@gmail.com"
ALERT_PASSWORD = os.getenv("EMAIL_PASSWORD")

TEMPERATURE_ALERT = 26
HUMIDITY_ALERT = 60

def send_alert(subject, body):
    msg = MIMEMultipart()
    msg['From'] = ALERT_EMAIL
    msg['To'] = ALERT_EMAIL
    msg['Subject'] = subject
    msg.attach(MIMEText(body, 'plain'))

    server = smtplib.SMTP('smtp.gmail.com', 587)
    server.starttls()
    server.login(ALERT_EMAIL, ALERT_PASSWORD)
    text = msg.as_string()
    server.sendmail(ALERT_EMAIL, ALERT_EMAIL, text)
    server.quit()

@app.route('/')
def home():
    return "Server is up!"

@app.route('/api/alert/temperature/<int:temperature_alert>', methods=['GET'])
def set_temperature_alert(temperature_alert):
    global TEMPERATURE_ALERT
    try:
        TEMPERATURE_ALERT = temperature_alert
        return jsonify({"message": "Temperature alert threshold updated!", "temperature_alert": TEMPERATURE_ALERT}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 400

@app.route('/api/alert/humidity/<int:humidity_alert>', methods=['GET'])
def set_humidity_alert(humidity_alert):
    global HUMIDITY_ALERT
    try:
        HUMIDITY_ALERT = humidity_alert
        return jsonify({"message": "Humidity alert threshold updated!", "humidity_alert": HUMIDITY_ALERT}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 400

@app.route('/api/data', methods=['POST'])
def add_data():
    data = request.get_json()
    try:
        new_data = {
            'sensor_id': int(data.get('sensor_id')),
            'temperature': int(data.get('temperature')),
            'humidity': int(data.get('humidity')),
            'time': datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
        }
        db.insert(new_data)
        if new_data['temperature'] > TEMPERATURE_ALERT:
            send_alert("Temperature Alert", f"Temperature is {new_data['temperature']}°C")
        if new_data['humidity'] > HUMIDITY_ALERT:
            send_alert("Humidity Alert", f"Humidity is {new_data['humidity']}%")
        print(f'Inserted: {new_data}')
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