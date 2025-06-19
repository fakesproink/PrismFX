Blockly.Blocks["led16x8.gfx_print"] = {
	init: function() {
		this.appendDummyInput().appendField("print GFX");
		this.appendValueInput('Str').setCheck('String').appendField("Str");
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip("");
		this.setHelpUrl("");
}};

Blockly.Blocks["basic_led16x8_gfx_clr"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.BASIC_LED16X8_CLR_GFX_TITLE);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.BASIC_LED16X8_CLR_GFX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.BASIC_LED16X8_CLR_GFX_HELPURL);
	}
};

Blockly.Blocks["led16x8.gfx_dot"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.LED16X8_GFX_TITLE);

		var x = [];
		var y = [];
		for (var i = 0;i < 16; i++) {
			x.push([String(i), String(i)]);
		}
		for (var i = 0;i < 8; i++) {
			y.push([String(i), String(i)]);
		}
		this.appendDummyInput()
			.appendField("X=")
			.appendField(new Blockly.FieldDropdown(x), 'X');
		this.appendDummyInput()
			.appendField("Y=")
			.appendField(new Blockly.FieldDropdown(y), 'Y');

		this.appendDummyInput()
			.appendField(Blockly.Msg.GFX_TYPE)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.GFX_TYPE_PLOT, "0"],
				[Blockly.Msg.GFX_TYPE_UNPLOT, "1"],
				[Blockly.Msg.GFX_TYPE_TOGGLE, "2"],
				[Blockly.Msg.GFX_TYPE_BAR, "3"]
			]), 'TYPE');

		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.LED16X8_GFX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.LED16X8_GFX_HELPURL);
	}
};

Blockly.Blocks["led16x8.gfx_var"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.LED16X8_GFX_TITLE);
		this.appendDummyInput()
			.appendField("X=");
		this.appendValueInput('X')
					.setCheck('Number');
		this.appendDummyInput()
			.appendField("Y=");
		this.appendValueInput('Y')
					.setCheck('Number');

		this.appendDummyInput()
			.appendField(Blockly.Msg.GFX_TYPE)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.GFX_TYPE_PLOT, "0"],
				[Blockly.Msg.GFX_TYPE_UNPLOT, "1"],
				[Blockly.Msg.GFX_TYPE_TOGGLE, "2"],
				[Blockly.Msg.GFX_TYPE_BAR, "3"]
			]), 'TYPE');

		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.LED16X8_GFX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.LED16X8_GFX_HELPURL);
	}
};

Blockly.Blocks["led16x8.gfx_move"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.LED16X8_MOVE_TITLE);

		this.appendDummyInput()
			.appendField(Blockly.Msg.GFX_TYPE)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.GFX_TYPE_ROTATE, "0"],
				[Blockly.Msg.GFX_TYPE_SHIFT, "1"]
			]), 'MOVE');

		this.appendDummyInput()
			.appendField(Blockly.Msg.GFX_TYPE)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.GFX_TYPE_RIGHT, "0"],
				[Blockly.Msg.GFX_TYPE_LEFT, "1"]
			]), 'TYPE');

		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.LED16X8_MOVE_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.LED16X8_MOVE_HELPURL);
	}
};

Blockly.Blocks["led16x8.gfx_stack"] = {
	init: function() {
		this.appendDummyInput()
			.appendField(Blockly.Msg.LED16X8_GFX_STACK_TITLE);
		this.appendDummyInput()
			.appendField("X=");
		this.appendValueInput('X')
					.setCheck('Number');
		this.appendDummyInput()
			.appendField("Y=");
		this.appendValueInput('Y')
					.setCheck('Number');

		this.appendDummyInput()
			.appendField(Blockly.Msg.GFX_TYPE)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.GFX_TYPE_PLOT, "0"],
				[Blockly.Msg.GFX_TYPE_UNPLOT, "1"],
			]), 'TYPE');

		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.LED16X8_GFX_STACK_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.LED16X8_GFX_STACK_HELPURL);
	}
};