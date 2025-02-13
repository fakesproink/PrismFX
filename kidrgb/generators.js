
Blockly.JavaScript['rectangle'] = function(block) {
  var x1  = Blockly.JavaScript.valueToCode(block, 'X1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
  var y1  = Blockly.JavaScript.valueToCode(block, 'Y1'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
  var x2  = Blockly.JavaScript.valueToCode(block, 'X2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
  var y2  = Blockly.JavaScript.valueToCode(block, 'Y2'     , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
  var col = Blockly.JavaScript.valueToCode(block, 'COLOR'  , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
  var fil = Blockly.JavaScript.valueToCode(block, 'FILL'   , Blockly.JavaScript.ORDER_ASSIGNMENT) || 'false';
  return 'DEV_SPI.LCD_SPI('+block.getFieldValue('CHANNEL')+','+block.getFieldValue('ADDRESS')+',20,4).rectangle('+x1+','+y1+','+x2+','+y2+','+col+','+fil+');\n';
};

Blockly.JavaScript['picker'] = function(block) {
	var col = block.getFieldValue('COLOR');
	var num = parseInt(col.substring(1),16); 
	return [num, Blockly.JavaScript.ORDER_ATOMIC];
};

Blockly.JavaScript['lcd_spi_20x4.clear'] = function(block) {
	var col = Blockly.JavaScript.valueToCode(block, 'COLOR' , Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var rot = block.getFieldValue('Rotation');
	return 'DEV_SPI.LCD_SPI('+block.getFieldValue('CHANNEL')+','+block.getFieldValue('ADDRESS')+',20,4).clear('+rot+','+col+');\n';
};

Blockly.JavaScript['lcd_spi_20x4.print'] = function(block) {
	var argument0 = Blockly.JavaScript.valueToCode(block, 'VALUE', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var column = Blockly.JavaScript.valueToCode(block, 'COLUMN', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var row = Blockly.JavaScript.valueToCode(block, 'ROW', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var tiny = Blockly.JavaScript.valueToCode(block, 'Tiny', Blockly.JavaScript.ORDER_ASSIGNMENT) || 'false';

	return 'DEV_SPI.LCD_SPI(' + block.getFieldValue('CHANNEL') + ',' + block.getFieldValue('ADDRESS') + ',20,4).print(' + column + ',' + row + ',' + argument0 + ',' + tiny +');\n';
};

Blockly.JavaScript['lcd_spi_20x4.print_prec'] = function(block) {
	var argument0 = Blockly.JavaScript.valueToCode(block, 'VALUE', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var column = Blockly.JavaScript.valueToCode(block, 'COLUMN', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var row = Blockly.JavaScript.valueToCode(block, 'ROW', Blockly.JavaScript.ORDER_ASSIGNMENT) || '0';
	var prec = parseInt(block.getFieldValue('PREC'));

	if (prec < 0) {
		prec = 0;
	}
	if (prec > 4) {
		prec = 4;
	}

	return 'DEV_SPI.LCD_SPI(' + block.getFieldValue('CHANNEL') + ', ' + block.getFieldValue('ADDRESS') + ', 20, 4).print(' + column + ', ' + row + ', ' + argument0 + ', ' + prec + ');\n';
};

Blockly.JavaScript["lcd_spi_20x4.column_number"] = function(block) {
	// Numeric value.
	var code = parseFloat(block.getFieldValue('VALUE'));
	var order = code >= 0 ? Blockly.JavaScript.ORDER_ATOMIC : Blockly.JavaScript.ORDER_UNARY_NEGATION;
	return [code, order];
};

Blockly.JavaScript["lcd_spi_20x4.row_number"] = function(block) {
	return Blockly.JavaScript["lcd_spi_20x4.column_number"](block);
};

Blockly.JavaScript["lcd_spi_20x4.string_number"] = function(block) {
	return [
		'(char *)"' + block.getFieldValue('VALUE') + '"',
		Blockly.JavaScript.ORDER_ATOMIC
	];
}

Blockly.JavaScript["lcd_spi_20x4.number"] = function(block) {
	return Blockly.JavaScript["lcd_spi_20x4.column_number"](block);
}

Blockly.JavaScript['lcd_spi_20x4.backlight'] = function(block) {
	return 'DEV_SPI.LCD_SPI(' + block.getFieldValue('CHANNEL') + ', ' + block.getFieldValue('ADDRESS') + ', 20, 4).backlight(' + block.getFieldValue('STATUS') + ');\n';
}