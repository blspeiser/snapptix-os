package Snapptix_modules;

import java.io.*;
import java.util.*;
import Snapptix_utils.*;
import Snapptix_backend.Janix;


// Module name MUST be the same as the package name += "Module" -?
public class JanixModule extends AppModule {

  public JanixModule() throws IOException {
    super("Janix");
  }
  public void showWindow() {
    if(ConfirmBox.ask("Snapptix Application Module Example: " +
                      "Click OK to save settings, or cancel to cancel!",
                      this.owner))
                            this.saveSettings();
  }
  /*
  Normally, these two functions would be responsible for reading/writing configuration files.
  In this case, there are no config files to parse and change, but you could if they existed.
  */
  public boolean saveSettings() {
    // parse and change this.configFileNames
    this.restart(); // restart procs
    return true; // No settings to change in this case.
  }
  public boolean loadSettings() {
    // parse and load this.configFileNames
    return true; // No settings to load in this case.
  }
}