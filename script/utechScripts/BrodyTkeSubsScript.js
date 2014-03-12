
var thisOrganizName = "��� \"���������������������\"";
var fileTypes = "*.dbf";
var functionNames = "brodyTkeSubs";

var brodyTkeSubs = function(filePath)
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
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("RASH");
		sumStr = form.queryValue("SUBS");
		if (rahStr!="" && sumStr!=""){
			fontColor = clBlack;
			if (rahStr<1 || sumStr<0.01)
				fontColor = clRed;
			form.populateTable(row, 1, rahStr, fontColor);
			form.populateTable(row, 2, sumStr, fontColor);
			form.populateTable(row, 3, "0,00", fontColor);
			form.populateTable(row, 4, "0,00", fontColor);
			form.populateTable(row, 5, "0,00", fontColor);
			row++;
		}
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! ��������� ���������� !!!", "�������� ��������� ���������� �������� ���� ����� �����䳿.");
}
