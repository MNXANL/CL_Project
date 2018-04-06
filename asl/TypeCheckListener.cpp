//////////////////////////////////////////////////////////////////////
//
//    TypeCheckListener - Walk the parser tree to do the semantic
//                        typecheck for the Asl programming language
//
//    Copyright (C) 2018  Universitat Politecnica de Catalunya
//
//    This library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU General Public License
//    as published by the Free Software Foundation; either version 3
//    of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public
//    License along with this library; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
//    contact: José Miguel Rivero (rivero@cs.upc.edu)
//             Computer Science Department
//             Universitat Politecnica de Catalunya
//             despatx Omega.110 - Campus Nord UPC
//             08034 Barcelona.  SPAIN
//
//////////////////////////////////////////////////////////////////////

#include "TypeCheckListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/SemErrors.h"

#include <iostream>
#include <string>

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
TypeCheckListener::TypeCheckListener(TypesMgr       & Types,
				     SymTable       & Symbols,
				     TreeDecoration & Decorations,
				     SemErrors      & Errors) :
  Types{Types},
  Symbols {Symbols},
  Decorations{Decorations},
  Errors{Errors} {
}

void TypeCheckListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void TypeCheckListener::exitProgram(AslParser::ProgramContext *ctx) {
  if (Symbols.noMainProperlyDeclared())
    Errors.noMainProperlyDeclared(ctx);
  Symbols.popScope();
  Errors.print();
  DEBUG_EXIT();
}

void TypeCheckListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  //Symbols.print();
}
void TypeCheckListener::exitFunction(AslParser::FunctionContext *ctx) {
  Symbols.popScope();
  DEBUG_EXIT();
}

void TypeCheckListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitType(AslParser::TypeContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitStatements(AslParser::StatementsContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr());
  
  //std::cout << "LINE "<< ctx->ASSIGN()->getSymbol()->getLine() << ": "; Types.dump(t1); std::cout << ' '; Types.dump(t2); std::cout << std::endl; 
  
  if ((not Types.isErrorTy(t1)) and ((not Types.isErrorTy(t2)) and (not Types.copyableTypes(t1, t2)))){
      Errors.incompatibleAssignment(ctx->ASSIGN());
      //std::cout << "LINE "<< ctx->ASSIGN()->getSymbol()->getLine() << ": " << t1 << ' ' << t2 << std::endl;
  }
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr()))){
    Errors.nonReferenceableLeftExpr(ctx->left_expr());
  }
  DEBUG_EXIT();
}

void TypeCheckListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}
void TypeCheckListener::enterWhileLoop(AslParser::WhileLoopContext * ctx) {
  DEBUG_ENTER();
}

void TypeCheckListener::exitWhileLoop(AslParser::WhileLoopContext * ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1)))
    Errors.booleanRequired(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterProcedure(AslParser::ProcedureContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitProcedure(AslParser::ProcedureContext *ctx) {
  //std::cout << "CHIVATO" << std::endl;
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
  //std::cout << "Chivato2: " << Types.to_string(t1) << std::endl;
  
  if (not Types.isErrorTy(t1) and not Types.isFunctionTy(t1)) {
    Errors.isNotCallable(ctx->ident());
  } else if (not Types.isErrorTy(t1)){
	  unsigned int n=Types.getNumOfParameters(t1);
	  std::vector<AslParser::ExprContext *> expressions = ctx->expr();
	  if(n!=expressions.size()) Errors.numberOfParameters(ctx);
	  else {
		for (unsigned int i=0; i<n; i++) {
			if(not Types.copyableTypes(Types.getParameterType(t1,i), getTypeDecor(expressions[i])) and not Types.isErrorTy(getTypeDecor(expressions[i]))) 
				Errors.incompatibleParameter(expressions[i], i+1, ctx);
		}
	  }
	  //
	  putTypeDecor(ctx, Types.getFuncReturnType(t1));
	  //std::cout << "CHIVATO END else" << std::endl;
  } else {
          putTypeDecor(ctx,t1);
  }
  //std::cout << "CHIVATO END" << std::endl;
  DEBUG_EXIT();
}


void TypeCheckListener::enterExprProcedure(AslParser::ExprProcedureContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitExprProcedure(AslParser::ExprProcedureContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->procedure());
  if (Types.isVoidTy(t1) and not Types.isErrorTy(t1)) {
	Errors.isNotFunction(ctx);
  } else {
  putTypeDecor(ctx, t1);
  }
  DEBUG_EXIT();
}


void TypeCheckListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->left_expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)) and
      (not Types.isFunctionTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  if ((not Types.isErrorTy(t1)) and (not getIsLValueDecor(ctx->left_expr())))
    Errors.nonReferenceableExpression(ctx);
  DEBUG_EXIT();
}

void TypeCheckListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr());
  if ((not Types.isErrorTy(t1)) and (not Types.isPrimitiveTy(t1)))
    Errors.readWriteRequireBasic(ctx);
  DEBUG_EXIT();
}
void TypeCheckListener::enterRet(AslParser::RetContext * ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitRet(AslParser::RetContext * ctx) {
  TypesMgr::TypeId t1;
  if (not ctx->expr()) {
     t1 = Types.createVoidTy();
  } else {
     t1 = getTypeDecor(ctx->expr());
  }
  //get function type
  TypesMgr::TypeId t2 = Types.getFuncReturnType(Symbols.getCurrentFunctionTy());
  if (not Types.isErrorTy(t1) and not Types.isErrorTy(t2) and not Types.copyableTypes (t2,t1)) {
     if (ctx->expr()) Errors.incompatibleReturn(ctx->expr());
     else Errors.incompatibleReturn(ctx);
  }
  DEBUG_EXIT();
}

void TypeCheckListener::enterWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_EXIT();
}

void TypeCheckListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
  if (ctx->expr()) {
	  bool good = true;
	  if (Types.isErrorTy(t1) or Types.isErrorTy(getTypeDecor(ctx->expr()))) putTypeDecor(ctx, Types.createErrorTy());
          else{
		  if (not Types.isArrayTy(t1)) {
			Errors.nonArrayInArrayAccess(ctx);
			good = false;
		  }
		  if (not Types.isIntegerTy(getTypeDecor(ctx->expr())) ) {
		        Errors.nonIntegerIndexInArrayAccess(ctx->expr());
			good = false;
		  } 
		  if (good) {
			putTypeDecor(ctx, Types.getArrayElemType(t1) );
		  	bool b = getIsLValueDecor(ctx->ident());
		  	putIsLValueDecor(ctx, b);
		  }
	  }
  } else {
	  //control of assignment to arrays ?
	  putTypeDecor(ctx, t1);
	  bool b = getIsLValueDecor(ctx->ident());
	  putIsLValueDecor(ctx, b);
  }
  DEBUG_EXIT();
}
/*
bool         isArrayTy        (TypeId tid) const;
  unsigned int getArraySize     (TypeId tid) const;
  TypeId       getArrayElemType (TypeId tid) const;

*/
void TypeCheckListener::enterArrayAccess(AslParser::ArrayAccessContext * ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitArrayAccess(AslParser::ArrayAccessContext * ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
  bool good = true;
  //std::cout << Types.to_string(getTypeDecor(ctx->expr())) << std::endl;
  if (Types.isErrorTy(t1) or Types.isErrorTy(getTypeDecor(ctx->expr()))) {
	putTypeDecor(ctx, Types.createErrorTy());
	//std::cout << "CHIVATO3" << std::endl;
  } else {
	  if (not Types.isArrayTy(t1)) {
		Errors.nonArrayInArrayAccess(ctx);
		good = false;
	  }
	  if (not Types.isIntegerTy(getTypeDecor(ctx->expr())) ) {
		Errors.nonIntegerIndexInArrayAccess(ctx->expr());
		good = false;
	  } 
	  if (good) {
		putTypeDecor(ctx, Types.getArrayElemType(t1) );
	  }
  }
  DEBUG_EXIT();
}

void TypeCheckListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  if (((not Types.isErrorTy(t1)) and (not Types.isNumericTy(t1))) or
      ((not Types.isErrorTy(t2)) and (not Types.isNumericTy(t2)))) {
	Errors.incompatibleOperator(ctx->op);
	putTypeDecor(ctx, Types.createErrorTy());
	//putIsLValueDecor(ctx, false);
  } else {
	  TypesMgr::TypeId t;
	  if(Types.isIntegerTy(t1) and Types.isIntegerTy(t2)) t = Types.createIntegerTy();
	  else t = Types.createFloatTy();
	  if (Types.isErrorTy(t1) or Types.isErrorTy(t2)) t = Types.createErrorTy();
	  //NEW BEHAVIOUR: WILL RECAST TO FLOAT IF NOT TWO INTS!
	  putTypeDecor(ctx, t);
	  putIsLValueDecor(ctx, false);
  }
  DEBUG_EXIT();
}

void TypeCheckListener::enterBoolean(AslParser::BooleanContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitBoolean(AslParser::BooleanContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  if (((not Types.isErrorTy(t1)) and (not Types.isBooleanTy(t1))) or
      ((not Types.isErrorTy(t2)) and (not Types.isBooleanTy(t2))))
    Errors.incompatibleOperator(ctx->op);
  TypesMgr::TypeId t = Types.createBooleanTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterParenthesis(AslParser::ParenthesisContext * ctx){
  DEBUG_ENTER();
}
void TypeCheckListener::exitParenthesis(AslParser::ParenthesisContext * ctx){
  TypesMgr::TypeId t = getTypeDecor(ctx->expr());
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
}

void TypeCheckListener::enterUnary(AslParser::UnaryContext * ctx){
  DEBUG_ENTER();
}
void TypeCheckListener::exitUnary(AslParser::UnaryContext * ctx){
  TypesMgr::TypeId t = getTypeDecor(ctx->expr());
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
}

void TypeCheckListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitRelational(AslParser::RelationalContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  std::string oper = ctx->op->getText();
  if ((not Types.isErrorTy(t1)) and (not Types.isErrorTy(t2)) and
      (not Types.comparableTypes(t1, t2, oper)))
    Errors.incompatibleOperator(ctx->op);
  TypesMgr::TypeId t = Types.createBooleanTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitValue(AslParser::ValueContext *ctx) {
  TypesMgr::TypeId t;
  if (ctx->INTVAL()) t = Types.createIntegerTy();
  else if (ctx->CHARVAL()) t = Types.createCharacterTy();
  else if (ctx->BOOLVAL()) t = Types.createBooleanTy();
  else if (ctx->FLOATVAL()) t = Types.createFloatTy();
  putTypeDecor(ctx, t);
  putIsLValueDecor(ctx, false);
  DEBUG_EXIT();
}

void TypeCheckListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitExprIdent(AslParser::ExprIdentContext *ctx) {
  TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
  putTypeDecor(ctx, t1);
  bool b = getIsLValueDecor(ctx->ident());
  putIsLValueDecor(ctx, b);
  DEBUG_EXIT();
}

void TypeCheckListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void TypeCheckListener::exitIdent(AslParser::IdentContext *ctx) {
  std::string ident = ctx->getText();
  if (Symbols.findInStack(ident) == -1) {
    Errors.undeclaredIdent(ctx->ID());
    TypesMgr::TypeId te = Types.createErrorTy();
    putTypeDecor(ctx, te);
    putIsLValueDecor(ctx, true);
  }
  else {
    TypesMgr::TypeId t1 = Symbols.getType(ident);
    putTypeDecor(ctx, t1);
    if (Symbols.isFunctionClass(ident))
      putIsLValueDecor(ctx, false);
    else
      putIsLValueDecor(ctx, true);
  }
  DEBUG_EXIT();
}



// void TypeCheckListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void TypeCheckListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void TypeCheckListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG("visitTerminal");
// }
// void TypeCheckListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type ans IsLValue
SymTable::ScopeId TypeCheckListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId TypeCheckListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
bool TypeCheckListener::getIsLValueDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getIsLValue(ctx);
}

// Setters for the necessary tree node attributes:
//   Scope, Type ans IsLValue
void TypeCheckListener::putScopeDecor(antlr4::ParserRuleContext *ctx, SymTable::ScopeId s) {
  Decorations.putScope(ctx, s);
}
void TypeCheckListener::putTypeDecor(antlr4::ParserRuleContext *ctx, TypesMgr::TypeId t) {
  Decorations.putType(ctx, t);
}
void TypeCheckListener::putIsLValueDecor(antlr4::ParserRuleContext *ctx, bool b) {
  Decorations.putIsLValue(ctx, b);
}
