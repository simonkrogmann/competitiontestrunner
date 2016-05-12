#include <iostream>
#include <string>
#include <chrono>

#include <utilgpu/cpp/file.h>
#include <utilgpu/cpp/str.h>
#include <utilgpu/qt/Config.h>

#include "language.h"

/*
TODO:
    better diff view
    better performance testing
    option for more test files
*/

struct RunResult
{
    int returnValue;
    double runtime;
};

void requireFileExists(const std::string &filename)
{
    if (!util::fileExists(filename))
    {
        std::cout << filename << " does not exist" << std::endl;
        exit(1);
    }
}

void diff(const std::string &actual, const std::string &expected)
{
    std::cout << "============ Actual ============" << std::endl
              << actual << "=========== Expected ===========" << std::endl
              << expected;
}

int runProgram(const std::string &command)
{
    return system(command.c_str());
}

bool alreadyBuilt(std::string artifact, std::string source)
{
    return util::File(artifact).exists() &&
           util::File(artifact).timeStamp() > util::File(source).timeStamp();
}

void exitOnErrorReturnValue(const int &returnValue,
                            const std::string &message = "")
{
    if (returnValue)
    {
        if (message != "")
        {
            std::cout << message << std::endl;
        }
        exit(2);
    }
}

void runProgramExitOnError(const std::string &command,
                           const std::string &message = "")
{
    const auto returnValue = runProgram(command);
    exitOnErrorReturnValue(returnValue, message);
}

RunResult measure(const std::string &command)
{
    const auto start = std::chrono::steady_clock::now();
    const auto returnValue = runProgram(command);
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> duration = end - start;
    return {returnValue, duration.count()};
}

std::string prepareAndGetRunCommand(Language language, Run run,
                                    std::string source)
{
    if (language.compiled)
    {
        const std::string cmp = source + "." + run.name + ".cmp";
        if (!alreadyBuilt(cmp, source))
        {
            const auto compile_command =
                run.command + " " + source + " -o " + cmp;
            runProgramExitOnError(compile_command);
        }
        return cmp;
    }
    return run.command + source;
}

int main(int argc, char *argv[])
{
    util::Config config{"simonkrogmann", "test-runner"};
    config.setDefaults({
        {"filename", ""},
    });
    auto name = config.value("filename");
    if (argc == 2)
    {
        name = argv[1];
        config.setValue("filename", name);
    }
    if (name == "")
    {
        std::cout << "Missing filename" << std::endl;
        exit(3);
    }
    requireFileExists(name);
    auto extension = util::stripWhitespace(util::loadFile(name));
    if (languages.find(extension) == languages.end())
    {
        std::cout << "Language not supported" << std::endl;
        exit(5);
    }
    auto &language = languages[extension];
    const std::string source = name + "." + extension;
    const std::string out = name + ".out";
    const std::string ref = name + ".ref";
    const std::string in = name + ".in";
    const std::string generate = name + ".test.py";
    const std::string largeIn = name + ".large.in";

    std::map<std::string, double> runtimes;
    requireFileExists(source);
    requireFileExists(in);
    for (const auto &preparation_command : language.preparationCommands)
    {
        runProgramExitOnError(preparation_command + source);
    }
    for (const auto &run : language.runs)
    {
        const auto command = prepareAndGetRunCommand(language, run, source) +
                             " < " + in + " > " + out;
        const auto result = measure(command);
        exitOnErrorReturnValue(
            result.returnValue,
            "Program exited with " + std::to_string(result.returnValue));
        requireFileExists(out);
        requireFileExists(ref);
        const auto output = util::loadFile(out);
        const auto reference = util::loadFile(ref);
        if (output != reference)
        {
            std::cout << "Test failed on run " << run.name << ":" << std::endl;
            diff(output, reference);
            exit(4);
        }
        runtimes[run.name] = result.runtime;
    }

    if (util::fileExists(generate))
    {
        if (!alreadyBuilt(largeIn, generate))
        {
            const std::string generateCommand =
                "python " + generate + " > " + largeIn;
            runProgramExitOnError(generateCommand,
                                  "Error during large test case creation.");
        }
        requireFileExists(largeIn);
        for (const auto &run : language.runs)
        {
            const auto runCommand =
                prepareAndGetRunCommand(language, run, source) + " < " +
                largeIn + " > /dev/null";
            const auto result = measure(runCommand);
            exitOnErrorReturnValue(result.returnValue,
                                   "Program exited with " +
                                       std::to_string(result.returnValue) +
                                       " on large run.");
            if (run.name == "perf")
            {
                std::cout << "Large test case needed " << result.runtime << "s."
                          << std::endl;
            }
        }
    }
    else
    {
        std::cout << "Small test case needed " << runtimes["perf"] << "s."
                  << std::endl;
    }
}
