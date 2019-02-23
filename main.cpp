#include "all.hpp"

#define DPRINT(a) std::cout<<"line "<<__LINE__<<": "<<a<<std::endl;

int main() {
  // Client(token, num_threads)
  Client client {"TOKEN", 2};
  client.dictionary.recover();
  std::thread repl {&Client::repl, &client};
  client.run();

  return 0;
}

/*** Client members ***/
void Client::repl() {
  std::string command;
  std::cout << "> ";
  // handle repl command TODO: abstract
  while (std::cin >> command) {
    if (command == "quit") {
      this->dictionary.save();
      this->quit();
      return;
    } else if (command == "restore") {
      this->dictionary.clear().recover();
    } else if (command == "save") {
      this->dictionary.save();
    } else {
      std::cout << "invalid command" << std::endl;
    }
    std::cout << "> ";
  }
  return;
}

void Client::onMessage(SleepyDiscord::Message message) {
  if (message.author == getCurrentUser().cast()) {
    return; // ignore itself
  } else if (message.startsWith("$")) {
    sendMessage(message.channelID, handle_cmd(message.content.substr(1)));
    return;
  }
  dictionary.log(clean(message.content));
}

std::string Client::handle_cmd(std::string msg) {
  std::istringstream sstr {msg};
  std::string cmd, cur, res;
  sstr >> cmd;
  // handle user command TODO: abstract
  if (cmd == "say") {
    res = dictionary.generate();
    return res;
  } else if (cmd == "echo") {
    while (sstr >> cur)
      res += " " + cur;
    return res;
  } else if (cmd == "yikes") {
    return "                     ▄▄▄▄██▄▄▄▄███▄▄▄█▄▄███▄██▄▄▄███▄▄▄\
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
";
  } else {
    return "invalid command"; // help command - keep a table of strings and func ptrs?
  }
}

/*** Markov members ***/

void Markov::save() const {
  std::ofstream fstr {this->filename};

  std::lock_guard<std::mutex> lock {dicc_lck};
  for (const auto& word : dict) {
    // key
    fstr << word.first;
    for (const auto& assoc : word.second)
      fstr << " " << assoc;
    fstr << std::endl;
  }
}

Markov& Markov::recover() {
  std::ifstream fstr {this->filename};
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

// TODO: put in class and make it into a 'state' (haha) machine
std::string filter(std::string phrase, const std::string& errortype, const std::forward_list<std::string> stack)
{
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

	if((res = filter("salad a is cereal", "misleading consumers", filter_stack)) != "pass")
      return res;

    if((res = filter("socialism to superior", "illegal phrase", filter_stack)) != "pass")
      return res;
  }
  return sentence;
}

/*** nonmember functions ***/

// remember to update count
std::array<char, 12> clean_chars {'`', '*', '\'', '"', '|', '_', '\\', '\n', '\t', ':'};

inline std::string& clean(std::string& msg) {
  for(const char& c : clean_chars)
    msg.erase(std::remove(msg.begin(), msg.end(), c), msg.end());
  return msg;
};

// returns random *index*
inline int get_random(int max) {
  static std::mt19937 rng;
  static bool seed_done;
  if (!seed_done) {
    rng.seed(std::random_device()());
    seed_done = true;
  }
  std::uniform_int_distribution<std::mt19937::result_type> dist(0, max-1);
  return static_cast<int>(dist(rng));
}
