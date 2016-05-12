#include <iostream>
#include <string>

#include <utilgpu/cpp/file.h>
#include <utilgpu/cpp/resource.h>
#include <utilgpu/cpp/str.h>
#include <utilgpu/qt/Config.h>

int runProgram(const std::string &command)
{
    return system(command.c_str());
}

int main(int argc, char *argv[])
{
    util::Config config{"simonkrogmann", "competitiontestrunner"};
    if (argc != 3)
    {
        std::cout << "Usage: createfiles <name> <extension>" << std::endl;
        exit(1);
    }
    std::string name = argv[1];
    std::string language = argv[2];
    config.setValue("filename", name);
    util::File base{name};
    util::File source{name + "." + language};
    util::File generate{name + ".test.py"};
    std::string directory = base.directory();
    runProgram("mkdir -p " + directory);
    runProgram("touch " + name + ".ref");
    runProgram("touch " + name + ".in");
    auto sourceTemplate = util::loadResource<competitiontestrunner>(
        "resource/template." + language);
    source.setContent(sourceTemplate.content());
    auto testTemplate =
        util::loadResource<competitiontestrunner>("resource/generate.py");
    generate.setContent(testTemplate.content());
    base.setContent(language);
    return 0;
}
