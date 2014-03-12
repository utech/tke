var thisOrganizName = "��� \"������������������\"";
var fileTypes = "*.dbf";
var functionNames = "zolochivTkeSubs";
var vydSubs = "CO";

var zolochivTkeSubs = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! ������� ��������� !!!", "�������� �� ������ ��������� ���� '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! ������� ��'������� !!!", "�������� �� ������ ��'������� �� �������� ���� �����.");
		form.closeDbfConnection();
		return;
	}
	var CO_GV=0;
	if (form.pureFileName(filePath).match(/^0007/))
		CO_GV = 1;
	else if (form.pureFileName(filePath).match(/^0101/) || form.pureFileName(filePath).match(/^0108/))
		CO_GV = 2;
	else if (form.pureFileName(filePath).match(/^d0007/))
		CO_GV = 3;
	else if (form.pureFileName(filePath).match(/^d0101/) || form.pureFileName(filePath).match(/^d0108/))
		CO_GV = 4;
	if (CO_GV == 0){
		form.closeDbfConnection();
		form.showMessage("!!! ������ ����� dbf ����� !!!", "������� ������� ���� �����䳿.\n������� ������� ���� �������:\n  - 0007*.dbf - �����. ����. ��;\n  - 0101*.dbf - �����. ����. ��;\n  - 0108*.dbf - �����. ����. ��.");
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("RASH");
		sumStr = form.queryValue("SUMMA");
		if (rahStr!="" && sumStr!=""){
			fontColor = clBlack;
			if (rahStr<1 || sumStr<0.01)
				fontColor = clRed;
			form.populateTable(row, 1, rahStr, fontColor);
			if (CO_GV == 1){
				form.populateTable(row, 2, sumStr, fontColor);
				form.populateTable(row, 3, "0,00", fontColor);
				form.populateTable(row, 4, "0,00", fontColor);
				form.populateTable(row, 5, "0,00", fontColor);
			}
			else if (CO_GV == 2){
				form.populateTable(row, 2, "0,00", fontColor);
				form.populateTable(row, 3, "0,00", fontColor);
				form.populateTable(row, 4, sumStr, fontColor);
				form.populateTable(row, 5, "0,00", fontColor);
			}
			else if (CO_GV == 3){
				form.populateTable(row, 2, "0,00", fontColor);
				form.populateTable(row, 3, sumStr, fontColor);
				form.populateTable(row, 4, "0,00", fontColor);
				form.populateTable(row, 5, "0,00", fontColor);
			}
			else if (CO_GV == 4){
				form.populateTable(row, 2, "0,00", fontColor);
				form.populateTable(row, 3, "0,00", fontColor);
				form.populateTable(row, 4, "0,00", fontColor);
				form.populateTable(row, 5, sumStr, fontColor);
			}
			row++;
		}
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! ��������� ���������� !!!", "�������� ��������� ���������� �������� ���� ����� �����䳿.");
}
