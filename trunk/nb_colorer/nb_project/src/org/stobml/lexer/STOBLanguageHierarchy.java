package org.stobml.lexer;

import java.util.*;
import org.netbeans.spi.lexer.LanguageHierarchy;
import org.netbeans.spi.lexer.Lexer;
import org.netbeans.spi.lexer.LexerRestartInfo;

public class STOBLanguageHierarchy extends LanguageHierarchy<STOBTokenId> {

    private static List<STOBTokenId> tokens;
    private static Map<Integer, STOBTokenId> idToToken;

    private static void init() {
        tokens = Arrays.<STOBTokenId>asList(new STOBTokenId[]{
            //[PENDING]
        });
        idToToken = new HashMap<Integer, STOBTokenId>();
        for (STOBTokenId token : tokens) {
            idToToken.put(token.ordinal(), token);
        }
    }

    static synchronized STOBTokenId getToken(int id) {
        if (idToToken == null) {
            init();
        }
        return idToToken.get(id);
    }

    @Override
    protected synchronized Collection<STOBTokenId> createTokenIds() {
        if (tokens == null) {
            init();
        }
        return tokens;
    }

    @Override
    protected synchronized Lexer<STOBTokenId> createLexer(LexerRestartInfo<STOBTokenId> info) {
        return new STOBLexer(info);
    }

    @Override
    protected String mimeType() {
        return "text/x-stob";
    }

}