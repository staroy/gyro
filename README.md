# Gyro blockchain
Based on the consensus algorithm "Proof of Gyro". Instead of spending energy to achieve consensus is used frozen forever coins, which in the form of newly generated, return and pay network support. The blockchain is initialized based on the file configurations, which allows to implement different strategies of token or coins. Primary initialization with the lack of further emissions, or on the contrary, smooth emission. Freeze large paws for sustainability and mutual confidence of partners. Based on the transaction pool implemented SMS subsystem - short text messages, they are used to multisign addresses. All these operations occur based on the Cryptonote protocol, that is opaque for a third-party observer. But available to observe the "view" owner of the keys. Added subsystem of P2P interaction at the local network level, based on framework zyre. On an orchestra can be created, with encryption of services groups. The interface is implemented by wallet, and in the zyre-lua control utility, allowing to run management scenarios, analysis and interaction. The zyre-lua also added the implementation of the HTTP server and websocket.

# Proof of Gyro
EN : [gyro-PoG-en.pdf](https://github.com/staroy/gyro/blob/main/gyro-PoG-en.pdf)<br/>
RU : [gyro-PoG-ru.pdf](https://github.com/staroy/gyro/blob/main/gyro-PoG-ru.pdf)

Build :

```
apt install build-essential cmake git pkg-config libssl-dev libzmq3-dev libczmq-dev \
    libunbound-dev libsodium-dev libunwind-dev liblzma-dev libreadline-dev libldns-dev \
    libexpat1-dev doxygen graphviz libpgm-dev libhidapi-dev \
    libusb-dev libprotobuf-dev protobuf-compiler libx11-xcb-dev \
    libnorm-dev libgcrypt20-dev
```

in depends directory start
```
./depends-release.sh
```
and in gyro directory start
```
./gyro-release.sh
```

Bitcoin: **bc1q3nqrakqnsf967qu7rpydwf32fgyypms44pgm27**

![BitCoin](https://staroy.github.io/bc1q3nqrakqnsf967qu7rpydwf32fgyypms44pgm27.png)
