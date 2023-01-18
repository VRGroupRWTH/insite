#pragma once

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

struct InsiteSimulatorEvent {
  int type;
  int event;

  InsiteSimulatorEvent(int event)
      : type(0), event(event) {}

  rapidjson::StringBuffer SerializeToJSON() {
    rapidjson::StringBuffer Buffer;
    rapidjson::Writer<rapidjson::StringBuffer> Writer(Buffer);

    Writer.StartObject();
    Writer.Key("type");
    Writer.Int(type);
    Writer.Key("event");
    Writer.Int(event);
    return Buffer;
  };
  virtual ~InsiteSimulatorEvent(){};
};
