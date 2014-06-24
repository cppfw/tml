package org.stobml.lexer;

import org.netbeans.api.lexer.TokenId;
import org.netbeans.api.lexer.Language;

/**
 *
 * @author igagis
 */
public class STOBTokenId implements TokenId {

    private final String name;
    private final String primaryCategory;
    private final int id;

    STOBTokenId(
            String name,
            String primaryCategory,
            int id) {
        this.name = name;
        this.primaryCategory = primaryCategory;
        this.id = id;
    }

    @Override
    public String primaryCategory() {
        return primaryCategory;
    }

    @Override
    public int ordinal() {
        return id;
    }

    @Override
    public String name() {
        return name;
    }

	public static Language<STOBTokenId> getLanguage() {
		return new STOBLanguageHierarchy().language();
	}
}
