
#pragma once

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

#include <string>

enum HairType
{
  STRAIGHT_HAIR,      // Straight hair
  CURLY_HAIR,         // Curly hair.
  WAVY_HAIR           // Wavy hair.
};


// System and scene file parsing information.
class ConfigParser
{
public:
  ConfigParser();
  ~ConfigParser();

  void parseConfigData(const std::string& json_data);
  

  int                 getHairType() const;
  int                 getView() const;

  static ConfigParser* getInstance();

protected:
    static ConfigParser* config_parser;

private:
  rapidjson::Document document;
  int hair_type;
};

#endif // CONFIG_PARSER_H
