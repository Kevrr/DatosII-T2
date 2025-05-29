### Needed libraries 

```
sudo apt install libssl-dev -libcurl4-openssl-dev
```

### Compile
```
g++ https_server.cpp -o server -lssl -lcrypto
```
```
g++ https_client.cpp -o client -lcurl
```