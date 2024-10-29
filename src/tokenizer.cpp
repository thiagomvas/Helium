#include "tokenizer.hpp"
#include <vector>
#include <string>
#include <regex>
#include <sstream>
#include <unordered_map>
#include "utils.hpp"

namespace Helium {
bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

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
    bool multiline = false;
    TokenType multilineType;
    Token token;

    while (std::getline(stream, line)) {
        if (line.empty()) {
            continue;
        }

        if(line.starts_with("> ")) {
            std::string content = line.substr(2);
            if (!multiline) {
                multiline = true;
                multilineType = TokenType::QUOTE;
                token = Token(TokenType::QUOTE, "");

                // Check if the content starts with a specific type indicator
                if (content.starts_with("[!NOTE]")) {
                    token.attributes["type"] = "note";
                    content = content.substr(7); // Remove "[!NOTE]"
                } else if (content.starts_with("[!TIP]")) {
                    token.attributes["type"] = "tip";
                    content = content.substr(6); // Remove "[!TIP]"
                } else if (content.starts_with("[!IMPORTANT]")) {
                    token.attributes["type"] = "important";
                    content = content.substr(13); // Remove "[!IMPORTANT]"
                } else if (content.starts_with("[!WARNING]")) {
                    token.attributes["type"] = "warning";
                    content = content.substr(10); // Remove "[!WARNING]"
                } else if (content.starts_with("[!CAUTION]")) {
                    token.attributes["type"] = "caution";
                    content = content.substr(10); // Remove "[!CRITICAL]"
                } else {
                    token.attributes["type"] = "default"; // Optional: Set a default type if none match
                }

                // Trim leading whitespace from the content
                content.erase(content.begin(), std::find_if(content.begin(), content.end(), [](unsigned char ch) {
                    return !std::isspace(ch);
                }));
            }
            std::vector<std::string> wrapped = Utils::WrapText(content, 
                                                                Configuration::getInstance().Formatting.CodeFont, 
                                                                Configuration::getInstance().Formatting.Paragraph,
                                                                Configuration::getInstance().Formatting.CharSpacing,
                                                                Configuration::getInstance().MaxNoteWidth);
            for(const std::string& wline : wrapped) {
                token.children.push_back(Token(TokenType::QUOTECHILD, wline, tokenizeInline(wline)));
            }

            if(line.starts_with("> ")) {
                continue;
            } else {
                tokens.push_back(token);
                multiline = false;
                multilineType = TokenType::UNKNOWN;
            }
        } else if(multilineType == TokenType::QUOTE) {
                tokens.push_back(token);
                multiline = false;
                multilineType = TokenType::UNKNOWN;
        }
        // MULTILINE CODE BLOCKS
        if(line.starts_with("```") || (multiline && multilineType == TokenType::CODE)) {
            if(!multiline) {
                multiline = true;
                multilineType = TokenType::CODE;
                token = Token(TokenType::CODE, "");
            }


            std::vector<std::string> wrapped = Utils::WrapText(line, 
                                                                Configuration::getInstance().Formatting.CodeFont, 
                                                                Configuration::getInstance().Formatting.Paragraph,
                                                                Configuration::getInstance().Formatting.CharSpacing,
                                                                Configuration::getInstance().MaxNoteWidth);
            for(std::string wline : wrapped) {
                wline.erase(std::remove(wline.begin(), wline.end(), '`'), wline.end());
                token.children.push_back(Token(TokenType::INLINECODE, wline));
            }
            if(line.ends_with("```")) {
                multiline = false;
                multilineType = TokenType::UNKNOWN;
                tokens.push_back(token);
            }
        } 
        
        else if (token = tokenizeHeading(line); token.type != TokenType::UNKNOWN) {
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if (token = tokenizeList(line); token.type != TokenType::UNKNOWN) {
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if (token = tokenizeLink(line); token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        } else if(token = tokenizeCode(line); token.type != TokenType::UNKNOWN) { 
            tokens.push_back(token);
        } else {
            tokens.emplace_back(TokenType::PARAGRAPH, line);
        }
    }

    if(multiline) {
        multiline = false;
        multilineType = TokenType::UNKNOWN;
        tokens.push_back(token);
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
