# markov-zuckerberg
A intellectual discord bot that mimicks humans

Build [sleepy-discord](https://github.com/yourWaifu/sleepy-discord.git) and move this repo into the `examples` directory.

Replace "TOKEN" in `main.cpp` with your token and run: 
```sh
c++ -std=c++11 -I . -I ../../include -I ../../deps -I ../../deps/include -I ../../include/sleepy_discord/IncludeNonexistent main.cpp -L../../buildtools -L/usr/lib -lsleepy_discord -lcpr -lcurl
-lssl -lcrypto -lpthread
```

