
// Generated from Asl.g4 by ANTLR 4.7.1

#pragma once


#include "antlr4-runtime.h"
#include "AslParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by AslParser.
 */
class  AslListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(AslParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(AslParser::ProgramContext *ctx) = 0;

  virtual void enterFunction(AslParser::FunctionContext *ctx) = 0;
  virtual void exitFunction(AslParser::FunctionContext *ctx) = 0;

  virtual void enterParams(AslParser::ParamsContext *ctx) = 0;
  virtual void exitParams(AslParser::ParamsContext *ctx) = 0;

  virtual void enterDeclarations(AslParser::DeclarationsContext *ctx) = 0;
  virtual void exitDeclarations(AslParser::DeclarationsContext *ctx) = 0;

  virtual void enterVariable_decl(AslParser::Variable_declContext *ctx) = 0;
  virtual void exitVariable_decl(AslParser::Variable_declContext *ctx) = 0;

  virtual void enterType(AslParser::TypeContext *ctx) = 0;
  virtual void exitType(AslParser::TypeContext *ctx) = 0;

  virtual void enterBasictype(AslParser::BasictypeContext *ctx) = 0;
  virtual void exitBasictype(AslParser::BasictypeContext *ctx) = 0;

  virtual void enterArray(AslParser::ArrayContext *ctx) = 0;
  virtual void exitArray(AslParser::ArrayContext *ctx) = 0;

  virtual void enterStatements(AslParser::StatementsContext *ctx) = 0;
  virtual void exitStatements(AslParser::StatementsContext *ctx) = 0;

  virtual void enterProcedure(AslParser::ProcedureContext *ctx) = 0;
  virtual void exitProcedure(AslParser::ProcedureContext *ctx) = 0;

  virtual void enterAssignStmt(AslParser::AssignStmtContext *ctx) = 0;
  virtual void exitAssignStmt(AslParser::AssignStmtContext *ctx) = 0;

  virtual void enterIfStmt(AslParser::IfStmtContext *ctx) = 0;
  virtual void exitIfStmt(AslParser::IfStmtContext *ctx) = 0;

  virtual void enterWhileLoop(AslParser::WhileLoopContext *ctx) = 0;
  virtual void exitWhileLoop(AslParser::WhileLoopContext *ctx) = 0;

  virtual void enterProcCall(AslParser::ProcCallContext *ctx) = 0;
  virtual void exitProcCall(AslParser::ProcCallContext *ctx) = 0;

  virtual void enterReadStmt(AslParser::ReadStmtContext *ctx) = 0;
  virtual void exitReadStmt(AslParser::ReadStmtContext *ctx) = 0;

  virtual void enterWriteExpr(AslParser::WriteExprContext *ctx) = 0;
  virtual void exitWriteExpr(AslParser::WriteExprContext *ctx) = 0;

  virtual void enterWriteString(AslParser::WriteStringContext *ctx) = 0;
  virtual void exitWriteString(AslParser::WriteStringContext *ctx) = 0;

  virtual void enterRet(AslParser::RetContext *ctx) = 0;
  virtual void exitRet(AslParser::RetContext *ctx) = 0;

  virtual void enterLeft_expr(AslParser::Left_exprContext *ctx) = 0;
  virtual void exitLeft_expr(AslParser::Left_exprContext *ctx) = 0;

  virtual void enterBoolean(AslParser::BooleanContext *ctx) = 0;
  virtual void exitBoolean(AslParser::BooleanContext *ctx) = 0;

  virtual void enterExprIdent(AslParser::ExprIdentContext *ctx) = 0;
  virtual void exitExprIdent(AslParser::ExprIdentContext *ctx) = 0;

  virtual void enterArithmetic(AslParser::ArithmeticContext *ctx) = 0;
  virtual void exitArithmetic(AslParser::ArithmeticContext *ctx) = 0;

  virtual void enterExprProcedure(AslParser::ExprProcedureContext *ctx) = 0;
  virtual void exitExprProcedure(AslParser::ExprProcedureContext *ctx) = 0;

  virtual void enterRelational(AslParser::RelationalContext *ctx) = 0;
  virtual void exitRelational(AslParser::RelationalContext *ctx) = 0;

  virtual void enterUnary(AslParser::UnaryContext *ctx) = 0;
  virtual void exitUnary(AslParser::UnaryContext *ctx) = 0;

  virtual void enterArrayAccess(AslParser::ArrayAccessContext *ctx) = 0;
  virtual void exitArrayAccess(AslParser::ArrayAccessContext *ctx) = 0;

  virtual void enterParenthesis(AslParser::ParenthesisContext *ctx) = 0;
  virtual void exitParenthesis(AslParser::ParenthesisContext *ctx) = 0;

  virtual void enterValue(AslParser::ValueContext *ctx) = 0;
  virtual void exitValue(AslParser::ValueContext *ctx) = 0;

  virtual void enterIdent(AslParser::IdentContext *ctx) = 0;
  virtual void exitIdent(AslParser::IdentContext *ctx) = 0;


};

