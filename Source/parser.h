#include <map>
#include <vector>

#include "token.h"

class parser {
public:
    std::vector<token> tokens;
    std::map<std::string, int> precedenceMap;

    int currentTokenIndex;

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
    }

private:
    void error(std::string expectedToken, std::string tokenValue, std::string fileName, int lineNumber);

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

    void VarDeclList();

    void VarDeclId();

    void FuncDecl();

    void FuncDeclParams();

    void FuncDeclParamList();

    void FuncDeclParam();

    void Block();

    void Stmt();

    void StmtList();

    void Expr();

    void LValue();

    void ExprList();

    bool UnaryOp();

    bool BinaryOp();

    bool AssignOp();

    void Type();

    void Id();

    void Constant();

    void IntegerConstant();

    void DoubleConstant();

    void CharConstant();

    void StringConstant();

    void GroupingExpr();
};