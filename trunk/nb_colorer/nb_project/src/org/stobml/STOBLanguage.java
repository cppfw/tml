package org.stobml;

import org.netbeans.api.lexer.Language;
import org.netbeans.modules.csl.spi.DefaultLanguageConfig;
import org.netbeans.modules.csl.spi.LanguageRegistration;
import org.stobml.lexer.STOBTokenId;

@LanguageRegistration(mimeType = "text/x-stob")
public class STOBLanguage extends DefaultLanguageConfig {

    @Override
    public Language getLexerLanguage() {
        return STOBTokenId.getLanguage();
    }

    @Override
    public String getDisplayName() {
        return "STOB";
    }

}
