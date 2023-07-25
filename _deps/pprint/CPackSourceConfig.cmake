# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. The list of available CPACK_xxx variables and their associated
# documentation may be obtained using
#  cpack --help-variable-list
#
# Some variables are common to all generators (e.g. CPACK_PACKAGE_NAME)
# and some are specific to a generator
# (e.g. CPACK_NSIS_EXTRA_INSTALL_COMMANDS). The generator specific variables
# usually begin with CPACK_<GENNAME>_xxxx.


set(CPACK_BUILD_SOURCE_DIRS "/hqzn/rp/pprint;/hqzn/rp/pprint")
set(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
set(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_DEFAULT_PACKAGE_DESCRIPTION_SUMMARY "pprint built using CMake")
set(CPACK_GENERATOR "TGZ;TXZ")
set(CPACK_IGNORE_FILES ".*\\.suo;.*\\.user;.*\\.userosscache;.*\\.sln\\.docstates;.*\\.userprefs;[Dd]ebug/;[Dd]ebugPublic/;[Rr]elease/;[Rr]eleases/;x64/;x86/;bld/;[Bb]in/;[Oo]bj/;[Ll]og/;\\.vs/;\\.vscode/;[Tt]est[Rr]esult.*/;[Bb]uild[Ll]og\\..*;.*\\.VisualState\\.xml;TestResult\\.xml;[Dd]ebugPS/;[Rr]eleasePS/;dlldata\\.c;project\\.lock\\.json;project\\.fragment\\.lock\\.json;artifacts/;.*_i\\.c;.*_p\\.c;.*_i\\.h;.*\\.ilk;.*\\.meta;.*\\.obj;.*\\.pch;.*\\.pdb;.*\\.pgc;.*\\.pgd;.*\\.rsp;.*\\.sbr;.*\\.tlb;.*\\.tli;.*\\.tlh;.*\\.tmp;.*\\.tmp_proj;.*\\.log;.*\\.vspscc;.*\\.vssscc;\\.builds;.*\\.pidb;.*\\.svclog;.*\\.scc;_Chutzpah.*;ipch/;.*\\.aps;.*\\.ncb;.*\\.opendb;.*\\.opensdf;.*\\.sdf;.*\\.cachefile;.*\\.VC\\.db;.*\\.VC\\.VC\\.opendb;.*\\.psess;.*\\.vsp;.*\\.vspx;.*\\.sap;$tf/;.*\\.gpState;_ReSharper.*/;.*\\.[Rr]e[Ss]harper;.*\\.DotSettings\\.user;\\.JustCode;_TeamCity.*;.*\\.dotCover;_NCrunch_.*;\\..*crunch.*\\.local\\.xml;nCrunchTemp_.*;.*\\.mm\\..*;AutoTest\\.Net/;\\.sass-cache/;[Ee]xpress/;DocProject/buildhelp/;DocProject/Help/.*\\.HxT;DocProject/Help/.*\\.HxC;DocProject/Help/.*\\.hhc;DocProject/Help/.*\\.hhk;DocProject/Help/.*\\.hhp;DocProject/Help/Html2;DocProject/Help/html;publish/;.*\\.[Pp]ublish\\.xml;.*\\.azurePubxml;.*\\.publishproj;PublishScripts/;.*\\.nupkg;.*.*/packages/.*;!.*.*/packages/build/;.*\\.nuget\\.props;.*\\.nuget\\.targets;csx/;.*\\.build\\.csdef;ecf/;rcf/;AppPackages/;BundleArtifacts/;Package\\.StoreAssociation\\.xml;_pkginfo\\.txt;.*\\.[Cc]ache;!.*\\.[Cc]ache/;ClientBin/;~$.*;.*~;.*\\.dbmdl;.*\\.dbproj\\.schemaview;.*\\.jfm;.*\\.pfx;.*\\.publishsettings;node_modules/;orleans\\.codegen\\.cs;Generated_Code/;_UpgradeReport_Files/;Backup.*/;UpgradeLog.*\\.XML;UpgradeLog.*\\.htm;.*\\.mdf;.*\\.ldf;.*\\.rdl\\.data;.*\\.bim\\.layout;.*\\.bim_.*\\.settings;FakesAssemblies/;.*\\.GhostDoc\\.xml;\\.ntvs_analysis\\.dat;.*\\.plg;.*\\.opt;.*.*/.*\\.HTMLClient/GeneratedArtifacts;.*.*/.*\\.DesktopClient/GeneratedArtifacts;.*.*/.*\\.DesktopClient/ModelManifest\\.xml;.*.*/.*\\.Server/GeneratedArtifacts;.*.*/.*\\.Server/ModelManifest\\.xml;_Pvt_Extensions;\\.paket/paket\\.exe;paket-files/;\\.fake/;\\.idea/;.*\\.sln\\.iml;\\.cr/;__pycache__/;.*\\.pyc;build.*;analysis-cppcheck-build-dir;ideas;desktop\\.iniimages/;.editorconfig;.git;.gitignore;.travis.yml;.appveyor.yml")
set(CPACK_INSTALLED_DIRECTORIES "/hqzn/rp/pprint;/")
set(CPACK_INSTALL_CMAKE_PROJECTS "")
set(CPACK_INSTALL_PREFIX "/usr/local")
set(CPACK_MODULE_PATH "")
set(CPACK_NSIS_DISPLAY_NAME "pprint 1.0.0")
set(CPACK_NSIS_INSTALLER_ICON_CODE "")
set(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES")
set(CPACK_NSIS_PACKAGE_NAME "pprint 1.0.0")
set(CPACK_NSIS_UNINSTALL_NAME "Uninstall")
set(CPACK_OUTPUT_CONFIG_FILE "/hqzn/rp/pprint/CPackConfig.cmake")
set(CPACK_PACKAGE_DEFAULT_LOCATION "/")
set(CPACK_PACKAGE_DESCRIPTION_FILE "/usr/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Pretty Printer for Modern C++")
set(CPACK_PACKAGE_FILE_NAME "pprint-1.0.0")
set(CPACK_PACKAGE_HOMEPAGE_URL "https://github.com/p-ranav/pprint")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "pprint 1.0.0")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "pprint 1.0.0")
set(CPACK_PACKAGE_NAME "pprint")
set(CPACK_PACKAGE_RELOCATABLE "true")
set(CPACK_PACKAGE_VENDOR "Humanity")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_RESOURCE_FILE_LICENSE "/usr/share/cmake-3.22/Templates/CPack.GenericLicense.txt")
set(CPACK_RESOURCE_FILE_README "/usr/share/cmake-3.22/Templates/CPack.GenericDescription.txt")
set(CPACK_RESOURCE_FILE_WELCOME "/usr/share/cmake-3.22/Templates/CPack.GenericWelcome.txt")
set(CPACK_RPM_PACKAGE_SOURCES "ON")
set(CPACK_SET_DESTDIR "OFF")
set(CPACK_SOURCE_GENERATOR "TGZ;TXZ")
set(CPACK_SOURCE_IGNORE_FILES ".*\\.suo;.*\\.user;.*\\.userosscache;.*\\.sln\\.docstates;.*\\.userprefs;[Dd]ebug/;[Dd]ebugPublic/;[Rr]elease/;[Rr]eleases/;x64/;x86/;bld/;[Bb]in/;[Oo]bj/;[Ll]og/;\\.vs/;\\.vscode/;[Tt]est[Rr]esult.*/;[Bb]uild[Ll]og\\..*;.*\\.VisualState\\.xml;TestResult\\.xml;[Dd]ebugPS/;[Rr]eleasePS/;dlldata\\.c;project\\.lock\\.json;project\\.fragment\\.lock\\.json;artifacts/;.*_i\\.c;.*_p\\.c;.*_i\\.h;.*\\.ilk;.*\\.meta;.*\\.obj;.*\\.pch;.*\\.pdb;.*\\.pgc;.*\\.pgd;.*\\.rsp;.*\\.sbr;.*\\.tlb;.*\\.tli;.*\\.tlh;.*\\.tmp;.*\\.tmp_proj;.*\\.log;.*\\.vspscc;.*\\.vssscc;\\.builds;.*\\.pidb;.*\\.svclog;.*\\.scc;_Chutzpah.*;ipch/;.*\\.aps;.*\\.ncb;.*\\.opendb;.*\\.opensdf;.*\\.sdf;.*\\.cachefile;.*\\.VC\\.db;.*\\.VC\\.VC\\.opendb;.*\\.psess;.*\\.vsp;.*\\.vspx;.*\\.sap;$tf/;.*\\.gpState;_ReSharper.*/;.*\\.[Rr]e[Ss]harper;.*\\.DotSettings\\.user;\\.JustCode;_TeamCity.*;.*\\.dotCover;_NCrunch_.*;\\..*crunch.*\\.local\\.xml;nCrunchTemp_.*;.*\\.mm\\..*;AutoTest\\.Net/;\\.sass-cache/;[Ee]xpress/;DocProject/buildhelp/;DocProject/Help/.*\\.HxT;DocProject/Help/.*\\.HxC;DocProject/Help/.*\\.hhc;DocProject/Help/.*\\.hhk;DocProject/Help/.*\\.hhp;DocProject/Help/Html2;DocProject/Help/html;publish/;.*\\.[Pp]ublish\\.xml;.*\\.azurePubxml;.*\\.publishproj;PublishScripts/;.*\\.nupkg;.*.*/packages/.*;!.*.*/packages/build/;.*\\.nuget\\.props;.*\\.nuget\\.targets;csx/;.*\\.build\\.csdef;ecf/;rcf/;AppPackages/;BundleArtifacts/;Package\\.StoreAssociation\\.xml;_pkginfo\\.txt;.*\\.[Cc]ache;!.*\\.[Cc]ache/;ClientBin/;~$.*;.*~;.*\\.dbmdl;.*\\.dbproj\\.schemaview;.*\\.jfm;.*\\.pfx;.*\\.publishsettings;node_modules/;orleans\\.codegen\\.cs;Generated_Code/;_UpgradeReport_Files/;Backup.*/;UpgradeLog.*\\.XML;UpgradeLog.*\\.htm;.*\\.mdf;.*\\.ldf;.*\\.rdl\\.data;.*\\.bim\\.layout;.*\\.bim_.*\\.settings;FakesAssemblies/;.*\\.GhostDoc\\.xml;\\.ntvs_analysis\\.dat;.*\\.plg;.*\\.opt;.*.*/.*\\.HTMLClient/GeneratedArtifacts;.*.*/.*\\.DesktopClient/GeneratedArtifacts;.*.*/.*\\.DesktopClient/ModelManifest\\.xml;.*.*/.*\\.Server/GeneratedArtifacts;.*.*/.*\\.Server/ModelManifest\\.xml;_Pvt_Extensions;\\.paket/paket\\.exe;paket-files/;\\.fake/;\\.idea/;.*\\.sln\\.iml;\\.cr/;__pycache__/;.*\\.pyc;build.*;analysis-cppcheck-build-dir;ideas;desktop\\.iniimages/;.editorconfig;.git;.gitignore;.travis.yml;.appveyor.yml")
set(CPACK_SOURCE_INSTALLED_DIRECTORIES "/hqzn/rp/pprint;/")
set(CPACK_SOURCE_OUTPUT_CONFIG_FILE "/hqzn/rp/pprint/CPackSourceConfig.cmake")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "pprint-1.0.0")
set(CPACK_SOURCE_TOPLEVEL_TAG "Linux-Source")
set(CPACK_STRIP_FILES "")
set(CPACK_SYSTEM_NAME "Linux")
set(CPACK_THREADS "1")
set(CPACK_TOPLEVEL_TAG "Linux-Source")
set(CPACK_WIX_SIZEOF_VOID_P "8")

if(NOT CPACK_PROPERTIES_FILE)
  set(CPACK_PROPERTIES_FILE "/hqzn/rp/pprint/CPackProperties.cmake")
endif()

if(EXISTS ${CPACK_PROPERTIES_FILE})
  include(${CPACK_PROPERTIES_FILE})
endif()
