var thisOrganizName = "��� \"������������������\"";
var banks = ["�������� Txxxxxxx.arj", "�������� Txxxxxxx.arj", "���� \"�����\" PBANK.dbf"];
var fileTypes = ["T*.arj", "T*.dbf", "*.dbf"];
var onePackInFile = [true, true, true];
var functionNames = ["oshchadKvyt", "oshchadKvytDbf", "nadraKvyt"];

var oshchadKvyt = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! ������� ��������� !!!", "�������� �� ������ ��������� ���� '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	//form.execProcess("..\\arj -y e " + form.fileName(filePath) + " *.dbf");
	form.execProcess("..\\7z e -tARJ "+form.fileName(filePath)+" *.dbf -y");
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! ������� ��'������� !!!", "�������� �� ������ ��'�������� �� �������� ���� �����.");
		form.closeDbfConnection();
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("NRA");
		dateStr = form.queryDateValue("DATAO");
		sumStr = form.queryValue("SUMA");
		kasaStr = form.queryValue("NKA");
		fontColor = clBlack;
		if (rahStr<1 || sumStr<0.01)
				fontColor = clRed;
		form.populateTable(row, 1, rahStr, fontColor);
		form.populateTable(row, 2, dateStr, fontColor);
		form.populateTable(row, 3, sumStr, fontColor);
		form.populateTable(row, 4, kasaStr, fontColor);
		form.populateTable(row, 5, "", fontColor);
		row++;
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! ��������� ���������� !!!", "�������� ��������� ���������� �������� ���� ����� ���������.");
}

var oshchadKvytDbf = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! ������� ��������� !!!", "�������� �� ������ ��������� ���� '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! ������� ��'������� !!!", "�������� �� ������ ��'�������� �� �������� ���� �����.");
		form.closeDbfConnection();
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("NRA");
		dateStr = form.queryDateValue("DATAO");
		sumStr = form.queryValue("SUMA");
		kasaStr = form.queryValue("NKA");
		fontColor = clBlack;
		if (rahStr<1 || sumStr<0.01)
				fontColor = clRed;
		form.populateTable(row, 1, rahStr, fontColor);
		form.populateTable(row, 2, dateStr, fontColor);
		form.populateTable(row, 3, sumStr, fontColor);
		form.populateTable(row, 4, kasaStr, fontColor);
		form.populateTable(row, 5, "", fontColor);
		row++;
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! ��������� ���������� !!!", "�������� ��������� ���������� �������� ���� ����� ���������.");
}

var nadraKvyt = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! ������� ��������� !!!", "�������� �� ������ ��������� ���� '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	//form.execProcess("..\\arj -y e " + form.fileName(filePath) + " *.dbf");
	form.execProcess("..\\7z e -tARJ "+form.fileName(filePath)+" *.dbf -y");
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! ������� ��'������� !!!", "�������� �� ������ ��'�������� �� �������� ���� �����.");
		form.closeDbfConnection();
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("LS_PLAT");
		dateStr = form.queryDateValue("DAT_BANK");
		sumStr = form.queryValue("SUMMA");
		kasaStr = 1;
		fontColor = clBlack;
		if (rahStr<1 || sumStr<0.01)
				fontColor = clRed;
		form.populateTable(row, 1, rahStr, fontColor);
		form.populateTable(row, 2, dateStr, fontColor);
		form.populateTable(row, 3, sumStr, fontColor);
		form.populateTable(row, 4, kasaStr, fontColor);
		form.populateTable(row, 5, "", fontColor);
		row++;
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! ��������� ���������� !!!", "�������� ��������� ���������� �������� ���� ����� ���������.");
}