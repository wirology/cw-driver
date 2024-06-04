#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <thread>
#include <mutex>
#include <memory>
#include <queue>
#include <stack>
#include <map>
#include <set>
#include <future>
#include <cstdlib>
#include <cstring>
#include <signal.h> // sigint handler
#include <unistd.h> // getpid

#include "DualPipe.h"
#include "Event.h"
#include "LineReader.h"
#include "StringUtils.h"

// exit program if single input line matches quit
static const std::string quit(":q");
// name of directory containing driver binary plus trailing '/'
static std::string binary_dirname;

// Loads function texts from files
class FunctionTexts {
public:
  FunctionTexts() : _pattern(defaultKeyPattern()) {
  }
  const std::string& getText(int function) {
    auto iter = _functionTexts.find(function);
    if (iter == _functionTexts.end()) {
      std::ifstream f(getFileName(function)); //taking file as inputstream
      std::string str;
      if(f) {
        std::ostringstream ss;
        ss << f.rdbuf(); // reading data
        str = ss.str();
        // vim appends a final line feed to files
        // removing the last line feed will result
        // in texts with a final empty line (two line feeds)
        // be sent instantly and ones with just one line feed
        // be appended for editing without instant sending
        size_t last_0a = str.find_last_of('\n');
        if (last_0a != std::string::npos) {
          str.erase(last_0a);
        }
      }
      iter = _functionTexts.insert(std::make_pair(function, str)).first;
    } 
    return iter->second;
  }
  void putText(int key, const  std::string& text) {
    std::string messageVal(text);
    StringUtils::replace_all(messageVal, "\n", "\\x0a");
    std::cout << "*** F" << key << " set to '" << messageVal << "' ***" << std::endl;
    _functionTexts[key] = text;
    std::ofstream f;
    f.open(getFileName(key), std::ios::out | std::ios::trunc);
    f << text;
    f.close();
  }
private:
  static void replaceKey(std::string& str, int key) {
    size_t pos = str.find(_keyPattern);
    if (pos != std::string::npos) {
      std::string keyText = std::to_string(key);
      str.replace(pos, _keyPattern.length(), keyText);
    }
  }
  static std::string defaultKeyPattern() {
    return "./function_texts/f" + _keyPattern + ".txt";
  }
  std::string getFileName(int function) const {
    std::string localname(_pattern);
    replaceKey(localname, function);
    if (localname[0] == '/') {
      return localname;
    } else {
      std::ostringstream fn;
      fn << binary_dirname << localname;
      return fn.str();
    }
  }
  std::string _pattern;
  std::map<int, std::string> _functionTexts;
public:
  static const std::string _keyPattern;
};
const std::string FunctionTexts::_keyPattern("{key}");

// Defines escape sequences for function keys
class FunctionKeys {
public:
  static const std::list<std::string>& getKeyList() {
    static std::list<std::string> keyList(_getKeyList());
    return keyList;
  }
  static const std::map<std::string, int>& getFunctionMap() {
    static std::map<std::string, int> functionMap(_getFunctionMap());
    return functionMap;
  }
private:
  /**
   * F1 ^[OP
   * F2 ^[OQ
   * F3 ^[OR
   * F4 ^[OS
   * F5 ^[[15~
   * F6 ^[[17~
   * F7 ^[[18~
   * F8 ^[[19~
   * F9 ^[[20~
   * F10 ^[[21~
   * F11 ^[[23~
   * F12 ^[[24~
  */
  static std::list<std::string> _getKeyList() {
    std::list<std::string> keyList = {
      _f1,
      _f2,
      _f3,
      _f4,
      _f5,
      _f6,
      _f7,
      _f8,
      _f9,
      _f10,
      _f11,
      _f12
    }; 
    return keyList;
  }

  static std::map<std::string, int> _getFunctionMap() {
    int functionNo = 0;
    std::map<std::string, int> functionMap;
    for (const auto& key : getKeyList()) {
      functionMap[key] = ++functionNo;
    }
    return functionMap;
  }
public:
  static const std::string _f1;
  static const std::string _f2;
  static const std::string _f3;
  static const std::string _f4;
  static const std::string _f5;
  static const std::string _f6;
  static const std::string _f7;
  static const std::string _f8;
  static const std::string _f9;
  static const std::string _f10;
  static const std::string _f11;
  static const std::string _f12;
};
const std::string FunctionKeys::_f1("\x1b\x4fP");
const std::string FunctionKeys::_f2("\x1b\x4fQ");
const std::string FunctionKeys::_f3("\x1b\x4fR");
const std::string FunctionKeys::_f4("\x1b\x4fS");
const std::string FunctionKeys::_f5("\x1b\x5b\x31\x35~");
const std::string FunctionKeys::_f6("\x1b\x5b\x31\x37~");
const std::string FunctionKeys::_f7("\x1b\x5b\x31\x38~");
const std::string FunctionKeys::_f8("\x1b\x5b\x31\x39~");
const std::string FunctionKeys::_f9("\x1b\x5b\x32\x30~");
const std::string FunctionKeys::_f10("\x1b\x5b\x32\x31~");
const std::string FunctionKeys::_f11("\x1b\x5b\x32\x33~");
const std::string FunctionKeys::_f12("\x1b\x5b\x32\x34~");

class CursorKeys {
  public:
  static const std::list<std::string>& getCursorKeys() {
    static std::list<std::string> keys(_cursorKeys());
    return keys;
  }
private:
  static std::list<std::string> _cursorKeys() {
    std::list<std::string> ret = {
      _up,
      _down,
      _right,
      _left
    };
    return ret;
  }
public:
  static const std::string _up;
  static const std::string _down;
  static const std::string _right;
  static const std::string _left;
};
const std::string CursorKeys::_up("\x1b\x5b\x41");
const std::string CursorKeys::_down("\x1b\x5b\x42");
const std::string CursorKeys::_right("\x1b\x5b\x43");
const std::string CursorKeys::_left("\x1b\x5b\x44");

/**
 * Smart function key programming options
 * #<sometext>
 * will put <sometext> into f5
 * otherwise
 * #nn-<sometext> will put sometest into f{nn} where nn is a 
 * number between 1 and 12. 
*/
class LineParser {
public:
  LineParser() {
  }
  std::string parseLine(const std::string& line) {
    std::string ret(line);
    if (line[0] == '#') {
      size_t pos = line.find_first_of("-");
      if (pos != std::string::npos) {
        std::string fNo = line.substr(1, pos - 1);
        if (fNo.find_first_not_of(_digits) == std::string::npos) {
          int key = std::stoi(fNo);
          if (key > 0 && key < 13) {
            ret = line.substr(pos + 1);
	    putFunctionText(key, ret);
          } 
        }
      } else {
        ret = line.substr(1);
	putFunctionText(5, ret);
      }
    }
    return ret;
  }
private:
  void putFunctionText(int key, std::string& val) {
    FunctionTexts ft;
    size_t length = val.length();
    if (val.find_last_of(_linefeed_keys) == length - 1) {
      val = val.substr(0, length - 1); 
      std::string keyval = val + "\n\n";
      ft.putText(key, keyval);
    } else {
      ft.putText(key, val);
    }
  }
public:
  static const std::string _digits;
  static const std::string _linefeed_keys;
};
const std::string LineParser::_digits("0123456789");
const std::string LineParser::_linefeed_keys("!.");

class Reader {

  class FunctionExecutor : public EventReceiver {
  public:
    FunctionExecutor(LineReader& lineReader) : _lineReader(lineReader) {}
    void notify(const Event& event) {
      int key = 0;
      auto iter = FunctionKeys::getFunctionMap().find(event.getVal());
      if (iter != FunctionKeys::getFunctionMap().end()) {
        key = iter->second;
      }
      _lineReader.push_back(FunctionTexts().getText(key)); 
    }
  private:
    LineReader& _lineReader;
  };

  class EscapeExecutor : public EventReceiver {
  public:
    EscapeExecutor(LineReader& lineReader) : _lineReader(lineReader), _triggered(false) {}
    void notify(const Event& event) {
      _lineReader.push_back("\n");
      _triggered = true;
      // Notify globally
      EventSender::getInstance().notify(event);
    }
    bool isTriggered() const { 
      return _triggered;
    }
  private:
    LineReader& _lineReader;
    bool _triggered;
  };

  class CursorExecutor : public EventReceiver {
  public:
    void notify(const Event& event) {
      Event cursorEvent(Event::Cursor, event.getVal());
      EventSender::getInstance().notify(cursorEvent);  
    }
  };

  class EofExecutor : public EventReceiver {
  public:
    EofExecutor(LineReader& lineReader) : _lineReader(lineReader) {}
    void notify(const Event& event) {
      _lineReader.push_back(::quit + "\n");
    }
  private:
    LineReader _lineReader;
  };

  // Multi char handler. Will detect multi char special characters,
  // echo them to stdout as multichar, but replace them with an 
  // ascii code that the ascii2pcm generator understands 
  class UmlautExecutor : public EventReceiver {
  public:
    UmlautExecutor(LineReader& lineReader, char umlautCode) 
    : _lineReader(lineReader), _umlautCode(umlautCode)
    {}
    void notify(const Event& event) {
      _lineReader.push_back_umlaut(event.getVal(), _umlautCode);
    }
  private:
    LineReader& _lineReader;
    char _umlautCode;
  };

public:
  Reader() {
  }

  void read() {
    bool quit = false;
    do {
      LineReader lineReader;
      LineParser lineParser;
      for (const std::string& func : FunctionKeys::getKeyList()) {
        lineReader.addCallback(func, std::make_shared<FunctionExecutor>(lineReader));
      }
      for (const std::string& key : CursorKeys::getCursorKeys()) {
        lineReader.addCallback(key, std::make_shared<CursorExecutor>());
      }
      
      // Kill code generator and reset text buffer on these keys:
      // double escape
      std::shared_ptr<EscapeExecutor> escapeExecutor = std::make_shared<EscapeExecutor>(lineReader);
      lineReader.addCallback("\x1b\x1b", escapeExecutor);
      // delete key
      lineReader.addCallback("\x1b\x5b\x33~", escapeExecutor);
      addUmlautHandler(lineReader, "\xc3\xbc", '\x81'); // ue 
      addUmlautHandler(lineReader, "\xc3\x9c", '\x81'); // ue 
      addUmlautHandler(lineReader, "\xc3\xa4", '\x84'); // ae 
      addUmlautHandler(lineReader, "\xc3\x84", '\x84'); // Ae 
      addUmlautHandler(lineReader, "\xc3\xb6", '\x92'); // oe 
      addUmlautHandler(lineReader, "\xc3\x96", '\x92'); // Oe 
      addUmlautHandler(lineReader, "\xc3\xa0", '\x85'); // a` 
      addUmlautHandler(lineReader, "\xc3\x80", '\x85'); // A` 
      addUmlautHandler(lineReader, "\xc3\xa9", '\x82'); // e' 
      addUmlautHandler(lineReader, "\xc3\x89", '\x82'); // E' 
      addUmlautHandler(lineReader, "\xc3\xa8", '\x8a'); // e` 
      addUmlautHandler(lineReader, "\xc3\x88", '\x8a'); // E` 
      addUmlautHandler(lineReader, "\xc3\xa7", '\x87'); // cc
      addUmlautHandler(lineReader, "\xc3\x87", '\x87'); // Cc 
      addUmlautHandler(lineReader, "\xc4\x88", '\x87'); // c^ 
      addUmlautHandler(lineReader, "\xc4\x89", '\x87'); // C^ 

      // Quit on ^d
      lineReader.addCallback("\x04", std::make_shared<EofExecutor>(lineReader));

      // further processing of read line by line parser
      std::string line(lineParser.parseLine(lineReader.getLine()));
      {
        std::lock_guard<std::mutex> lock(_mutex);
        if (escapeExecutor->isTriggered()) {
          std::queue<std::string> emptyQueue;
          _messages.swap(emptyQueue);
        } else {
          // Quit signal only if consumer isn't still busy as in the busy
          // case a " " will be added to the quit keyword, preventing
          // consumer thread to join
          quit = line == ::quit && !_requests.empty();
          if (quit) {
            EventSender::getInstance().notify(Event(Event::Quit, ""));
          }
          _messages.push(line);
          if (!_requests.empty()) {
            std::shared_ptr<std::promise<std::string>> firstRequest = _requests.front();
            _requests.pop();
            line = _messages.front();
            _messages.pop();
            firstRequest->set_value(line);
          }
        }
      }
    } while (!quit);
  }

  void getMessage(std::shared_ptr<std::promise<std::string>> line_request) {
    std::lock_guard<std::mutex> lock(_mutex);
    if (!_messages.empty()) {
      // unlikely to be reached in keep_alive mode, where all input
      // instantly fills the pipe. Messages will line up in _messages
      // for slow output and output tool started line by line
      std::string line = _messages.front();
      _messages.pop();
      line_request->set_value(line);
    } else {
      _requests.push(line_request);
    }
  }
private:
  static void addUmlautHandler(LineReader& lineReader, const std::string& umlautSequence, char umlautCode) {
    std::shared_ptr<EventReceiver> er = std::make_shared<UmlautExecutor>(lineReader, umlautCode);
    lineReader.addCallback(umlautSequence, er);
  }

private: 
  std::queue<std::string> _messages;
  std::queue<std::shared_ptr<std::promise<std::string>>> _requests;
  std::mutex _mutex;
};

class Consumer {
  /**
   * Piper wrapper synchronizes output thread loop with main thread signals
   * and maintains state information.
   * The driver has two operation modes: 
   * #1 one morse generator invocation per line
   *    for each output line, child process will be opened, content piped
   *    and afterwards wait for child process to finish.
   * #2 keep_alive mode where text is sent to a child process with
   *    open pipe. In keep_alive mode the open and close methods 
   *    will skip open() and close() operations on the DualPipe
   *    child connection unless a signal (reset, killed) signals otherwise
   *
  */
  class PiperWrapper {
  public:
    PiperWrapper(bool keep_alive) : _keep_alive(keep_alive), _signal_reset(false), _killed(false) {}
    void open(const std::list<std::string>& argv) {
      bool refresh_keepalive = false;
      {
        std::lock_guard<std::mutex> lock(_mutex);
        refresh_keepalive = _keep_alive && (_signal_reset || _killed);
      }
      if (refresh_keepalive) {
        close();
      }
      std::lock_guard<std::mutex> lock(_mutex);
      if (!_piper) {
        _piper = std::make_shared<DualPipe>(); 
        _piper->open(argv);
      }
    }
    ~PiperWrapper() {
      std::lock_guard<std::mutex> lock(_mutex);
      if (_piper) {
        _piper->close();
        _piper = std::make_shared<DualPipe>(); 
      }
    }
    void close() {
      std::shared_ptr<DualPipe> piper;
      bool keep_alive = false;
      bool signal_reset = false;
      bool killed = false; 
      {
        std::lock_guard<std::mutex> lock(_mutex);
        piper = _piper;
        keep_alive = _keep_alive;
        signal_reset = _signal_reset;
        killed = _killed;
      }
      if (!killed && keep_alive && !signal_reset) {
        return;
      }
      if (piper) {
        std::pair<std::string,int> status = piper->close();
        {
          std::lock_guard<std::mutex> lock(_mutex);
          _piper = std::shared_ptr<DualPipe>();
          // no keep alive mode might receive kill in close() call
          killed = killed || _killed;
        }
        if (status.second != 0 && !killed) {
          std::cerr << "child exit(" << status.second << ") returns " << status.first << std::endl;
        }
      }
      {
        std::lock_guard<std::mutex> lock(_mutex);
        _killed = false;
        _signal_reset = false;
      }
    }
    void kill() {
      std::lock_guard<std::mutex> lock(_mutex);
      if (_piper) {
        _piper->kill();
        _killed = true;
      }
    }
    void signalReset() {
      std::lock_guard<std::mutex> lock(_mutex);
      _signal_reset = true;
    }
    void write(const std::string& message) {
      std::lock_guard<std::mutex> lock(_mutex);
      if (_piper) {
        _piper->write(message);
      } else {
        std::cerr << "write on closed piper(" << message << ")" << std::endl;
      }
    }
    std::string read() {
      std::lock_guard<std::mutex> lock(_mutex);
      std::string ret;
      if (_piper) {
        ret = _piper->read();
      } else {
        std::cerr << "read on closed piper" << std::endl;
      }
      return ret;
    }
  private:
    bool _keep_alive;
    bool _signal_reset;
    bool _killed;
    std::shared_ptr<DualPipe> _piper; 
    std::mutex _mutex;
  };
public:
  Consumer(std::shared_ptr<Reader> reader, const std::string& run_command,
    const std::string& speed, const std::string& letter_speed, const std::string& keep_alive,
    const std::string& append_space) 
    : _reader(reader), _run_command(run_command), _speed(speed),
    _letter_speed(letter_speed), _piper(StringUtils::equalsIgnoreCase("yes", keep_alive)),
    _quit(false), _append_space(StringUtils::equalsIgnoreCase("yes", append_space)), _eventExecutor() 
  {
    _eventExecutor = std::make_shared<EventExecutor<Consumer>>(*this);
    EventSender::getInstance().registerEventReceiver(Event::Escape, _eventExecutor);
    EventSender::getInstance().registerEventReceiver(Event::Cursor, _eventExecutor);
    EventSender::getInstance().registerEventReceiver(Event::Quit, _eventExecutor);
  }
  ~Consumer() {
    // Unregistering event will prevent event calls during destruction
    EventSender::getInstance().unregisterEventReceiver(Event::Escape, _eventExecutor);
    EventSender::getInstance().unregisterEventReceiver(Event::Cursor, _eventExecutor);
    EventSender::getInstance().unregisterEventReceiver(Event::Quit, _eventExecutor);
  }

  void onEvent(const Event& event) {
    switch (event.getType()) {
    case Event::Escape:
      _piper.kill();
      std::cout << "*** interrupted ***" << std::endl;
      break;
    case Event::Cursor:
      {
        std::lock_guard<std::mutex> lock(_cursorMutex);
        if (event.getVal() == CursorKeys::_up) {
          incrementInteger(_speed, 2);
          incrementInteger(_letter_speed, 2);
        } else if (event.getVal() == CursorKeys::_down) {
          incrementInteger(_speed, -2);
          incrementInteger(_letter_speed, -2);
        } else if (event.getVal() == CursorKeys::_right) {
          incrementInteger(_letter_speed, 2);
        } else if (event.getVal() == CursorKeys::_left) {
          incrementInteger(_letter_speed, -2);
	  if (std::stoi(_letter_speed) < std::stoi(_speed)) {
	    incrementInteger(_speed, -2);
	  }
        }
        _piper.signalReset();
      }
      std::cout << "*** speed " << _speed << " letter speed " << _letter_speed << " ***" << std::endl;
      break;
    case Event::Quit:
      {
        std::lock_guard<std::mutex> lock(_cursorMutex);
        _quit = true; 
      }
      break;
    default:
      break;
    }
  }
  void consume() {
    do {
      {
        std::lock_guard<std::mutex> lock(_cursorMutex);
        if (_quit) break;
      }
      std::shared_ptr<std::promise<std::string>> request = std::make_shared<std::promise<std::string>>(); 
      _reader->getMessage(request);
      std::future<std::string> requestData = request->get_future();
      std::string message = requestData.get();
      if (_append_space) {
        message = message + " ";
      }
      std::string speed, letter_speed;
      {
        std::lock_guard<std::mutex> lock(_cursorMutex);
        if (_quit) break;
        speed = _speed;
        letter_speed = _letter_speed;
      }
      std::list<std::string> args { _run_command, speed, letter_speed };
      _piper.open(args);
      _piper.write(message);
      //_piper.read();
      _piper.close();
    } while (true);
  }
private:
  
  static void incrementInteger(std::string& intString, int inc) { 
    int val = std::stoi(intString);
    val += inc;
    intString = std::to_string(val);
  }

private:
  std::shared_ptr<Reader> _reader;
  std::string _run_command;
  std::string _speed;
  std::string _letter_speed;
  PiperWrapper _piper;
  std::shared_ptr<EventExecutor<Consumer>> _eventExecutor;
  bool _quit;
  bool _append_space;
  std::mutex _cursorMutex;
};

class MainUtils {
public:
  static void parseCommandLine(int argc, char* argv[], std::map<std::string, std::string>& command_line_options) {
    std::string binary_name = argv[0];
    size_t npos = binary_name.find_last_of('/');
    ::binary_dirname = binary_name.substr(0, npos + 1);
    _binary_filename = binary_name.substr(npos+1);

    std::map<std::string, std::string>::iterator command_line_iter = command_line_options.end();
    for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '-') {
        command_line_iter = command_line_options.insert(std::make_pair(argv[i],"")).first; 
      } else if (command_line_iter != command_line_options.end()) {
        command_line_iter->second = argv[i];
        command_line_iter = command_line_options.end();
      } else {
        command_line_iter = command_line_options.end();
        std::cerr << "unmatched command line arg " << argv[i] << std::endl; 
      }
    }
  }

  static bool displayHelpMessage(const std::map<std::string, std::string>& command_line_options) {
    auto iter = command_line_options.find(_help_opt);
    std::string indent = "  ";
    if (iter != command_line_options.end()) {
      std::cout << _binary_filename << " " << _help_opt << std::endl;
      std::cout << indent << "displays help" << std::endl;
      std::cout << std::endl;
      std::cout << _binary_filename << "[ -option value ]" << std::endl;
      std::cout << "Supported options and values:" << std::endl;
      std::cout << indent << _run_command_opt << " defaults " << _run_command << ": morse sender command" << std::endl;
      std::cout << indent << _cw_speed_opt << " defaults " << _cw_speed << ": overall cw speed" << std::endl;
      std::cout << indent << _cw_letter_speed_opt << " defaults " << _cw_letter_speed << ": cw letter speed" << std::endl;
      std::cout << indent << _keep_alive_opt << " defaults " << _keep_alive << ": keep sender instance running after line end" << std::endl;
      std::cout << indent << _append_space_opt << " defaults " << _append_space << ": append a space after line feed" << std::endl;
      std::cout << std::endl;
      std::cout << "Overall CW speed is set by " << _cw_speed_opt << " and is in wpm" << std::endl;
      std::cout << "Speed for CW letter can differ and is set by " << _cw_letter_speed_opt << std::endl;
      std::cout << "A slightly faster letter speed / longer than standard gaps between letters may improve readability of code" << std::endl;
      std::cout << "CW generator child process kept alive if " << _keep_alive_opt << " == yes, default " << _keep_alive_opt << std::endl;
      std::cout << "Cursur up / down increases / decreases overall morse speed." << std::endl;
      std::cout << "Cursor right / left increases / decreases letter speed." << std::endl;
      std::cout << "Delete key or 2x escape interrupts transmission." << std::endl;
      std::cout << std::endl;
      std::cout << "Special keys: '*' toggles \"professional\" mode reducing all spacing to zero with original cwtext cw2pcm." << std::endl;
      std::cout << "Don't hit it!" << std::endl;
      std::cout << "A line \"#nn-<some text>\" sets text for function key F{nn}, where 0 < nn < 13, to <some text>." << std::endl;
      std::cout << "A line \"#<some text>\" sets text for function key F5 to <some text>. So if you write" << std::endl;
      std::cout << "a line \"#df5oc de dl3lc\" then the driver will instantly send \"df5oc de dl3lc\" and" << std::endl;
      std::cout << "keep the call sign text bloc for reuse." << std::endl;
      std::cout << "Function texts ending with an empty line will append a linefeed \\n to morse input, resulting" << std::endl;
      std::cout << "in instant sending. Without an empty line input can be edited." << std::endl;
      std::cout << "In operation, appending one of these special characters \"" << LineParser::_linefeed_keys << "\" to a text block input" << std::endl;
      std::cout << "stores the text with two trailing \\n\\n for instant sending. Example \"#<some text>" << LineParser::_linefeed_keys[0] << "\"." << std::endl;


      return true;
    }
    return false;
  }

  static void setDefaults(std::map<std::string, std::string>& command_line_options) {
    std::map<std::string,std::string> defaults = {
      { _run_command_opt, _run_command },
      { _cw_speed_opt, _cw_speed },
      { _cw_letter_speed_opt, _cw_letter_speed },
      { _keep_alive_opt, _keep_alive },
      { _append_space_opt, _append_space }
    };
    for (auto& def : defaults) {
      auto iter = command_line_options.find(def.first);
      if (iter == command_line_options.end()) {
        command_line_options.insert(def);
      } else {
        if (iter->second.empty()) {
          iter->second = def.second;
        }
      }
    }
  }
  static void ensureInteger(std::string& str, std::string def) {
    size_t pos = str.find_first_not_of(LineParser::_digits);
    if (pos != std::string::npos) {
      str = def;
    }
  }
  static std::string _binary_filename;
  static const std::string _help_opt;
  static const std::string _run_command_opt;
  static const std::string _run_command;
  static const std::string _cw_speed_opt;
  static const std::string _cw_speed;
  static const std::string _cw_letter_speed_opt;
  static const std::string _cw_letter_speed;
  static const std::string _keep_alive_opt;
  static const std::string _keep_alive;
  static const std::string _append_space_opt;
  static const std::string _append_space;
};

std::string MainUtils::_binary_filename;
const std::string MainUtils::_help_opt("-help");
const std::string MainUtils::_run_command_opt("-run_command");
const std::string MainUtils::_run_command("run_ascii2pcm_from_driver.sh");
const std::string MainUtils::_cw_speed_opt("-speed");
const std::string MainUtils::_cw_speed("24"); 
const std::string MainUtils::_cw_letter_speed_opt("-letter_speed");
const std::string MainUtils::_cw_letter_speed("30");
const std::string MainUtils::_keep_alive_opt("-keep_alive");
const std::string MainUtils::_keep_alive("yes");
const std::string MainUtils::_append_space_opt("-append_space");
const std::string MainUtils::_append_space("yes");

static struct sigaction new_handler, old_handler;
static sighandler_t default_sig_handler;
void sig_handler_function(int sig){
  // Child processes may continue running after parent exits
  // b/c of ::setpgid(0,0); in DualPipe
  EventSender::getInstance().notify(Event(Event::Escape, ""));  
  sigaction(sig, &old_handler, NULL);
  // NB: old handler continually got NULL as the default handler
  // so what seems to work to get default ctrl-c behavior after
  // killing child processes is to just restore the old NULL handler
  // and raise a new signal.
  kill(getpid(), sig);   
}

int main(int argc, char* argv[]) {
  std::map<std::string, std::string> command_line_options;
  MainUtils::parseCommandLine(argc, argv, command_line_options);
  if (MainUtils::displayHelpMessage(command_line_options)) {
    return 1;
  }
  MainUtils::setDefaults(command_line_options);
  std::string run_command(command_line_options[MainUtils::_run_command_opt]);
  if (*run_command.begin() != '/') {
    run_command = ::binary_dirname + run_command;
  }
  std::string speed(command_line_options[MainUtils::_cw_speed_opt]);
  MainUtils::ensureInteger(speed, MainUtils::_cw_speed);
  std::string letter_speed(command_line_options[MainUtils::_cw_letter_speed_opt]);
  MainUtils::ensureInteger(letter_speed, MainUtils::_cw_letter_speed);
  std::string keep_alive(command_line_options[MainUtils::_keep_alive_opt]);
  std::string append_space(command_line_options[MainUtils::_append_space_opt]);

  LineReader::TcAttr tcAttr; 
  if (tcAttr.getStatus() != 0) {
    std::cerr << "ERROR: " << MainUtils::_binary_filename << " does not read files from stdin. Please check the README" << std::endl;
    std::cerr << "on how to generate morse code from input files or see \"training\" folder." << std::endl;
    return 1;
  }

  // Signal hander for ctrl-c to kill off child processes 
  ::sigemptyset(&new_handler.sa_mask);
  ::sigemptyset(&old_handler.sa_mask);
  new_handler.sa_handler = ::sig_handler_function;
  ::sigaction(SIGINT, &new_handler, &old_handler);

  std::cout << "Type text to morse, '" << ::quit << "' or CTRL-D to quit:" << std::endl;
  std::shared_ptr<Reader> reader = std::make_shared<Reader>();
  std::thread consumerThread([&] {
    Consumer consumer(reader, run_command, speed, letter_speed, keep_alive,
      append_space);
    consumer.consume();
  });
  reader->read();
  consumerThread.join();
  return 0;
}

