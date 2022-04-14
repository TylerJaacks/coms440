#include <map>
#include <vector>

#include "token.h"

class parser {
public:
    typedef enum {
        LEFT_TO_RIGHT,
        RIGHT_TO_LEFT
    } associativity_t;

    typedef enum {
        UNDEFINED = 0,
        VOID = 1,
        INTEGER = 2,
        CHAR = 3,
        DOUBLE = 4,
        STRING = 5
    } type_t;

    typedef struct {
        type_t type;
        std::string name;
    } function_parameter_t;

    typedef struct {
        std::string name;
        type_t return_type;
        std::vector<function_parameter_t> parameters;
    } function_t;

    typedef struct {
        type_t type;
        std::string name;
        std::string value;
    } variable_t;

public:
    std::vector<token> tokens;
    std::map<std::string, int> precedenceMap;
    std::map<std::string, associativity_t> associativityMap;

    std::map<std::string, type_t> global_variables;
    std::map<std::string, type_t> local_variables;

    std::vector<function_t> functions;

    int currentTokenIndex;

    bool global = true;

public:
    parser(std::vector<token> tokens) {
        this->tokens = tokens;
        currentTokenIndex = -1;

        // A → A α | β
        // A → β A'
        // A' → α A' | ϵ

        // E –> E + T | T
        // T –> T * F | F
        // F –> ( E ) | id

        // E –> T E’
        // E’ –> + T E’ | e
        // T –> F T’
        // T’ –> * F T’ | e
        // F –> ( E ) | id

        // Expr         := Expr '+' MultiExpr | MultiExpr
        // MultiExpr    := MultiExpr '*' Literal

        // Expr         := MultiExpr Expr2 | MultiExpr
        // Expr2        :=  '+' MultiExpr Expr2 | e

        // MultiExpr    := Literal MultiExpr2 | Literal
        // MultiExpr2   := '*' Literal MultiExpr2 | e


        // MultiExpr        := Expr '*' Expr | IntegerLiteral
        // IntegerLiteral   := 0-9*

        this->precedenceMap["("] = 12;
        this->precedenceMap[")"] = 12;
        this->precedenceMap["["] = 12;
        this->precedenceMap["]"] = 12;
        this->precedenceMap["."] = 12;

        this->precedenceMap["!"] = 11;
        this->precedenceMap["~"] = 11;
        this->precedenceMap["-"] = 11; // - (Unary)
        this->precedenceMap["--"] = 11;

        // Type
        this->precedenceMap["Type"] = 11;

        this->precedenceMap["*"] = 10;
        this->precedenceMap["/"] = 10;
        this->precedenceMap["%"] = 10;

        this->precedenceMap["+"] = 9;
        this->precedenceMap["-"] = 9;

        this->precedenceMap["<"] = 8;
        this->precedenceMap["<="] = 8;
        this->precedenceMap[">"] = 8;
        this->precedenceMap[">="] = 8;

        this->precedenceMap["=="] = 7;
        this->precedenceMap["!="] = 7;

        this->precedenceMap["&"] = 6;

        this->precedenceMap["|"] = 5;

        this->precedenceMap["&&"] = 4;

        this->precedenceMap["||"] = 3;

        // ?:
        this->precedenceMap["?"] = 2;
        this->precedenceMap[":"] = 2;

        this->precedenceMap["+="] = 1;
        this->precedenceMap["-="] = 1;
        this->precedenceMap["*="] = 1;
        this->precedenceMap["/="] = 1;
        this->precedenceMap["%="] = 1;

        this->precedenceMap[","] = 0;



        this->associativityMap["("] = LEFT_TO_RIGHT;
        this->associativityMap[")"] = LEFT_TO_RIGHT;
        this->associativityMap["["] = LEFT_TO_RIGHT;
        this->associativityMap["]"] = LEFT_TO_RIGHT;
        this->associativityMap["."] = LEFT_TO_RIGHT;

        this->associativityMap["!"] = RIGHT_TO_LEFT;
        this->associativityMap["~"] = RIGHT_TO_LEFT;
        this->associativityMap["-"] = RIGHT_TO_LEFT; // - (Unary)
        this->associativityMap["--"] = RIGHT_TO_LEFT;

        // Type
        this->associativityMap["Type"] = LEFT_TO_RIGHT;

        this->associativityMap["*"] = LEFT_TO_RIGHT;
        this->associativityMap["/"] = LEFT_TO_RIGHT;
        this->associativityMap["%"] = LEFT_TO_RIGHT;

        this->associativityMap["+"] = LEFT_TO_RIGHT;
        this->associativityMap["-"] = LEFT_TO_RIGHT;

        this->associativityMap["<"] = LEFT_TO_RIGHT;
        this->associativityMap["<="] = LEFT_TO_RIGHT;
        this->associativityMap[">"] = LEFT_TO_RIGHT;
        this->associativityMap[">="] = LEFT_TO_RIGHT;

        this->associativityMap["=="] = LEFT_TO_RIGHT;
        this->associativityMap["!="] = LEFT_TO_RIGHT;

        this->associativityMap["&"] = LEFT_TO_RIGHT;

        this->associativityMap["|"] = LEFT_TO_RIGHT;

        this->associativityMap["&&"] = LEFT_TO_RIGHT;

        this->associativityMap["||"] = LEFT_TO_RIGHT;

        // ?:
        this->associativityMap["?"] = RIGHT_TO_LEFT;
        this->associativityMap[":"] = RIGHT_TO_LEFT;

        this->associativityMap["+="] = RIGHT_TO_LEFT;
        this->associativityMap["-="] = RIGHT_TO_LEFT;
        this->associativityMap["*="] = RIGHT_TO_LEFT;
        this->associativityMap["/="] = RIGHT_TO_LEFT;
        this->associativityMap["%="] = RIGHT_TO_LEFT;

        this->associativityMap[","] = LEFT_TO_RIGHT;
    }

private:
    static void error(const std::string& expectedToken, const std::string& tokenValue, const std::string& fileName, int lineNumber);

    static void unexpected_token_error(const std::string& tokenValue, const std::string& fileName, int lineNumber);

    /**
     * Gets the next token without consuming the current token.
     * @return the next token.
     */
    token peak_next_token();

    /**
     * Gets the next n token.
     * @param n the offset.
     * @return the nth token.
     */
    token peak(int n);

    /**
     * Checks if the next token_type is the same as the excepted.
     * @param type the expected token_type.
     * @return if the type matches.
     */
    bool match(token_type type);

    /**
     * Gets the current token and advance the current token index.
     * @return the current token.
     */
    token consume();

public:
    void Program();

    void Decl();

    void DeclList();

    void VarDecl();

    void VarDeclList(type_t type);

    void VarDeclId(type_t type);

    void FuncDecl();

    void FuncDeclParamList(std::vector<function_parameter_t> *function_);

    void Block();

    void Stmt();

    void StmtList();

    type_t Expr();

    type_t Type();

    std::string Id();

    type_t ComputeExpression(int precedence);

    type_t ComputeTerm();

    token rollback();

    void FuncCallParamList(std::string name, std::vector<type_t> *types);
};