--- xc/programs/xterm/Imakefile	2002-11-22 23:56:06.000000000 +0100
+++ xc/programs/xterm/Imakefile.new	2003-03-02 14:38:26.000000000 +0100
@@ -88,7 +88,7 @@
 #endif
 
 #ifdef UTF8support
-    UTF8_OPTION = -DOPT_WIDE_CHARS -DOPT_LUIT_PROG
+    UTF8_OPTION = -DOPT_WIDE_CHARS -DOPT_LUIT_PROG -DOPT_256_COLORS
         UTF8SRC = charclass.c precompose.c wcwidth.c xutf8.c
         UTF8OBJ = charclass.o precompose.o wcwidth.o xutf8.o
 #endif
