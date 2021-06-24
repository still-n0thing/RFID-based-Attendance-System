import thingspeak as ts
import json
import csv
import time

import apikey  # comment this code 

# API Key
channel_id = apikey.my_channel_id # Type your channel id here
write_api_key = apikey.my_write_api_key # Type your write api key here
read_api_key = apikey.my_read_api_key # Type your read api key here

def main():
    # Channel Object
    ch = ts.Channel(channel_id, read_api_key)

    # Data from server and its format
    data = json.loads(ch.get())["feeds"] # List with dictonary as each index
    kys = ["created_at", "entry_id", "field1"] # Keys of each dictonary 

    # CSV Format
    fields = ['Date Time', 'Entry_ID', 'Roll No.', 'Name']

    # Students Data
    students = []
    with open("students.json",) as infile:
        students = json.load(infile)

    arr = []
    arr.append(fields)

    for entry in data:
        lst = [None for i in range(len(fields))]
        
        i = 0
        for k in kys:
            lst[i] = entry[k]
            i += 1

        lst[i] = students[lst[i-1]]
        
        arr.append(lst)
        
    with open("records.csv", 'w', newline = '') as csvfile: 
        # creating a csv writer object 
        csvwriter = csv.writer(csvfile) 
            
        # writing the data rows 
        csvwriter.writerows(arr)

while True:
    print("Updating the csv...")
    main()
    print("Updating done!")
    time.sleep(5)

# print(arr)

# To create students list
# mp = {
#         1 : "Student 1",
#         2 : "Student 2",
#         3 : "Student 3",
#         4 : "Student 4"
        
# }
# with open("students.json", "w") as outfile: 
#     json.dump(mp, outfile)


# students = []
# with open("students.json",) as infile:
#     students = json.load(infile)

# print(data)
