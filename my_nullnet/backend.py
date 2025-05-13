from pymongo import MongoClient 
from pymongo.server_api import ServerApi
from dotenv import load_dotenv
import os
import datetime
import re

load_dotenv()

fifo_path = "tmp/out_pipe"
if not os.path.exists(fifo_path):
    os.mkfifo(fifo_path)

uri = os.getenv("MONGODB_URI")

# Create a new client and connect to the server
client = MongoClient(uri, server_api=ServerApi('1'))

mydb = client["mydatabase"]
mycol = mydb["data"]

# Send a ping to confirm a successful connection
try:
    client.admin.command('ping')
    print("Pinged your deployment. You successfully connected to MongoDB!")
except Exception as e:
    print(e)

# Function to parse the data received in the parent node
def parseData(line):
    pattern = r"\d+(?:\.\d+)?"
    numbers = list(filter(None, re.findall(pattern, line)))

    mydict = {
        "ID"            : numbers[6],
        "Time"          : datetime.datetime.now(),
        "Count"         : numbers[7],
        "Temperature"   : numbers[8],
        "Humidity"      : numbers[9]
    }
    result = mycol.insert_one(mydict)
    print("Inserted document ID:", result.inserted_id)

# Main loop
with open(fifo_path, 'r') as fifo:
    print("Waiting for data...")
    substring = "Message:"
    for line in fifo:
        
        print(line.strip())
        if substring in line:
            parseData(line.strip())