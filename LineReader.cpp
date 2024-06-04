#include <iostream>
#include <stack>
#include <stdio.h>      // perror(), stderr, stdin, fileno()
#include "LineReader.h"

LineReader::TcAttr::TcAttr() : _status(-1) {
  _status = setAttr();
}

int LineReader::TcAttr::getStatus() const {
  return _status;
} 

LineReader::TcAttr::~TcAttr() {
  // terminal attributes need be restored before normal process exit
  // as these affect the calling shell
  if (_status == 0 && tcsetattr(fileno(stdin), TCSANOW, &_t) < 0) {
    perror("Unable to reset terminal to regular character mode");
  }
}

int LineReader::TcAttr::setAttr() {
  struct termios t;

  // Set terminal to single character mode.
  tcgetattr(fileno(stdin), &t);
  _t = t;
  t.c_lflag &= (~ICANON & ~ECHO);
  t.c_cc[VTIME] = 0;
  t.c_cc[VMIN] = 1;
  if (tcsetattr(fileno(stdin), TCSANOW, &t) < 0) {
    perror("Unable to set terminal to single character mode");
    return -1;
  }
  return 0;
}

LineReader::LineReader() : _popped_back(0), _pbuf(std::cin.rdbuf()) {}

void LineReader::push_back(const std::string& message) {
  for (auto riter = message.rbegin(); riter != message.rend(); ++riter) {
    _pbuf->sputbackc(*riter);
    ++_popped_back;
  }
}

void LineReader::push_back_umlaut(const std::string& umlautMultiChar, char umlautCode) {
  _line = _line + umlautCode;
  std::cout << umlautMultiChar;
}

void LineReader::addCallback(const std::string& sequence, const std::shared_ptr<EventReceiver>& receiver) {
  _callbacks[sequence] = receiver;
}

std::string LineReader::getLine() {
  EventMap callbackCopy(_callbacks);
  // all keys that initiate an escape sequence check
  std::string escape_keys = getEscapeKeys();
  _line.clear();
  int escapeMatched = 0;
  do {
    char c = _pbuf->sbumpc();
    // popped back inidcator to avoid infinite loops from pushed back escape sequences
    int popped_back = _popped_back;
    if (popped_back > 0) --_popped_back;
#define ECHO_CHAR 0
#if ECHO_CHAR
    printf("character: %x %c\n", c, c);
#endif
    if (popped_back == 0 && (escapeMatched > 0 || escape_keys.find(c) != std::string::npos)) {
      _pushback.push(c);
      handleEscape(callbackCopy, escapeMatched, c);
    } else if (c == '\n') {
      std::cout << std::endl;
      break;
    } else if (c == '\x7f') {
      if (_line.size() > 0) {
        _line = _line.substr(0, _line.length() - 1);
        std::cout << "\b \b";
      }
    } else if (c == '\t') {
      c = ' ';
      std::cout << c;
      _line = _line + c;
    } else if (c == '\x1b') {
      const char* seq = "^[";
      std::cout << seq;
      _line = _line + seq;
    } else {
      std::cout << c;
#define ECHO_HEX 0
#if ECHO_HEX
      char szTmp[80];
      sprintf(szTmp, "0x%08x-", c);
      std::cout << szTmp;
#endif
      _line = _line + c;
    }
  } while (true);
  return _line;
}

void LineReader::popback() {
  while (!_pushback.empty()) {
    _pbuf->sputbackc(_pushback.top());
    _pushback.pop();
    ++_popped_back;
  } 
}

void LineReader::clear_pushback() {
  std::stack<char> empty;
  _pushback.swap(empty);
}

// the first key of all escape sequences
std::string LineReader::getEscapeKeys() const {
  std::set<char> ks;
  for (const auto& ev : _callbacks) {
    ks.insert(ev.first[0]);
  }
  ks.erase('\0');
  std::string keys(ks.size(),'\x1b');
  std::string::iterator ki = keys.begin();
  for (char ch : ks) {
    *ki++ =  ch;
  }
  return keys;
}

void LineReader::handleEscape(EventMap& callbacks, int& escapeMatched, char c) {
  bool notified = false;
  // initiate escape sequence check
  for (auto iter = callbacks.begin(); iter != callbacks.end(); ) {
    if (iter->first[escapeMatched] == c) {
      if (iter->first[escapeMatched+1] == '\0') {
        Event e(Event::Escape, iter->first);
        iter->second->notify(e);
        escapeMatched = 0;
        callbacks = _callbacks;
        clear_pushback();
        notified = true;
        break;
      } else {
        ++iter;
      }
    } else {
      callbacks.erase(iter++);
    }
  }
  if (callbacks.empty()) {
    // Not an escape sequence
    escapeMatched = 0;
    callbacks = _callbacks;
    popback();
  } else if (!notified) {
    ++escapeMatched;
  }
}

