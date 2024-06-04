#pragma once

#include <string>
#include <map>
#include <memory>
#include <set>
#include <fstream>
#include <termios.h>    // struct termios, tcgetattr(), tcsetattr()

#include "Event.h"


// line reader with escape handling
class LineReader {
public:
  typedef std::map<std::string, std::shared_ptr<EventReceiver>> EventMap;
  class TcAttr {
  public:
    TcAttr();
    int getStatus() const;
    ~TcAttr();
  private:
    int setAttr();
    struct termios _t;
    int _status;
  };

public:
  LineReader();
  
  void push_back(const std::string& message);
  void push_back_umlaut(const std::string& umlautMultiChar, char umlautCode);
  void addCallback(const std::string& sequence, const std::shared_ptr<EventReceiver>& receiver);

  /** Line reader method with special key (F1 to F12) handling
  */
  std::string getLine();
private:
  void popback();
  void clear_pushback();

  // the first key of all escape sequences
  std::string getEscapeKeys() const;

  void handleEscape(EventMap& callbacks, int& escapeMatched, char c);

private: 
  int _popped_back;
  EventMap _callbacks;
  std::streambuf* _pbuf;
  std::stack<char> _pushback;
  std::string _line;
};

