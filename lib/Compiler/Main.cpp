#include <fmt/format.h>
#include <ghc/filesystem.hpp>
#include <ya_getopt.h>

#ifdef _WIN32

#include <Windows.h>

#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif

#endif

#include <string>
#include <system_error>

#include <type_traits>
#include <array>

#include "fosl/parser/Lexer.h"
#include "fosl/parser/Parser.h"
#include "fosl/Error.h"
#include "fosl/Timing.h"

#include "Dump.h"

void *operator new(std::size_t size) {
    fmt::print(stderr, "\033[33;1mallocation\033[0m\n");
    return malloc(size);
}

void displayUsage(const std::string &programName) {
    fmt::print(stderr, "usage: {} [options...] inputFiles\n\n", programName);
    const char *info =
        "Options:\n"
        "    -h, --help               display this message and quit.\n"
        "    -c, --compile            compile, but don't link.\n"
        "    -o, --out    <filename>  set the output file name.\n"
        "    -t, --triple <triple>    set the target triple.\n"
        "    -l, --link   <linkable>  link an external library in the output executable.\n"
    ;

    fmt::print(stderr, "{}", info);
}

std::size_t getNthSubstr(std::size_t n, const std::string_view &s,
               const std::string_view &p,
               bool repeats = false) {
   std::size_t i = s.find(p);
   std::size_t adv = (repeats) ? 1 : p.length();

   std::size_t j;
   for (j = 1; j < n && i != -1; ++j)
      i = s.find(p, i+adv);

   if (j == n)
     return(i);
   else
     return(-1);
}

void colorizeTerminal() {
    #ifdef _WIN32

    {
        DWORD dwMode;
        GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &dwMode);
        SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }

    #endif
}

int main(int argc, char **argv) {
    colorizeTerminal();

    std::string programName = *argv;
    std::string outputFile;
    std::string targetTriple;

    std::vector<std::string> links;

    bool compileOnly = false;

    bool emitLlvm = false;
    bool emitObj = true;
    bool emitAssembly = false;

    std::array<option, 9> longopts {{
        { "help", ya_no_argument, nullptr, 'h' },
        { "compile", ya_no_argument, nullptr, 'c' },
        { "out", ya_required_argument, nullptr, 'o' },
        { "triple", ya_required_argument, nullptr, 't' },
        { "link", ya_required_argument, nullptr, 'l' },
        { "emit-llvm", ya_no_argument, nullptr, 301 },
        { "emit-obj", ya_no_argument, nullptr, 302 },
        { "emit-asm", ya_no_argument, nullptr, 303 },
        { nullptr, 0, nullptr, 0 }
    }};

    int optv, longopt;
    while ((optv = ya_getopt_long_only(argc, argv, "h?co:t:l:", longopts.data(), &longopt)) != -1) {
        switch (optv) {
            case '?':
            case 'h': {
                displayUsage(programName);
                return -1;
            }

            case 'c': {
                compileOnly = true;
                break;
            }

            case 'o': {
                outputFile = optarg;
                break;
            }

            case 't': {
                targetTriple = optarg;
                break;
            }

            case 'l': {
                links.emplace_back(optarg);
                break;
            }
        }
    }

    argv += optind;
    argc -= optind;

    if (!argc) {
        fmt::print(stderr, "{}: \033[31;1merror: \033[0mno input files.\n", programName);
        return -1;
    }

    std::vector<std::string> inputFiles;

    for (int i = 0; i < argc; i++) {
        inputFiles.emplace_back(argv[i]);
    }

    if (!ghc::filesystem::exists(inputFiles[0])) {
        fmt::print(stderr, "{}: \033[31;1merror: \033[0mno such file or directory: `{}'.\n", programName, inputFiles[0]);
        return -1;
    }

    if (!ghc::filesystem::is_regular_file(inputFiles[0])) {
        fmt::print(stderr, "{}: \033[31;1merror: \033[0m`{}' is not a source file.\n", programName, inputFiles[0]);
        return -1;
    }

    std::vector<std::shared_ptr<fosl::parser::ASTNode>> nodes;
    auto parser = std::make_shared<fosl::parser::Parser>(nodes);
    parser->initFromFile(inputFiles[0]);

    try {
        std::shared_ptr<fosl::parser::ASTNode> expr = parser->parseExpr();
        fosl::compiler::dumpNode(expr);
        std::putchar('\n');
    } catch (fosl::Error &e) {
        const char *color = "";

        switch (e.getPriority()) {
            case fosl::Error::Priority::Info: {
                color = "\033[34;1m";
                break;
            }

            case fosl::Error::Priority::Warning: {
                color = "\033[35;1m";
                break;
            }

            case fosl::Error::Priority::Error:
            case fosl::Error::Priority::Fatal: {
                color = "\033[31;1m";
                break;
            }
        }

        const char *type = "";

        if (e.getPriority() == fosl::Error::Priority::Error) {
            switch (e.getType()) {
                case fosl::Error::Type::Lexical: {
                    type = "lex ";
                    break;
                }

                case fosl::Error::Type::Syntactical: {
                    type = "syntax ";
                    break;
                }

                case fosl::Error::Type::Semantic: {
                    type = "semantic ";
                    break;
                }

                case fosl::Error::Type::Intermediate: {
                    type = "interm ";
                    break;
                }
            }
        }

        const char *priority = "";

        switch (e.getPriority()) {
            case fosl::Error::Priority::Info: {
                priority = "info";
                break;
            }

            case fosl::Error::Priority::Warning: {
                priority = "warning";
                break;
            }

            case fosl::Error::Priority::Error: {
                priority = "error";
                break;
            }

            case fosl::Error::Priority::Fatal: {
                priority = "fatal error";
                break;
            }
        }

        fmt::print(stderr, "{}:{}:{}: {}{}{}: \033[0m{}\n\n", e.getModuleName(), e.getLine(), e.getLexpos(), color, type, priority, e.getMessage());
        std::size_t errorLineIndex = getNthSubstr(e.getLine() - 1,  parser->getLexer()->getSource(), "\n");
        if (errorLineIndex == -1) {
            fmt::print(stderr, "\033[35;1m(failed to acquire source)\033[0m");
        } else {
            ++errorLineIndex;
            std::size_t length = 0;

            while (errorLineIndex + length < parser->getLexer()->getSource().size() && parser->getLexer()->getSource()[errorLineIndex + length] != '\n') {
                ++length;
            }

            fmt::print(stderr, "\t\t{:.{}}\n\t\t\033[32;1m", &parser->getLexer()->getSource()[errorLineIndex], length);

            std::size_t i = 0;

            for (; i < e.getLexpos() - 1; i++) {
                std::fputc(' ', stderr);
            }

            std::fputc('^', stderr);
            ++i;

            while (i++ < length) {
                std::fputc(' ', stderr);
            }

            fmt::print(stderr, "\033[0m");
        }
    } catch (std::exception &e) {
        fmt::print(stderr, "{}\n", e.what());
    }

    return 0;
}
