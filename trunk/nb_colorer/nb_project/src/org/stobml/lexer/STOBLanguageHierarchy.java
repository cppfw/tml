package org.stobml.lexer;

import java.util.*;
import org.netbeans.spi.lexer.LanguageHierarchy;
import org.netbeans.spi.lexer.Lexer;
import org.netbeans.spi.lexer.LexerRestartInfo;

public class STOBLanguageHierarchy extends LanguageHierarchy<STOBTokenId> {

    private static List<STOBTokenId> tokens = Arrays.asList(new STOBTokenId[]{
		new STOBTokenId("EOF", "whitespace", 0),
		new STOBTokenId("WHITESPACE", "whitespace", 1),
		new STOBTokenId("SINGLE_LINE_COMMENT", "comment", 4),
		new STOBTokenId("FORMAL_COMMENT", "comment", 5),
		new STOBTokenId("MULTI_LINE_COMMENT", "comment", 6),
		new STOBTokenId("ABSTRACT", "keyword", 8),
		new STOBTokenId("ASSERT", "keyword", 9),
		new STOBTokenId("BOOLEAN", "keyword", 10),
		new STOBTokenId("BREAK", "keyword", 11),
		new STOBTokenId("BYTE", "keyword", 12),
		new STOBTokenId("CASE", "keyword", 13),
		new STOBTokenId("CATCH", "keyword", 14),
		new STOBTokenId("CHAR", "keyword", 15),
		new STOBTokenId("CLASS", "keyword", 16),
		new STOBTokenId("CONST", "keyword", 17),
		new STOBTokenId("CONTINUE", "keyword", 18),
		new STOBTokenId("_DEFAULT", "keyword", 19),
		new STOBTokenId("DO", "keyword", 20),
		new STOBTokenId("DOUBLE", "keyword", 21),
		new STOBTokenId("ELSE", "keyword", 22),
		new STOBTokenId("ENUM", "keyword", 23),
		new STOBTokenId("EXTENDS", "keyword", 24),
		new STOBTokenId("FALSE", "keyword", 25),
		new STOBTokenId("FINAL", "keyword", 26),
		new STOBTokenId("FINALLY", "keyword", 27),
		new STOBTokenId("FLOAT", "keyword", 28),
		new STOBTokenId("FOR", "keyword", 29),
		new STOBTokenId("GOTO", "keyword", 30),
		new STOBTokenId("IF", "keyword", 31),
		new STOBTokenId("IMPLEMENTS", "keyword", 32),
		new STOBTokenId("IMPORT", "keyword", 33),
		new STOBTokenId("INSTANCEOF", "keyword", 34),
		new STOBTokenId("INT", "keyword", 35),
		new STOBTokenId("INTERFACE", "keyword", 36),
		new STOBTokenId("LONG", "keyword", 37),
		new STOBTokenId("NATIVE", "keyword", 38),
		new STOBTokenId("NEW", "keyword", 39),
		new STOBTokenId("NULL", "keyword", 40),
		new STOBTokenId("PACKAGE", "keyword", 41),
		new STOBTokenId("PRIVATE", "keyword", 42),
		new STOBTokenId("PROTECTED", "keyword", 43),
		new STOBTokenId("PUBLIC", "keyword", 44),
		new STOBTokenId("RETURN", "keyword", 45),
		new STOBTokenId("SHORT", "keyword", 46),
		new STOBTokenId("STATIC", "keyword", 47),
		new STOBTokenId("STRICTFP", "keyword", 48),
		new STOBTokenId("SUPER", "keyword", 49),
		new STOBTokenId("SWITCH", "keyword", 50),
		new STOBTokenId("SYNCHRONIZED", "keyword", 51),
		new STOBTokenId("THIS", "keyword", 52),
		new STOBTokenId("THROW", "keyword", 53),
		new STOBTokenId("THROWS", "keyword", 54),
		new STOBTokenId("TRANSIENT", "keyword", 55),
		new STOBTokenId("TRUE", "keyword", 56),
		new STOBTokenId("TRY", "keyword", 57),
		new STOBTokenId("VOID", "keyword", 58),
		new STOBTokenId("VOLATILE", "keyword", 59),
		new STOBTokenId("WHILE", "keyword", 60),
		new STOBTokenId("INTEGER_LITERAL", "literal", 61),
		new STOBTokenId("DECIMAL_LITERAL", "literal", 62),
		new STOBTokenId("HEX_LITERAL", "literal", 63),
		new STOBTokenId("OCTAL_LITERAL", "literal", 64),
		new STOBTokenId("FLOATING_POINT_LITERAL", "literal", 65),
		new STOBTokenId("DECIMAL_FLOATING_POINT_LITERAL", "literal", 66),
		new STOBTokenId("DECIMAL_EXPONENT", "number", 67),
		new STOBTokenId("HEXADECIMAL_FLOATING_POINT_LITERAL", "literal", 68),
		new STOBTokenId("HEXADECIMAL_EXPONENT", "number", 69),
		new STOBTokenId("CHARACTER_LITERAL", "literal", 70),
		new STOBTokenId("STRING_LITERAL", "literal", 71),
		new STOBTokenId("IDENTIFIER", "identifier", 72),
		new STOBTokenId("LETTER", "literal", 73),
		new STOBTokenId("PART_LETTER", "literal", 74),
		new STOBTokenId("LPAREN", "operator", 75),
		new STOBTokenId("RPAREN", "operator", 76),
		new STOBTokenId("LBRACE", "operator", 77),
		new STOBTokenId("RBRACE", "operator", 78),
		new STOBTokenId("LBRACKET", "operator", 79),
		new STOBTokenId("RBRACKET", "operator", 80),
		new STOBTokenId("SEMICOLON", "operator", 81),
		new STOBTokenId("COMMA", "operator", 82),
		new STOBTokenId("DOT", "operator", 83),
		new STOBTokenId("AT", "operator", 84),
		new STOBTokenId("ASSIGN", "operator", 85),
		new STOBTokenId("LT", "operator", 86),
		new STOBTokenId("BANG", "operator", 87),
		new STOBTokenId("TILDE", "operator", 88),
		new STOBTokenId("HOOK", "operator", 89),
		new STOBTokenId("COLON", "operator", 90),
		new STOBTokenId("EQ", "operator", 91),
		new STOBTokenId("LE", "operator", 92),
		new STOBTokenId("GE", "operator", 93),
		new STOBTokenId("NE", "operator", 94),
		new STOBTokenId("SC_OR", "operator", 95),
		new STOBTokenId("SC_AND", "operator", 96),
		new STOBTokenId("INCR", "operator", 97),
		new STOBTokenId("DECR", "operator", 98),
		new STOBTokenId("PLUS", "operator", 99),
		new STOBTokenId("MINUS", "operator", 100),
		new STOBTokenId("STAR", "operator", 101),
		new STOBTokenId("SLASH", "operator", 102),
		new STOBTokenId("BIT_AND", "operator", 103),
		new STOBTokenId("BIT_OR", "operator", 104),
		new STOBTokenId("XOR", "operator", 105),
		new STOBTokenId("REM", "operator", 106),
		new STOBTokenId("LSHIFT", "operator", 107),
		new STOBTokenId("PLUSASSIGN", "operator", 108),
		new STOBTokenId("MINUSASSIGN", "operator", 109),
		new STOBTokenId("STARASSIGN", "operator", 110),
		new STOBTokenId("SLASHASSIGN", "operator", 111),
		new STOBTokenId("ANDASSIGN", "operator", 112),
		new STOBTokenId("ORASSIGN", "operator", 113),
		new STOBTokenId("XORASSIGN", "operator", 114),
		new STOBTokenId("REMASSIGN", "operator", 115),
		new STOBTokenId("LSHIFTASSIGN", "operator", 116),
		new STOBTokenId("RSIGNEDSHIFTASSIGN", "operator", 117),
		new STOBTokenId("RUNSIGNEDSHIFTASSIGN", "operator", 118),
		new STOBTokenId("ELLIPSIS", "operator", 119),
		new STOBTokenId("RUNSIGNEDSHIFT", "operator", 120),
		new STOBTokenId("RSIGNEDSHIFT", "operator", 121),
		new STOBTokenId("GT", "operator", 122)
	});

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