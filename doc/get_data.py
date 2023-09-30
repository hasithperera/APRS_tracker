import requests
import json

# Define the URL of the JSON data
json_url = "https://api.aprs.fi/api/get?name=KE8TJE-11&what=loc&apikey=165271.TFyvnYi9KK3Kda9&format=jason"

# Send an HTTP GET request to the URL
response = requests.get(json_url)

# Check if the request was successful (status code 200)
if response.status_code == 200:

	json_data = response.json()
	data = json_data['entries'][0]
	with open('test.html','w') as fp:
		fp.write(f"<h2>lat: {data['lat']}</h2> \n")
		fp.write(f"<h2>lng:{data['lng']}</h2>")
else:
	pass
	#print("Failed to retrieve JSON data. Status code:", response.status_code)

