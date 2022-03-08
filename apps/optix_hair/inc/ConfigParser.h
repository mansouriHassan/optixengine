
#pragma once

#ifndef CONFIG_PARSER_H
#define CONFIG_PARSER_H

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON
#include "shaders/function_indices.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>

enum HairType
{
  STRAIGHT_HAIR,      // Straight hair
  CURLY_HAIR,         // Curly hair.
  WAVY_HAIR           // Wavy hair.
};
struct View
{
    std::string view_name;             // The name used in the scene description to identify this material instance.
	std::string view_phi;
	std::string view_theta;
	std::string view_fov;
	std::string view_distance;
};
class ConfigParser
{
public:
  ConfigParser();
  ~ConfigParser();

  void parseConfigData(const std::string& json_data);
  

  int getHairType() const;
  int getHair1HT() const;
  int getHair2HT() const;
  int getVertRouge_Concentration() const;
  int getCendreCuivre_Concentration() const;
  int getIriseDore_Concentration() const;
  int getColorL() const;
  int getColorA() const;
  int getColorB() const;
  FunctionIndex getIndexBSDF() const;
  std::vector<View> getViews() const;

  static ConfigParser* getInstance();

protected:
    static ConfigParser* config_parser;

private:
  rapidjson::Document document;
  std::vector<std::string> vecBSDF;
  FunctionIndex indexBSDF;
  View default_view;
  int hair_type;
  int hair1_HT;
  int hair2_HT;
  int vertRouge_Concentration;
  int cendreCuivre_Concentration;
  int iriseDore_Concentration;

  int colorL;
  int colorA;
  int colorB;

public:
	int view_index;
	bool isCamreaChanged;
	bool isMaterialChanged;
	bool isFirstBxDFTypeChanged;
	bool isSecondBxDFTypeChanged;
	bool isFirstHTChanged;
	bool isSecondHTChanged;
	bool isFirstHairColorChanged;
	bool isSecondHairColorChanged;
	bool isDynamicSettingsChanged;
	bool isHairTypeChanged;
	bool isDyeNeutralHTChanged;
	bool isMaterialGUIVertChanged;
	bool isVertRougeConcentrationChanged;
	bool isMaterialGUIRedChanged;
	bool isMaterialGUICendreChanged;
	bool isCendreCuivreConcentrationChanged;
	bool isMaterialGUICuivreChanged;
	bool isMaterialGUIIriseChanged;
	bool IriseDoreConcentrationChanged;
	bool isMaterialGUIDoreeChanged;

	bool isConfigFinished;
	std::vector<View> camera_views;
	std::string view_name;
};

#endif // CONFIG_PARSER_H
