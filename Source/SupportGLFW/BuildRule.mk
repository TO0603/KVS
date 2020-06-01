#  DON'T EDIT THIS FILE.
#  THIS IS GENERATED BY "Configuration/configure_BuildRule.py".


OBJECTS := \
$(OUTDIR)/./Viewer/Application.o \
$(OUTDIR)/./Viewer/Screen.o \
$(OUTDIR)/./Viewer/ScreenBase.o \
$(OUTDIR)/./Viewer/Timer.o \



$(OUTDIR)/./Viewer/%.o: ./Viewer/%.cpp ./Viewer/%.h
	$(MKDIR) $(OUTDIR)/./Viewer
	$(CPP) -c $(CPPFLAGS) $(DEFINITIONS) $(INCLUDE_PATH) -o $@ $<


install::
	$(MKDIR) $(INSTALL_DIR)/include/SupportGLFW/.
	$(INSTALL) ./*.h $(INSTALL_DIR)/include/SupportGLFW/.
	$(MKDIR) $(INSTALL_DIR)/include/SupportGLFW/./Viewer
	$(INSTALL) ./Viewer/*.h $(INSTALL_DIR)/include/SupportGLFW/./Viewer
