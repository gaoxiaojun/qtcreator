cplusplus = { }

function cplusplus.Configure()
    local d = solution().basedir .. "/src/libs/3rdparty/cplusplus"

    includedirs
    {
        d
    } 

    qt4.HEADERS 
    {
          d .. "/CPlusPlus.h"
        , d .. "/AST.h"
        , d .. "/ASTVisitor.h"
        , d .. "/ASTMatcher.h"
        , d .. "/ASTPatternBuilder.h"
        , d .. "/ASTfwd.h"
        , d .. "/TypeMatcher.h"
        , d .. "/CPlusPlusForwardDeclarations.h"
        , d .. "/Control.h"
        , d .. "/CoreTypes.h"
        , d .. "/DiagnosticClient.h"
        , d .. "/FullySpecifiedType.h"
        , d .. "/Lexer.h"
        , d .. "/LiteralTable.h"
        , d .. "/Literals.h"
        , d .. "/MemoryPool.h"
        , d .. "/Name.h"
        , d .. "/NameVisitor.h"
        , d .. "/Names.h"
        , d .. "/Parser.h"
        , d .. "/Scope.h"
        , d .. "/Bind.h"
        , d .. "/Symbol.h"
        , d .. "/Symbols.h"
        , d .. "/SymbolVisitor.h"
        , d .. "/Token.h"
        , d .. "/TranslationUnit.h"
        , d .. "/Type.h"
        , d .. "/TypeVisitor.h"
        , d .. "/ObjectiveCTypeQualifiers.h"
        , d .. "/QtContextKeywords.h"
        , d .. "/Templates.h"
    }


    files
    {
          d .. "/AST.cpp"
        , d .. "/ASTVisit.cpp"
        , d .. "/ASTMatch0.cpp"
        , d .. "/ASTVisitor.cpp"
        , d .. "/ASTClone.cpp"
        , d .. "/ASTPatternBuilder.cpp"
        , d .. "/ASTMatcher.cpp"
        , d .. "/TypeMatcher.cpp"
        , d .. "/Control.cpp"
        , d .. "/CoreTypes.cpp"
        , d .. "/DiagnosticClient.cpp"
        , d .. "/FullySpecifiedType.cpp"
        , d .. "/Keywords.cpp"
        , d .. "/ObjectiveCAtKeywords.cpp"
        , d .. "/ObjectiveCTypeQualifiers.cpp"
        , d .. "/Lexer.cpp"
        , d .. "/LiteralTable.cpp"
        , d .. "/Literals.cpp"
        , d .. "/MemoryPool.cpp"
        , d .. "/Name.cpp"
        , d .. "/NameVisitor.cpp"
        , d .. "/Names.cpp"
        , d .. "/Parser.cpp"
        , d .. "/Scope.cpp"
        , d .. "/Bind.cpp"
        , d .. "/Symbol.cpp"
        , d .. "/Symbols.cpp"
        , d .. "/SymbolVisitor.cpp"
        , d .. "/Token.cpp"
        , d .. "/TranslationUnit.cpp"
        , d .. "/Type.cpp"
        , d .. "/TypeVisitor.cpp"
        , d .. "/QtContextKeywords.cpp"
        , d .. "/Templates.cpp"
    }

end
