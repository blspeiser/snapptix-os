package Snapptix_backend;
import java.lang.*;

public interface GlobalIP {

    public final int iPort      = 6789;
    public final int oPort      = 9876;
    public final int JanixPort  = 1111;

    public final String localHost = "127.0.0.1";
    public final String BaseIP    = "192.168.217.";

    public final String PROCESS       = "P";
    public final String RESTART       = "R";
    public final String KILL          = "K";
    public final String APP           = "X"; /* as in "eXecutable"   */
    public final String INSTALL       = "I";
    public final String UNINSTALL     = "U";
    public final String ALPHA         = "A";
    public final String STATUSREQ     = "?";
    public final String ECHO          = "E";
    public final String MANAGE        = "M";
    public final String NETWORK       = "N";
    public final String BACKUP        = "B";
    public final String PASSWORD      = "P";
    public final String MOUNT         = "M";
    public final String UNMOUNT       = "U";
    public final String EMAIL         = "E";
    public final String SKIP          = "-";
    public final String NOCOMMAND     = "\0";
    public final String PROTOCOLERROR   = "X-";
    public final String YES             = "Y-";
    public final String NO              = "N-";
    public final String SUCCEEDED       = "S-";
    public final String FAILED          = "F-";
    public final String ALPHAUNIT       = "A-";
    public final String BETAUNIT        = "B-";
    public final String ECHOHEADER	= "E-";
    public final String EMPTYRESPONSE	= "--";

    public final int PROTOCOLMAXSIZE    = 64;
    public final int MAXSOCKETSIZE      = 4096;
    public final int FAILOVERTHRESHOLD	= 3;
    public final int MSGQUEUESIZE       = 5;

    public final String CIPHER          = "DES-CBC3-MD5";
    public final String PASSPHRASE	= "Snappware";
    public final String SUBJECTCODE     = "/C=US/ST=New Jersey/L=Newark/O=Snapptix Consulting/OU=Janix Development/CN=B. L. Speiser/Email=blspeiser@yahoo.com";
    public final String ISSUERCODE      = "/C=US/ST=New Jersey/L=Newark/O=Snapptix Consulting/OU=Janix Development/CN=B. L. Speiser/Email=blspeiser@yahoo.com";
}


