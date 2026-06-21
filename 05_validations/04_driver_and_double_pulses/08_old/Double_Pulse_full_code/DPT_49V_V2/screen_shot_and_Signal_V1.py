################## IMAGE ######################################


from datetime import datetime
import requests


# Make timestamp
now = datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
print(timestamp)


import urllib3, certifi, sys
#print("urllib3:", urllib3.__version__)
#print("certifi:", certifi.__version__)
# print("Python:", sys.version)
"""
Expectation 
urllib3: 1.26.20
certifi: 2025.07.14

USE this
pip install requests==2.32.5
pip install "urllib3<2"
"""



filename = f"screen_{timestamp}"
scope_ip = '169.254.104.98'





import requests
def get_image(title, scope_ip ):
    url = url = f"http://{scope_ip}/Image.png"
    
    # Download the image as binary you can add timout=10
    response = requests.get(url, stream=True)
    response.raise_for_status()  # check for HTTP errors
    with open(f"{title}.png", "wb") as f:
        f.write(response.content)

get_image(filename, scope_ip )
print(filename)












########################## SIGNAL ###############################################
from datetime import datetime



# Make timestamp
now = datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
print(timestamp)


import numpy as np  


#%pip install pyvisa
#%pip install pyvisa-py
import pyvisa  
#print("pyvisa==",pyvisa.__version__)

#scope_ip = '169.254.104.98'

rm = pyvisa.ResourceManager('@py') # Use pyvisa-py backend  

 
resource_str= f"TCPIP::{scope_ip}::INSTR"  
try:  
    scope.close()  
except: pass  
try:  
    scope = rm.open_resource(resource_str)  
    print("Connected to:", scope.query("*IDN?"))  
except Exception as e:  
    print("Connection failed:", e)  

def get_active_channels():
    active_channels = []
    for i in range(1, 5):  # assuming up to CH4
        try:
            response = scope.query(f"SELECT:CH{i}?").strip()
            response = response.split(" ")[-1]
            print(f"CH{i}", response)
            if response in ['1', 'ON']:  # depending on oscilloscope
                active_channels.append(f"CH{i}")
        except Exception as e:
            print(f"Error querying CH{i}: {e}")
    return active_channels
chanels = get_active_channels()
print(chanels)



def get_general_config(): 

    DIC = {}

    params = ["HORIZONTAL:MAIN:SCALE?", "WFMPRE:NR_Pt?"]
    dic={}
    for p in params: 
        try: 
            dic[p]= scope.query(p)#.replace("\n", "")
        except: pass
    DIC["general"]= dic


    params = [
        "TRIGger:STATE?",
        "TRIGger:MAIn:EDGE:SOURce?",
        "TRIGger:MAIn:EDGE:SLOPe?",
        "TRIGger:MAIn:EDGE:COUPling?",
        "TRIGger:MAIn:LEVel?"
    ]
    dic={}
    for p in params: 
        try: 
            dic[p]= scope.query(p).replace("\n", "")
        except: pass
    DIC["trigger"]= dic
    
    return DIC
d = get_general_config()
print(d)

def get_raw_data(channel = "CH1"): 
    channel = channel .upper()

    dic ={}
    dic["channel"]= channel

    
    scope.write("DATA:SOURCE "+channel)  
    # Set binary format  
    scope.write("DATA:ENC RIB") # Signed binary  
    scope.write("DATA:WIDTH 2") # 2 byte per point  
    
    # Read waveform settings for scaling  
    x_increment = scope.query("WFMPRE:XINCR?")
    x_origin = scope.query("WFMPRE:XZERO?") 
    y_increment = scope.query("WFMPRE:YMULT?")
    y_origin = scope.query("WFMPRE:YZERO?")  
    y_offset = scope.query("WFMPRE:YOFF?")
    dic["x_increment"]= x_increment
    dic["x_origin"]= x_origin
    dic["y_increment"]= y_increment
    dic["y_origin"]= y_origin
    dic["y_offset"]= y_offset
    
    
    # set your desired record length  
    scope.write("DATA:START 1")  
    scope.write("DATA:STOP 10000")  
    
    # this issues the CURVE? query and returns a numpy int16 array  
    raw = scope.query_binary_values(  
    'CURVE?',  
    datatype='h', # 'h' = signed 16‑bit  
    is_big_endian=True, # or False depending on your scope  
    container=np.array  
    )  
    
    
    # Convert bytes to numpy array  
    wave = np.array( raw)  
    # Scale data  
    #voltages = (wave - y_offset) * y_increment + y_origin  
    #times = np.arange(len(voltages)) * x_increment + x_origin 
    dic["raw_data"]= raw
    
    # Scale data  
    #voltages = (wave - y_offset) * y_increment + y_origin  
    #times = np.arange(len(voltages)) * x_increment + x_origin 



    params = ["PROBe", "IMPEDANCE", "COUPLING", 
              "OFFSET", "POSITION", "BANDWIDTH", "INVERT", "SCALE"]


    for p in params: 
        try:
            dic[p]= scope.query(channel+":"+p+"?")#.replace("\n", "")
        except: pass
    

        
    return dic# times, voltages

d = get_raw_data(channel = "CH1")
print(d.keys())



from datetime import datetime
import requests


# Make timestamp
now = datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
print(timestamp)

chanels = get_active_channels()
res={}
res["general_config"]= get_general_config()
channels = get_active_channels()
for channel in channels: 
    res[channel]= get_raw_data(channel = channel)
    lis= list(res[channel]['raw_data'].astype("int16"))
    lis =[int(x) for x in lis]
    res[channel]['raw_data'] = lis
    
import pprint
with open(f"{timestamp}.txt", "w") as f:
    #f.write(str(res))
    f.write(pprint.pformat(res))

print("TXT DONNNNNNNNE")



########################### FROM TEXT TO CSV
file = f"{timestamp}.txt"

import ast
import numpy as np 

with open(file , "r") as f:
    text = f.read()

res = ast.literal_eval(text)  # Safe conversion from string to dict
tim_div = float(res['general_config']['general']['HORIZONTAL:MAIN:SCALE?'].split(" ")[-1])
print(tim_div)


Npoints= float(res['general_config']['general']['WFMPRE:NR_Pt?'].split(" ")[-1])
print(Npoints)

# Other method 
ch = list(res.keys())[0]
Ts = float(res[ch]['x_increment'].split(" ")[-1])
print(Ts)


dicres = {}
for channel in res.keys(): 
    if "CH" in channel : 
        y_increment =  float(res[channel]["y_increment"].split(" ")[-1])
        y_origin =  float(res[channel]["y_origin"].split(" ")[-1])
        y_offset =  float(res[channel]["y_offset"].split(" ")[-1])
        raw = np.array(res[channel]['raw_data'])
        voltage = (raw - y_offset) * y_increment + y_origin
        dicres[channel]= voltage
time = np.arange(0, len(voltage)*Ts, Ts)
dicres["time"]=time




import matplotlib.pyplot as plt
import numpy as np 
import pandas as pd

df= pd.DataFrame(dicres).astype("float64")
df.loc[0, "Ts"]=Ts
display(df)
df.to_csv(file.replace(".txt",".csv"), index = False)

print("CSV DONNNNNE")


