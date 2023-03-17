
/*
 
 Gauss Editor
 
 This is an ediotr primarily for use with randomvals and randomvals~
 
 You can set the mean and dev values of multiple gaussian curves, as well as adding and removing curves.
 
 Output is also provided with a list of values for the current curve. 
 
 Note that all mean values are dealt with in the range 0 - 1 and any scaling must be done externally.
 
 */

inlets = 1; 
outlets = 2;

////////////// internal state ///////////// 

var mean_list = new Array();
var dev_list = new Array();
var weight_list = new Array();
var num_curves = 1;
var sel_curve = 0;

mean_list[0] = 0.5;
dev_list[0] = 6;
weight_list[0] = 1.;

var width = box.rect[2] - box.rect[0];
var height = box.rect[3] - box.rect[1];

var x_scale = 1.;
var y_scale = 1.;

var drag_mode = 0;

//////////// visual appearance //////////// 

var max_height = 0.8;

// tag radius

var tag_r = 0.025;

// handle vars 

var handle_h_ratio = 0.55;
var handle_h = handle_h_ratio * (1 - max_height);
var handle_w = 0.06;
var handle_l = 3.;

// dev conversion vars

var dev_curve = 0.333;
var dev_max = 6;
var dev_min_point_ratio = 1.5;
var dev_min_point = handle_w * dev_min_point_ratio;
var dev_max_point = 0.45;

// weight conversion

var weight_curve = 0.75;

// colours

var m_brgb = [0, 0, 0];
var m_frgb = [0.4, 0, 0];
var m_frgb2 = [0, 1, 0, 0.6];
var m_frgb3 = [0, 0.8, 1];
var m_frgb4 = [0.7, 0.7, 0.7];
var m_frgb5 = [0.95, 0.95, 0];

// interface vars

var tag_hot_ratio = 1.8;
var tag_sel_sq_dist = (tag_r * tag_hot_ratio) *  (tag_r * tag_hot_ratio);

sketch.default2d();
onresize (width, height);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Erfc routine /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Algorithm from Milton Abramowitz and Irene A. Stegun "Handbook of Mathematical Functions: with Formulas, Graphs, and Mathematical Tables"

function erf(x)
{
	// Save the sign of x

	var sign = 1;
	if (x < 0) 
		sign = -1;
	x = Math.abs(x);

	// Constants
	
	var a1 =  0.254829592;
	var a2 = -0.284496736;
	var a3 =  1.421413741;
	var a4 = -1.453152027;
	var a5 =  1.061405429;
	var p  =  0.3275911;

	// A&S formula 7.1.26
	
	var t = 1.0/(1.0 + p*x);
	var y = 1.0 - (((((a5*t + a4)*t) + a3)*t + a2)*t + a1)*t*Math.exp(-x*x);
	
	// erf(-x) = -erf(x)
	
	return sign * y;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////// Draw routine /////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function draw()
{
    var g1, g2;
    var x1, x2;
	var l1, l2, r1, r2, y1, y2;
	var mean_local, dev_local, weight_local, sum_weight;
	var tagx, tagy, tagw;
	var numpoints = width;
    var i, j, k;
	var done_curve_max;
	
	var final_curve = new Array();
	var norm_val = 0;
		
    with (sketch) {
		
        glmatrixmode("modelview");
        glpushmatrix();
        glclearcolor(m_brgb);            
        glclear();
		gllinewidth(1);
		
		for (j = 1; j < numpoints; j++)
			final_curve[j] = 0;
		
		for (i = 0; i < num_curves; i++)
		{
			// Draw the selected curve last
			
			if (i == num_curves - 1)
			{
				k = sel_curve;
				glcolor(m_frgb3);
			}
			else
			{
				k = i + (i >= sel_curve);
				glcolor(m_frgb);
			}
			
			mean_local = mean_list[k];
			dev_local = dev_list[k];
			weight_local = weight_list[k];
			
			var dev_temp = Math.max(dev_local, 0.001);
			
			sum_weight = weight_local / (dev_temp * (erf((1.0 - mean_local) / dev_temp) - erf(-mean_local / dev_temp))); 
			
			done_curve_max = 0;
			
			g1 = -mean_local /  dev_local;
			g1 =  max_height * weight_local * Math.exp (-0.5 * (g1 * g1));
			
			x2 = -x_scale;
			g2 = norm2y(g1);
			
			for (j = 1; j < numpoints; j++)
			{
				x1 = j / numpoints;
								
				if (!done_curve_max && x1 >= mean_local)
				{
					// We always draw the curve maximum as the maximum value so as to preserve visual accuracy for small dev vals

					g1 = max_height * weight_local;
					done_curve_max = 1;
				}
				else
				{
					if (done_curve_max == 1 && dev_local < 0.005)
					{
						g1 = max_height * weight_local;
						done_curve_max = 2;
					}
					else
					{
						// Otherwise calculate a gaussian curve 
					
						g1 = 0;    
						g1 = (x1 - mean_local) / dev_local;
						g1 =  max_height * weight_local * Math.exp (-0.5 * (g1 * g1));
					}
				}
				
				// Sum to the final probability curve
				
				final_curve[j] += g1 * sum_weight;
				
				// Convert and draw
				
				x1 = norm2x(x1);
				g1 = norm2y(g1);
				
				quad (x2, -y_scale, 0, x2, g2, 0, x1, g1, 0, x1, -y_scale, 0)
				
				x2 = x1;
				g2 = g1;
			}
		}
		
		// Get normalisation value for the summation curve
		
		for (j = 0; j < numpoints; j++)
		{
			if (final_curve[j] / max_height > norm_val)
				norm_val = final_curve[j] / max_height;
		}
		
		// Draw normalised summation curve
		
		if (norm_val)
		{
			glcolor(m_frgb2);
			x2 = -x_scale;
			g2 = norm2y(final_curve[0]);
			
			for (j = 0; j < numpoints; j++)
			{
				x1 = norm2x(j / numpoints);
				g1 = norm2y(final_curve[j] / norm_val);
				
				quad (x2, -y_scale, 0, x2, g2, 0, x1, g1, 0, x1, -y_scale, 0)
				
				x2 = x1;
				g2 = g1;
			}
		}
		
		// Draw tags and handles
		
		for (i = 0; i < num_curves; i++)
		{
			// Draw the selected curvs tag / handlese last
			
			if (i == num_curves - 1)
			{
				k = sel_curve;
				glcolor(m_frgb5);
			}
			else
			{
				k = i + (i >= sel_curve);
				glcolor(m_frgb4);
			}
			
			tagx = norm2x(mean_list[k]);
			tagy = weight2height(weight_list[k]);
			tagw = dev2width(dev_list[k]);
			
			moveto (tagx, tagy);
			circle (tag_r);
			
			gllinewidth(handle_l);
			
			var halfline = 0; 
			
			l1 = tagx - tagw;    
			r1 = tagx + tagw;
			l2 = l1 + handle_w;
			r2 = r1 - handle_w;
			y1 = tagy + (y_scale * handle_h);
			y2 = tagy - (y_scale * handle_h);
			
			linesegment (l1, y1, 0, l1, y2, 0);
			linesegment (l1 - halfline, y1, 0, l2, y1, 0);
			linesegment (l1 - halfline, y2, 0, l2, y2, 0);
			linesegment (r1, y1, 0, r1, y2, 0);
			linesegment (r1 + halfline, y1, 0, r2, y1, 0);
			linesegment (r1 + halfline, y2, 0, r2, y2, 0);
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////// User message interaction /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function fsaa(v)
{
    sketch.fsaa = v;
    bang();
}


function bang()
{
   	var params = new Array();

	for (i = 0; i < num_curves; i++)
	{
		params.push(mean_list[i]);
		params.push(dev_list[i]);
		params.push(weight_list[i]);
	}
	
	outlet(0, params);
	outlet(1, num_curves, sel_curve + 1, mean_list[sel_curve], dev_list[sel_curve], weight_list[sel_curve]);
	
	draw();
    refresh();
}


function mean(m)
{
    mean_list[sel_curve] = clip (m, 0, 1);      
    bang();
}


function dev(d)
{    
    dev_list[sel_curve] = clip (d, 0, dev_max);        
    bang();
}


function weight(w)
{
    weight_list[sel_curve] = clip (w, 0, 1);        
    bang();
}


function sel(s)
{
    sel_curve = clip (s, 1, num_curves) - 1;        
    bang();
}


function reset()
{
	num_curves = 0;
	add();
}


function add()
{
	if (num_curves < 32)
	{
		mean_list[num_curves] = 0.5;
		dev_list[num_curves] = 0.5;
		weight_list[num_curves] = 1.;
		
		num_curves++;
		sel_curve = num_curves - 1;
		bang();
	}
}


function remove()
{
	var i;
	
	if (num_curves > 1)
	{
		for (i = sel_curve; i < num_curves - 1; i++)
		{
			mean_list[i] = mean_list[i + 1];
			dev_list[i] = dev_list[i + 1];
			weight_list[i] = weight_list[i + 1];
		}
		
		num_curves--;
		sel_curve = 0;
		
		bang();
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////// User mouse interaction //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function onclick (x, y) 
{
    var coords = sketch.screentoworld(x,y);
	var xworld = coords[0];
    var yworld = coords[1];
	var mindist = tag_sel_sq_dist;
	var i;
	
	drag_mode = 0;
	
	for (i = 0; i < num_curves; i++)
	{
		var tagx = norm2x(mean_list[i]);
		var tagy = weight2height(weight_list[i]);
		var tagw = dev2width(dev_list[i]);
	
		var xdist = xworld - tagx;
		var ydist = yworld - tagy;
		var distsq = xdist * xdist + ydist * ydist;
		
		if (distsq < tag_sel_sq_dist)  
		{
			drag_mode = 1;
			if (distsq < mindist)
				sel_curve = i;
		}
				
		var l1 = tagx - tagw - (handle_w / 2);    
		var r1 = tagx + tagw + (handle_w / 2);
		var l2 = l1 + handle_w;
		var r2 = r1 - handle_w;
		y1 = tagy + (y_scale * handle_h);
		y2 = tagy - (y_scale * handle_h);
	
		if (!drag_mode && (yworld < y1 && yworld > y2))
		{    
			if (xworld > l1 && xworld < l2)
			{
				drag_mode = 2;
				sel_curve = i
			}
			if (xworld > r2 && xworld < r1)
			{
				drag_mode = 3;
				sel_curve = i
			}
		}
	}
	
	ondrag(x, y, 1);	   
}


function ondblclick (x, y)
{
    drag_mode = 0;
	
	if (num_curves < 32)
	{
		mean_list[num_curves] = 0.5;
		dev_list[num_curves] = 0.5;
		weight_list[num_curves] = 1.;
		
		num_curves++;
		sel_curve = num_curves - 1;
		drag_mode = 1;
		
		ondrag(x, y, 1);
	}
}


function ondrag (x, y, button)
{
    var dist;
	var coords = sketch.screentoworld(x,y);
	var xworld = clip (coords[0], -x_scale, x_scale);
	var yworld = clip (coords[1], -max_height * y_scale, max_height * y_scale);
	
	switch (drag_mode)
    {
		case 1:
			mean_list[sel_curve] = x2norm(xworld);
			weight_list[sel_curve] = height2weight(yworld);
			break;
			
		case 2:
			dist = (norm2x(mean_list[sel_curve])) - xworld;
			dev_list[sel_curve] = width2dev(dist);	
			break;
			
		case 3:
			dist = xworld - (norm2x(mean_list[sel_curve]));
			dev_list[sel_curve] = width2dev(dist);	
			break;
	}
		
	if (drag_mode)
		bang();
	
	if (!button)
		drag_mode = 0;
}


function onresize (w, h)
{
	width = w;
	height = h;
	
	x_scale = w / h;
	y_scale = 1;
	
	draw();
	refresh();
	
} onresize.local = 1;         // private


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////// helpers for clipping and scaling /////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


function clip (x, l, h)
{
    if (x < l)
        x = l;
    if (x > h)
        x = h;
    
    return x;
	
} clip.local = 1;			// private


function x2norm (x)
{
	return (x / (2 * x_scale)) + 0.5;

} x2norm.local = 1;			// private


function norm2x (n)
{
	return x_scale * ((2 * n) - 1);
	
} norm2x.local = 1;			// private


function norm2y (n)
{
	return y_scale * ((2 * n) - 1);
	
} norm2y.local = 1;			// private


function dev2width (d)
{
    return x_scale * (dev_min_point + Math.pow((d / dev_max), dev_curve) * (dev_max_point - dev_min_point));
	
} dev2width.local = 1;		// private


function width2dev (w)
{
	return Math.pow(clip(((w / x_scale) - dev_min_point) / (dev_max_point - dev_min_point), 0, 1), 1 / dev_curve) * dev_max;
	
} width2dev.local = 1;		// private


function weight2height (w)
{
	return y_scale * (-max_height + Math.pow(w, weight_curve) * (2 * max_height));
	
} weight2height.local = 1;	// private


function height2weight (h)
{
	return Math.pow(((h / y_scale) + max_height) /  (2 * max_height), 1 / weight_curve);
	
} height2weight.local = 1;	// private
