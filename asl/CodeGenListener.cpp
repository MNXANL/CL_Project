//////////////////////////////////////////////////////////////////////
//
//    CodeGenListener - Walk the parser tree to do
//                             the generation of code
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

#include "CodeGenListener.h"

#include "antlr4-runtime.h"

#include "../common/TypesMgr.h"
#include "../common/SymTable.h"
#include "../common/TreeDecoration.h"
#include "../common/code.h"

#include <cstddef>    // std::size_t

// uncomment the following line to enable debugging messages with DEBUG*
// #define DEBUG_BUILD
#include "../common/debug.h"

// using namespace std;


// Constructor
CodeGenListener::CodeGenListener(TypesMgr       & Types,
				 SymTable       & Symbols,
				 TreeDecoration & Decorations,
				 code           & Code) :
  Types{Types},
  Symbols{Symbols},
  Decorations{Decorations},
  Code{Code} {
}

void CodeGenListener::enterProgram(AslParser::ProgramContext *ctx) {
  DEBUG_ENTER();
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
}
void CodeGenListener::exitProgram(AslParser::ProgramContext *ctx) {
  Symbols.popScope();
  DEBUG_EXIT();
}

void CodeGenListener::enterFunction(AslParser::FunctionContext *ctx) {
  DEBUG_ENTER();
  subroutine subr(ctx->ID()->getText()); //NEED TO REVISIT
  if (ctx->basictype()) {
    subr.add_param("_result");
  }
  if (ctx->params()){
    for (auto iCtx : ctx->params()->ID()) {
      subr.add_param(iCtx->getText());
    }
  }
  Code.add_subroutine(subr);
  SymTable::ScopeId sc = getScopeDecor(ctx);
  Symbols.pushThisScope(sc);
  codeCounters.reset();
}
void CodeGenListener::exitFunction(AslParser::FunctionContext *ctx) {
  subroutine & subrRef = Code.get_last_subroutine();
  instructionList code = getCodeDecor(ctx->statements());
  if (ctx->expr()) {
      instructionList codeExpr = getCodeDecor(ctx->expr());
      std::string temp = getAddrDecor(ctx->expr());
      code = code || codeExpr || instruction::LOAD("_result", temp);
  }
  code = code || instruction::RETURN();
  subrRef.set_instructions(code);
  Symbols.popScope(); //TODO: other returns inside same function (= more than 1 return) seems unneeded
  DEBUG_EXIT();
}

void CodeGenListener::enterDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitDeclarations(AslParser::DeclarationsContext *ctx) {
  DEBUG_EXIT();
}

void CodeGenListener::enterVariable_decl(AslParser::Variable_declContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitVariable_decl(AslParser::Variable_declContext *ctx) {
  subroutine       & subrRef = Code.get_last_subroutine();
  DEBUG(subrRef.dump()); 
  TypesMgr::TypeId        t1 = getTypeDecor(ctx->type()); 
  DEBUG(Types.to_string(t1));
  std::size_t           size = Types.getSizeOfType(t1);
  DEBUG(1);
  for (auto IdChild : ctx -> ID()) {
    DEBUG(2);
    DEBUG(IdChild -> getText());
    DEBUG(size);
    subrRef.add_var(IdChild -> getText(), size); 

    DEBUG(3);
  }
  DEBUG(1);
  DEBUG_EXIT();
}

void CodeGenListener::enterType(AslParser::TypeContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitType(AslParser::TypeContext *ctx) {

  DEBUG_EXIT();
}

void CodeGenListener::enterStatements(AslParser::StatementsContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitStatements(AslParser::StatementsContext *ctx) {
  instructionList code;
  for (auto stCtx : ctx->statement()) {
    code = code || getCodeDecor(stCtx);
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterAssignStmt(AslParser::AssignStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitAssignStmt(AslParser::AssignStmtContext *ctx) {
  instructionList  code;
  std::string     addr1 = getAddrDecor(ctx->left_expr());
  std::string     offs1 = getOffsetDecor(ctx->left_expr());
  instructionList code1 = getCodeDecor(ctx->left_expr());
  // TypesMgr::TypeId tid1 = getTypeDecor(ctx->left_expr());
  std::string     addr2 = getAddrDecor(ctx->expr());
  // std::string     offs2 = getOffsetDecor(ctx->expr());
  instructionList code2 = getCodeDecor(ctx->expr());
  // TypesMgr::TypeId tid2 = getTypeDecor(ctx->expr());

  if (offs1 != "") {
    if (not Symbols.isParameterClass(ctx->left_expr()->ident()->ID()->getText())) {
        //std::cerr << "CHIVATO 1 " << ctx->left_expr()->ident()->ID()->getText() << " -> "  << std::endl;
        code = code1 || code2 || instruction::XLOAD(addr1,offs1,  addr2);
    }
    else {
        //std::cerr << "CHIVATO 2 " << ctx->left_expr()->ident()->ID()->getText() << " -> " <<  std::endl;
        std::string tmp = "%" + codeCounters.newTEMP();
        code = code1 || code2 || instruction::LOAD(tmp,  addr1) || instruction::XLOAD(tmp,offs1,  addr2);
    }
  }
  else code = code1 || code2 || instruction::LOAD(addr1, addr2);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterIfStmt(AslParser::IfStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIfStmt(AslParser::IfStmtContext *ctx) {
  instructionList   code;
  std::string      addr1 = getAddrDecor(ctx->expr());
  instructionList  code1 = getCodeDecor(ctx->expr());
  instructionList  code2 = getCodeDecor(ctx->statements(0));
  //Add code for else
  std::string      label = codeCounters.newLabelIF();
  std::string labelEndIf = "endif"+label;
   if (ctx->statements().size() == 1) {
      code = code1 
	    || instruction::FJUMP(addr1, labelEndIf) 
	    || code2 
        || instruction::LABEL(labelEndIf);
   }
   else {
        instructionList  code3 = getCodeDecor(ctx->statements(1));
        std::string labelThen = "then"+label;// codeCounters.newLabelIF();
        code = code1 
	    || instruction::FJUMP(addr1, labelThen) 
	    || code2 
	    || instruction::UJUMP(labelEndIf)
        || instruction::LABEL(labelThen)
        || code3
        || instruction::LABEL(labelEndIf);
    }
    
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}


void CodeGenListener::enterWhileLoop(AslParser::WhileLoopContext *ctx) {
    DEBUG_ENTER();
}

void CodeGenListener::exitWhileLoop(AslParser::WhileLoopContext *ctx) {
  std::string      label = codeCounters.newLabelWHILE();
  std::string      labelDo = "do" + label;
  std::string      labelWhile = "while" + label;
  instructionList code = instruction::UJUMP(labelWhile) || instruction::LABEL(labelDo);
    
  code = code || getCodeDecor(ctx->statements());
  
  code = code || instruction::LABEL(labelWhile) || getCodeDecor(ctx->expr());
  std::string addrExpr = getAddrDecor(ctx->expr());
  std::string tmp = "%" + codeCounters.newTEMP();
  
  code = code || instruction::NOT(tmp, addrExpr) || instruction::FJUMP(tmp, labelDo);
  
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}


void CodeGenListener::enterProcCall(AslParser::ProcCallContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitProcCall(AslParser::ProcCallContext *ctx) {
  instructionList code = getCodeDecor(ctx->procedure());
  // std::string name = ctx->ident()->ID()->getSymbol()->getText();
  //std::string name = ctx->procedure()->ident()->getText();
  ///code = instruction::CALL(name);

  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterReadStmt(AslParser::ReadStmtContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitReadStmt(AslParser::ReadStmtContext *ctx) {
  instructionList  code;
  std::string     addr1 = getAddrDecor(ctx->left_expr()); //TODO: HERE IS THE CHANGE FOR TEST 6! read to arrays!
  
  std::string     offs1 = getOffsetDecor(ctx->left_expr());
  instructionList code1 = getCodeDecor(ctx->left_expr());
  TypesMgr::TypeId tid1 = getTypeDecor(ctx->left_expr());
  bool isArray = false;
  std::string tmp;
  if (ctx->left_expr()->expr()){
      isArray = true;
      tmp = addr1;
      addr1 = "%"+codeCounters.newTEMP();
  }
  if (Types.isIntegerTy(tid1)) {
      code = code1 || instruction::READI(addr1);
  }
  if (Types.isCharacterTy(tid1)) {
      code = code1 || instruction::READC(addr1);
  }
  if (Types.isFloatTy(tid1)) {
      code = code1 || instruction::READF(addr1);
  }
  if (Types.isBooleanTy(tid1)) {
      code = code1 || instruction::READI(addr1);
  }
  if(isArray) {
      code = code ||  instruction::XLOAD(tmp, offs1, addr1);   
  }
  
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWriteExpr(AslParser::WriteExprContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitWriteExpr(AslParser::WriteExprContext *ctx) {
  instructionList code;
  std::string     addr1 = getAddrDecor(ctx->expr());
  // std::string     offs1 = getOffsetDecor(ctx->expr());
  instructionList code1 = getCodeDecor(ctx->expr());
  // TypesMgr::TypeId tid1 = getTypeDecor(ctx->expr());
  if (Types.isFloatTy(getTypeDecor(ctx->expr())) )
    code = code1 || instruction::WRITEF(addr1);
  else if (Types.isCharacterTy(getTypeDecor(ctx->expr())))
    code = code1 || instruction::WRITEC(addr1);
  else
    code = code1 || instruction::WRITEI(addr1);
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterWriteString(AslParser::WriteStringContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitWriteString(AslParser::WriteStringContext *ctx) {
  instructionList code;
  std::string s = ctx->STRING()->getText();
  std::string temp = "%"+codeCounters.newTEMP();
  int i = 1;
  while (i < int(s.size())-1) {
    if (s[i] != '\\') {
      code = code ||
	     instruction::CHLOAD(temp, s.substr(i,1)) ||
	     instruction::WRITEC(temp);
      i += 1;
    }
    else {
      assert(i < int(s.size())-2);
      if (s[i+1] == 'n') {
        code = code || instruction::WRITELN();
        i += 2;
      }
      else if (s[i+1] == 't' or s[i+1] == '"' or s[i+1] == '\\') {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,2)) ||
	       instruction::WRITEC(temp);
        i += 2;
      }
      else {
        code = code ||
               instruction::CHLOAD(temp, s.substr(i,1)) ||
	       instruction::WRITEC(temp);
        i += 1;
      }
    }
  }
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}



void CodeGenListener::enterArrayAccess(AslParser::ArrayAccessContext *ctx){ 
  DEBUG_ENTER();
}
void CodeGenListener::exitArrayAccess(AslParser::ArrayAccessContext *ctx){
  std::string addr = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());
  std::string size = "%"+codeCounters.newTEMP();
  std::string offset = "%"+codeCounters.newTEMP();
  std::string temp = "%"+codeCounters.newTEMP();
  int s = Types.getSizeOfType(Types.getArrayElemType(getTypeDecor(ctx->ident())));
  code = code || instruction::ILOAD(size, std::to_string(s)); //REVISE!
  code = code || instruction::MUL(offset, size, addr); //Fix me
  if (not Symbols.isParameterClass(ctx->ident()->ID()->getText())) {
    code = code || instruction::LOADX(temp, ctx->ident()->ID()->getText(), offset);
  }
  else {
    std::string temp2 = "%"+codeCounters.newTEMP();
    
    code = code || instruction::LOAD(temp2, ctx->ident()->ID()->getText()) || instruction::LOADX(temp,   temp2, offset);
    
      
  }
  putCodeDecor(ctx,code);
  putAddrDecor(ctx,temp);
  DEBUG_EXIT();
}



void CodeGenListener::enterLeft_expr(AslParser::Left_exprContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitLeft_expr(AslParser::Left_exprContext *ctx) {
  // RECYCLE
  if ( ctx->expr() ) {
    TypesMgr::TypeId t1 = Types.getArrayElemType(getTypeDecor(ctx->ident()));
    int SIZE = Types.getSizeOfType(t1);
    instructionList code = getCodeDecor(ctx->expr());
    std::string temp = "%"+codeCounters.newTEMP();
    std::string offset = "%"+codeCounters.newTEMP();
    std::string addr = getAddrDecor(ctx->expr());
    code = code || instruction::ILOAD(temp, std::to_string(SIZE));
    code = code || instruction::MUL(offset, temp, addr); //Fix me
    putOffsetDecor(ctx, offset); //TODO: not good, probably will have to change
    putAddrDecor(ctx, getAddrDecor(ctx->ident()));
    putCodeDecor(ctx, code);
  }
  else {
    putAddrDecor(ctx, getAddrDecor(ctx->ident()));
    putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
    putCodeDecor(ctx, getCodeDecor(ctx->ident()));
  }
  DEBUG_ENTER();
}

void CodeGenListener::enterArithmetic(AslParser::ArithmeticContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitArithmetic(AslParser::ArithmeticContext *ctx) {
  std::string     addr1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     addr2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;
  // TypesMgr::TypeId t1 = getTypeDecor(ctx->expr(0));
  // TypesMgr::TypeId t2 = getTypeDecor(ctx->expr(1));
  // TypesMgr::TypeId t  = getTypeDecor(ctx);

  std::string temp = "%"+codeCounters.newTEMP();

  if (Types.isIntegerTy(getTypeDecor(ctx))){// TODO: keep doing...
    //Both parameters ARE integers
    if (ctx->MUL())
      code = code || instruction::MUL(temp, addr1, addr2);
    else if (ctx->PLUS())
      code = code || instruction::ADD(temp, addr1, addr2);
    else if (ctx->MINUS())
      code = code || instruction::SUB(temp, addr1, addr2);
    else if (ctx->DIV())
      code = code || instruction::DIV(temp, addr1, addr2);
    else if (ctx->MOD()) {
      std::string temp1 = "%"+codeCounters.newTEMP();
      std::string temp2 = "%"+codeCounters.newTEMP();
      code = code || instruction::DIV(temp1, addr1, addr2);
      code = code || instruction::MUL(temp2, temp1, addr2);
      code = code || instruction::SUB(temp, addr1, temp2);
      
    }
    //else 

  } 
  else {
    std::string f_addr1, f_addr2;

    if (Types.isIntegerTy(getTypeDecor(ctx->expr(0))) ){
      f_addr1 = "%"+codeCounters.newTEMP();
      f_addr2 = addr2;
      code = code || instruction::FLOAT(f_addr1, addr1);
    }
    else if (Types.isIntegerTy(getTypeDecor(ctx->expr(1))) ){
      f_addr1 = addr1;
      f_addr2 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(f_addr2, addr2);
    }
    else {
      f_addr1 = addr1;
      f_addr2 = addr2;
    }

    if (ctx->MUL())
      code = code || instruction::FMUL(temp, f_addr1, f_addr2);
    else if (ctx->PLUS())
      code = code || instruction::FADD(temp, f_addr1, f_addr2);
    else if (ctx->MINUS())
      code = code || instruction::FSUB(temp, f_addr1, f_addr2);
    else if (ctx->DIV())
      code = code || instruction::FDIV(temp, f_addr1, f_addr2);

  }  
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);

  
  DEBUG_EXIT();
}

void CodeGenListener::enterRelational(AslParser::RelationalContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitRelational(AslParser::RelationalContext *ctx) {
  std::string     addr1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     addr2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;
  
  

  std::string temp = "%"+codeCounters.newTEMP();
  
  if (
    (Types.isIntegerTy(getTypeDecor(ctx->expr(0))) 
      and Types.isIntegerTy(getTypeDecor(ctx->expr(1))))
  or (Types.isCharacterTy(getTypeDecor(ctx->expr(0))) 
    and Types.isCharacterTy(getTypeDecor(ctx->expr(1))))) {// TODO: keep doing...
    //Both parameters ARE integers
    if (ctx->EQUAL())
      code = code || instruction::EQ(temp, addr1, addr2);
    else if (ctx->NEQUAL()){
      std::string temp2 = "%"+codeCounters.newTEMP();
      code = code || instruction::EQ(temp2, addr1, addr2);
      code = code || instruction::NOT(temp, temp2);
    }
    else if (ctx->LT())
      code = code || instruction::LT(temp, addr1, addr2);
    else if (ctx->GT())
      code = code || instruction::LT(temp, addr2, addr1);
    else if (ctx->LE())
      code = code || instruction::LE(temp, addr1, addr2);
    else if (ctx->GE())
      code = code || instruction::LE(temp, addr2, addr1);
    
  } else if (Types.isBooleanTy(getTypeDecor(ctx->expr(0))) and Types.isBooleanTy(getTypeDecor(ctx->expr(1))) ){// TODO: keep doing...
    //Both parameters ARE integers
    if (ctx->EQUAL())
      code = code || instruction::EQ(temp, addr1, addr2);
    else if (ctx->NEQUAL()){
      std::string temp2 = "%"+codeCounters.newTEMP();
      code = code || instruction::EQ(temp2, addr1, addr2);
      code = code || instruction::NOT(temp, temp2);
    }
  } else /*if (Types.isFloatTy(getTypeDecor(ctx)))*/ { //Float
    std::string f_addr1, f_addr2;

    if (Types.isIntegerTy(getTypeDecor(ctx->expr(0))) ){
      f_addr1 = "%"+codeCounters.newTEMP();
      f_addr2 = addr2;
      code = code || instruction::FLOAT(f_addr1, addr1);
    }
    else if (Types.isIntegerTy(getTypeDecor(ctx->expr(1))) ){
      f_addr1 = addr1;
      f_addr2 = "%"+codeCounters.newTEMP();
      code = code || instruction::FLOAT(f_addr2, addr2);
    }
    else {
      f_addr1 = addr1;
      f_addr2 = addr2;
    }

    if (ctx->EQUAL())
      code = code || instruction::FEQ(temp, f_addr1, f_addr2);
    else if (ctx->NEQUAL()){
      std::string temp2 = "%"+codeCounters.newTEMP();
      code = code || instruction::FEQ(temp2, f_addr1, f_addr2);
      code = code || instruction::NOT(temp, temp2);
    }
    else if (ctx->LT())
      code = code || instruction::FLT(temp, f_addr1, f_addr2);
    else if (ctx->GT())
      code = code || instruction::FLT(temp, f_addr2, f_addr1);
    else if (ctx->LE())
      code = code || instruction::FLE(temp, f_addr1, f_addr2);
    else if (ctx->GE())
      code = code || instruction::FLE(temp, f_addr2, f_addr1);

  }  
  
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);

  
  DEBUG_EXIT();
}


void CodeGenListener::enterValue(AslParser::ValueContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitValue(AslParser::ValueContext *ctx) {
  instructionList code;
  std::string temp = "%"+codeCounters.newTEMP();
  if (ctx->INTVAL())
    code = instruction::ILOAD(temp, ctx->INTVAL()->getText());
  if (ctx->FLOATVAL())
    code = instruction::FLOAD(temp, ctx->FLOATVAL()->getText());
  if (ctx->CHARVAL())
    code = instruction::CHLOAD(temp, ctx->CHARVAL()->getText());
  if (ctx->BOOLVAL()){
    std::string val = "0";
    if(ctx->BOOLVAL()->getText() == "true") val = "1";
    code = instruction::ILOAD(temp, val);
  }
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterExprIdent(AslParser::ExprIdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitExprIdent(AslParser::ExprIdentContext *ctx) {
  putAddrDecor(ctx, getAddrDecor(ctx->ident()));
  putOffsetDecor(ctx, getOffsetDecor(ctx->ident()));
  putCodeDecor(ctx, getCodeDecor(ctx->ident()));
  DEBUG_EXIT();
}

void CodeGenListener::enterIdent(AslParser::IdentContext *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitIdent(AslParser::IdentContext *ctx) {
  putAddrDecor(ctx, ctx->ID()->getText());
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, instructionList());
  DEBUG_EXIT();
}






void CodeGenListener::enterProcedure(AslParser::ProcedureContext *ctx) {
    DEBUG_ENTER();
}
void CodeGenListener::exitProcedure(AslParser::ProcedureContext *ctx) {
    instructionList code = instructionList();
    if (not Types.isVoidFunction(Symbols.getType(ctx->ident()->ID()->getText())) )
        code = code || instruction::PUSH();
    
    TypesMgr::TypeId t1 = getTypeDecor(ctx->ident());
    int i=0;
    for (auto eCtx : ctx->expr()) {
        std::string addr = getAddrDecor(eCtx);
        instructionList codeTmp = getCodeDecor(eCtx);
        
        TypesMgr::TypeId paramTy = Types.getParameterType(t1,i++);
        TypesMgr::TypeId callTy = getTypeDecor(eCtx);
        if(Types.isFloatTy(paramTy) && Types.isIntegerTy(callTy)){
            std::string f_addr = "%" + codeCounters.newTEMP();
            codeTmp = codeTmp || instruction::FLOAT(f_addr, addr);
            addr = f_addr;
        }
        //TODO: implicit casting!
        if (Types.isArrayTy(paramTy)) {
            std::string f_addr = "%" + codeCounters.newTEMP();
            codeTmp = codeTmp || instruction::ALOAD(f_addr, addr);
            addr = f_addr;
        }
        
        
        code = code || codeTmp || instruction::PUSH(addr) ;
    }
    code = code || instruction::CALL(ctx->ident()->ID()->getText());
    for (auto eCtx : ctx->expr()) {
        code = code || instruction::POP() ;
    }
    if (not Types.isVoidFunction(Symbols.getType(ctx->ident()->ID()->getText())) ){
        std::string addr = "%" + codeCounters.newTEMP();
        code = code || instruction::POP(addr);
        putAddrDecor(ctx, addr);
    }
    putCodeDecor(ctx, code);
    DEBUG_EXIT();
}

void CodeGenListener::enterRet(AslParser::RetContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitRet(AslParser::RetContext *ctx){
  DEBUG_EXIT();
}

void CodeGenListener::enterExprProcedure(AslParser::ExprProcedureContext *ctx){
  DEBUG_ENTER();
}
  
void CodeGenListener::exitExprProcedure(AslParser::ExprProcedureContext *ctx){
  std::string     addr = getAddrDecor(ctx->procedure());
  instructionList code = getCodeDecor(ctx->procedure());
  
  putAddrDecor(ctx, addr);
  putCodeDecor(ctx, code);
  
  DEBUG_EXIT();
}


void CodeGenListener::enterParenthesis(AslParser::ParenthesisContext  *ctx) {
  DEBUG_ENTER();
}
void CodeGenListener::exitParenthesis(AslParser::ParenthesisContext *ctx) {
  std::string     addr = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());

  putAddrDecor(ctx, addr);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}



void CodeGenListener::enterBoolean(AslParser::BooleanContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitBoolean(AslParser::BooleanContext *ctx){
  std::string     addr1 = getAddrDecor(ctx->expr(0));
  instructionList code1 = getCodeDecor(ctx->expr(0));
  std::string     addr2 = getAddrDecor(ctx->expr(1));
  instructionList code2 = getCodeDecor(ctx->expr(1));
  instructionList code  = code1 || code2;

  std::string temp = "%"+codeCounters.newTEMP();

  if (ctx->AND())
    code = code || instruction::AND(temp, addr1, addr2);
  else if (ctx->OR())
    code = code || instruction::OR(temp, addr1, addr2);
  
  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}

void CodeGenListener::enterUnary(AslParser::UnaryContext *ctx){
  DEBUG_ENTER();
}
void CodeGenListener::exitUnary(AslParser::UnaryContext *ctx){
  std::string     addr = getAddrDecor(ctx->expr());
  instructionList code = getCodeDecor(ctx->expr());
  std::string temp;
  if (Types.isIntegerTy(getTypeDecor(ctx))) {
    if (ctx->MINUS()) {
      temp = "%"+codeCounters.newTEMP();
      code = code || instruction::NEG(temp, addr);
    }
    else if (ctx->PLUS()){
      temp = addr;
    }
  } else if (Types.isFloatTy(getTypeDecor(ctx))) {
    if (ctx->MINUS()) {
      temp = "%"+codeCounters.newTEMP();
      code = code || instruction::FNEG(temp, addr);
    }
    else if (ctx->PLUS()){
      temp = addr;
    }
  } else {
    if (ctx->NOT()) {
      temp = "%"+codeCounters.newTEMP();
      code = code || instruction::NOT(temp, addr);
    }
  }

  putAddrDecor(ctx, temp);
  putOffsetDecor(ctx, "");
  putCodeDecor(ctx, code);
  DEBUG_EXIT();
}


/* WHILE LOOPS AND ELSE CHAINS MUST BE IMPLEMENTED */

// void CodeGenListener::enterEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_ENTER();
// }
// void CodeGenListener::exitEveryRule(antlr4::ParserRuleContext *ctx) {
//   DEBUG_EXIT();
// }
// void CodeGenListener::visitTerminal(antlr4::tree::TerminalNode *node) {
//   DEBUG(">>> visit " << node->getSymbol()->getLine() << ":" << node->getSymbol()->getCharPositionInLine() << " CodeGen TerminalNode");
// }
// void CodeGenListener::visitErrorNode(antlr4::tree::ErrorNode *node) {
// }


// Getters for the necessary tree node atributes:
//   Scope, Type, Addr, Offset and Code
SymTable::ScopeId CodeGenListener::getScopeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getScope(ctx);
}
TypesMgr::TypeId CodeGenListener::getTypeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getType(ctx);
}
std::string CodeGenListener::getAddrDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getAddr(ctx);
}
std::string  CodeGenListener::getOffsetDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getOffset(ctx);
}
instructionList CodeGenListener::getCodeDecor(antlr4::ParserRuleContext *ctx) {
  return Decorations.getCode(ctx);
}

// Setters for the necessary tree node attributes:
//   Addr, Offset and Code
void CodeGenListener::putAddrDecor(antlr4::ParserRuleContext *ctx, const std::string & a) {
  Decorations.putAddr(ctx, a);
}
void CodeGenListener::putOffsetDecor(antlr4::ParserRuleContext *ctx, const std::string & o) {
  Decorations.putOffset(ctx, o);
}
void CodeGenListener::putCodeDecor(antlr4::ParserRuleContext *ctx, const instructionList & c) {
  Decorations.putCode(ctx, c);
}
