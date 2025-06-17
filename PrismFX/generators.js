Blockly.JavaScript['prismfx.clear'] = function(block) {
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR' , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var rot = block.getFieldValue('Rotation');
	return 'DEV_SPI.PrismFX('+block.getFieldValue('ADDRESS')+').clear('+rot+','+col+');\n';
};

Blockly.JavaScript['prismfx.setTextColor'] = function(block) {
	var fore = Blockly.JavaScript.valueToCode(block, 'Fore', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0xffffff';	// default white
	var back = Blockly.JavaScript.valueToCode(block, 'Back', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';			// default black
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').setTextColor(' + fore + ',' + back +');\n';
};

Blockly.JavaScript['prismfx.print'] = function(block) {
	var argument0 = Blockly.JavaScript.valueToCode(block, 'Str', Blockly.JavaScript.ORDER_ASSIGNMENT) || '""';
	var column = Blockly.JavaScript.valueToCode(block, 'Col', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var row = Blockly.JavaScript.valueToCode(block, 'Row', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var sz = block.getFieldValue('Size');
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').print(' + column + ',' + row + ',' + argument0 + ',' + sz +');\n';
};

Blockly.JavaScript['prismfx.printGFX'] = function(block) {
	var argument0 = Blockly.JavaScript.valueToCode(block, 'Str', Blockly.JavaScript.ORDER_ASSIGNMENT) || '""';
	var column = Blockly.JavaScript.valueToCode(block, 'Col', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var row = Blockly.JavaScript.valueToCode(block, 'Row', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').printGFX(' + column + ',' + row + ',' + argument0 +');\n';
};

Blockly.JavaScript['prismfx.point'] = function(block) {
	var x1  = Blockly.JavaScript.valueToCode(block, 'X1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y1  = Blockly.JavaScript.valueToCode(block, 'Y1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR'  , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return 'DEV_SPI.PrismFX('+block.getFieldValue('ADDRESS')+').point('+x1+','+y1+','+col+');\n';
};

Blockly.JavaScript['prismfx.line'] = function(block) {
	var x1  = Blockly.JavaScript.valueToCode(block, 'X1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y1  = Blockly.JavaScript.valueToCode(block, 'Y1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var x2  = Blockly.JavaScript.valueToCode(block, 'X2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y2  = Blockly.JavaScript.valueToCode(block, 'Y2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR'  , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return 'DEV_SPI.PrismFX('+block.getFieldValue('ADDRESS')+').line('+x1+','+y1+','+x2+','+y2+','+col+');\n';
};

Blockly.JavaScript['prismfx.rectangle'] = function(block) {
	var x1  = Blockly.JavaScript.valueToCode(block, 'X1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y1  = Blockly.JavaScript.valueToCode(block, 'Y1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var x2  = Blockly.JavaScript.valueToCode(block, 'X2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y2  = Blockly.JavaScript.valueToCode(block, 'Y2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR'  , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var fil = block.getFieldValue('Fill');
	return 'DEV_SPI.PrismFX('+block.getFieldValue('ADDRESS')+').rectangle('+x1+','+y1+','+x2+','+y2+','+col+','+fil+');\n';
};

Blockly.JavaScript['prismfx.triangle'] = function(block) {
	var x1  = Blockly.JavaScript.valueToCode(block, 'X1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y1  = Blockly.JavaScript.valueToCode(block, 'Y1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var x2  = Blockly.JavaScript.valueToCode(block, 'X2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y2  = Blockly.JavaScript.valueToCode(block, 'Y2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var x3  = Blockly.JavaScript.valueToCode(block, 'X3'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y3  = Blockly.JavaScript.valueToCode(block, 'Y3'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR'  , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var fil = block.getFieldValue('Fill');
	return 'DEV_SPI.PrismFX('+block.getFieldValue('ADDRESS')+').triangle('+x1+','+y1+','+x2+','+y2+','+x3+','+y3+','+col+','+fil+');\n';
};
	
Blockly.JavaScript['prismfx.picker'] = function(block) {
	var col = block.getFieldValue('COLOR');
	var num = parseInt(col.substring(1),16); 
	return [num, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['prismfx.num2str'] = function(block) {
	var fmt = block.getFieldValue("Format");
	var dec = block.getFieldValue("Decimals");
	var wid = Blockly.JavaScript.valueToCode(block, 'Width', Blockly.JavaScript.ORDER_ASSIGNMENT) || '6';
	var val = Blockly.JavaScript.valueToCode(block, 'Value', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return ['PrismFX::num2str('+ val +','+ wid +','+ dec +','+ fmt +')', Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['prismfx.initplot'] = function(block) {
	var ind = block.getFieldValue("inx");
	var nam = Blockly.JavaScript.valueToCode(block, 'nam', Blockly.JavaScript.ORDER_ASSIGNMENT) || '"?"';
	var unt = Blockly.JavaScript.valueToCode(block, 'unt', Blockly.JavaScript.ORDER_ASSIGNMENT) || '"?"';
	var min = Blockly.JavaScript.valueToCode(block, 'min', Blockly.JavaScript.ORDER_ASSIGNMENT) || 'NAN';	// defaults to 'Not A Number', plot will auto range
	var max = Blockly.JavaScript.valueToCode(block, 'max', Blockly.JavaScript.ORDER_ASSIGNMENT) || 'NAN';	// defaults to 'Not A Number', plot will auto range
	var dex = block.getFieldValue("dec");
	var col = Blockly.JavaScript.valueToCode(block, 'col', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0xfffffa';	// default will be changed to R, G, or B
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').initPlot(' + ind + ',' + nam + ',' + unt + ',' + min + ',' + max + ',' + dex + ',' + col +');\n';
};

Blockly.JavaScript['prismfx.plotdata'] = function(block) {
	var v1 = Blockly.JavaScript.valueToCode(block, 'v1', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var v2 = Blockly.JavaScript.valueToCode(block, 'v2', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var v3 = Blockly.JavaScript.valueToCode(block, 'v3', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').plotPoint(' + v1 + ',' + v2 + ',' + v3 + ');\n';
};

Blockly.JavaScript['prismfx.image'] = function(block) {
	var x   = Blockly.JavaScript.valueToCode(block, 'X', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var y   = Blockly.JavaScript.valueToCode(block, 'Y', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var img = block.getFieldValue("img");
	var sz  = block.getFieldValue("siz");
	return 'DEV_SPI.PrismFX(' + block.getFieldValue('ADDRESS') + ').drawImage(' + x + ',' + y + ',' + img + ',' + sz +');\n';
};
