#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include "MidIPS.hpp"
#include "BigEdian.hpp"
#include "Hunk.hpp"

//! @brief Computes the size of an array within the scope.
#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x)[0])

//! @brief Computes the max length of a char* buffer.
#define MAX_BUFFER_LENGTH 500

//! @brief Base header for every IPS patch, translates literally to "PATCH".
const u8 gMagicHeader[] = {0x50, 0x41, 0x54, 0x43, 0x48};

//! @brief Global size of the header.
const size_t gMagicHeaderLength = ARRAY_COUNT(gMagicHeader);

/**
 * @param bytes1
 * @param bytes2
 *
 * @brief Compares two u8 arrays.
 *
 * @returns If they're exactly the same or not.
 *
 * @warning Does *NOT* work like strcmp.
 *
 * @todo Maybe use a template instead ? Not exactly
 * useful though.
 */
static bool areBytesEqual(const u8 *bytes1, const u8 *bytes2, const size_t &length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (bytes1[i] != bytes2[i])
            return false;
    }

    return true;
}

/**
 * @param argc
 * @param argv
 *
 * @brief Parses arguments given as an array of C Strings
 * into a C++ vector of std::strings.
 *
 * @todo Reserve space within the vector equal to argc.
 */
static std::vector<std::string> *parseArgs(int argc, char **argv)
{
    std::vector<std::string> *retVal = new std::vector<std::string>();

    if (argc == 0)
        return retVal;

    for (int i = 0; i < argc; i++)
        retVal->push_back(argv[i]);

    return retVal;
}

/**
 * @param args
 * @param prefix
 *
 * @brief Gets an argument (defined by prefix)'s parameter.
 *
 * @returns The parameter's value or an empty string if it
 * doesn't exist.
 *
 * @example `-Iinclude` and `-I include` will both return include.
 */
static std::string getArg(const std::vector<std::string> *args, const std::string &prefix, bool isEmpty = false)
{
    for (size_t i = 0, max = args->size(); i < max; i++)
    {
        const std::string &current = args->at(i);
        const size_t prefixLength = prefix.length();

        //! It does not hold the prefix at all.
        if (current.find(prefix) != 0)
            continue;
        // This argument does NOT expect a paremeter,
        // so we just return it.
        if (isEmpty && current == prefix)
            return prefix;
        // It's wide enough to hold the parameter with it.
        if (current.length() > prefixLength + 1 && current[prefixLength] == '=')
            return current.substr(prefixLength + 1);
        // It wasn't wide enough, so we assume it's the next
        // argument.
        if (i + 1 < max)
            return args->at(i + 1);
    }

    return {""};
}

static void logHunk(const Hunk &hunk, std::ofstream &maybeOut)
{
    char logBuffer[MAX_BUFFER_LENGTH] = {0};

    if (hunk.bytes() == nullptr)
        return;

    std::snprintf(logBuffer, MAX_BUFFER_LENGTH, "Offset: %X\tSize: %lX\n", hunk.offset(), hunk.bytes()->size());

    if (maybeOut.is_open())
    {
        maybeOut.write(logBuffer, std::strlen(logBuffer));
        maybeOut.flush();
    }
    else
    {
        std::printf("%s", logBuffer);
    }
}

/**
 * @param args
 *
 * @brief Creates an IPS patch out of the given args.
 *
 * @details Expects a source, a target and an output file.
 * It will then parse the differences one by one and write
 * them into the IPS file.
 */
static int createIPSPatch(const std::vector<std::string> *args)
{
    const std::string sourceFileName = getArg(args, "-c");
    const std::string targetFileName = getArg(args, "-t");
    const std::string outputFileName = getArg(args, "-o");
    const std::string logFileName = getArg(args, "-l");
    const bool allowAboveU24 = getArg(args, "--allow-above-u24", true) == "--alow-above-u24";
    std::ofstream logFile = std::ofstream(logFileName);

    // If there were missing parameters.
    if (sourceFileName.empty())
        FATAL_ERROR("Empty -c argument provided.");
    if (targetFileName.empty())
        FATAL_ERROR("Empty -t argument provided.");
    if (outputFileName.empty())
        FATAL_ERROR("Empty -o argument provided.");

    // BigEdian handles opening files and errors regarding those.
    BigEdian sourceFile = {sourceFileName, std::ios::in | std::ios::out | std::ios::binary};
    BigEdian targetFile = {targetFileName, std::ios::in | std::ios::out | std::ios::binary};
    BigEdian outputFile = {outputFileName, std::ios::out | std::ios::binary};

    // Writing the standard IPS header, whether or not there are changes.
    outputFile.writeBytes(gMagicHeader, gMagicHeaderLength);

    // Looping until we reach the end of one of the files.
    while (!sourceFile.isEnd() && !targetFile.isEnd())
    {
        Hunk diffHunk = Hunk::fromDiff(&sourceFile, &targetFile);
        diffHunk.asIPS(&outputFile, allowAboveU24);

        logHunk(diffHunk, logFile);
    }

    // Making sure the changes are actually written.
    outputFile.flush();
    logFile.close();
    return 0;
}

/**
 * @param args
 *
 * @brief Applies an IPS patch on a file.
 *
 * @details Expects an IPS file and a 'subject'
 * file. It will first check for the header, and then
 * try to apply each section of the patch.
 */
static int applyIPSPatch(const std::vector<std::string> *args)
{
    const std::string IPSFileName = getArg(args, "-p");
    const std::string fileToApplyOnFileName = getArg(args, "-a");
    const std::string logFileName = getArg(args, "-l");
    const bool allowAboveU24 = getArg(args, "--allow-above-u24", true) == "--allow-above-u24";
    std::ofstream logFile = std::ofstream(logFileName);

    // Missing parameters.
    if (IPSFileName.empty())
        FATAL_ERROR("Empty -p argument provided.");
    if (fileToApplyOnFileName.empty())
        FATAL_ERROR("Empty -a argument provided.");

    BigEdian IPSFile = {IPSFileName, std::ios::in | std::ios::out | std::ios::binary};
    BigEdian fileToApplyOn = {fileToApplyOnFileName, std::ios::in | std::ios::out | std::ios::binary};

    if (!areBytesEqual(IPSFile.readBytes(gMagicHeaderLength), gMagicHeader, gMagicHeaderLength))
        FATAL_ERROR("The passed file is not a valid IPS Patch.");

    // Looping until we reach the end of the IPS file, the inner
    // processes also check for the end and exit the program if they
    // unexpectedly encounter it.
    while (!IPSFile.isEnd())
    {
        Hunk toApply = Hunk::fromIPS(&IPSFile);
        toApply.write(&fileToApplyOn, allowAboveU24);

        logHunk(toApply, logFile);
    }

    logFile.close();
    return 0;
}

/**
 * @brief Prints the usage "manual" of
 * this program.
 *
 * @todo Explain the arguments and their
 * parameter more thoroughly.
 */
static int printUsage()
{
    std::printf("Usage: midips -m=[apply|a]|[create|c] [-p=PATCH] [-a=FILE] [-c=SOURCE] [-t=TARGET] [-o=OUTPUT PATCH]\n");
    return 0;
}

/**
 * @brief Entry point.
 */
int main(int argc, char **argv)
{
    const std::vector<std::string> *args = parseArgs(--argc, ++argv);
    const std::string modeArg = getArg(args, "-m");

    // Only valid modes are apply/a and create/c
    if (modeArg == "apply" || modeArg == "a")
        return applyIPSPatch(args);
    if (modeArg == "create" || modeArg == "c")
        return createIPSPatch(args);

    return printUsage();
}