from datetime import datetime
import requests


# Make timestamp
now = datetime.now()
timestamp = now.strftime("%Y-%m-%d_%H-%M-%S") + f"-{int(now.microsecond/1000):03d}"
print(timestamp)


import urllib3, certifi, sys
print("urllib3:", urllib3.__version__)
print("certifi:", certifi.__version__)
# print("Python:", sys.version)
"""
Expectation 
urllib3: 1.26.20
certifi: 2025.07.14

USE this
pip install requests==2.32.5
pip install "urllib3<2"
"""



filename = f"{timestamp}"
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
