#include "mods_tabs.hpp"

#include <stdio.h>
#include <switch.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <borealis.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

#include "config.hpp"
#include "inipp.h"

#define QUASAR_WORKSPACES "sdmc://Quasar"

u64 runningTID2()
{
  u64 pid = 0;
  u64 tid = 0;
  pmdmntInitialize();
  pminfoInitialize();
  pmdmntGetApplicationProcessId(&pid);
  pminfoGetProgramId(&tid, pid);
  pminfoExit();
  pmdmntExit();
  return tid;
}


std::string replace(std::string str, const std::string from, const std::string to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return str;
    str.replace(start_pos, from.length(), to);
    return str;
}

int move_file(std::string og_path, std::string out_path)
{
    return std::rename(og_path.c_str(), out_path.c_str());
}

std::string clean_dot(std::string str)
{
    if (str.at(0) == '.')
        return str.substr(1, str.length());
    else
        return str;
}

bool compareFunction(const ModInfo& lhs, const ModInfo& rhs) { return lhs.name < rhs.name; }

// REWORK THIS!
std::vector<brls::ListItem*> ModsList::skylinePlugins()
{
    std::vector<brls::ListItem*> skylineList;
    std::string tid = fmt::format("{:x}", runningTID2());
    std::string enabled_path  = fmt::format("sdmc://atmosphere/contents/0{}/romfs/skyline/plugins/", tid);
    std::string disabled_path = fmt::format("sdmc://atmosphere/contents/0{}/romfs/skyline/disabled_plugins/", tid);

    if (std::filesystem::exists(enabled_path))
    {
        for (auto& directoryPath : std::filesystem::directory_iterator(enabled_path))
        {
            std::string pluginFilename = directoryPath.path().filename();
            brls::ListItem* dialogItem = new brls::ListItem(pluginFilename);
            dialogItem->setChecked(true);

            dialogItem->getClickEvent()->subscribe([enabled_path, disabled_path, pluginFilename, dialogItem](brls::View* view) {
                std::string disabled_plugin_path = enabled_path + "/" + pluginFilename;
                std::string enabled_plugin_path  = disabled_path + "/" + pluginFilename;

                if (std::filesystem::exists(disabled_plugin_path))
                {
                    if (move_file(disabled_plugin_path, enabled_plugin_path) != 0)
                        brls::Application::notify("Failed enabling plugin!");
                    else
                        dialogItem->setChecked(false);
                }
                else if (std::filesystem::exists(enabled_plugin_path))
                {
                    if (move_file(enabled_plugin_path, disabled_plugin_path) != 0)
                        brls::Application::notify("Failed disabling plugin!");
                    else
                        dialogItem->setChecked(true);
                }
                else
                {
                    brls::Application::notify("Failed");
                };
            });
            skylineList.push_back(dialogItem);
        }
    }
    else
    {
        std::filesystem::create_directories(enabled_path);
    }

    if (std::filesystem::exists(disabled_path))
    {
        for (auto& directoryPath : std::filesystem::directory_iterator(disabled_path))
        {
            std::string pluginFilename = directoryPath.path().filename();
            brls::ListItem* dialogItem = new brls::ListItem(pluginFilename);

            dialogItem->getClickEvent()->subscribe([enabled_path, disabled_path, pluginFilename, dialogItem](brls::View* view) {
                std::string disabled_plugin_path = enabled_path + "/" + pluginFilename;
                std::string enabled_plugin_path  = disabled_path + "/" + pluginFilename;

                if (std::filesystem::exists(disabled_plugin_path))
                {
                    if (move_file(disabled_plugin_path, enabled_plugin_path) != 0)
                        brls::Application::notify("Failed enabling plugin!");
                    else
                        dialogItem->setChecked(false);
                }
                else if (std::filesystem::exists(enabled_plugin_path))
                {
                    if (move_file(enabled_plugin_path, disabled_plugin_path) != 0)
                        brls::Application::notify("Failed disabling plugin!");
                    else
                        dialogItem->setChecked(true);
                }
                else
                {
                    brls::Application::notify("Failed");
                };
            });
            skylineList.push_back(dialogItem);
        }
    }
    else
    {
        std::filesystem::create_directories(disabled_path);
    }

    return skylineList;
}