CONFIG		+= release
CONFIG		-= debug debug_and_release debug_and_release_target

message(Configuring project)

!exists(./tmp){
	message(No directory ./tmp)
	system(mkdir tmp){message(Directory ./tmp created.)}
	else{
		error(Directory ./tmp not created!!! Create directory ./tmp yourSelf.)
	}
}
!exists(./tmp/demo){
	message(No directory ./tmp/demo)
	system(mkdir .\tmp\demo){message(Directory ./tmp/demo created.)}
	else{
		error(Directory ./tmp/demo not created!!! Create directory ./tmp/demo yourSelf.)
	}
}

!exists(./tmp/tkeQtSqlBrowser){
	message(No directory ./tmp/tkeQtSqlBrowser)
	system(mkdir .\tmp\tkeQtSqlBrowser){message(Directory ./tmp/tkeQtSqlBrowser created.)}
	else{
		error(Directory ./tmp/tkeQtSqlBrowser not created!!! Create directory ./tmp/tkeQtSqlBrowser yourSelf.)
	}
}

system(configure)

TEMPLATE    = subdirs
SUBDIRS     = tkeAbonent\
	tkeOrganizacii \
	install/configure \
	tkeQtSqlBrowser #\
#	tkeAbonent/tkeAbonentDemo \
#	tkeOrganizacii/tkeOrganizaciiDemo

message(Project configured OK!)