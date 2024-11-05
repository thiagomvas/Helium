#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <vector>
#include <regex>
#include <functional>
#include <string>
#include <map>

namespace Helium {

const std::string ATTRIBUTE_QUOTE_TYPE = "type";
const std::string ATTRIBUTE_QUOTE_TYPE_DEFAULT = "default";
const std::string ATTRIBUTE_QUOTE_TYPE_NOTE = "note";
const std::string ATTRIBUTE_QUOTE_TYPE_TIP = "tip";
const std::string ATTRIBUTE_QUOTE_TYPE_IMPORTANT = "important";
const std::string ATTRIBUTE_QUOTE_TYPE_WARNING = "warning";
const std::string ATTRIBUTE_QUOTE_TYPE_CAUTION = "caution";

const std::string ATTRIBUTE_HEADER_LEVEL = "level";

const std::string ATTRIBUTE_HYPERLINK_URL = "url";

const std::string ATTRIBUTE_LIST_ORDERED = "ordered";

const std::string ATTRIBUTE_COLORCHIP_COLOR = "hex";

const std::string ATTRIBUTE_COLOREDTEXT_COLOR = "colorhex";
// Token types enumeration
enum class TokenType {
    HEADER,
    LIST,
    WRAPPEDLIST,
    LINK,
    PARAGRAPH,
    TEXT,
    BOLD,
    ITALIC,
    STRIKETHROUGH,
    QUOTE,
    QUOTECHILD,
    CODE,
    INLINECODE,
    HORIZONTALLINE,
    TODO,
    COLORCHIP,
    COLOREDTEXT,
    UNKNOWN,
};

struct TokenPattern {
    TokenType type;
    std::regex regex;
    bool isMultiline;
};
// Token struct

struct Token {
    TokenType type;                                 // Type of the token
    std::string value;                              // The actual content of the token
    std::map<std::string, std::string> attributes;  // Additional attributes (optional)
    std::vector<Token> children;                    // Nested tokens (inline tokens) if supported

    // Default constructor
    Token() : type(TokenType::UNKNOWN), value("") {}

    // Parameterized constructor
    Token(TokenType t) : type(std::move(t)), value("") {}
    Token(TokenType t, std::string v) : type(std::move(t)), value(std::move(v)) {}

    // Constructor with attributes
    Token(TokenType t, std::string v, std::map<std::string, std::string> attrs) 
        : type(std::move(t)), value(std::move(v)), attributes(std::move(attrs)) {}

    // Constructor for nested tokens
    Token(TokenType t, std::string v, std::vector<Token> childTokens) 
        : type(std::move(t)), value(std::move(v)), children(std::move(childTokens)) {}
};

class Tokenizer {
public:
    std::vector<Token> tokenize(const std::string& text);
    std::vector<Token> tokenizeInline(const std::string& line);

private:
    Token tokenizeHeading(const std::string& line);
    Token tokenizeBold(const std::string& text);
	Token tokenizeLink(const std::string& text);
	Token tokenizeList(const std::string& line);
    Token tokenizeQuote(const std::string& text);
    Token tokenizeCode(const std::string& text);
    // Add more helper functions for other Markdown elements as needed
};
// Tokenizer class
} // namespace Helium

#endif // TOKENIZER_HPP
