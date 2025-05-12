from pymongo import MongoClient  
import os
import datetime
import re

myclient = MongoClient('mongodb://localhost:27017/')
mydb = myclient["mydatabase"]
mycol = mydb["data"]

fifo_path = "tmp/out_pipe"

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
    mycol.insert_one(mydict)
    print(mycol.inserted_ids)     

with open(fifo_path, 'r') as fifo:
    print("Waiting for data...")
    substring = "Message:"
    for line in fifo:
        
        print(line.strip())
        #print("Received:", line.strip())
        if substring in line:
            parseData(line.strip())



