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

inline std::string clean(std::string msg);
inline int get_random(int max);
std::string filter(std::string phrase, const std::string& errortype, const std::forward_list<std::string> stack);
std::string filter_all(const std::forward_list<std::string> stack);

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
  Markov& restore();
  void save() const;

  // should probably prompt for this but cbf
  std::string dict_file = "dict";
};

struct Client : public SleepyDiscord::DiscordClient {
  void repl();
  Markov dictionary;
  // channels
  using ID = long long;
  std::unordered_map<std::string, ID> channels;
  std::string chan_file = "channels";
  void restore_channels();
  // sleepydiscord stuff
  using SleepyDiscord::DiscordClient::DiscordClient; // inherit constructor
  using Reply = std::pair<std::string, std::string>; // channel, message
  void onMessage(SleepyDiscord::Message message) override;
  inline Reply dispatch_cmd(SleepyDiscord::Message msg);
};
