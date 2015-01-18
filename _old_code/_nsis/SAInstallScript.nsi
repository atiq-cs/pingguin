# Simple Installer : http://nsis.sourceforge.net/Simple_tutorials#The_bare_minimum
# define the name of the installer
outfile "PingGUIN_installer.exe"
 
# define the directory to install to, the desktop in this case as specified  
# by the predefined $DESKTOP variable
installDir $PROGRAMFILES\PingGUIn

 
# default section
section
#StrCpy $APPNAME "PingGUIn"
#messageBox MB_OK "Application name $APPNAME"
 
# define the output path for this file
setOutPath $INSTDIR
 
# define what to install and place it in the output path
file mfc100u.dll
file msvcr100.dll
file PingGUIN.exe
 
sectionEnd
