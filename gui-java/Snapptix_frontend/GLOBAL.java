package Snapptix_frontend;

import Snapptix_backend.*;
import javax.swing.*;

public interface GLOBAL {
  API                 SnapptixAPI = new API();
  BEAN_TrafficLight       Light   = new BEAN_TrafficLight(SnapptixAPI.getMaxNodes());
  BEAN_ServerStatistics   sStats  = new BEAN_ServerStatistics();
  ProgramTree             PT      = new ProgramTree();
  Monitor                 MT      = new Monitor();
  public AppFrame         App     = new AppFrame();

  public final String AlphaUnit = "Alpha";
  public final String BetaUnit  = "Beta";
  public final static long guiREFRESHTIME = 350; //milliseconds
}