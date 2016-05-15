#include <chrono>
#include <iostream>
#include <string>

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

void diff(const util::File &actual, const util::File &expected)
{
    std::cout << "============ Actual ============" << std::endl
              << actual.content()
              << "=========== Expected ===========" << std::endl
              << expected.content();
}

int runProgram(const std::string &command)
{
    return system(command.c_str());
}

bool alreadyBuilt(const util::File &artifact, const util::File &source)
{
    return artifact.exists() && artifact.timeStamp() > source.timeStamp();
}

void exitOnError(const int &returnValue, const std::string &message = "")
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

void exitOnError(const RunResult &result, const std::string &message = "")
{
    exitOnError(result.returnValue, message);
}

void runProgramExitOnError(const std::string &command,
                           const std::string &message = "")
{
    const auto returnValue = runProgram(command);
    exitOnError(returnValue, message);
}

RunResult measure(const std::string &command)
{
    const auto start = std::chrono::steady_clock::now();
    const auto returnValue = runProgram(command);
    const auto end = std::chrono::steady_clock::now();
    const std::chrono::duration<double> duration = end - start;
    return {returnValue, duration.count()};
}

std::string prepareAndGetRunCommand(const Language &language, const Run &run,
                                    const util::File &source)
{
    if (language.compiled)
    {
        const util::File cmp{"./" + source.path + "." + run.name + ".cmp"};
        if (!alreadyBuilt(cmp, source))
        {
            const auto compile_command =
                run.command + " " + source.path + " -o " + cmp.path;
            runProgramExitOnError(compile_command);
        }
        return cmp.path;
    }
    return run.command + source.path;
}

void prepare(const util::File &source, const Language &language)
{
    for (const auto &preparation_command : language.preparationCommands)
    {
        runProgramExitOnError(preparation_command + source.path);
    }
}

void test(const std::string &name, const std::string &extension)
{
    auto &language = languages[extension];
    const util::File source{name + "." + extension};
    const util::File out{name + ".out"};
    const util::File ref{name + ".ref"};
    const util::File in{name + ".in"};
    const util::File generate{name + ".test.py"};
    const util::File largeIn{name + ".large.in"};

    source.requireExists();
    prepare(source, language);

    // small test cases
    std::map<std::string, double> runtimes;
    in.requireExists();
    ref.requireExists();
    for (const auto &run : language.runs)
    {
        const auto command = prepareAndGetRunCommand(language, run, source) +
                             " < " + in.path + " > " + out.path;
        const auto result = measure(command);
        exitOnError(result, "Program exited with " +
                                std::to_string(result.returnValue));
        out.requireExists();
        if (!out.contentEquals(ref))
        {
            std::cout << "Test failed on run " << run.name << ":" << std::endl;
            diff(out, ref);
            exit(4);
        }
        runtimes[run.name] = result.runtime;
    }
    std::cout << "Small test case needed " << runtimes["perf"] << "s."
              << std::endl;

    // large generated test cases
    if (generate.exists())
    {
        if (!alreadyBuilt(largeIn, generate))
        {
            const std::string generateCommand =
                "python " + generate.path + " > " + largeIn.path;
            runProgramExitOnError(generateCommand,
                                  "Error during large test case creation.");
        }
        largeIn.requireExists();
        for (const auto &run : language.runs)
        {
            const auto runCommand =
                prepareAndGetRunCommand(language, run, source) + " < " +
                largeIn.path + " > /dev/null";
            const auto result = measure(runCommand);
            exitOnError(result, "Program exited with " +
                                    std::to_string(result.returnValue) +
                                    " on large run.");
            if (run.name == "perf")
            {
                std::cout << "Large test case needed " << result.runtime << "s."
                          << std::endl;
            }
        }
    }
}

int main(int argc, char *argv[])
{
    util::Config config{"simonkrogmann", "competitiontestrunner"};
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
    util::File info{name};
    info.requireExists();
    auto extension = util::stripWhitespace(info.content());
    if (languages.find(extension) == languages.end())
    {
        std::cout << "Language not supported" << std::endl;
        exit(5);
    }
    test(name, extension);
}
