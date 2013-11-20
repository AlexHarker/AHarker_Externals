{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 6,
			"minor" : 1,
			"revision" : 4,
			"architecture" : "x86"
		}
,
		"rect" : [ 200.0, 56.0, 768.0, 606.0 ],
		"bgcolor" : [ 0.22, 0.22, 0.22, 1.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"statusbarvisible" : 2,
		"toolbarvisible" : 0,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"hidden" : 1,
					"id" : "obj-27",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 220.0, 540.0, 257.0, 17.0 ],
					"text" : "window flags grow, window exec, savewindow 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"hidden" : 1,
					"id" : "obj-16",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 14.0, 540.0, 190.0, 17.0 ],
					"text" : "window flags nogrow, window exec"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-26",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"patching_rect" : [ 14.0, 567.0, 69.0, 20.0 ],
					"save" : [ "#N", "thispatcher", ";", "#Q", "window", "flags", "grow", "close", "zoom", "nofloat", "menu", "minimize", ";", "#Q", "window", "constrain", 50, 50, 32768, 32768, ";", "#Q", "window", "size", 200, 56, 968, 662, ";", "#Q", "window", "title", ";", "#Q", "window", "exec", ";", "#Q", "savewindow", 1, ";", "#Q", "end", ";" ],
					"text" : "thispatcher"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-25",
					"maxclass" : "newobj",
					"numinlets" : 4,
					"numoutlets" : 0,
					"patching_rect" : [ 14.0, 591.0, 143.0, 20.0 ],
					"text" : "bgcolor 0.22 0.22 0.22 1."
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-20",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 215.809509, 86.0, 23.0 ],
					"text" : "desciptors",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.811765, 0.921569, 0.678431, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-21",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 243.809509, 76.0, 18.0 ],
					"text" : "descriptors~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.811765, 0.921569, 0.678431, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-22",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 266.809509, 84.0, 18.0 ],
					"text" : "descriptorsrt~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.811765, 0.921569, 0.678431, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-23",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 289.809509, 81.0, 18.0 ],
					"text" : "entrymatcher"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.811765, 0.921569, 0.678431, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 312.809509, 88.0, 18.0 ],
					"text" : "entrymatcher~"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-19",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 383.0, 428.0, 47.0, 23.0 ],
					"text" : "vMSP",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-18",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 383.0, 301.1875, 51.0, 23.0 ],
					"text" : "utility",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-17",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 383.0, 201.1875, 78.0, 23.0 ],
					"text" : "threading",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-13",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 383.0, 9.0, 175.0, 23.0 ],
					"text" : "sample accurate suite",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-12",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 196.0, 423.0, 128.0, 23.0 ],
					"text" : "random numbers",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-11",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 196.0, 344.0, 105.0, 23.0 ],
					"text" : "miscellaneous",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-10",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 196.0, 9.0, 116.0, 23.0 ],
					"text" : "gesture suite",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-9",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 355.272736, 127.0, 23.0 ],
					"text" : "dynamicdsp suite",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-8",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 103.0, 88.0, 23.0 ],
					"text" : "convolution",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 9.0, 85.0, 23.0 ],
					"text" : "conversion",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"frgb" : 0.0,
					"id" : "obj-6",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 196.0, 118.0, 90.0, 23.0 ],
					"text" : "ibuffer suite",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 18.0,
					"frgb" : 0.0,
					"id" : "obj-3",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 19.0, 553.0, 374.0, 32.0 ],
					"text" : "AHarker Externals v 1.05  - Overview",
					"textcolor" : [ 1.0, 0.999954, 1.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 14.0,
					"hidden" : 1,
					"id" : "obj-4032",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 858.0, 177.0, 92.0, 22.0 ],
					"text" : "prepend help",
					"varname" : "helper"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 14.0,
					"hidden" : 1,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 858.0, 200.745972, 60.0, 22.0 ],
					"text" : "pcontrol"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3854",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 146.809509, 105.0, 18.0 ],
					"text" : "ibufconcatedrive~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3856",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 169.809509, 142.0, 18.0 ],
					"text" : "ibufconcatenate_attach~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3858",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 192.809509, 102.0, 18.0 ],
					"text" : "ibufconcatenate~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3860",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 215.809509, 51.0, 18.0 ],
					"text" : "ibuffer~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3862",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 238.809509, 76.0, 18.0 ],
					"text" : "ibuffermulti~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3864",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 261.809509, 56.0, 18.0 ],
					"text" : "ibufinfo~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3866",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 284.809509, 88.0, 18.0 ],
					"text" : "ibufmultitable~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3868",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 307.809509, 70.0, 18.0 ],
					"text" : "ibufplayer~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3870",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 665.5, 610.997009, 63.0, 18.0 ],
					"text" : "ibuftable~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.810909, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3872",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 37.0, 65.0, 18.0 ],
					"text" : "valconvert"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.810909, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3874",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 60.0, 72.0, 18.0 ],
					"text" : "valconvert~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3882",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 131.0, 84.0, 18.0 ],
					"text" : "kernelmaker~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3884",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 154.0, 86.0, 18.0 ],
					"text" : "partconvolve~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3886",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 177.0, 88.0, 18.0 ],
					"text" : "timeconvolve~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3890",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 383.272736, 75.0, 18.0 ],
					"text" : "dynamic.in~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3892",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 406.272736, 82.0, 18.0 ],
					"text" : "dynamic.out~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3894",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 429.272736, 95.0, 18.0 ],
					"text" : "dynamic.patch~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3896",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 452.272736, 106.0, 18.0 ],
					"text" : "dynamic.request~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3898",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 475.272736, 84.0, 18.0 ],
					"text" : "dynamic.this~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3900",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 498.272736, 82.0, 18.0 ],
					"text" : "dynamicdsp~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.92, 0.68, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3902",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 521.272705, 91.0, 18.0 ],
					"text" : "dynamicserial~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.92, 0.723636, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3904",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 37.0, 91.0, 18.0 ],
					"text" : "gesture_maker"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.92, 0.723636, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3906",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 60.0, 56.0, 18.0 ],
					"text" : "timefilter"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.92, 0.723636, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3908",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 83.0, 57.0, 18.0 ],
					"text" : "timemap"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.92, 0.854545, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3910",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 372.0, 81.0, 18.0 ],
					"text" : "ajh.wiiremote"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.92, 0.854545, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3912",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 395.0, 60.0, 18.0 ],
					"text" : "meandev"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.678431, 0.847059, 0.921569, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3914",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 451.0, 61.0, 18.0 ],
					"text" : "ahnoise~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.678431, 0.847059, 0.921569, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3916",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 474.0, 64.0, 18.0 ],
					"text" : "randfloats"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.678431, 0.847059, 0.921569, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3918",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 497.0, 74.0, 18.0 ],
					"text" : "randomvals"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.678431, 0.847059, 0.921569, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3920",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 196.0, 520.0, 81.0, 18.0 ],
					"text" : "randomvals~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3922",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 37.1875, 83.0, 18.0 ],
					"text" : "chebyshape~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3924",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 60.1875, 59.0, 18.0 ],
					"text" : "rbiquad~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3926",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 83.1875, 63.0, 18.0 ],
					"text" : "rdcblock~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3928",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 106.1875, 74.0, 18.0 ],
					"text" : "triggerlimit~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3930",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 129.1875, 41.0, 18.0 ],
					"text" : "tsah~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3932",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 152.1875, 73.0, 18.0 ],
					"text" : "voicedrive~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.68, 0.723636, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3934",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 175.1875, 94.0, 18.0 ],
					"text" : "voicemanager~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3936",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 229.1875, 62.0, 18.0 ],
					"text" : "getthread"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3938",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 252.1875, 108.0, 18.0 ],
					"text" : "schedulemessage"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.767273, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3940",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 275.1875, 68.0, 18.0 ],
					"text" : "threadfilter"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3942",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 329.1875, 84.0, 18.0 ],
					"text" : "denormkiller~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3944",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 352.1875, 50.0, 18.0 ],
					"text" : "depack"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3946",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 375.1875, 80.0, 18.0 ],
					"text" : "MACaddress"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.898182, 0.68, 0.92, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3948",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 398.1875, 90.0, 18.0 ],
					"text" : "recursivefolder"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3950",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 455.272736, 44.0, 18.0 ],
					"text" : "vabs~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3952",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 477.272766, 50.0, 18.0 ],
					"text" : "vacos~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3954",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 499.272705, 57.0, 18.0 ],
					"text" : "vacosh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3956",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 383.0, 521.272705, 47.0, 18.0 ],
					"text" : "vasin~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3958",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 37.27272, 53.0, 18.0 ],
					"text" : "vasinh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3960",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 59.045456, 48.0, 18.0 ],
					"text" : "vatan~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3962",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 81.045456, 55.0, 18.0 ],
					"text" : "vatanh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3964",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 103.045456, 55.0, 18.0 ],
					"text" : "vatodb~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3966",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 125.045456, 50.0, 18.0 ],
					"text" : "vcosh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3968",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 147.045456, 49.0, 18.0 ],
					"text" : "vcosx~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3970",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 169.045456, 55.0, 18.0 ],
					"text" : "vdbtoa~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3972",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 191.045456, 40.0, 18.0 ],
					"text" : "vdiv~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3974",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 213.045456, 60.0, 18.0 ],
					"text" : "vequals~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3976",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 235.045441, 48.0, 18.0 ],
					"text" : "vftom~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3978",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 257.045441, 86.0, 18.0 ],
					"text" : "vgreaterthan~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3980",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 279.045441, 99.0, 18.0 ],
					"text" : "vgreaterthaneq~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3982",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 301.045441, 69.0, 18.0 ],
					"text" : "vlessthan~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3984",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 323.045441, 83.0, 18.0 ],
					"text" : "vlessthaneq~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3986",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 345.045441, 41.0, 18.0 ],
					"text" : "vlog~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3988",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 367.045441, 77.0, 18.0 ],
					"text" : "vmaximum~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3990",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 389.045441, 73.0, 18.0 ],
					"text" : "vminimum~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3992",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 411.045441, 57.0, 18.0 ],
					"text" : "vminus~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3994",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 433.045441, 48.0, 18.0 ],
					"text" : "vmtof~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3996",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 455.045441, 77.0, 18.0 ],
					"text" : "vnotequals~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3998",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 477.045471, 47.0, 18.0 ],
					"text" : "vplus~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4000",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 499.045471, 47.0, 18.0 ],
					"text" : "vpow~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4002",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 561.0, 521.045471, 44.0, 18.0 ],
					"text" : "vrdiv~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4004",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 37.0, 61.0, 18.0 ],
					"text" : "vrminus~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4006",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 59.0, 47.0, 18.0 ],
					"text" : "vsinh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4008",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 81.0, 46.0, 18.0 ],
					"text" : "vsinx~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4010",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 103.0, 45.0, 18.0 ],
					"text" : "vsqrt~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4012",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 125.0, 48.0, 18.0 ],
					"text" : "vtanh~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4014",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 147.0, 47.0, 18.0 ],
					"text" : "vtanx~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4016",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 169.0, 53.0, 18.0 ],
					"text" : "vtimes~"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.92, 0.68, 0.810909, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4018",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 684.0, 191.0, 51.0, 18.0 ],
					"text" : "vtrunc~"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-21", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-22", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-23", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-26", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"midpoints" : [ 229.5, 562.0, 23.5, 562.0 ],
					"source" : [ "obj-27", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3854", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3856", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3858", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3860", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3862", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3864", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3866", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3868", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3870", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3872", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3874", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3882", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3884", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3886", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3890", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3892", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3894", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3896", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3898", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3900", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3902", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3904", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3906", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3908", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3910", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3912", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3914", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3916", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3918", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3920", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3922", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3924", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3926", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3928", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3930", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3932", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3934", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3936", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3938", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3940", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3942", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3944", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3946", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3948", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3950", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3952", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3954", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3956", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3958", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3960", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3962", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3964", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3966", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3968", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3970", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3972", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3974", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3976", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3978", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3980", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3982", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3984", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3986", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3988", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3990", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3992", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3994", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3996", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-3998", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4000", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4002", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4004", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4006", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4008", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4010", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4012", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4014", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4016", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-4032", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4018", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-2", 0 ],
					"disabled" : 0,
					"hidden" : 1,
					"source" : [ "obj-4032", 0 ]
				}

			}
 ],
		"dependency_cache" : [  ]
	}

}
