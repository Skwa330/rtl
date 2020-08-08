#include <fmt/format.h>
#include <filesystem>
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

#include "lang/Parser/Lexer.h"
#include "lang/Parser/Parser.h"
#include "lang/Parser/Error.h"

#include "Dump.h"

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

std::size_t getNthSubstr(std::size_t n, const std::string_view &str, const std::string_view &substr) {
    std::size_t c = 0;

    std::size_t pos = 0, last = 0;

    while (c < n && pos != -1) {
        pos = str.find(substr);

        if (pos != -1) {
            last = pos + 1;
        }

        ++c;
    }

    return last;
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

void formatError(const lang::parser::Error &e, const std::string_view &source) {
    const char *type;

    if (e.getType() == lang::parser::Error::Type::Lexical) {
        type = "lex";
    } else if (e.getType() == lang::parser::Error::Type::Syntactical) {
        type = "syntax";
    }

    fmt::print(stderr, "{}: {} \033[31;1merror: \033[0m{}\n\n", e.getSourceLocation().getFormatted(), type, e.getMessage());
    std::size_t errorLineIndex = getNthSubstr(e.getSourceLocation().line - 1,  source, "\n");
    if (errorLineIndex == -1) {
        fmt::print(stderr, "\t\t\033[35;1m(failed to acquire source)\033[0m");
    } else {
        std::size_t length = 0;

        while (errorLineIndex + length < source.size() && source[errorLineIndex + length] != '\n') {
            ++length;
        }

        fmt::print(stderr, "\t\t{:.{}}\n\t\t\033[32;1m", &source[errorLineIndex], length);

        std::size_t i = 1;

        for (; i < e.getSourceLocation().lexpos; i++) {
            std::fputc(' ', stderr);
        }

        std::fputc('^', stderr);
        ++i;

        while (i++ < length) {
            std::fputc(' ', stderr);
        }

        fmt::print(stderr, "\033[0m");
    }
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

    if (!std::filesystem::exists(inputFiles[0])) {
        fmt::print(stderr, "{}: \033[31;1merror: \033[0mno such file or directory: `{}'.\n", programName, inputFiles[0]);
        return -1;
    }

    if (!std::filesystem::is_regular_file(inputFiles[0])) {
        fmt::print(stderr, "{}: \033[31;1merror: \033[0m`{}' is not a source file.\n", programName, inputFiles[0]);
        return -1;
    }

    std::vector<std::shared_ptr<lang::parser::ASTNode>> nodes;
    auto parser = std::make_shared<lang::parser::Parser>(nodes);
    parser->initFromFile(inputFiles[0]);

    parser->getLexer()->sourceLocation.moduleName = "Lolz";

    try {
        std::shared_ptr<lang::parser::ASTNode> expr = parser->parseExpr();
        lang::compiler::dumpNode(expr);
        std::putchar('\n');
    } catch (const lang::parser::Error &e) {
        formatError(e, parser->getLexer()->source);
    } catch (const std::exception &e) {
        fmt::print(stderr, "{}\n", e.what());
    }

    parser->getLexer()->sourceLocation.moduleName = "Lolz2";

    try {
        std::shared_ptr<lang::parser::ASTNode> expr = parser->parseExpr();
        lang::compiler::dumpNode(expr);
        std::putchar('\n');
    } catch (const lang::parser::Error &e) {
        formatError(e, parser->getLexer()->source);
    } catch (const std::exception &e) {
        fmt::print(stderr, "{}\n", e.what());
    }

    return 0;
}
