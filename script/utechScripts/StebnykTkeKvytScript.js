
var thisOrganizName = "ДКП \"Стебниктеплокомуненерго\"";
var banks = ["Укрпошта","Ощадбанк"];
var fileTypes = ["*.dbf", "*.dbf"];
var onePackInFile = [false, false];
var functionNames = ["poshtaKvyt","oshchadKvyt"];

var poshtaKvyt = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! Помилка копіювання !!!", "Програма не змогла зкопіювати файл '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! Помилка під'єднання !!!", "Програма не змогла під'єднатись до зовнішньої бази даних.");
		form.closeDbfConnection();
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("RAH");
		dateStr = form.queryDateValue("DATE");
		sumStr = form.queryValue("OPLATA");
		kasaStr = "-";
		if (rahStr!="" && dateStr!="" && sumStr!="" && kasaStr!=""){
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
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! Завершено втягування !!!", "Програма завершила втягування зовнішньої бази даних квитанцій.");
}

var oshchadKvyt = function(filePath)
{
	if (!form.copyFile(filePath, form.binDirPath()+"/"+kvytDir+"/"+form.fileName(filePath))){
		form.showMessage("!!! Помилка копіювання !!!", "Програма не змогла зкопіювати файл '"+filePath+"'");
		return;
	}
	form.setCurrentDir(form.binDirPath() + "/" + kvytDir);
	if (!form.openDbfConnection(form.binDirPath()+"/"+kvytDir+"/"+form.pureFileName(filePath)+".dbf")){
		form.showMessage("!!! Помилка під'єднання !!!", "Програма не змогла під'єднатись до зовнішньої бази даних.");
		form.closeDbfConnection();
		return;
	}
	numRows = form.queryNumRows();
	form.setTableRowCount(numRows);
	row = 0;
	while (form.queryNext()){
		if (row+1>numRows)
			form.setTableRowCount(row+1);
		rahStr = form.queryValue("N_NAB");
		dateStr = form.queryDateValue("DATAO");
		sumStr = form.queryValue("SUMA");
		kasaStr = form.queryValue("NKA");
		if (rahStr!="" && dateStr!="" && sumStr!="" && kasaStr!=""){
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
	}
	if (row!=numRows)
		form.setTableRowCount(row);
	form.closeDbfConnection();
	form.showMessage("!!! Завершено втягування !!!", "Програма завершила втягування зовнішньої бази даних квитанцій.");
}
