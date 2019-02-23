#include "sleepy_discord/websocketpp_websocket.h"
#include <forward_list>
#include <fstream>
#include <iostream>
#include <locale>
#include <mutex>
#include <random>
#include <sstream>
#include <thread>
#include <unordered_map>
#include <vector>

inline std::string& clean(std::string& msg);
inline int get_random(int max);
std::string filter(std::string phrase, const std::string& errortype, const std::forward_list<std::string> stack);

class Markov {
  using Key = std::string;
  using Chain = std::vector<std::string>;
  using Dictionary = std::unordered_map<std::string, Chain>;

private:
  mutable std::mutex dicc_lck;
  Dictionary dict;

public:
  void log(std::string msg);
  inline void add_assoc(std::string word, std::string assoc);
  std::string generate();
  Markov& clear();
  Markov& recover();
  void save() const;

  // should probably prompt for this but cbf
  std::string filename = "dict";
};

struct Client : public SleepyDiscord::DiscordClient {
  Markov dictionary;
  void repl();
  // inherit constructor
  using SleepyDiscord::DiscordClient::DiscordClient;
  void onMessage(SleepyDiscord::Message message) override;
  inline std::string handle_cmd(std::string msg);
};
