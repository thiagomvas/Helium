#include "tokenizer.hpp"
#include <vector>
#include <string>
#include <regex>
#include <sstream>

namespace Helium {



std::vector<Token> Tokenizer::tokenize(const std::string& text) {
    std::vector<Token> tokens;
    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        // Tokenize headings
        Token headingToken = tokenizeHeading(line);
        if (headingToken.type != TokenType::UNKNOWN) {
            tokens.push_back(headingToken);
            continue;
        }

        // Tokenize lists
        Token listToken = tokenizeList(line);
        if (listToken.type != TokenType::UNKNOWN) {
            tokens.push_back(listToken);
            continue;
        }

        // Tokenize links
        Token linkToken = tokenizeLink(line);
        if (linkToken.type != TokenType::UNKNOWN) {
            tokens.push_back(linkToken);
            continue;
        }

        // Tokenize other elements like bold
        Token boldToken = tokenizeBold(line);
        if (boldToken.type != TokenType::UNKNOWN) {
            tokens.push_back(boldToken);
            continue;
        }

        // Handle unrecognized lines as paragraphs
        if (!line.empty()) {
            tokens.emplace_back(TokenType::PARAGRAPH, line);
        }
    }

    return tokens;
}


Token Tokenizer::tokenizeHeading(const std::string& line) {
    std::regex headingRegex(R"(^(\#{1,6})\s*(.*)$)");
    std::smatch matches;

    if (std::regex_match(line, matches, headingRegex)) {
        int level = matches[1].str().length();
        std::string content = matches[2].str();
        Token token(TokenType::HEADER, content);
        token.attributes["level"] = std::to_string(level);
        return token;
    }

    return Token(TokenType::UNKNOWN, "");
}


Token Tokenizer::tokenizeBold(const std::string& text) {
    std::regex boldRegex(R"(\*\*(.*?)\*\*)");
    std::smatch matches;

    if (std::regex_search(text, matches, boldRegex)) {
        return Token(TokenType::PARAGRAPH, matches[1].str());
    }

    return Token(TokenType::UNKNOWN, "");
}

Token Tokenizer::tokenizeLink(const std::string& text) {
    std::regex linkRegex(R"(\[(.*?)\]\((.*?)\))");
    std::smatch matches;

    if (std::regex_search(text, matches, linkRegex)) {
        std::string linkText = matches[1].str();
        std::string linkUrl = matches[2].str();
        Token token(TokenType::LINK, linkText);
        token.attributes["url"] = linkUrl;
        return token;
    }

    return Token(TokenType::UNKNOWN, "");
}

Token Tokenizer::tokenizeList(const std::string& line) {
    std::regex unorderedListRegex(R"(^[\-\*\+]\s+(.*)$)");
    std::regex orderedListRegex(R"(^\d+\.\s+(.*)$)");
    std::smatch matches;

    if (std::regex_match(line, matches, unorderedListRegex)) {
        return Token(TokenType::LIST, matches[1].str());
    }

    if (std::regex_match(line, matches, orderedListRegex)) {
        return Token(TokenType::LIST, matches[1].str());
    }

    return Token(TokenType::UNKNOWN, "");
}
}// Constructor for Tokenizer
