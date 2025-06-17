function prismfx_init(_this) {
	_this.appendDummyInput()
		.appendField(Blockly.Msg.PRISMFX_TITLE);

	// device addess 0x20 to 0x23 for channel 0, addess 0x20 to 0x27 for channel 1 to 64
	_this.appendDummyInput()
		.appendField(Blockly.Msg.ADDRESS)
		.appendField(new Blockly.FieldDropdown(function() {
			try {
				if ((typeof(this.sourceBlock_) != "undefined") && (typeof(this.sourceBlock_.inputList) != "undefined")) {
					var inputlist = this.sourceBlock_.inputList;
					var selected_channel = parseInt(inputlist[1].fieldRow[1].value_);
					return Blockly.mcp23s17_address_dropdown_menu(selected_channel);
				}
			} catch (e) {

			}
			// default
			return Blockly.mcp23s17_address_dropdown_menu(0);
		}), 'ADDRESS');
}

Blockly.Blocks["prismfx.clear"] = {
	init: function() {
		// init
		prismfx_init(this);
        this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_Clear);
        this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_Rotation)
					.appendField(new Blockly.FieldDropdown([
					[Blockly.Msg.PRISMFX_Clear_Top   , "0"],
					[Blockly.Msg.PRISMFX_Clear_Left  , "1"],
					[Blockly.Msg.PRISMFX_Clear_Bottom, "2"],
					[Blockly.Msg.PRISMFX_Clear_Right , "3"]
					]), "Rotation");
		this.appendValueInput("COLOR").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_Clear_ToolTip);
		this.setHelpUrl(Blockly.Msg.PRISMFX_Clear_HelpURL);
}};

Blockly.Blocks["prismfx.setTextColor"] = {
	init: function() {
		prismfx_init(this);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_TEXT_COLOR_TITLE);
		this.appendValueInput("Fore").setCheck("Number").appendField(Blockly.Msg.PRISMFX_TEXT_COLOR_Foreground);
		this.appendValueInput("Back").setCheck("Number").appendField(Blockly.Msg.PRISMFX_TEXT_COLOR_Background);
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_TEXT_COLOR_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_TEXT_COLOR_HELPURL);
}};

Blockly.Blocks["prismfx.print"] = {
	init: function() {
		prismfx_init(this);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PRINT_TITLE);
		this.appendValueInput("Row").setCheck("Number" ).appendField(Blockly.Msg.PRISMFX_PRINT_Row);
		this.appendValueInput("Col").setCheck("Number" ).appendField(Blockly.Msg.PRISMFX_PRINT_Col);
		this.appendValueInput('Str').setCheck("String" ).appendField(Blockly.Msg.PRISMFX_PRINT_Str);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PRINT_Size).appendField(new Blockly.FieldDropdown(
			[ [ Blockly.Msg.PRISMFX_PRINT_Small   , "0" ], [ Blockly.Msg.PRISMFX_PRINT_Medium, "1" ] ]), "Size");
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_PRINT_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_PRINT_HELPURL);
}};

Blockly.Blocks["prismfx.printGFX"] = {
	init: function() {
		prismfx_init(this);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PRINTGFX_TITLE);
		this.appendValueInput("Col").setCheck("Number" ).appendField(Blockly.Msg.PRISMFX_X);
		this.appendValueInput("Row").setCheck("Number" ).appendField(Blockly.Msg.PRISMFX_Y);
		this.appendValueInput('Str').setCheck("String" ).appendField(Blockly.Msg.PRISMFX_PRINTGFX_MSG);
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_PRINTGFX_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_PRINTGFX_HELPURL);
}};

Blockly.Blocks['prismfx.point'] = {
	init: function() {
	  prismfx_init(this);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_POINT_TITLE);
	  this.appendValueInput("X1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X);
	  this.appendValueInput("Y1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y);
	  this.appendValueInput("COLOR").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
	  this.setInputsInline(true);
	  this.setPreviousStatement(true, null);
	  this.setNextStatement(true, null);
	  this.setColour(150);
	  this.setTooltip(Blockly.Msg.PRISMFX_POINT_TOOLTIP);
	  this.setHelpUrl(Blockly.Msg.PRISMFX_POINT_HELPURL);
  }};
  
  Blockly.Blocks['prismfx.line'] = {
	init: function() {
	  prismfx_init(this);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_LINE_TITLE);
	  this.appendValueInput("X1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X1);
	  this.appendValueInput("Y1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y1);
	  this.appendValueInput("X2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X2);
	  this.appendValueInput("Y2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y2);
	  this.appendValueInput("COLOR").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
	  this.setInputsInline(true);
	  this.setPreviousStatement(true, null);
	  this.setNextStatement(true, null);
	  this.setColour(150);
	  this.setTooltip(Blockly.Msg.PRISMFX_LINE_TOOLTIP);
	  this.setHelpUrl(Blockly.Msg.PRISMFX_LINE_HELPURL);
  }};
  
  Blockly.Blocks['prismfx.rectangle'] = {
	init: function() {
	  prismfx_init(this);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_RECT_TITLE);
	  this.appendValueInput("X1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X1);
	  this.appendValueInput("Y1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y1);
	  this.appendValueInput("X2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X2);
	  this.appendValueInput("Y2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y2);
	  this.appendValueInput("COLOR").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_Fill).appendField(new Blockly.FieldDropdown(
		[ [ Blockly.Msg.PRISMFX_Fill, "true" ], [ Blockly.Msg.PRISMFX_Outline, "false" ] ]), "Fill");
	  this.setInputsInline(true);
	  this.setPreviousStatement(true, null);
	  this.setNextStatement(true, null);
	  this.setColour(150);
	  this.setTooltip(Blockly.Msg.PRISMFX_RECT_TOOLTIP);
	  this.setHelpUrl(Blockly.Msg.PRISMFX_RECT_HELPURL);
  }};

  Blockly.Blocks['prismfx.triangle'] = {
	init: function() {
	  prismfx_init(this);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_TRIANGLE_TITLE);
	  this.appendValueInput("X1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X1);
	  this.appendValueInput("Y1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y1);
	  this.appendValueInput("X2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X2);
	  this.appendValueInput("Y2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y2);
	  this.appendValueInput("X3").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X3);
	  this.appendValueInput("Y3").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y3);
	  this.appendValueInput("COLOR").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_Fill).appendField(new Blockly.FieldDropdown(
		[ [ Blockly.Msg.PRISMFX_Fill, "true" ], [ Blockly.Msg.PRISMFX_Outline, "false" ] ]), "Fill");
	  this.setInputsInline(true);
	  this.setPreviousStatement(true, null);
	  this.setNextStatement(true, null);
	  this.setColour(150);
	  this.setTooltip(Blockly.Msg.PRISMFX_TRIANGLE_TOOLTIP);
	  this.setHelpUrl(Blockly.Msg.PRISMFX_TRIANGLE_HELPURL);
  }};
  
Blockly.Blocks['prismfx.picker'] = {
  init: function() {
    this.appendDummyInput()
        .appendField(new Blockly.FieldColour("#f70000"), "COLOR");
    this.setOutput(true, "Number");
    this.setColour(230);
	this.setTooltip(Blockly.Msg.PRISMFX_PICKER_TOOLTIP);
    this.setHelpUrl(Blockly.Msg.PRISMFX_PICKER_HELPURL);
}};


Blockly.Blocks['prismfx.num2str'] = {
	init: function() {
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_NUM2STR_TITLE);
		//read this in generator V       type check V                     V display this in block
		this.appendValueInput("Value" ).setCheck("Number").appendField(Blockly.Msg.PRISMFX_NUM2STR_Value);
		this.appendValueInput("Width" ).setCheck("Number").appendField(Blockly.Msg.PRISMFX_NUM2STR_Width);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_NUM2STR_Decimals)
	 		.appendField(new Blockly.FieldDropdown([
	 			["0", "0"],
				["1", "1"],
	  			["2", "2"],
	  			["3", "3"],
	  			["4", "4"],
	  			["5", "5"],
	  			["6", "6"]
				]), "Decimals");
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_NUM2STR_Format)
			.appendField(new Blockly.FieldDropdown([
				[Blockly.Msg.PRISMFX_NUM2STR_integer        , "0"],
				[Blockly.Msg.PRISMFX_NUM2STR_hexadecimal	, "1"],
				[Blockly.Msg.PRISMFX_NUM2STR_hexw0s			, "2"],
				[Blockly.Msg.PRISMFX_NUM2STR_fixed_point    , "3"],
				[Blockly.Msg.PRISMFX_NUM2STR_exponential   	, "4"]
			   ]), "Format");
		this.setInputsInline(true);
		this.setOutput(true, "String");
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_NUM2STR_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_NUM2STR_HELPURL);
}};

Blockly.Blocks["prismfx.initplot"] = {
	init: function() {
		// init
		prismfx_init(this);
        this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PLOT_INIT);
        this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PLOT_index).appendField(new Blockly.FieldDropdown([	// <-- name displayed in block
				["1", "0"],
				["2", "1"],
				["3", "2"]]), "inx");	// << name for generator
		this.appendValueInput("nam").setCheck("String").appendField(Blockly.Msg.PRISMFX_PLOT_variable_name);
		this.appendValueInput("unt").setCheck("String").appendField(Blockly.Msg.PRISMFX_PLOT_units);
		this.appendValueInput("min").setCheck("Number").appendField(Blockly.Msg.PRISMFX_PLOT_plotMin);
		this.appendValueInput("max").setCheck("Number").appendField(Blockly.Msg.PRISMFX_PLOT_plotMax);
		this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PLOT_decimals).appendField(new Blockly.FieldDropdown([	// <-- name displayed in block
				["0", "0"],
				["1", "1"],
				["2", "2"],
				["3", "3"],
				["4", "4"]]), "dec");	// << name for generator
		this.appendValueInput("col").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Color);
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_PLOT_INIT_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_PLOT_INIT_HELPURL);
}};

Blockly.Blocks["prismfx.plotdata"] = {
	init: function() {
		// init
		prismfx_init(this);
        this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_PLOT_DATA);
		this.appendValueInput("v1").setCheck("Number").appendField(Blockly.Msg.PRISMFX_PLOT_DATA1);
		this.appendValueInput("v2").setCheck("Number").appendField(Blockly.Msg.PRISMFX_PLOT_DATA2);
		this.appendValueInput("v3").setCheck("Number").appendField(Blockly.Msg.PRISMFX_PLOT_DATA3);
		this.setInputsInline(true);
		this.setPreviousStatement(true);
		this.setNextStatement(true);
		this.setColour(160);
		this.setTooltip(Blockly.Msg.PRISMFX_PLOT_DATA_TOOLTIP);
		this.setHelpUrl(Blockly.Msg.PRISMFX_PLOT_DATA_HELPURL);
}};

Blockly.Blocks['prismfx.image'] = {
	init: function() {
	  prismfx_init(this);
	  this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_IMAGE_TITLE);
	  this.appendValueInput("X").setCheck("Number").appendField(Blockly.Msg.PRISMFX_X);
	  this.appendValueInput("Y").setCheck("Number").appendField(Blockly.Msg.PRISMFX_Y);
      this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_IMAGE_ID).appendField(new Blockly.FieldDropdown([	// <-- name displayed in block
				["image1", "1"],
				["image2", "2"]]), "img");	// << name for generator
      this.appendDummyInput().appendField(Blockly.Msg.PRISMFX_IMAGE_SCALE).appendField(new Blockly.FieldDropdown([	// <-- name displayed in block
				["Full Size", "12"],
				["3/4  Size", "9"],
				["2/3  Size", "8"],
				["Half Size", "6"],
				["1/3  Size", "4"],
				["1/4  Size", "3"]]), "siz");	// << name for generator
	  this.setInputsInline(true);
	  this.setPreviousStatement(true, null);
	  this.setNextStatement(true, null);
	  this.setColour(150);
	  this.setTooltip(Blockly.Msg.PRISMFX_IMAGE_TOOLTIP);
	  this.setHelpUrl(Blockly.Msg.PRISMFX_IMAGE_HELPURL);
  }};
 
