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
    std::string::const_iterator searchStart = line.cbegin();
    std::smatch match;

    // Combined regex for bold, italic, strikethrough, and code
    std::regex inlineRegex(R"((\*\*(.*?)\*\*)|(\*(.*?)\*)|(~(.*?)~)|(`(#[A-Fa-f0-9]{6})`)|(`(.*?)`))");

    while (searchStart != line.cend()) {
        if (std::regex_search(searchStart, line.cend(), match, inlineRegex)) {
            // Add any preceding text as a TEXT token
            if (match.prefix().length() > 0) {
                tokens.push_back(Token(TokenType::TEXT, match.prefix().str()));
            }

            // Determine the type of token by examining which group matched
            if (match[1].matched) { // Bold
                tokens.push_back(Token(TokenType::BOLD, match[2].str()));
            } else if (match[3].matched) { // Italic
                tokens.push_back(Token(TokenType::ITALIC, match[4].str()));
            } else if (match[5].matched) { // Strikethrough
                tokens.push_back(Token(TokenType::STRIKETHROUGH, match[6].str()));
            } else if (match[7].matched) { 
                std::string hexColor = match[8].str(); 

                Token token(TokenType::COLORCHIP, hexColor);
                token.attributes[ATTRIBUTE_COLORCHIP_COLOR] = hexColor;
                tokens.push_back(token);
            } else if (match[9].matched) { // Inline Code
                tokens.push_back(Token(TokenType::INLINECODE, match[10].str()));
            }

            // Move search start position to the end of the match
            searchStart = match.suffix().first;
        } else {
            // No more inline formatting found, add remaining text as a TEXT token
            tokens.push_back(Token(TokenType::TEXT, std::string(searchStart, line.cend())));
            break;
        }
    }

    return tokens;
}


std::vector<Token> Tokenizer::tokenize(const std::string& text) {
    std::vector<Token> tokens;
    std::istringstream stream(text);
    std::regex listRegex(R"(^(\s*)([\-\*\+])\s+(.*)$)");
    std::regex unorderedListRegex(R"(^[\-\*\+]\s+(.*)$)");
    std::regex orderedListRegex(R"(^\d+\.\s+(.*)$)");
    std::smatch matches;
    std::string line;
    std::vector<std::string> wrappedLines;
    bool multiline = false;
    TokenType multilineType;
    Token token;

    while (std::getline(stream, line)) {
        if (line.empty()) {
            tokens.push_back(Token(TokenType::PARAGRAPH));
        }

        

        if(line.starts_with("> ")) {
            std::string content = line.substr(2);
            token.type = TokenType::QUOTE;
            if (!multiline) {
                multiline = true;
                multilineType = TokenType::QUOTE;
                token = Token(TokenType::QUOTE, "");

                // Check if the content starts with a specific type indicator
                if (content.starts_with("[!NOTE]")) {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_NOTE;
                    content = content.substr(7); // Remove "[!NOTE]"
                    Token headerParent = Token(TokenType::QUOTECHILD, "");
                    Token header = Token(TokenType::COLOREDTEXT, "Note");
                    header.attributes[ATTRIBUTE_COLOREDTEXT_COLOR] = Utils::ColorToHex(Helium::Configuration::getInstance().ColorTheme.QuoteNote);
                    headerParent.children.push_back(header);
                    token.children.push_back(headerParent);
                } else if (content.starts_with("[!TIP")) {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_TIP;
                    content = content.substr(6); // Remove "[!TIP]"
                    Token headerParent = Token(TokenType::QUOTECHILD, "");
                    Token header = Token(TokenType::COLOREDTEXT, "Tip");
                    header.attributes[ATTRIBUTE_COLOREDTEXT_COLOR] = Utils::ColorToHex(Helium::Configuration::getInstance().ColorTheme.QuoteTip);
                    headerParent.children.push_back(header);
                    token.children.push_back(headerParent);
                } else if (content.starts_with("[!IMPORTANT]")) {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_IMPORTANT;
                    content = content.substr(13); // Remove "[!IMPORTANT]"
                    Token headerParent = Token(TokenType::QUOTECHILD, "");
                    Token header = Token(TokenType::COLOREDTEXT, "Important");
                    header.attributes[ATTRIBUTE_COLOREDTEXT_COLOR] = Utils::ColorToHex(Helium::Configuration::getInstance().ColorTheme.QuoteImportant);
                    headerParent.children.push_back(header);
                    token.children.push_back(headerParent);
                } else if (content.starts_with("[!WARNING]")) {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_WARNING;
                    content = content.substr(10); // Remove "[!WARNING]"
                    Token headerParent = Token(TokenType::QUOTECHILD, "");
                    Token header = Token(TokenType::COLOREDTEXT, "Warning");
                    header.attributes[ATTRIBUTE_COLOREDTEXT_COLOR] = Utils::ColorToHex(Helium::Configuration::getInstance().ColorTheme.QuoteWarning);
                    headerParent.children.push_back(header);
                    token.children.push_back(headerParent);
                } else if (content.starts_with("[!CAUTION]")) {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_CAUTION;
                    content = content.substr(10); // Remove "[!CRITICAL]"
                    Token headerParent = Token(TokenType::QUOTECHILD, "");
                    Token header = Token(TokenType::COLOREDTEXT, "Caution");
                    header.attributes[ATTRIBUTE_COLOREDTEXT_COLOR] = Utils::ColorToHex(Helium::Configuration::getInstance().ColorTheme.QuoteCaution);
                    headerParent.children.push_back(header);
                    token.children.push_back(headerParent);
                } else {
                    token.attributes[ATTRIBUTE_QUOTE_TYPE] = ATTRIBUTE_QUOTE_TYPE_DEFAULT; // Optional: Set a default type if none match
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

        if (line == "---" || line == "***" || line == "___") {
            tokens.push_back(Token(TokenType::HORIZONTALLINE));
            continue;
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
            
            int headerFontSize = Helium::Configuration::getInstance().Formatting.GetFontSizeForHeader(std::stoi(token.attributes.at(ATTRIBUTE_HEADER_LEVEL)));
            std::vector<std::string> wrapped = Utils::WrapText(token.value, 
                                                                Configuration::getInstance().Formatting.DefaultFont, 
                                                                headerFontSize,
                                                                Configuration::getInstance().Formatting.CharSpacing,
                                                                Configuration::getInstance().MaxNoteWidth);
            for(std::string line : wrapped) {
                Helium::Token t(token.type, line, tokenizeInline(line));
                t.attributes[ATTRIBUTE_HEADER_LEVEL] = token.attributes.at(ATTRIBUTE_HEADER_LEVEL);
                tokens.push_back(t);
            }
        } else if (line.starts_with("TODO: ")) {
            token.type = TokenType::TODO;
            token.value = line.substr(6);
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else if (token = tokenizeLink(line); token.type != TokenType::UNKNOWN) {
            tokens.push_back(token);
        } else if(token = tokenizeCode(line); token.type != TokenType::UNKNOWN) { 
            tokens.push_back(token);
        } else if (token = tokenizeList(line); token.type != TokenType::UNKNOWN) {
            token.children = tokenizeInline(token.value);
            tokens.push_back(token);
        } else {
            token.type = TokenType::PARAGRAPH;
            token.value = line;
            std::vector<std::string> wrapped = Utils::WrapText(token.value, 
                                                                Configuration::getInstance().Formatting.DefaultFont, 
                                                                Configuration::getInstance().Formatting.Paragraph,
                                                                Configuration::getInstance().Formatting.CharSpacing,
                                                                Configuration::getInstance().MaxNoteWidth);
            for(std::string line : wrapped) {
                Helium::Token t(token.type, line, tokenizeInline(line));
                tokens.push_back(t);
            }
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
        token.attributes[ATTRIBUTE_HEADER_LEVEL] = std::to_string(level);
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
        token.attributes[ATTRIBUTE_HYPERLINK_URL] = linkUrl;
        return token;
    }

    return Token(TokenType::UNKNOWN, "");
}

Token Tokenizer::tokenizeList(const std::string& line) {
    std::regex unorderedListRegex(R"(^[\-\*\+]\s+(.*)$)");
    std::regex orderedListRegex(R"(^\d+\.\s+(.*)$)");
    std::smatch matches;
    Token token(TokenType::UNKNOWN, "");

    if (std::regex_match(line, matches, unorderedListRegex)) {
        token.type = TokenType::LIST;
        token.value = matches[1].str();
        token.attributes[ATTRIBUTE_LIST_ORDERED] = "false";
        return token;
    }

    if (std::regex_match(line, matches, orderedListRegex)) {
        token.type = TokenType::LIST;
        token.value = matches[1].str();
        token.attributes[ATTRIBUTE_LIST_ORDERED] = "true";
        return token;
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
