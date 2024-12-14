from flask import jsonify
import requests
from math import sqrt
from time import sleep

if __name__ == '__main__':
    data_list = []
    for i in range (0, 100):
        data_list.append({
            'temperature': sqrt(i) + 2,
            'humidity': i,
            'date': f'2024-12-{i%30}T17:51:48'
        })
    for data in data_list:
        response = requests.post('http://localhost:9000/api/data', json=data)
        print(response.status_code)
        print(response.json())
        sleep(1.2)