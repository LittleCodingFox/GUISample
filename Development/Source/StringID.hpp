#pragma once

typedef unsigned long StringID;

StringID MakeStringID(const std::string &Name);
const std::string &GetStringIDString(StringID ID);
