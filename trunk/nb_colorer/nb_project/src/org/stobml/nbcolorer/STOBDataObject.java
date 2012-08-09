/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package org.stobml.nbcolorer;

import java.io.IOException;
import org.netbeans.core.spi.multiview.MultiViewElement;
import org.netbeans.core.spi.multiview.text.MultiViewEditorElement;
import org.openide.filesystems.FileObject;
import org.openide.loaders.DataObjectExistsException;
import org.openide.loaders.MultiDataObject;
import org.openide.loaders.MultiFileLoader;
import org.openide.util.Lookup;
import org.openide.util.NbBundle.Messages;
import org.openide.windows.TopComponent;

public class STOBDataObject extends MultiDataObject {

	public STOBDataObject(FileObject pf, MultiFileLoader loader) throws DataObjectExistsException, IOException {
		super(pf, loader);
		registerEditor("text/x-stob", true);
	}

	@Override
	protected int associateLookup() {
		return 1;
	}

	@MultiViewElement.Registration(displayName = "#LBL_STOB_EDITOR",
	iconBase = "SET/PATH/TO/ICON/HERE",
	mimeType = "text/x-stob",
	persistenceType = TopComponent.PERSISTENCE_ONLY_OPENED,
	preferredID = "STOB",
	position = 1000)
	@Messages("LBL_STOB_EDITOR=Source")
	public static MultiViewEditorElement createEditor(Lookup lkp) {
		return new MultiViewEditorElement(lkp);
	}
}
