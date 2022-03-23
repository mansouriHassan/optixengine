
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
    hair_HT = 10;
    indexBSDF = INDEX_BCSDF_HAIR;

    colorL = 255;
    colorA = 255;
    colorB = 255;

    isCamreaChanged = false;
    isMaterialChanged = false;
    isHairSelected = false;
    isBxDFTypeChanged = false;
    isHTChanged = false;
    isHairColorChanged = false;
    isDynamicSettingsChanged = false;
    isHairTypeChanged = false;
    isDyeNeutralHTChanged = false;
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
        /*
        Hair type: "Straight hair, Curly hair, Wavy hair"
        */
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
        colorShade.hairType = hairType;

        /***************************** ColorShade data ***************************/
        const rapidjson::Value& ColorShade = document["ColorShade"];
        assert(ColorShade.IsObject());

        assert(ColorShade.HasMember("BxDfType"));
        assert(ColorShade["BxDfType"].IsString());
        std::string bsdf = std::string(ColorShade["BxDfType"].GetString());
        colorShade.BxDfType = bsdf;
        printf("ColorShade BxDfType = %s\n", bsdf.c_str());
        colorShade.BxDfIndex = indexBSDF;
        config_json = config_json + "BxDfType : " + bsdf + "\n";

        assert(ColorShade.HasMember("HT"));
        assert(ColorShade["HT"].IsInt());
        hair_HT = ColorShade["HT"].GetInt();
        printf("ColorShade HT = %d\n", hair_HT);
        colorShade.shadeHT = hair_HT;
        config_json = config_json + "HT : " + std::to_string(colorShade.shadeHT) + "\n";

        assert(ColorShade.HasMember("L"));
        assert(ColorShade["L"].IsInt());
        colorShade.shadeColorL = ColorShade["L"].GetInt();
        printf("ColorShade L = %d\n", colorShade.shadeColorL);
        config_json = config_json + "L : " + std::to_string(colorShade.shadeColorL) + "\n";

        assert(ColorShade.HasMember("A"));
        assert(ColorShade["A"].IsInt());
        colorShade.shadeColorA = ColorShade["A"].GetInt();
        printf("BaseShade = %d\n", colorShade.shadeColorA);
        config_json = config_json + "A : " + std::to_string(colorShade.shadeColorA) + "\n";

        assert(ColorShade.HasMember("B"));
        assert(ColorShade["B"].IsInt());
        colorShade.shadeColorB = ColorShade["B"].GetInt();
        printf("ColorShade B = %d\n", colorShade.shadeColorB);
        config_json = config_json + "B : " + std::to_string(colorShade.shadeColorB) + "\n";

        /********************************************* RGB Color ***************************************/
        assert(ColorShade.HasMember("colorRed"));
        assert(ColorShade["colorRed"].IsInt());
        colorShade.shadeColorRed = ColorShade["colorRed"].GetInt();
        printf("ColorShade Red = %d\n", colorShade.shadeColorRed);
        config_json = config_json + "colorRed : " + std::to_string(colorShade.shadeColorRed) + "\n";

        assert(ColorShade.HasMember("colorGreen"));
        assert(ColorShade["colorGreen"].IsInt());
        colorShade.shadeColorGreen = ColorShade["colorGreen"].GetInt();
        printf("ColorShade Green= %d\n", colorShade.shadeColorGreen);
        config_json = config_json + "colorGreen : " + std::to_string(colorShade.shadeColorGreen) + "\n";

        assert(ColorShade.HasMember("colorBlue"));
        assert(ColorShade["colorBlue"].IsInt());
        colorShade.shadeColorBlue = ColorShade["colorBlue"].GetInt();
        printf("ColorShade Blue = %d\n", colorShade.shadeColorBlue);
        config_json = config_json + "colorBlue : " + std::to_string(colorShade.shadeColorBlue) + "\n";
            
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
            isHairSelected = true;
            isBxDFTypeChanged = true;
            isHTChanged = true;
            isHairColorChanged = true;
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
    return hair_HT;
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


