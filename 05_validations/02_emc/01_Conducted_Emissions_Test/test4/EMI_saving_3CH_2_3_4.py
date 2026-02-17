#### SEETING
_= """
N_points	10000	points
time s/div (us)	20	us/div
Ts 	2.00E-08	s
saving time by windaw	200	us
Fmin	5	kHz
Fsampling	50.00	Mhz
N 	50	files
measurement time 	10	ms
conclusion 20µs/div is best choice, 50 files 				
"""
# 2026-02-03 17:49:38.999046
# 2026-02-03 17:50:00.457093
# 11 seconds for one channels 

# with delay (2 chanels)
# 2026-02-03 20:14:23.139811
# 2026-02-03 20:15:09.409055

####################################
from datetime import datetime

print(datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"))
################## IMAGE ######################################
import time

from datetime import datetime
import requests


# Make timestamp
now = datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
print("date time = " , timestamp)

import os
folder = "EMI_"+timestamp
os.makedirs(folder , exist_ok=True)


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



#filename = f"screen_{timestamp}"
filename = timestamp
scope_ip = '169.254.104.98'





import requests
def get_image(title, scope_ip ):
    url = url = f"http://{scope_ip}/Image.png"
    
    # Download the image as binary you can add timout=10
    response = requests.get(url, stream=True)
    response.raise_for_status()  # check for HTTP errors
    with open(f"{title}.png", "wb") as f:
        f.write(response.content)

get_image(folder +"/screenshoot.png", scope_ip )
print(f"png saved")













########################## SIGNAL ###############################################
#from datetime import datetime



# Make timestamp
#now = datetime.now()
#timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
#print(timestamp)


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
    time .sleep(0.5)
except Exception as e:  
    print("Connection failed:", e)
    _=input("Press any key to exit")
    exit()
chanels = CHNS = ["CH2", "CH3", "CH4"]
print("Available chanels ", chanels)



def get_general_config(): 

    DIC = {}

    params = ["HORIZONTAL:MAIN:SCALE?", "WFMPRE:NR_Pt?"]
    dic={}
    for p in params: 
        try: 
            dic[p]= scope.query(p)#.replace("\n", "")
        except: pass
    DIC["general"]= dic

    _= """
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
    """
    
    return DIC
d = get_general_config()
print("get_general_config", d)

def get_seeting(channel = "CH1"): 
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


    _="""
    raw = scope.query_binary_values(  
    'CURVE?',  
    datatype='h', # 'h' = signed 16‑bit  
    is_big_endian=True, # or False depending on your scope  
    container=np.array  
    )  
    wave = np.array( raw)  
    dic["raw_data"]= raw
    """
    
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
import pprint
# chanels = ["CH4"]

for c in chanels:
    dic = get_seeting(channel = c)
    with open(f"{folder}/{c}_seeting.txt", "w") as f:
        #f.write(str(res))
        f.write(pprint.pformat(dic))



def get_raw_data( n=10, delay_ms=10):
    arr1=[]
    arr2=[]
    arr3=[]
    print("|"*n)
    for i in range(n):
        scope.write("ACQUIRE:STATE OFF")  # Or scope.write("ACQUIRE:STATE STOP")
        # 1. Select the channel
        scope.write("DATA:SOURCE "+CHNS [0])
        raw1 = scope.query_binary_values(  
        'CURVE?',  
        datatype='h', # 'h' = signed 16‑bit  
        is_big_endian=True, # or False depending on your scope  
        container=np.array  
        )
        arr1.append(raw1)
        
        # 1. Select the channel
        scope.write("DATA:SOURCE "+CHNS [1])
        raw2 = scope.query_binary_values(  
        'CURVE?',  
        datatype='h', # 'h' = signed 16‑bit  
        is_big_endian=True, # or False depending on your scope  
        container=np.array  
        )
        arr2.append(raw2)

        # 1. Select the channel
        scope.write("DATA:SOURCE "+CHNS [2])
        raw3 = scope.query_binary_values(  
        'CURVE?',  
        datatype='h', # 'h' = signed 16‑bit  
        is_big_endian=True, # or False depending on your scope  
        container=np.array  
        )
        arr3.append(raw3)

        
        scope.write("ACQUIRE:STATE ON")   # Or scope.write("ACQUIRE:STATE RUN")
        #print(f"\rProcessing {100*i/n:.2f} % ...", end="", flush = True)
        print("|", end="")
        time.sleep(0.1)
        
    print("")
    arr1, arr2, arr3 = np.array(arr1), np.array(arr2), np.array(arr3)



    return  arr1, arr2, arr3
# from datetime import datetime

print(datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"))
N=50


arr1, arr2, arr3=get_raw_data( n=N, delay_ms=10)

np.save(f"{folder}/{CHNS [0]}_raw.npy", arr1)
np.save(f"{folder}/{CHNS [1]}_raw.npy", arr2)
np.save(f"{folder}/{CHNS [2]}_raw.npy", arr3)
print(datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f"))

