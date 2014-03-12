@echo off
echo   --- qmake for tkeAbonent ---
cd tkeAbonent
qmake
cd tkeAbonentDemo
qmake
cd ../..

echo   --- qmake for tkeOrganizacii ---
cd tkeOrganizacii
qmake
cd tkeOrganizaciiDemo
qmake
cd ../..

echo   --- qmake for configure ---
cd install/configure
qmake
cd ../..

echo   --- qmake for tkeQtSqlBrowser ---
cd tkeQtSqlBrowser
qmake
cd ..

echo   --- qmake for tke parts done Ok ---