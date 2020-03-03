import json

from flask import render_template, request, flash, redirect, url_for, jsonify, abort
from app import app, db
from app.models import Humidity, Temperature


@app.route('/')
def index():
    temperatures = Temperature.query.all()
    return jsonify(json_list=[i.serialize for i in temperatures])

@app.route('/temperature', methods=['GET', 'POST'])
def temperature():
    if request.method == 'POST':
        if not request.json or not 'data' in request.json:
            abort(400)
        else:
            if not request.json.get('data').get('temperature'):
                abort(404)
            t = Temperature(temperature=request.json.get('data')['temperature'])
            db.session.add(t)
            db.session.commit()
            return jsonify({'status': 'ok'})
    temperatures = Temperature.query.all()
    return jsonify(temperature_data=[i.serialize for i in temperatures])

@app.route('/humidity', methods=['GET', 'POST'])
def humidity():
    if request.method == 'POST':
        if not request.json or not 'data' in request.json:
            abort(400)
        else:
            if not request.json.get('data').get('humidity'):
                abort(404)
            h = Humidity(humidity=request.json.get('data')['humidity'])
            db.session.add(h)
            db.session.commit()
            return jsonify({'status': 'ok'})
    humidity = Humidity.query.all()

    return jsonify(humidity_data=[i.serialize for i in humidity])

