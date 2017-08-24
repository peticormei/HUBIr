from flask import Flask, json, request
from flask_restful import Resource, Api

app = Flask(__name__)
api = Api(app)

class SSID(Resource):
    def post(self):
        json_data = request.get_json()
        print(json_data)
        json_response = json.dumps({"status":"OK"})
        return json_response

class IR(Resource):
    def post(self):
        json_data = request.get_json()
        print(json_data)
        json_response = json.dumps({"status":"OK"})
        return json_response

api.add_resource(IR, '/ir')
api.add_resource(SSID, '/new_ssid')

if __name__ == '__main__':
    app.run(host="0.0.0.0", debug=True)
