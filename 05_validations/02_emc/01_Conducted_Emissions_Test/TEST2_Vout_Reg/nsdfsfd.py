Python 3.13.5 (tags/v3.13.5:6cb20a2, Jun 11 2025, 16:15:46) [MSC v.1943 64 bit (AMD64)] on win32
Enter "help" below or click "Help" above for more information.

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-16-10-158
Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 174, in _new_conn
    conn = connection.create_connection(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\connection.py", line 95, in create_connection
    raise err
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\connection.py", line 85, in create_connection
    sock.connect(sa)
OSError: [WinError 10065] A socket operation was attempted to an unreachable host

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 716, in urlopen
    httplib_response = self._make_request(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 416, in _make_request
    conn.request(method, url, **httplib_request_kw)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 244, in request
    super(HTTPConnection, self).request(method, url, body=body, headers=headers)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1338, in request
    self._send_request(method, url, body, headers, encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1384, in _send_request
    self.endheaders(body, encode_chunked=encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1333, in endheaders
    self._send_output(message_body, encode_chunked=encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1093, in _send_output
    self.send(msg)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1037, in send
    self.connect()
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 205, in connect
    conn = self._new_conn()
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 186, in _new_conn
    raise NewConnectionError(
urllib3.exceptions.NewConnectionError: <urllib3.connection.HTTPConnection object at 0x000001D96C517A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\adapters.py", line 644, in send
    resp = conn.urlopen(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 802, in urlopen
    retries = retries.increment(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\retry.py", line 594, in increment
    raise MaxRetryError(_pool, url, error or ResponseError(cause))
urllib3.exceptions.MaxRetryError: HTTPConnectionPool(host='169.254.104.98', port=80): Max retries exceeded with url: /Image.png (Caused by NewConnectionError('<urllib3.connection.HTTPConnection object at 0x000001D96C517A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host'))

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 52, in <module>
    get_image(folder +"/screenshoot.png", scope_ip )
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 47, in get_image
    response = requests.get(url, stream=True)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\api.py", line 73, in get
    return request("get", url, params=params, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\api.py", line 59, in request
    return session.request(method=method, url=url, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\sessions.py", line 589, in request
    resp = self.send(prep, **send_kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\sessions.py", line 703, in send
    r = adapter.send(request, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\adapters.py", line 677, in send
    raise ConnectionError(e, request=request)
requests.exceptions.ConnectionError: HTTPConnectionPool(host='169.254.104.98', port=80): Max retries exceeded with url: /Image.png (Caused by NewConnectionError('<urllib3.connection.HTTPConnection object at 0x000001D96C517A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host'))

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-17-16-349
Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 174, in _new_conn
    conn = connection.create_connection(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\connection.py", line 95, in create_connection
    raise err
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\connection.py", line 85, in create_connection
    sock.connect(sa)
OSError: [WinError 10065] A socket operation was attempted to an unreachable host

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 716, in urlopen
    httplib_response = self._make_request(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 416, in _make_request
    conn.request(method, url, **httplib_request_kw)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 244, in request
    super(HTTPConnection, self).request(method, url, body=body, headers=headers)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1338, in request
    self._send_request(method, url, body, headers, encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1384, in _send_request
    self.endheaders(body, encode_chunked=encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1333, in endheaders
    self._send_output(message_body, encode_chunked=encode_chunked)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1093, in _send_output
    self.send(msg)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\http\client.py", line 1037, in send
    self.connect()
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 205, in connect
    conn = self._new_conn()
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connection.py", line 186, in _new_conn
    raise NewConnectionError(
urllib3.exceptions.NewConnectionError: <urllib3.connection.HTTPConnection object at 0x0000024A9CE97A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\adapters.py", line 644, in send
    resp = conn.urlopen(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\connectionpool.py", line 802, in urlopen
    retries = retries.increment(
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\urllib3\util\retry.py", line 594, in increment
    raise MaxRetryError(_pool, url, error or ResponseError(cause))
urllib3.exceptions.MaxRetryError: HTTPConnectionPool(host='169.254.104.98', port=80): Max retries exceeded with url: /Image.png (Caused by NewConnectionError('<urllib3.connection.HTTPConnection object at 0x0000024A9CE97A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host'))

During handling of the above exception, another exception occurred:

Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 52, in <module>
    get_image(folder +"/screenshoot.png", scope_ip )
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 47, in get_image
    response = requests.get(url, stream=True)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\api.py", line 73, in get
    return request("get", url, params=params, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\api.py", line 59, in request
    return session.request(method=method, url=url, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\sessions.py", line 589, in request
    resp = self.send(prep, **send_kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\sessions.py", line 703, in send
    r = adapter.send(request, **kwargs)
  File "C:\Users\a037702\AppData\Local\Programs\Python\Python313\Lib\site-packages\requests\adapters.py", line 677, in send
    raise ConnectionError(e, request=request)
requests.exceptions.ConnectionError: HTTPConnectionPool(host='169.254.104.98', port=80): Max retries exceeded with url: /Image.png (Caused by NewConnectionError('<urllib3.connection.HTTPConnection object at 0x0000024A9CE97A10>: Failed to establish a new connection: [WinError 10065] A socket operation was attempted to an unreachable host'))

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-17-57-834
png saved
Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 55, in <module>
    sdsbhdijsbdjbsd
NameError: name 'sdsbhdijsbdjbsd' is not defined

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-23-20-157
png saved
Connected to: TEKTRONIX,TDS 3034B,0,CF:91.1CT FV:v3.35 TDS3FFT:v1.00 TDS3TRG:v1.00

CH1 0
CH2 0
CH3 0
CH4 1
Available chanels  ['CH4']
get_general_config {'general': {'HORIZONTAL:MAIN:SCALE?': '4.0E-5\n', 'WFMPRE:NR_Pt?': '10000\n'}}
Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 223, in <module>
    stooooooooooooooooooooop
NameError: name 'stooooooooooooooooooooop' is not defined

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-24-41-935
png saved
Connected to: TEKTRONIX,TDS 3034B,0,CF:91.1CT FV:v3.35 TDS3FFT:v1.00 TDS3TRG:v1.00

CH1 0
CH2 0
CH3 0
CH4 1
Available chanels  ['CH4']
get_general_config {'general': {'HORIZONTAL:MAIN:SCALE?': '4.0E-5\n', 'WFMPRE:NR_Pt?': '10000\n'}}
Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 218, in <module>
    with open(f"folder/{c}_seeting.txt", "w") as f:
FileNotFoundError: [Errno 2] No such file or directory: 'folder/CH4_seeting.txt'
f"{folder}/{c}_seeting.txt"
'EMI_2026-02-03_17-24-41-935/CH4_seeting.txt'

= RESTART: C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py
date time =  2026-02-03_17-25-26-372
png saved
Connected to: TEKTRONIX,TDS 3034B,0,CF:91.1CT FV:v3.35 TDS3FFT:v1.00 TDS3TRG:v1.00

CH1 0
CH2 0
CH3 0
CH4 1
Available chanels  ['CH4']
get_general_config {'general': {'HORIZONTAL:MAIN:SCALE?': '4.0E-5\n', 'WFMPRE:NR_Pt?': '10000\n'}}
Traceback (most recent call last):
  File "C:\Users\a037702\NotSynchronized\git\Design_of_1200W_LLC_DCDC\05_validations\08_EMI\TEST2_Vout_Reg\EMI_saving.py", line 223, in <module>
    stooooooooooooooooooooop
NameError: name 'stooooooooooooooooooooop' is not defined
time
...   
<module 'time' (built-in)>
>>> time.sleep(0.01)
...   
>>> def get_raw_data(channel = "CH1", n=10, delay_ms=10):
...     arr=[]
...     for i in range(n):
...         raw = scope.query_binary_values(  
...         'CURVE?',  
...         datatype='h', # 'h' = signed 16‑bit  
...         is_big_endian=True, # or False depending on your scope  
...         container=np.array  
...         )
...         arr.append(raw)
...         #wave = np.array( raw)
...         time.sleep(0.01)
... 
...         
>>> a=get_raw_data(channel = "CH1", n=10, delay_ms=10)

>>> 
>>> np.array(a)
array(None, dtype=object)
>>> a
>>> def get_raw_data(channel = "CH1", n=10, delay_ms=10):
...     arr=[]
...     for i in range(n):
...         raw = scope.query_binary_values(  
...         'CURVE?',  
...         datatype='h', # 'h' = signed 16‑bit  
...         is_big_endian=True, # or False depending on your scope  
...         container=np.array  
...         )
...         arr.append(raw)
...         #wave = np.array( raw)
...         time.sleep(0.01)
...     return arr
... 
>>> a=get_raw_data(channel = "CH1", n=10, delay_ms=10)
