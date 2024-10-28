#ifndef TOKENIZER_HPP
#define TOKENIZER_HPP

#include <vector>
#include <regex>
#include <functional>
#include <string>
#include <map>

namespace Helium {

// Token types enumeration
enum class TokenType {
    HEADER,
    LIST,
    LINK,
    PARAGRAPH,
    TEXT,
    BOLD,
    ITALIC,
    STRIKETHROUGH,
    QUOTE,
    CODE,
    INLINECODE,
    UNKNOWN,
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
