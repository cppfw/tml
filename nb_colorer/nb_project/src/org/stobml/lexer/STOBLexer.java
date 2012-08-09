package org.stobml.lexer;

import org.netbeans.spi.lexer.Lexer;
import org.netbeans.spi.lexer.LexerRestartInfo;
import org.stobml.jcclexer.JavaCharStream;
import org.stobml.jcclexer.STOBParserTokenManager;
import org.stobml.jcclexer.Token;

class STOBLexer implements Lexer<STOBTokenId> {

    private LexerRestartInfo<STOBTokenId> info;
    private STOBParserTokenManager javaParserTokenManager;

    STOBLexer(LexerRestartInfo<STOBTokenId> info) {
        this.info = info;
        JavaCharStream stream = new JavaCharStream(info.input());
        javaParserTokenManager = new STOBParserTokenManager(stream);
    }

    @Override
    public org.netbeans.api.lexer.Token<STOBTokenId> nextToken() {
        Token token = javaParserTokenManager.getNextToken();
        if (info.input().readLength() < 1) {
            return null;
        }
        return info.tokenFactory().createToken(STOBLanguageHierarchy.getToken(token.kind));
    }

    @Override
    public Object state() {
        return null;
    }

    @Override
    public void release() {
    }

}