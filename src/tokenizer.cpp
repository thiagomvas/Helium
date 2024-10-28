#include "tokenizer.hpp"
#include <vector>
#include <string>
#include <regex>
#include <sstream>

namespace Helium {

std::vector<Token> Tokenizer::tokenizeInline(const std::string& line) {
    std::vector<Token> tokens;
    std::string::const_iterator searchStart(line.cbegin());
    std::smatch match;
    std::regex boldRegex(R"(\*\*(.*?)\*\*)");
    std::regex italicRegex(R"(\*(.*?)\*)");
    std::regex strikethroughRegex(R"(~(.*?)~)");
    std::regex codeRegex(R"(`(.*?)`)");  

    while(searchStart != line.cend()) {
        // BOLD
        if(std::regex_search(searchStart, line.cend(), match, boldRegex)) {
            if(match.prefix().length() > 0) {
                tokens.push_back(Token(TokenType::TEXT,match.prefix().str()));
            }
            tokens.push_back(Token(TokenType::BOLD, match[1].str()));
            searchStart = match.suffix().first;
        } 
        // ITALIC
        else if (std::regex_search(searchStart, line.cend(), match, italicRegex)) {
            if(match.prefix().length() > 0 ) {
                tokens.push_back(Token(TokenType::TEXT, match.prefix().str()));
            }
            tokens.push_back(Token(TokenType::ITALIC, match[1].str()));
            searchStart = match.suffix().first;
        } 
        // STRIKETHROUGH
        else if (std::regex_search(searchStart, line.cend(), match, strikethroughRegex)) {
            if(match.prefix().length() > 0 ) {
                tokens.push_back(Token(TokenType::TEXT, match.prefix().str()));
            }
            tokens.push_back(Token(TokenType::STRIKETHROUGH, match[1].str()));
            searchStart = match.suffix().first;
        }
        // INLINE CODE 
        else if (std::regex_search(searchStart, line.cend(), match, codeRegex)) {
            if (match.prefix().length() > 0) {
                tokens.push_back(Token(TokenType::TEXT, match.prefix().str()));
            }
            tokens.push_back(Token(TokenType::INLINECODE, match[1].str()));
            searchStart = match.suffix().first;
        }
        
        else {
            tokens.push_back(Token(TokenType::TEXT, std::string(searchStart, line.cend())));
            break;
        }
    }

    return tokens;
}

std::vector<Token> Tokenizer::tokenize(const std::string& text) {
    std::vector<Token> tokens;
    std::istringstream stream(text);
    std::string line;

    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        Token token;
        if (token = tokenizeHeading(line); token.type != TokenType::UNKNOWN) {
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if (token = tokenizeList(line); token.type != TokenType::UNKNOWN) {
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if (token = tokenizeLink(line); token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        } else if (token = tokenizeQuote(line); token.type != TokenType::UNKNOWN) { 
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if(token = tokenizeCode(line); token.type != TokenType::UNKNOWN) { 
            tokens.push_back(token);
        } else {
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

Token Tokenizer::tokenizeQuote(const std::string& text) {
    if (text.rfind(">", 0) == 0) { // Check if line starts with '>'
        std::string quoteContent = text.substr(1); // Remove the '>'
        return Token(TokenType::QUOTE, quoteContent);
    }
    return Token();
}

Token Tokenizer::tokenizeCode(const std::string& text) {
    if (text.starts_with("`") && text.ends_with("`")) { // Check for inline code
        std::string codeContent = text.substr(1, text.size() - 2); // Remove backticks
        return Token(TokenType::CODE, codeContent);
    }
    return Token();
}
}// Constructor for Tokenizer
