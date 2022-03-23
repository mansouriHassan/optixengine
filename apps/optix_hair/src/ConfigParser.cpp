
#include "inc/ConfigParser.h"

#include <iostream>
#include <fstream>
#include <sstream>

#include "rapidjson/document.h"     // rapidjson's DOM-style API
#include "rapidjson/prettywriter.h" // for stringify JSON

ConfigParser* ConfigParser::config_parser = nullptr;

ConfigParser::ConfigParser()
{
    view_index = 0;
    default_view.view_phi = "0.815f";
    default_view.view_theta = "0.6f";
    default_view.view_fov = "45.f";
    default_view.view_distance = "10.f";
    camera_views = {default_view};
    vecBSDF = {"BRDF Diffuse", "BRDF Specular", "BSDF Specular", "BRDF GGX Smith", "BSDF GGX Smith", "BSDF Hair"};
    hair_type = 0;
    hair1_HT = 5;
    hair2_HT = 5;
    indexBSDF = INDEX_BCSDF_HAIR;
    vertRouge_Concentration = 4;
    cendreCuivre_Concentration = 4;
    iriseDore_Concentration = 4;

    colorL = 255;
    colorA = 255;
    colorB = 255;

    isCamreaChanged = false;
    isMaterialChanged = false;
    isFirstBxDFTypeChanged = false;
    isSecondBxDFTypeChanged = false;
    isFirstHTChanged = false;
    isSecondHTChanged = false;
    isFirstHairColorChanged = false;
    isSecondHairColorChanged = false;
    isDynamicSettingsChanged = false;
    isHairTypeChanged = false;
    isDyeNeutralHTChanged = false;
    isMaterialGUIVertChanged = false;
    isVertRougeConcentrationChanged = false;
    isMaterialGUIRedChanged = false;
    isMaterialGUICendreChanged = false;
    isCendreCuivreConcentrationChanged = false;
    isMaterialGUICuivreChanged = false;
    isMaterialGUIIriseChanged = false;
    isMaterialGUIDoreeChanged = false;

    isConfigFinished = false;
}

ConfigParser::~ConfigParser()
{
}

ConfigParser* ConfigParser::getInstance() {
    if (config_parser == nullptr) {
        config_parser = new ConfigParser();
    }

    return config_parser;
}

void ConfigParser::parseConfigData(const std::string& json_data)
{
    std::string config_json = "";
    if (!json_data.empty())
    {
        document.Parse(json_data.c_str());
        assert(document.IsObject());

        assert(document.HasMember("HairType"));
        assert(document["HairType"].IsString());
        std::string hairType = std::string(document["HairType"].GetString());
        printf("HairType = %s\n", hairType.c_str());
        if(hairType == "Straight hair") {
            hair_type = STRAIGHT_HAIR;
        } else if (hairType == "Curly hair") {
            hair_type = CURLY_HAIR;
        } else if (hairType == "Wavy hair") {
            hair_type = WAVY_HAIR;
        }
        baseShade.HairType = hair_type;
        customShade.HairType = hair_type;
        /***************************** BaseShade data ***************************/
        const rapidjson::Value& BaseShade = document["BaseShade"];
        assert(BaseShade.IsObject());

        /*
        assert(BaseShade.HasMember("HairType"));
        assert(BaseShade["HairType"].IsString());
        std::string hairType = std::string(BaseShade["HairType"].GetString());
        printf("HairType = %s\n", hairType.c_str());
        if(hairType == "Straight hair") {
            hair_type = STRAIGHT_HAIR;
        } else if (hairType == "Curly hair") {
            hair_type = CURLY_HAIR;
        } else if (hairType == "Wavy hair") {
            hair_type = WAVY_HAIR;
        }
        baseShade.HairType = hair_type;
        */
        assert(BaseShade.HasMember("BxDfType"));
        assert(BaseShade["BxDfType"].IsString());
        std::string bsdf = std::string(BaseShade["BxDfType"].GetString());
        baseShade.BxDfType = bsdf;
        printf("BaseShade BxDfType = %s\n", bsdf.c_str());
        vecBSDF;
        indexBSDF;
        baseShade.BxDfIndex = indexBSDF;
        config_json = config_json + "BxDfType : " + BaseShade["BxDfType"].GetString() + "\n";

        assert(BaseShade.HasMember("HT"));
        assert(BaseShade["HT"].IsInt());
        printf("BaseShade HT = %d\n", BaseShade["HT"].GetInt());
        hair1_HT = BaseShade["HT"].GetInt();
        baseShade.shadeHT = BaseShade["HT"].GetInt();
        config_json = config_json + "HT : " + std::to_string(BaseShade["HT"].GetInt()) + "\n";

        assert(BaseShade.HasMember("L"));
        assert(BaseShade["L"].IsInt());
        baseShade.shadeColorL = BaseShade["L"].GetInt();
        printf("BaseShade L = %d\n", BaseShade["L"].GetInt());
        config_json = config_json + "L : " + std::to_string(BaseShade["L"].GetInt()) + "\n";

        assert(BaseShade.HasMember("A"));
        assert(BaseShade["A"].IsInt());
        baseShade.shadeColorA = BaseShade["A"].GetInt();
        printf("BaseShade = %d\n", BaseShade["A"].GetInt());
        config_json = config_json + "A : " + std::to_string(BaseShade["A"].GetInt()) + "\n";

        assert(BaseShade.HasMember("B"));
        assert(BaseShade["B"].IsInt());
        baseShade.shadeColorB = BaseShade["B"].GetInt();
        printf("BaseShade B = %d\n", BaseShade["B"].GetInt());
        config_json = config_json + "B : " + std::to_string(BaseShade["B"].GetInt()) + "\n";

        /********************************************* RGB Color ***************************************/
        assert(BaseShade.HasMember("Red"));
        assert(BaseShade["Red"].IsInt());
        baseShade.shadeColorRed = BaseShade["Red"].GetInt();
        printf("BaseShade Red = %d\n", BaseShade["Red"].GetInt());
        config_json = config_json + "Red : " + std::to_string(BaseShade["Red"].GetInt()) + "\n";

        assert(BaseShade.HasMember("Green"));
        assert(BaseShade["Green"].IsInt());
        baseShade.shadeColorGreen = BaseShade["Green"].GetInt();
        printf("BaseShade Green= %d\n", BaseShade["Green"].GetInt());
        config_json = config_json + "Green : " + std::to_string(BaseShade["Green"].GetInt()) + "\n";

        assert(BaseShade.HasMember("Blue"));
        assert(BaseShade["Blue"].IsInt());
        baseShade.shadeColorBlue = BaseShade["B"].GetInt();
        printf("BaseShade Blue = %d\n", BaseShade["Blue"].GetInt());
        config_json = config_json + "B : " + std::to_string(BaseShade["Blue"].GetInt()) + "\n";
            
        /************************************************************************/

        /***************************** CustomShade data ***************************/
        const rapidjson::Value& CustomShade = document["CustomShade"];
        assert(CustomShade.IsObject());

        /*
       assert(CustomShade.HasMember("HairType"));
       assert(CustomShade["HairType"].IsString());
       std::string hairType = std::string(CustomShade["HairType"].GetString());
       printf("HairType = %s\n", hairType.c_str());
       if(hairType == "Straight hair") {
           hair_type = STRAIGHT_HAIR;
       } else if (hairType == "Curly hair") {
           hair_type = CURLY_HAIR;
       } else if (hairType == "Wavy hair") {
           hair_type = WAVY_HAIR;
       }
       customShade.HairType = hair_type;
       */

        assert(CustomShade.HasMember("BxDfType"));
        assert(CustomShade["BxDfType"].IsString());
        printf("CustomShade BxDfType = %s\n", CustomShade["BxDfType"].GetString());
        config_json = config_json + "BxDfType : " + CustomShade["BxDfType"].GetString() + "\n";

        assert(CustomShade.HasMember("HT"));
        assert(CustomShade["HT"].IsInt());
        printf("CustomShade HT = %d\n", CustomShade["HT"].GetInt());
        hair2_HT = CustomShade["HT"].GetInt();
        config_json = config_json + "HT : " + std::to_string(CustomShade["HT"].GetInt()) + "\n";

        assert(CustomShade.HasMember("L"));
        assert(CustomShade["L"].IsString());
        printf("CustomShade L = %s\n", CustomShade["L"].GetString());
        config_json = config_json + "L : " + CustomShade["L"].GetString() + "\n";

        assert(CustomShade.HasMember("A"));
        assert(CustomShade["A"].IsString());
        printf("CustomShade A = %s\n", CustomShade["A"].GetString());
        config_json = config_json + "A : " + CustomShade["A"].GetString() + "\n";

        assert(CustomShade.HasMember("B"));
        assert(CustomShade["B"].IsString());
        printf("CustomShade B = %s\n", CustomShade["B"].GetString());
        config_json = config_json + "B : " + CustomShade["B"].GetString() + "\n";
        /************************************************************************/

        /*************************** Cameras data ******************************/
        const rapidjson::Value& cameras = document["Cameras"];
        View view;
        assert(cameras.IsArray());
        for (int i = 0; i < cameras.Size(); i++) { // Uses SizeType instead of size_t
            assert(cameras[i].IsObject());

            assert(cameras[i].HasMember("name"));
            assert(cameras[i]["name"].IsString());
            view.view_name = std::string(cameras[i]["name"].GetString());
            printf("name = %s\n", view.view_name.c_str());
            config_json = config_json + "cameras name : " + view.view_name + "\n";

            assert(cameras[i].HasMember("m_camera.m_phi"));
            assert(cameras[i]["m_camera.m_phi"].IsString());
            view.view_phi = cameras[i]["m_camera.m_phi"].GetString();
            printf("m_camera.m_phi = %s\n", view.view_phi.c_str());
            config_json = config_json + "m_camera.m_phi : " + view.view_phi + "\n";

            assert(cameras[i].HasMember("m_camera.m_theta"));
            assert(cameras[i]["m_camera.m_theta"].IsString());
            view.view_theta = std::string(cameras[i]["m_camera.m_theta"].GetString());
            printf("m_camera.m_theta = %s\n", view.view_theta.c_str());
            config_json = config_json + "m_camera.m_theta : " + view.view_theta + "\n";

            assert(cameras[i].HasMember("m_camera.m_fov"));
            assert(cameras[i]["m_camera.m_fov"].IsString());
            view.view_fov = std::string(cameras[i]["m_camera.m_fov"].GetString());
            printf("m_camera.m_fov = %s\n", view.view_fov.c_str());
            config_json = config_json + "m_camera.m_fov : " + view.view_fov + "\n";

            assert(cameras[i].HasMember("m_camera.m_distance"));
            assert(cameras[i]["m_camera.m_distance"].IsString());
            view.view_distance = std::string(cameras[i]["m_camera.m_distance"].GetString());
            printf("m_camera.m_distance = %s\n", view.view_fov.c_str());
            config_json = config_json + "m_camera.m_distance : " + view.view_fov + "\n";

            camera_views.push_back(view);
            isCamreaChanged = true;
            isMaterialChanged = true;
            isFirstBxDFTypeChanged = true;
            isSecondBxDFTypeChanged = true;
            isFirstHTChanged = true;
            isSecondHTChanged = true;
            isFirstHairColorChanged = true;
            isSecondHairColorChanged = true;
            isDynamicSettingsChanged = true;
            isHairTypeChanged = true;
            
        }
        /************************************************************************/
        isConfigFinished = true;
        std::ofstream configFile;
        configFile.open("config.json");
        configFile << config_json + "\n";
        configFile.close();
    }
}

int ConfigParser::getHairType() const {
    return hair_type;
}

int ConfigParser::getHair1HT() const {
    return hair1_HT;
}

int ConfigParser::getHair2HT() const {
    return hair2_HT;
}

int ConfigParser::getVertRouge_Concentration() const {
    return vertRouge_Concentration;
}

int ConfigParser::getCendreCuivre_Concentration() const {
    return cendreCuivre_Concentration;
}

int ConfigParser::getIriseDore_Concentration() const {
    return iriseDore_Concentration;
}

int ConfigParser::getColorL() const {
    return colorL;
}

int ConfigParser::getColorA() const {
    return colorA;
}

int ConfigParser::getColorB() const {
    return colorB;
}

FunctionIndex ConfigParser::getIndexBSDF() const {
    return indexBSDF;
}


