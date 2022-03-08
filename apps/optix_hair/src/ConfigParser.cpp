
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
        /***************************** ShadeColor1 data ***************************/
        const rapidjson::Value& shadeColor1 = document["ShadeColor1"];
        assert(shadeColor1.IsObject());

        assert(shadeColor1.HasMember("BxDfType"));
        assert(shadeColor1["BxDfType"].IsString());
        std::string bsdf = std::string(shadeColor1["BxDfType"].GetString());
        printf("shadeColor1 BxDfType = %s\n", bsdf.c_str());
        vecBSDF;
        indexBSDF;
        config_json = config_json + "BxDfType : " + shadeColor1["BxDfType"].GetString() + "\n";

        assert(shadeColor1.HasMember("HT"));
        assert(shadeColor1["HT"].IsInt());
        printf("shadeColor1 HT = %d\n", shadeColor1["HT"].GetInt());
        hair1_HT = shadeColor1["HT"].GetInt();
        config_json = config_json + "HT : " + std::to_string(shadeColor1["HT"].GetInt()) + "\n";

        assert(shadeColor1.HasMember("L"));
        assert(shadeColor1["L"].IsString());
        printf("shadeColor1 L = %s\n", shadeColor1["L"].GetString());
        config_json = config_json + "L : " + shadeColor1["L"].GetString() + "\n";
        assert(shadeColor1.HasMember("A"));
        assert(shadeColor1["A"].IsString());
        printf("shadeColor1 = %s\n", shadeColor1["A"].GetString());
        config_json = config_json + "A : " + shadeColor1["A"].GetString() + "\n";

        assert(shadeColor1.HasMember("B"));
        assert(shadeColor1["B"].IsString());
        printf("shadeColor1 B = %s\n", shadeColor1["B"].GetString());
        config_json = config_json + "B : " + shadeColor1["B"].GetString() + "\n";
            
        /************************************************************************/

        /***************************** ShadeColor2 data ***************************/
        const rapidjson::Value& shadeColor2 = document["ShadeColor2"];
        assert(shadeColor2.IsObject());

        assert(shadeColor2.HasMember("BxDfType"));
        assert(shadeColor2["BxDfType"].IsString());
        printf("shadeColor2 BxDfType = %s\n", shadeColor2["BxDfType"].GetString());
        config_json = config_json + "BxDfType : " + shadeColor2["BxDfType"].GetString() + "\n";

        assert(shadeColor2.HasMember("HT"));
        assert(shadeColor2["HT"].IsInt());
        printf("shadeColor2 HT = %d\n", shadeColor2["HT"].GetInt());
        hair2_HT = shadeColor2["HT"].GetInt();
        config_json = config_json + "HT : " + std::to_string(shadeColor2["HT"].GetInt()) + "\n";

        assert(shadeColor2.HasMember("L"));
        assert(shadeColor2["L"].IsString());
        printf("shadeColor2 L = %s\n", shadeColor2["L"].GetString());
        config_json = config_json + "L : " + shadeColor2["L"].GetString() + "\n";

        assert(shadeColor2.HasMember("A"));
        assert(shadeColor2["A"].IsString());
        printf("shadeColor2 A = %s\n", shadeColor2["A"].GetString());
        config_json = config_json + "A : " + shadeColor2["A"].GetString() + "\n";

        assert(shadeColor2.HasMember("B"));
        assert(shadeColor2["B"].IsString());
        printf("shadeColor2 B = %s\n", shadeColor2["B"].GetString());
        config_json = config_json + "B : " + shadeColor2["B"].GetString() + "\n";
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

            isConfigFinished = true;
            
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


