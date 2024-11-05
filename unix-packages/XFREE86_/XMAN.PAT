--- xc/programs/xman/vendor.h.old	2002-09-21 04:38:25.000000000 +0200
+++ xc/programs/xman/vendor.h	2003-03-02 14:44:46.000000000 +0100
@@ -193,7 +193,7 @@
 # if defined(CSRG_BASED)
 #  define FORMAT	"nroff -mandoc"
 # elif defined(linux)
-#  define FORMAT	"groff -Tlatin1 -mandoc"
+#  define FORMAT	"nroff -c -Tlatin1 -mandoc"
 # else
 #  define FORMAT	"groff -man"
 # endif
