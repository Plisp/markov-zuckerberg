# markov-zuckerberg
A intellectual discord bot that mimicks humans

## Building
Build [sleepy-discord](https://github.com/yourWaifu/sleepy-discord.git) and move this repo into the `examples` directory and run:

```sh
c++ -std=c++11 -I . -I ../../include -I ../../deps -I ../../deps/include -I ../../include/sleepy_discord/IncludeNonexistent main.cpp -L../../buildtools -L/usr/lib -lsleepy_discord -lcpr -lcurl
-lssl -lcrypto -lpthread -o zucc
```

## Usage
Create a file named `token` containing your bot token and run `./zucc`. Boom.
