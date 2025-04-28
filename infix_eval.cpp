#include <iostream>
#include <string>
#include <sstream>
#include <cctype>
#include <stdexcept>
#include <map>
#include <cmath>

class Token {
public:
    enum Type {
        Number, Plus, Minus, Multiply, Divide,
        LeftParen, RightParen, Assign, Identifier,
        Comma, Function, End
    };

    Type type;
    double value;
    std::string name;

    Token(Type type) : type(type), value(0) {}
    Token(Type type, double value) : type(type), value(value) {}
    Token(Type type, const std::string& name) : type(type), value(0), name(name) {}
};

class Lexer {
private:
    std::string input;
    size_t pos;
public:
    Lexer(const std::string& input) : input(input), pos(0) {}

    Token getNextToken() {
        while (pos < input.length() && isspace(input[pos])) {
            pos++;
        }
        if (pos >= input.length()) {
            return Token(Token::End);
        }
        char current = input[pos];
        if (isdigit(current) || current == '.') {
            size_t start_pos = pos;
            while (pos < input.length() && (isdigit(input[pos]) || input[pos] == '.')) {
                pos++;
            }
            double value = std::stod(input.substr(start_pos, pos - start_pos));
            return Token(Token::Number, value);
        } else if (isalpha(current)) {
            size_t start_pos = pos;
            while (pos < input.length() && (isalnum(input[pos]) || input[pos] == '_')) {
                pos++;
            }
            std::string name = input.substr(start_pos, pos - start_pos);
            if (name == "sin" || name == "cos" || name == "tan" ||
                name == "log" || name == "sqrt") {
                return Token(Token::Function, name);
            } else {
                return Token(Token::Identifier, name);
            }
        } else if (current == '+') {
            pos++;
            return Token(Token::Plus);
        } else if (current == '-') {
            pos++;
            return Token(Token::Minus);
        } else if (current == '*') {
            pos++;
            return Token(Token::Multiply);
        } else if (current == '/') {
            pos++;
            return Token(Token::Divide);
        } else if (current == '(') {
            pos++;
            return Token(Token::LeftParen);
        } else if (current == ')') {
            pos++;
            return Token(Token::RightParen);
        } else if (current == '=') {
            pos++;
            return Token(Token::Assign);
        } else if (current == ',') {
            pos++;
            return Token(Token::Comma);
        } else {
            throw std::runtime_error(std::string("Unknown character: ") + current);
        }
    }
};

class Parser {
private:
    Lexer lexer;
    Token currentToken;
    std::map<std::string, double> variables;

    void eat(Token::Type type) {
        if (currentToken.type == type) {
            currentToken = lexer.getNextToken();
        } else {
            throw std::runtime_error("Unexpected token");
        }
    }

    double factor() {
        Token token = currentToken;
        if (token.type == Token::Plus) {
            eat(Token::Plus);
            return factor();
        } else if (token.type == Token::Minus) {
            eat(Token::Minus);
            return -factor();
        } else if (token.type == Token::Number) {
            eat(Token::Number);
            return token.value;
        } else if (token.type == Token::Identifier) {
            std::string name = token.name;
            eat(Token::Identifier);
            if (currentToken.type == Token::LeftParen) {
                throw std::runtime_error("Unknown function: " + name);
            } else {
                if (variables.find(name) != variables.end()) {
                    return variables[name];
                } else {
                    throw std::runtime_error("Unknown variable: " + name);
                }
            }
        } else if (token.type == Token::Function) {
            std::string funcName = token.name;
            eat(Token::Function);
            eat(Token::LeftParen);
            double arg = expression();
            eat(Token::RightParen);
            return applyFunction(funcName, arg);
        } else if (token.type == Token::LeftParen) {
            eat(Token::LeftParen);
            double result = expression();
            eat(Token::RightParen);
            return result;
        } else {
            throw std::runtime_error("Invalid syntax");
        }
    }

    double term() {
        double result = factor();
        while (currentToken.type == Token::Multiply || currentToken.type == Token::Divide) {
            Token token = currentToken;
            if (token.type == Token::Multiply) {
                eat(Token::Multiply);
                result *= factor();
            } else if (token.type == Token::Divide) {
                eat(Token::Divide);
                double divisor = factor();
                if (divisor == 0) {
                    throw std::runtime_error("Division by zero");
                }
                result /= divisor;
            }
        }
        return result;
    }

    double expression() {
        double result = term();
        while (currentToken.type == Token::Plus || currentToken.type == Token::Minus) {
            Token token = currentToken;
            if (token.type == Token::Plus) {
                eat(Token::Plus);
                result += term();
            } else if (token.type == Token::Minus) {
                eat(Token::Minus);
                result -= term();
            }
        }
        return result;
    }

    double assignment() {
        if (currentToken.type == Token::Identifier) {
            std::string varName = currentToken.name;
            eat(Token::Identifier);
            if (currentToken.type == Token::Assign) {
                eat(Token::Assign);
                double value = expression();
                variables[varName] = value;
                return value;
            } else {
                lexer = Lexer(varName + remainingInput());
                currentToken = lexer.getNextToken();
                return expression();
            }
        } else {
            return expression();
        }
    }

    std::string remainingInput() {
        std::string rest = "";
        while (currentToken.type != Token::End) {
            if (currentToken.type == Token::Number) {
                rest += std::to_string(currentToken.value);
            } else if (currentToken.type == Token::Identifier) {
                rest += currentToken.name;
            } else {
                rest += tokenToString(currentToken.type);
            }
            currentToken = lexer.getNextToken();
        }
        return rest;
    }

    std::string tokenToString(Token::Type type) {
        switch (type) {
            case Token::Plus: return "+";
            case Token::Minus: return "-";
            case Token::Multiply: return "*";
            case Token::Divide: return "/";
            case Token::LeftParen: return "(";
            case Token::RightParen: return ")";
            case Token::Assign: return "=";
            case Token::Comma: return ",";
            default: return "";
        }
    }

    double applyFunction(const std::string& funcName, double arg) {
        if (funcName == "sin") {
            return sin(arg);
        } else if (funcName == "cos") {
            return cos(arg);
        } else if (funcName == "tan") {
            return tan(arg);
        } else if (funcName == "log") {
            if (arg <= 0) {
                throw std::runtime_error("Math error: log of non-positive number");
            }
            return log(arg);
        } else if (funcName == "sqrt") {
            if (arg < 0) {
                throw std::runtime_error("Math error: sqrt of negative number");
            }
            return sqrt(arg);
        } else {
            throw std::runtime_error("Unknown function: " + funcName);
        }
    }

public:
    Parser(const std::string& input) : lexer(input), currentToken(lexer.getNextToken()) {
    }


    double parse() {
        double result = assignment();
        if (currentToken.type != Token::End) {
            throw std::runtime_error("Unexpected characters at end of input");
        }
        return result;
    }
};

int main() {
    std::string input;
    std::cout << "Enter expression (type 'exit' to quit):\n";
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "exit") {
            break;
        }
        try {
            Parser parser(input);
            double result = parser.parse();
            std::cout << result << "\n";
        } catch (std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }
    return 0;
}
