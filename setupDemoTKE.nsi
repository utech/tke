; Script generated by the HM NIS Edit Script Wizard.

; HM NIS Edit Wizard helper defines
!define PRODUCT_NAME "�������� �������� (����)"
!define PRODUCT_VERSION "2.4.48"
!define PRODUCT_PUBLISHER "�������� �������"
!define PRODUCT_WEB_SITE "http://abonpro.com"
!define PRODUCT_UNINST_KEY "Software\AbonPro\tke\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"
!define PRODUCT_STARTMENU_REGVAL "NSIS:StartMenuDir"

SetCompressor lzma
XPStyle on

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\orange-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\orange-uninstall.ico"
!define MUI_HEADERIMAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Header\orange-r.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "${NSISDIR}\Contrib\Graphics\Wizard\orange-uninstall.bmp"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!define MUI_LICENSEPAGE_RADIOBUTTONS
!insertmacro MUI_PAGE_LICENSE "./licence.txt"
; Components page
!insertmacro MUI_PAGE_COMPONENTS
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Start menu page
var ICONS_GROUP
!define MUI_STARTMENUPAGE_NODISABLE
!define MUI_STARTMENUPAGE_DEFAULTFOLDER "�������� �������� (����)"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "${PRODUCT_STARTMENU_REGVAL}"
!insertmacro MUI_PAGE_STARTMENU Application $ICONS_GROUP
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\bin\tkeAbonentDemo.exe"
!define MUI_FINISHPAGE_SHOWREADME "$INSTDIR\bin\tkeOrganizaciiDemo.exe"
!define MUI_FINISHPAGE_RUN_TEXT "��������� ���� �������� ����� �������� �������"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "Ukrainian"

; Reserve files
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "dst\TKEnergoMenegerDEMO_v${PRODUCT_VERSION}.exe"
InstallDir "D:\MenegTKE"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show


Section "��������� ����������" SEC01
  SectionIn RO

  SetOutPath "$INSTDIR\bin\kvyt_ob"
  SetOverwrite try
  File "bin\kvyt_ob\Readme.txt"
  SetOutPath "$INSTDIR\bin\subs_dbf"
  File "bin\subs_dbf\Readme.txt"
  SetOutPath "$INSTDIR\bin\templates"
  SetOverwrite off
  File "bin\templates\borjnykyMessage_1.html"
  File "bin\templates\pilgovykyMessage_1.html"
  File "bin\templates\organizActVykonRobit_1.html"
  SetOutPath "$INSTDIR\bin\Temp_dbf"
  SetOverwrite try
  File "bin\Temp_dbf\Readme.txt"
  File "bin\Temp_dbf\z_pilg.DBF"
  SetOutPath "$INSTDIR\bin"
  File "bin\abonentLogo.png"
  File "bin\aboutDemo.html"
  SetOutPath "$INSTDIR\bin\sqldrivers"
  File "install\externDLLs\qsqlodbc4.dll"
  File "install\externDLLs\qsqlite4.dll"
  SetOutPath "$INSTDIR\bin"
  File "install\externDLLs\mingwm10.dll"
  File "install\externDLLs\libgcc_s_dw2-1.dll"
  File "install\externDLLs\QtAssistantClient4.dll"
  File "install\externDLLs\QtCore4.dll"
  File "install\externDLLs\QtGui4.dll"
  File "install\externDLLs\QtNetwork4.dll"
  File "install\externDLLs\QtSql4.dll"
  File "install\externDLLs\QtScript4.dll"
  File "bin\tkeAbonentDemo.exe"
  File "bin\tkeOrganizaciiDemo.exe"
  File "bin\utech_logo_80_60.GIF"
  File "bin\textedit.exe"
  File "bin\7z.exe"
  File "bin\7z.dll"
  File "bin\tkeDBUpdate.tkeDB"
  SetOutPath "$INSTDIR\docs"
  File "docs\cubeHelp.png"
  File "docs\dialog_change_password.PNG"
  File "docs\dialog_register.PNG"
  File "docs\dialog_wrong_ODBC.PNG"
  File "docs\index.html"
  File "docs\kontacts.html"
  File "docs\main_win_preview_all.PNG"
  File "docs\main_win_rahunok.PNG"
  File "docs\main_win_unloged.PNG"
  File "docs\rahunok_combobox.PNG"
  File "docs\rahunok_navigation.PNG"
  File "docs\rahunok_opal_change_state.PNG"
  File "docs\rahunok_opal_false.PNG"
  File "docs\rahunok_opal_true.PNG"
  File "docs\rahunok_operacii.PNG"
  File "docs\rahunok_poshuk_PIB.PNG"
  File "docs\sql_zapyty.html"
  File "docs\tablyci_budynky_add_record.PNG"
  File "docs\tablyci_budynky_preview.PNG"
  File "docs\tablyci_budynky_window_del_record.PNG"
  File "docs\tablyci_kotelni_add_record.PNG"
  File "docs\tablyci_kotelni_preview.PNG"
  File "docs\tablyci_normatyvnyi_taryf_preview.PNG"
  File "docs\tablyci_pilgy_add_record.PNG"
  File "docs\tablyci_pilgy_preview.PNG"
  File "docs\tkeAssistant.adp"
  File "docs\tkeAssistant.dcf"
  File "docs\tke_abonent.html"
  File "docs\tke_abonent_arhiv.html"
  File "docs\tke_abonent_avtoryzaciya.html"
  File "docs\tke_abonent_drukovani_dokumenty.html"
  File "docs\tke_abonent_operacii.html"
  File "docs\tke_abonent_rahunok.html"
  File "docs\tke_abonent_tablyci.html"
  File "docs\tke_abonent_tablyci_budynky.html"
  File "docs\tke_abonent_tablyci_diuchyi_taryf.html"
  File "docs\tke_abonent_tablyci_korectury_po_ploshchi.html"
  File "docs\tke_abonent_tablyci_korectury_po_sumi.html"
  File "docs\tke_abonent_tablyci_kotelni.html"
  File "docs\tke_abonent_tablyci_kvytancii.html"
  File "docs\tke_abonent_tablyci_normatyvnyi_taryf.html"
  File "docs\tke_abonent_tablyci_pilgovyky.html"
  File "docs\tke_abonent_tablyci_pilgy.html"
  File "docs\tke_abonent_tablyci_splata_zaborgovanosti.html"
  File "docs\tke_abonent_tablyci_spravy_subsydii.html"
  File "docs\tke_abonent_tablyci_subsydii.html"
  File "docs\tke_assistent.html"
  File "docs\tke_organizacii.html"
  File "docs\tke_organizacii_avtoryzaciya.html"
  File "docs\tke_organizacii_operacii.html"
  File "docs\tke_organizacii_tablyci.html"
  File "docs\utech_logo_80_60.png"
  File "docs\vstup.html"
  SetOutPath "$INSTDIR\script\utechScripts"
  File "script\utechScripts\SambirTkeSubsScript.js"
  File "script\utechScripts\SambirTkeKvytScript.js"
  File "script\utechScripts\BrodyTkeKvytScript.js"
  File "script\utechScripts\BrodyTkeSubsScript.js"
  File "script\utechScripts\ZolochivTkeKvytScript.js"
  File "script\utechScripts\ZolochivTkeSubsScript.js"
  File "script\utechScripts\StebnykTkeKvytScript.js"
  File "script\utechScripts\StebnykTkeSubsScript.js"
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  File "licence.txt"
  File "License.7z.txt"

  SetOutPath "$INSTDIR\bin"
  SetOverwrite ifnewer
  File "install\demo\tke_db_demo.mdb"
  File "install\demo\tke.ini"
  SetOutPath "$INSTDIR\"
  SetOverwrite ifnewer
  File "licence.txt"
  CreateDirectory "$INSTDIR\save_db"

; Shortcuts
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  SetOutPath "$INSTDIR\bin"
  CreateDirectory "$SMPROGRAMS\$ICONS_GROUP"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\�������� ��������-������� (����).lnk" "$INSTDIR\bin\tkeAbonentDemo.EXE"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\�������� ��������-����������� (����).lnk" "$INSTDIR\bin\tkeOrganizaciiDemo.EXE"
  CreateShortCut "$DESKTOP\�������� ��������-������� (����).lnk" "$INSTDIR\bin\tkeAbonentDemo.EXE"
  CreateShortCut "$DESKTOP\�������� ��������-����������� (����).lnk" "$INSTDIR\bin\tkeOrganizaciiDemo.EXE"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -AdditionalIcons
  SetOutPath $INSTDIR
  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
  WriteIniStr "$INSTDIR\${PRODUCT_NAME} ���-����.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  WriteIniStr "$SMPROGRAMS\$ICONS_GROUP\${PRODUCT_NAME} ���-����.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\$ICONS_GROUP\��������� ��������.lnk" "$INSTDIR\uninst.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\tkeAbonentDemo.EXE"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Section descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SEC01} "�����, ��������� ��� ������ ���� �������� "
!insertmacro MUI_FUNCTION_DESCRIPTION_END


Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "��������� �������� $(^Name) ���� ������ ���������."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "�� ��������, � ����, �� ������ �������� $(^Name) � �� ���� ����������?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  !insertmacro MUI_STARTMENU_GETFOLDER "Application" $ICONS_GROUP
  Delete "$INSTDIR\${PRODUCT_NAME} ���-����.url"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"

  Delete "$INSTDIR\licence.txt"
  Delete "$INSTDIR\License.7z.txt"
  Delete "$INSTDIR\script\utechScripts\BrodyTkeSubsScript.js"
  Delete "$INSTDIR\script\utechScripts\BrodyTkeKvytScript.js"
  Delete "$INSTDIR\script\utechScripts\SambirTkeKvytScript.js"
  Delete "$INSTDIR\script\utechScripts\SambirTkeSubsScript.js"
  Delete "$INSTDIR\script\utechScripts\ZolochivTkeKvytScript.js"
  Delete "$INSTDIR\script\utechScripts\ZolochivTkeSubsScript.js"
  Delete "$INSTDIR\script\utechScripts\StebnykTkeKvytScript.js"
  Delete "$INSTDIR\script\utechScripts\StebnykTkeSubsScript.js"
  Delete "$INSTDIR\docs\vstup.html"
  Delete "$INSTDIR\docs\utech_logo_80_60.png"
  Delete "$INSTDIR\docs\tke_organizacii_tablyci.html"
  Delete "$INSTDIR\docs\tke_organizacii_operacii.html"
  Delete "$INSTDIR\docs\tke_organizacii_avtoryzaciya.html"
  Delete "$INSTDIR\docs\tke_organizacii.html"
  Delete "$INSTDIR\docs\tke_assistent.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_subsydii.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_spravy_subsydii.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_splata_zaborgovanosti.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_pilgy.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_pilgovyky.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_normatyvnyi_taryf.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_kvytancii.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_kotelni.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_korectury_po_sumi.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_korectury_po_ploshchi.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_diuchyi_taryf.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci_budynky.html"
  Delete "$INSTDIR\docs\tke_abonent_tablyci.html"
  Delete "$INSTDIR\docs\tke_abonent_rahunok.html"
  Delete "$INSTDIR\docs\tke_abonent_operacii.html"
  Delete "$INSTDIR\docs\tke_abonent_drukovani_dokumenty.html"
  Delete "$INSTDIR\docs\tke_abonent_avtoryzaciya.html"
  Delete "$INSTDIR\docs\tke_abonent_arhiv.html"
  Delete "$INSTDIR\docs\tke_abonent.html"
  Delete "$INSTDIR\docs\tkeAssistant.dcf"
  Delete "$INSTDIR\docs\tkeAssistant.adp"
  Delete "$INSTDIR\docs\tablyci_pilgy_preview.PNG"
  Delete "$INSTDIR\docs\tablyci_pilgy_add_record.PNG"
  Delete "$INSTDIR\docs\tablyci_normatyvnyi_taryf_preview.PNG"
  Delete "$INSTDIR\docs\tablyci_kotelni_preview.PNG"
  Delete "$INSTDIR\docs\tablyci_kotelni_add_record.PNG"
  Delete "$INSTDIR\docs\tablyci_budynky_window_del_record.PNG"
  Delete "$INSTDIR\docs\tablyci_budynky_preview.PNG"
  Delete "$INSTDIR\docs\tablyci_budynky_add_record.PNG"
  Delete "$INSTDIR\docs\sql_zapyty.html"
  Delete "$INSTDIR\docs\rahunok_poshuk_PIB.PNG"
  Delete "$INSTDIR\docs\rahunok_operacii.PNG"
  Delete "$INSTDIR\docs\rahunok_opal_true.PNG"
  Delete "$INSTDIR\docs\rahunok_opal_false.PNG"
  Delete "$INSTDIR\docs\rahunok_opal_change_state.PNG"
  Delete "$INSTDIR\docs\rahunok_navigation.PNG"
  Delete "$INSTDIR\docs\rahunok_combobox.PNG"
  Delete "$INSTDIR\docs\main_win_unloged.PNG"
  Delete "$INSTDIR\docs\main_win_rahunok.PNG"
  Delete "$INSTDIR\docs\main_win_preview_all.PNG"
  Delete "$INSTDIR\docs\kontacts.html"
  Delete "$INSTDIR\docs\index.html"
  Delete "$INSTDIR\docs\dialog_wrong_ODBC.PNG"
  Delete "$INSTDIR\docs\dialog_register.PNG"
  Delete "$INSTDIR\docs\dialog_change_password.PNG"
  Delete "$INSTDIR\docs\cubeHelp.png"
  Delete "$INSTDIR\bin\textedit.exe"
  Delete "$INSTDIR\bin\aboutDemo.html"
  Delete "$INSTDIR\bin\utech_logo_80_60.GIF"
  Delete "$INSTDIR\bin\tke.ini"
  Delete "$INSTDIR\bin\tkeOrganizaciiDemo.exe"
  Delete "$INSTDIR\bin\tkeAbonentDemo.exe"
  Delete "$INSTDIR\bin\assistant.exe"
  Delete "$INSTDIR\bin\ARJ.EXE"
  Delete "$INSTDIR\bin\aboutDemo.html"
  Delete "$INSTDIR\bin\abonentLogo.png"
  Delete "$INSTDIR\bin\Temp_dbf\z_pilg.DBF"
  Delete "$INSTDIR\bin\Temp_dbf\Readme.txt"
  Delete "$INSTDIR\bin\templates\pilgovykyMessage_1.html"
  Delete "$INSTDIR\bin\templates\borjnykyMessage_1.html"
  Delete "$INSTDIR\bin\templates\organizActVykonRobit_1.html"
  Delete "$INSTDIR\bin\subs_dbf\Readme.txt"
  Delete "$INSTDIR\bin\kvyt_ob\Readme.txt"
  Delete "$INSTDIR\bin\QtSql4.dll"
  Delete "$INSTDIR\bin\QtGui4.dll"
  Delete "$INSTDIR\bin\QtCore4.dll"
  Delete "$INSTDIR\bin\QtAssistantClient4.dll"
  Delete "$INSTDIR\bin\sqldrivers\qsqlodbc4.dll"
  Delete "$INSTDIR\bin\sqldrivers\qsqlite4.dll"
  Delete "$INSTDIR\bin\libgcc_s_dw2-1.dll"
  Delete "$INSTDIR\bin\mingwm10.dll"
  Delete "$INSTDIR\bin\QtNetwork4.dll"
  Delete "$INSTDIR\bin\QtXml4.dll"
  Delete "$INSTDIR\bin\QtScript4.dll"
  Delete "$INSTDIR\bin\QtHelp4.dll"
  Delete "$INSTDIR\bin\QtCLucene4.dll"
  
  Delete "$INSTDIR\bin\tke_db_demo.mdb"

  Delete "$INSTDIR\bin\7z.exe"
  Delete "$INSTDIR\bin\7z.dll"
  Delete "$INSTDIR\bin\tke.testDb"
  Delete "$INSTDIR\bin\tkeDBUpdate.tkeDB"
  Delete "$INSTDIR\bin\tkeDBUpdateResults.tkeDB"


  Delete "$SMPROGRAMS\$ICONS_GROUP\�������� ��������-������� (����).lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\�������� ��������-����������� (����).lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\��������� ��������.lnk"
  Delete "$DESKTOP\�������� ��������-������� (����).lnk"
  Delete "$DESKTOP\�������� ��������-����������� (����).lnk"
  Delete "$SMPROGRAMS\$ICONS_GROUP\${PRODUCT_NAME} ���-����.url"

  ;��������� ����� Thumbs.db
  Delete "$INSTDIR\script\utechScripts\Thumbs.db"
  Delete "$INSTDIR\script\Thumbs.db"
  Delete "$INSTDIR\docs\Thumbs.db"
  Delete "$INSTDIR\save_db\Thumbs.db"
  Delete "$INSTDIR\tke_db\Thumbs.db"
  Delete "$INSTDIR\bin\templates\Thumbs.db"
  Delete "$INSTDIR\bin\Temp_dbf\Thumbs.db"
  Delete "$INSTDIR\bin\subs_dbf\Thumbs.db"
  Delete "$INSTDIR\bin\kvyt_ob\Thumbs.db"
  Delete "$INSTDIR\bin\sqldrivers\Thumbs.db"
  Delete "$INSTDIR\bin\Thumbs.db"
  Delete "$INSTDIR\Thumbs.db"

  RMDir "$SMPROGRAMS\$ICONS_GROUP"
  RMDir "$INSTDIR\script\utechScripts"
  RMDir "$INSTDIR\script"
  RMDir "$INSTDIR\docs"
  RMDir "$INSTDIR\save_db"
  RMDir "$INSTDIR\tke_db"
  RMDir "$INSTDIR\bin\templates"
  RMDir "$INSTDIR\bin\Temp_dbf"
  RMDir "$INSTDIR\bin\subs_dbf"
  RMDir "$INSTDIR\bin\kvyt_ob"
  RMDir "$INSTDIR\bin\sqldrivers"
  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR"

  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  SetAutoClose true
SectionEnd