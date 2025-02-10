//
// Created by DarsenOP on 2/9/25.
//

#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "../Core/Core.h"
#include <vector>
#include <memory>

class CommandProcessor {
    bool is_valid{true};
    size_t parsed_result_word_count{};
    std::shared_ptr<FileManagerException> exception{};
    std::vector<std::string> parsed_result{};
private:
    /* Parser
     * Parse:
     *      bool(const std::string&)    -> takes the input string as an input and returns false(true) if during parsing
     *                                     errors were thrown(not thrown)
     */
    bool Parse(const std::string& input);

    /* Helpers
     * Trim:
     *      void(std::string&)          -> Takes a string and deletes the tailing and ending spaces in it
     * NormalizeSpaces:
     *      void(std::string&)          -> Takes a string and all the duplicate spaces between words converts to a
     *                                     single space
     */
    static void Trim(std::string& input);
    static void NormalizeSpaces(std::string& input);
public:
    CommandProcessor() = default;

    /* Checkers
     * IsValid:
     *      bool() -> Returns true if no errors were handled during parsing and false if something went wrong
     */
    [[nodiscard]] inline bool IsValid() const { return is_valid; }

    /* Getters
     * GetResult:
     *      std::vector<std::string>()                  -> Returns the parsed result
     * GetException:
     *      std::shared_ptr<FileManagerException>()     -> Return an exception if errors occur during parsing
     * GetParsedResultWordCount:
     *      size_t()                                    -> Return the number of arguments in the parsed result
     */
    [[nodiscard]] inline std::vector<std::string> GetResult() const { return parsed_result; }
    [[nodiscard]] inline std::shared_ptr<FileManagerException> GetException() const { return exception; }
    [[nodiscard]] inline size_t GetParsedResultWordCount() const { return parsed_result_word_count; }

    /* Input Handling
     * ParseCommand:
     *      void(const std::string&)    -> Takes the input as a string and saves the parsed output (or error) into the
     *                                     corresponding member variables
     */
    void ParseCommand(const std::string& input);

    /* Delete move and copy constructors */
    CommandProcessor(const CommandProcessor&) = delete;
    CommandProcessor(CommandProcessor&&) = delete;
    CommandProcessor& operator=(const CommandProcessor&) = delete;
    CommandProcessor& operator=(CommandProcessor&&) = delete;
};


#endif //COMMANDPROCESSOR_H
