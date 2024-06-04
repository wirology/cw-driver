#include "Event.h"
#include <map>

EventSender& EventSender::getInstance() {
  static EventSender theSender;
  return theSender;
}
void EventSender::registerEventReceiver(Event::Type type, const std::shared_ptr<EventReceiver>& receiver) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto iter = _receiver_map.find(type);
  if (iter == _receiver_map.end()) {
    iter = _receiver_map.insert(ReceiverMap_t::value_type(type, ReceiverMap_t::mapped_type())).first; 
  }
  iter->second.insert(receiver);
}
void EventSender::unregisterEventReceiver(Event::Type type, const std::shared_ptr<EventReceiver>& receiver) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto iter = _receiver_map.find(type);
  if (iter != _receiver_map.end()) {
    iter->second.erase(receiver);
  }
}
void EventSender::notify(const Event& event) {
  std::lock_guard<std::mutex> lock(_mutex);
  auto iter = _receiver_map.find(event.getType());
  if (iter != _receiver_map.end()) {
    for (auto& receiver : iter->second) {
      receiver->notify(event);
    }
  }
}

