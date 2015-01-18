############################################################################
############# Fix for Windows Vista & 7 (shortcut removal)	   #############
############################################################################

# The name of the installer
Name "SAOS PingGUIn"

# define name of installer
outFile "PingGUIN_installer.exe"
 
Function un.CheckAdminRights
	System::Call "kernel32::GetModuleHandle(t 'shell32.dll') i .s"
	System::Call "kernel32::GetProcAddress(i s, i 680) i .r0"
	System::Call "::$0() i .r0"

	IntCmp $0 0 isNotAdmin isNotAdmin isAdmin
	isNotAdmin:
	DetailPrint "Missing Administrator Rights !!!"
	messageBox MB_YESNO|MB_DEFBUTTON2 "The current user's rights are too low!$\r\
	For r the installation account administrator rights!$\r\
	to libraries (. dll files) to register.$\r\
	$\r\
	In the case of an update may be sufficient but the current rights.$\r\
	$\r\
	If the installation without administrative rights to continue?" IDYES isAdmin
	quit

	isAdmin:
	DetailPrint "Administrator Rights granted"

FunctionEnd
 
# define installation directory
installDir $PROGRAMFILES\PingGUIn

# Give user option to choose installation directory :)
DirText "This will install SAOS PingGUIn Program on your computer. Choose an installation directory."
 
# start default section
section
 
    # set the installation directory as the destination for the following actions
    setOutPath $INSTDIR
	
	# define what to install and place it in the output path
	file mfc100u.dll
	file msvcr100.dll
	file PingGUIN.exe

    # create the uninstaller
    writeUninstaller "$INSTDIR\PingGUIn_uninstall.exe"
 
    # Create Shortcut folder
	CreateDirectory "$SMPROGRAMS\SAOS"
	# create a shortcut named "new shortcut" in the start menu programs directory
    # point the new shortcut at the program uninstaller
	createShortCut "$SMPROGRAMS\SAOS\PingGUIn.lnk" "$INSTDIR\PingGUIN.exe"
    createShortCut "$SMPROGRAMS\SAOS\Uninstall.lnk" "$INSTDIR\PingGUIn_uninstall.exe"
sectionEnd
 
# uninstaller section start
section "uninstall"
 
    # first, delete the uninstaller
	delete "$INSTDIR\PingGUIN.exe"
	delete "$INSTDIR\mfc100u.dll"
	delete "$INSTDIR\msvcr100.dll"
    delete "$INSTDIR\PingGUIn_uninstall.exe"
	RMDir $INSTDIR

	Call un.CheckAdminRights

    # second, remove the link from the start menu
	delete "$SMPROGRAMS\SAOS\PingGUIn.lnk"
    delete "$SMPROGRAMS\SAOS\Uninstall.lnk"
	RMDir "$SMPROGRAMS\SAOS"
	
# uninstaller section end
sectionEnd
