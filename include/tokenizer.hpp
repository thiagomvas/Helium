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
    UNKNOWN,
};

// Token struct

struct Token {
    TokenType type;                // Type of the token (e.g., "heading", "bold", "list", etc.)
    std::string value;               // The actual content of the token
    std::map<std::string, std::string> attributes;  // Additional attributes (optional)
    // Default constructor
    Token() : type(TokenType::UNKNOWN), value("") {}

    // Parameterized constructor
    Token(TokenType t, std::string v) : type(std::move(t)), value(std::move(v)) {}

    // Constructor with attributes
    Token(TokenType t, std::string v, std::map<std::string, std::string> attrs) 
        : type(std::move(t)), value(std::move(v)), attributes(std::move(attrs)) {}
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
    // Add more helper functions for other Markdown elements as needed
};
// Tokenizer class
} // namespace Helium

#endif // TOKENIZER_HPP
