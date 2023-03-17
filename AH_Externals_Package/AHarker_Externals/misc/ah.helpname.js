inlets=1;
outlets=1;

this.box.message("border", 0);
this.box.message("ignoreclick", 1);

mgraphics.init();
mgraphics.relative_coords = 0;
mgraphics.autofill = 0;
var name = jsarguments[1];
var isfirsttime = true;

function paint()
{
	if(name){
    	with(mgraphics) 
    	{
    		move_to(4, 40);
    		select_font_face("Lato");
 			var textcolor = this.patcher.getattr("textcolor");
			set_source_rgba(textcolor);
       		set_font_size(48);
        	show_text(name);
			fill();
		}
	}
}