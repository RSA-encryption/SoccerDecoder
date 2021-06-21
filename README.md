# Soccer Decoder C++

Implementation of Alessandro Ferrarini soccer decoder

### How to compile

Debug
``` g++ -g -o out.exe *.cpp ./headers/*.hpp ./classes/*.cpp```

Release
``` g++ -o out.exe *.cpp ./headers/*.hpp ./classes/*.cpp -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic```

![output.png](https://github.com/RSA-encryption/SoccerDecoder/blob/main/output.png)
