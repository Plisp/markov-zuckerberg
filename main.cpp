#include "all.hpp"

#define DPRINT(a) std::cout<<"line "<<__LINE__<<": "<<a<<std::endl;

int main() {
  std::ifstream str {"token"};
  std::string tok;
  str >> tok;
  Client client {tok, 2}; // Client(token, num_threads)
  client.dictionary.restore();
  client.restore_channels();
  std::thread repl {&Client::repl, &client};
  client.run();

    return 0;
}

/*** Client members ***/

void Client::repl() {
  std::string command;
  std::cout << "zuccv3> ";
  while (std::cin >> command) {
    if (command == "quit") {
      dictionary.save();
      quit();
      return;
    } else if (command == "restore") {
      // discard modifications since session start
      dictionary.clear().restore();
    } else if (command == "save") {
      dictionary.save();
    } else {
      std::cout << "invalid command" << std::endl;
    }
    std::cout << "zuccv3> ";
  }
  return;
}

// read channels from `chan_file`, called once at startup
void Client::restore_channels() {
  std::ifstream str {this->chan_file};
  std::string name, id;
  while(str >> name >> id)
    channels[name] = std::stoll(id);
}

void Client::onMessage(SleepyDiscord::Message message) {
  if (message.author == getCurrentUser().cast()) {
    return; // ignore itself
  } else if (message.startsWith("$")) {
    Reply reply = dispatch_cmd(message);
    sendMessage(reply.first, reply.second);
    return;
  }
  dictionary.log(clean(message.content));
}

Client::Reply Client::dispatch_cmd(SleepyDiscord::Message msg) {
  std::istringstream sstr {msg.content.substr(1)}; // skip $
  auto channel = msg.channelID;
  std::string cmd, cur, res;
  sstr >> cmd;
  if (cmd == "ask") {
    res = dictionary.generate();
    return Reply(channel, res);
  } else if (cmd == "say") {
    std::string err;
    std::forward_list<std::string> filter_stack;
    while(sstr >> cur) {
      if(cur.front() == '-' && cur.substr(1) == "channel") {
        sstr >> cur;
        if(sstr.fail()) return Reply(channel, "Usage: $say [message [-channel CHANNELNAME]]");
        if(channels.find(cur) != channels.end())
          channel = channels[cur];
        else
          return Reply(channel, "Error: unknown or invalid channel name");
        continue;
      }
      filter_stack.push_front(cur);
      if((err = filter_all(filter_stack)) != "pass")
        return Reply(channel, err);
      res += " " + cur;
    }
    return res != "" ? Reply(channel, res) : Reply(channel, "Usage: $say [message [-channel CHANNELNAME]]");
  } else if (cmd == "fetch!") { // not to be called by normal users
    int repeat {0};
    std::string channelname, msgid;
    sstr >> repeat >> channelname >> msgid;
    for(int n = 0; n < repeat; n++) {
      std::vector<std::string> vec = getMessages(channels[channelname], static_cast<GetMessagesKey>(2), 
                                                 msgid, static_cast<uint8_t>(100));
      auto it = vec.begin();
      while(it != vec.end() - 1) { // last message is recorded in next iteration
        dictionary.log(clean(std::string(SleepyDiscord::json::getValue(it->c_str(), "content"))));
        it++;
      }
      msgid = SleepyDiscord::json::getValue(it->c_str(), "id");
    }
    return Reply(channel, "fetched and logged!");
  } else if (cmd == "yikes") {
    return Reply(channel, "\
                                 ▄▄▄▄██▄▄▄▄███▄▄▄█▄▄███▄██▄▄▄███▄▄▄\
                          ▄    ▄▄██████▄█████▄▄██▄▄██████▄▄▄▄▄██████▄▄▄\
                         ██▄▄▄▄▄████████████████▄██████████████████████▄▄\
                   ▄▄▄▄██▄▄▄▄▄▄█▄▄▄▄▄▄███████████████████████████████████▄▄\
                 ▄▄▄███▄▄▄▄▄▄▄▄▄▄▄▄███▄▄███▄▄▄██████████████████████████████\
                ▄▄█▄▄██▄▄▄▄▄▄▄████▄▄▄███▄█▄▄▄███████████████████████████████▄▄\
               █▄▄█████▄▄█████▄████▄▄█▄▄▄▄▄▄███▄▄██████████████████████████████\
        ▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄▄█████████▄▄█████▄▄███▄▄████████████████████████████████▄▄\
    ▄▄▄▄█▄███▄▄█████▄███▄▄▄▄██▄▄█▄▄█▄▄▄▄▄▄█▄▄▄▄█████████▄█▄▄██████████▄▄▄█████████\
   ███▄██▄▄▄▄██████▄█▄▄▄▄███▄▄▄▄█▄▄▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█████████▄▄█▄▄▄████▄▄█▄▄████████▄▄▄▄▄\
  ██████████████████▄▄███▄█▄▄▄▄██▄▄███████▄▄▄█▄▄██▄▄████████████▄▄▄▄█▄▄▄▄▄█████████████▄▄\
 ██▄▄▄▄▄▄▄█▄███▄▄█▄▄█▄█▄▄▄▄▄█▄▄██▄▄▄▄▄▄▄▄██▄██▄▄▄███▄▄▄▄█▄████████████████████▄▄████████▄▄               ▄\
 ██▄▄▄█▄▄▄▄▄▄▄▄█▄▄▄▄█▄▄▄▄▄▄▄█████▄▄████████████▄▄███▄▄▄▄█████████████████████▄▄███████████             ▄▄██\
 ████▄█▄▄▄▄▄▄██▄▄████████▄▄▄▄▄▄▄▄▄▄▄██████▄▄███████████████████████████████▄▄▄████████████▄▄▄▄▄▄▄▄▄▄▄▄█▄█▄▀\
 █▄▄██▄▄▄███▄████████████████▄▄▄▄▄▄█▄▄▄█▄▄██████████▄▄████████████████████████████████▄▀▀▄██▄█████████▄ ▀\
 ▀▄████▄▄█▄▄▄████▄▄▄▄▄▄▄▄████▄▄████▄███▄▄█████████████████████████████████████████████    ▀█ █▄▀▄██ █ ▀\
   ▀▄▄▄▄▄▄▄▄▄▄▄▄███▄▄███▄▄█████▄▄███▄███▄▄███▄▄▄██▄▄▄██████████████████████████████▄▀         ▀  ▀\
     █████▄▄███▄▄▄▄███▄▄▄█▄▄█▄▄████▄████▄▄█▄▄▄████▄▄▄█▄█████████████████████████████\
     ██▄████████▄▄▄▄▄▄██▄▄█▄▄▄█▄▄▄█▄█▄▄██▄▄▄▄▄▄▄▄█████▄▄▄▄▄▄▄▄▄▄███████████████████\
     ▀▄████▄▄▄▄▀▀▀▀▄▄▄██▄▄█████████████▄▀████████████████████▄▄▄▀▀▀▀▄█▄▄▄▄▄▄▄▄▄████▄▄▄\
       ▀▀▀ ▀▄▄▀     ▀▀▄▄▄███████████▄▀▀    ▀▄█▄▄██████▄▄▄▄▄▀▀       ▄▄█████████▄█▄▄▄▀▀\
");
  } else {
    return Reply(channel, "invalid command"); // help command - keep a table of strings and func ptrs?
  }
}

/*** Markov members ***/

void Markov::save() const {
  std::ofstream fstr {this->dict_file};

  std::lock_guard<std::mutex> lock {dicc_lck};
  for (const auto& word : dict) {
    // key
    fstr << word.first;
    for (const auto& assoc : word.second)
      fstr << " " << assoc;
    fstr << std::endl;
  }
}

Markov& Markov::restore() {
  std::ifstream fstr {this->dict_file};
  std::string raw_chain;
  std::lock_guard<std::mutex> lock {dicc_lck};

  while (getline(fstr, raw_chain)) {
    std::istringstream sstr {raw_chain};
    Key key;
    sstr >> key;
    // remaining strings in raw_chain are part of Chain itself
    Chain chain;
    std::string cur_word;
    while (sstr >> cur_word)
      chain.push_back(cur_word);
    dict[key] = chain;
  }
  return *this;
}

Markov& Markov::clear() {
  std::lock_guard<std::mutex> lock {dicc_lck};
  this->dict = {};
  return *this;
}

void Markov::log(std::string msg) {
  std::string word, next;
  std::istringstream sstr {msg};
  sstr >> word;
  std::lock_guard<std::mutex> lock {this->dicc_lck};
  add_assoc("FIRST", word);
  while (sstr >> next) {
    // if word can be used as the start of a clause
    if (next.back() == '.' || next.back() == ',' ||
        next.back() == '?' || next.back() == '!') {
      // remove last character
      next = next.substr(0, next.size()-1);
      add_assoc(next, "FINAL");
      // first word of next clause
      std::string tmp;
      if (sstr >> tmp) {
        std::toupper(tmp.front());
        add_assoc("FIRST", tmp);
        word = tmp;
        continue;
      } else {
        break;
      }
    }
    add_assoc(word, next);
    word = next;
  }
  std::cout << "size: " << dict.size() << std::endl;
}

// should be a utility function but cbf
void Markov::add_assoc(std::string word, std::string assoc) {
  if (dict.find(word) != dict.end())
    dict[word].push_back(assoc);
  else
    dict[word] = Chain {assoc};
}

/** message generation **/

#define NFILTERS 4

std::array<std::string, NFILTERS> filters {
  "salad a is cereal", "misleading consumers",
  "socialism to superior", "illegal phrase"
};

// TODO: put in class and make it into a 'state' (haha) machine
std::string filter(std::string phrase, const std::string& errortype, const std::forward_list<std::string> stack) {
  auto it {stack.cbegin()};
  std::stringstream str {phrase};
  std::string cur;
  while(str >> cur) {
    if(cur != *it++)
      return "pass";
  }
  // TODO: reverse using constexpr
  std::reverse(phrase.begin(), phrase.end());
  return "Right wing propaganda error (" + errortype + "): '" + phrase + "'";
}

std::string filter_all(const std::forward_list<std::string> stack) {
  std::string ret;
  for(auto it = filters.begin(); it != filters.end(); it += 2) {
    if((ret = filter(*it, *(it + 1), stack)) != "pass")
      return ret;
  }
  return "pass";
}

std::string Markov::generate() {
  std::string cur_word {dict["FIRST"][get_random(dict["FIRST"].size())]};
  std::string sentence {cur_word};
  std::forward_list<std::string> filter_stack {cur_word}; // stack of words used by filters
  int counter {1}; //length in words of message TODO: use this

  while (dict.find(cur_word) != dict.end()) {
    cur_word = dict[cur_word][get_random(dict[cur_word].size())];
    if (cur_word == "FINAL")
      break;

    sentence += " " + cur_word;
    counter++;

    // filter system for suppressing right wing propaganda
    std::string res;
    filter_stack.push_front(cur_word);
    if((res = filter_all(filter_stack)) != "pass")
      return res;
  }
  return sentence;
}

/*** nonmember functions ***/

// remember to update count
std::array<char, 12> clean_chars {'`', '*', '\'', '"', '|', '_', '\\', '\n', '\t', ':'};

std::string clean(std::string msg) {
  for(const char& c : clean_chars)
    msg.erase(std::remove(msg.begin(), msg.end(), c), msg.end());
  return msg;
};

// returns random *index*
int get_random(int max) {
  static std::mt19937 rng;
  static bool seed_done;
  if (!seed_done) {
    rng.seed(std::random_device()());
    seed_done = true;
  }
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, max-1);
  return static_cast<int>(dist(rng));
}
