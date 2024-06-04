#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <map>
#include <set>

class Event {
public:
  enum Type {
    Escape,
    Cursor,
    Quit
  };
  Event(Type type, std::string val) : _type(type), _val(val) {}
  Type getType() const {
    return _type;
  }
  const std::string& getVal() const {
    return _val;
  }
private:
  Type _type;
  std::string _val;
};


class EventReceiver {
public:
  virtual ~EventReceiver() {
  }
  virtual void notify(const Event& event) = 0;
};

class EventSender {
  typedef std::map<Event::Type, std::set<std::shared_ptr<EventReceiver>>> ReceiverMap_t;
private:
  EventSender() {
  }
public:
  static EventSender& getInstance();
  void registerEventReceiver(Event::Type type, const std::shared_ptr<EventReceiver>& receiver);
  void unregisterEventReceiver(Event::Type type, const std::shared_ptr<EventReceiver>& receiver);
  void notify(const Event& event);
private:
  std::mutex _mutex;
  ReceiverMap_t _receiver_map; 
};

template <class Target> class EventExecutor : public EventReceiver {
public:
  EventExecutor(Target& target) : _target(target) {}
  ~EventExecutor() {
  }
  void notify(const Event& event) {
    _target.onEvent(event);
  }
private:
  std::mutex _mutex;
  Target& _target;
};


