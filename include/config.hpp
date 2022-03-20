#pragma once
#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <switch.h>
// #include <toml.hpp>
#include <toml++/toml.h>
#include "mini/ini.h"

#define ARCADIA_CONFIG_PATH "sdmc://switch/arcadia.ini"
#define SYSTEM_SETTINGS "sdmc://atmosphere/config/system_settings.ini"
#define ROM_PATH "sd://atmosphere/contents/01006A800016E000/romfs/"

class Infos {
    public:
        std::string version;
};

class Paths{
    public:
        std::string arc;
        std::string umm;
        std::vector<std::string> extra_paths;
};

class Updater{
    public:
        std::string server_ip;
        bool beta_updates;
};

class Logger{
    public:
        std::string logger_level;
};

class Miscellaneous{
    public:
        bool debug;
        std::string region;
};

class ConfigLayout {
    public:
        Infos infos;
        Paths paths;
        Updater updater;
        Logger logger;
        Miscellaneous misc;
};


class ARCadiaConfig {
    private:
        inline static mINI::INIStructure ini;
    public:
        inline static std::string sort_option;
        inline static bool sort_desc;


        static bool initConfig(){
            mINI::INIFile file(ARCADIA_CONFIG_PATH);
            file.read(ini);
            if(!std::filesystem::exists(ARCADIA_CONFIG_PATH)){
                sort_option = "name";
                sort_desc = true;
                ini["config"]["sort_option"] = sort_option;
                ini["config"]["sort_desc"] = std::to_string(sort_desc);
                file.generate(ini);
                return true;
            } else {
                sort_option = ini["config"]["sort_option"];
                if(ini["config"]["sort_desc"] == "0"){
                    sort_desc = false;
                }else{
                    sort_desc = true;
                }
                return true;
            }
        }

        static bool write(std::string section, std::string key, std::string value){
            mINI::INIFile file(ARCADIA_CONFIG_PATH);
            ini[section][key] = value;
            return file.write(ini);
        }
};
