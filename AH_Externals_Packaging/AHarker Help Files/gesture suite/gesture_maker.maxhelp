{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 46.0, 45.0, 949.0, 720.0 ],
		"bglocked" : 0,
		"defrect" : [ 46.0, 45.0, 949.0, 720.0 ],
		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 0,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 0,
		"toolbarvisible" : 1,
		"boxanimatetime" : 200,
		"imprint" : 0,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"boxes" : [ 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "gesture_maker  is currently implemented using WELLRNG by Francois Panneton and Pierre L'Ecuyer for pseudorandom number generation (see http://www.iro.umontreal.ca/~panneton/WELLRNG.html).",
					"linecount" : 2,
					"numinlets" : 1,
					"id" : "obj-23",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 180.0, 655.0, 554.0, 34.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p see_also",
					"hidden" : 1,
					"numinlets" : 1,
					"id" : "obj-18",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 848.0, 620.0, 69.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 50.0, 94.0, 264.0, 148.0 ],
						"bglocked" : 0,
						"defrect" : [ 50.0, 94.0, 264.0, 148.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "newobj",
									"varname" : "phelp",
									"text" : "prepend help",
									"numinlets" : 1,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 99.0, 63.5, 83.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pcontrol",
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 99.0, 90.5, 56.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"numinlets" : 0,
									"id" : "obj-4",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 36.0, 25.0, 25.0 ],
									"comment" : "From umenu label out"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-4", 0 ],
									"destination" : [ "obj-2", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-2", 0 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Impose Gesture",
					"numinlets" : 1,
					"id" : "obj-46",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 127.0, 504.0, 96.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "This line object just fades in / out",
					"numinlets" : 1,
					"id" : "obj-41",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 334.0, 504.0, 185.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p drive_mode",
					"numinlets" : 0,
					"id" : "obj-37",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.027451, 0.756863, 0.603922, 1.0 ],
					"patching_rect" : [ 630.0, 485.833344, 83.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 35.0, 80.0, 1326.0, 473.0 ],
						"bglocked" : 0,
						"defrect" : [ 35.0, 80.0, 1326.0, 473.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "This line object just fades in / out",
									"numinlets" : 1,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 485.0, 340.0, 185.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "^ Gesture Setup ^",
									"numinlets" : 1,
									"id" : "obj-34",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 805.0, 235.0, 106.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- 2. Realise gesture with specified duration",
									"numinlets" : 1,
									"frgb" : [ 1.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-33",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 274.0, 124.0, 244.0, 20.0 ],
									"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : " N.B. longer graintime gives less temporal resolution, but reduce calculations per unit time.",
									"numinlets" : 1,
									"id" : "obj-29",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 133.0, 82.0, 490.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- 1. Set graintime (default 20 ms).",
									"numinlets" : 1,
									"frgb" : [ 1.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-27",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 132.0, 57.0, 196.0, 20.0 ],
									"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "graintime 10.",
									"numinlets" : 2,
									"id" : "obj-24",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 37.0, 87.0, 80.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "graintime 200.",
									"numinlets" : 2,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 23.0, 58.0, 87.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "timings 0.25 0.4 0.6",
									"numinlets" : 2,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 798.333313, 194.0, 115.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "4569",
									"numinlets" : 2,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 225.0, 125.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "2354",
									"numinlets" : 2,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 172.0, 125.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "flonum",
									"numinlets" : 1,
									"id" : "obj-54",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 144.5, 280.399994, 58.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "-  Filter noise in a gestural fashion",
									"numinlets" : 1,
									"id" : "obj-52",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 231.0, 18.0, 193.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0. 300.",
									"numinlets" : 2,
									"id" : "obj-51",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 435.0, 310.399994, 47.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0.1 1.",
									"numinlets" : 2,
									"id" : "obj-48",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 1263.5, 310.399994, 41.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-46",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 434.0, 338.399994, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 8.",
									"numinlets" : 1,
									"id" : "obj-45",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 345.0, 280.399994, 75.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "biquad~",
									"numinlets" : 6,
									"id" : "obj-44",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 102.0, 338.399994, 263.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "filtercoeff~ resonant",
									"numinlets" : 3,
									"id" : "obj-42",
									"fontname" : "Arial",
									"numoutlets" : 5,
									"outlettype" : [ "signal", "signal", "signal", "signal", "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 151.0, 310.399994, 213.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "300.",
									"numinlets" : 2,
									"id" : "obj-41",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 66.0, 125.0, 34.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "noise~",
									"numinlets" : 1,
									"id" : "obj-40",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 102.0, 310.399994, 46.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "drive $1",
									"numinlets" : 2,
									"id" : "obj-32",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 66.0, 194.399994, 53.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "950.",
									"numinlets" : 2,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 119.0, 125.0, 34.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The gesture length is specified directly in milliseconds.",
									"numinlets" : 1,
									"id" : "obj-31",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 446.0, 40.5, 299.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Frequency in Hz",
									"numinlets" : 1,
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 239.5, 280.399994, 98.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "initial_inflections 0.5",
									"numinlets" : 2,
									"id" : "obj-11",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 919.333313, 194.0, 118.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "unpack 0.",
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "float" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 145.0, 254.399994, 63.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "In drive mode the gesture is evaluted at regular points in time (according to duration set by the \"graintime\" message).",
									"numinlets" : 1,
									"id" : "obj-49",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 446.0, 18.0, 635.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Example - Frequency Gesture",
									"numinlets" : 1,
									"id" : "obj-35",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 28.0, 16.0, 204.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "t f b",
									"numinlets" : 1,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 66.0, 160.0, 32.5, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"varname" : "autohelp_dac_text",
									"text" : "start audio",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-64",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 11.595187,
									"patching_rect" : [ 61.0, 409.0, 38.0, 33.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "ezdac~",
									"varname" : "autohelp_dac",
									"numinlets" : 2,
									"id" : "obj-65",
									"numoutlets" : 0,
									"patching_rect" : [ 102.0, 403.0, 45.0, 45.0 ],
									"local" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "panel",
									"varname" : "startwinwdow_panel",
									"numinlets" : 1,
									"id" : "obj-21",
									"border" : 2,
									"numoutlets" : 0,
									"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
									"patching_rect" : [ 54.0, 398.0, 100.0, 55.0 ],
									"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~ 0.1",
									"numinlets" : 2,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 102.0, 371.399994, 42.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 102.0, 280.399994, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "gesture_maker",
									"numinlets" : 1,
									"id" : "obj-1",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 66.0, 222.399994, 91.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_main triangle 4 0 8 4 0 3-5 3 0 3-5 3",
									"numinlets" : 2,
									"id" : "obj-5",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 136.0, 194.399994, 247.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_inflections triangle_return 5-6 last 9",
									"numinlets" : 2,
									"id" : "obj-6",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 389.333344, 194.0, 244.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main log 200 6900",
									"numinlets" : 2,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 639.333313, 194.0, 153.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections pitch -12 12",
									"numinlets" : 2,
									"id" : "obj-9",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 1043.333374, 194.0, 174.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset",
									"numinlets" : 2,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 1223.333374, 194.0, 37.0, 18.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1052.833374, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-8", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 648.833313, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-6", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 398.833344, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-51", 0 ],
									"destination" : [ "obj-46", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-5", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 145.5, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-48", 0 ],
									"destination" : [ "obj-46", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1273.0, 332.899994, 443.5, 332.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-46", 0 ],
									"destination" : [ "obj-18", 1 ],
									"hidden" : 0,
									"midpoints" : [ 443.5, 364.600006, 134.5, 364.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-45", 0 ],
									"destination" : [ "obj-42", 2 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-44", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 0 ],
									"destination" : [ "obj-44", 1 ],
									"hidden" : 0,
									"midpoints" : [ 160.5, 331.600006, 160.300003, 331.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 1 ],
									"destination" : [ "obj-44", 2 ],
									"hidden" : 0,
									"midpoints" : [ 209.0, 331.600006, 209.100006, 331.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 2 ],
									"destination" : [ "obj-44", 3 ],
									"hidden" : 0,
									"midpoints" : [ 257.5, 331.600006, 257.899994, 331.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 3 ],
									"destination" : [ "obj-44", 4 ],
									"hidden" : 0,
									"midpoints" : [ 306.0, 331.600006, 306.700012, 331.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 4 ],
									"destination" : [ "obj-44", 5 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-41", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [ 75.5, 151.0, 75.5, 151.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-40", 0 ],
									"destination" : [ "obj-44", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-32", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-54", 0 ],
									"hidden" : 0,
									"midpoints" : [ 154.5, 272.899994, 154.0, 272.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-9", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 1052.833374, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-8", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 648.833313, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-6", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 398.833344, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-5", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 145.5, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-48", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.700012, 1273.0, 186.700012 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 0 ],
									"destination" : [ "obj-32", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 1232.833374, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-16", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 807.833313, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-11", 0 ],
									"hidden" : 0,
									"midpoints" : [ 89.0, 186.699997, 928.833313, 186.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-24", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 46.5, 216.199997, 75.5, 216.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-23", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 32.5, 216.699997, 75.5, 216.699997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-22", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [ 128.5, 151.0, 75.5, 151.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1232.833374, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-65", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-16", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 807.833313, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-15", 0 ],
									"destination" : [ "obj-42", 0 ],
									"hidden" : 0,
									"midpoints" : [ 111.5, 305.899994, 160.5, 305.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-13", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [ 234.5, 151.0, 75.5, 151.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-12", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [ 181.5, 151.0, 75.5, 151.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-11", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 928.833313, 216.899994, 75.5, 216.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-51", 0 ],
									"hidden" : 0,
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"midpoints" : [ 75.5, 245.0, 444.5, 245.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 1 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [ 147.5, 250.399994, 154.5, 250.399994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 1 ],
									"destination" : [ "obj-15", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p all_messages_and_order",
					"numinlets" : 0,
					"id" : "obj-35",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.047059, 0.733333, 1.0, 1.0 ],
					"patching_rect" : [ 630.0, 581.833313, 155.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 60.0, 137.0, 1075.0, 445.0 ],
						"bglocked" : 0,
						"defrect" : [ 60.0, 137.0, 1075.0, 445.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Transport",
									"numinlets" : 1,
									"id" : "obj-44",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 286.0, 75.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Kernel Shapes",
									"numinlets" : 1,
									"id" : "obj-43",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 258.0, 106.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Initial Values",
									"numinlets" : 1,
									"id" : "obj-42",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 204.0, 97.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Scalings",
									"numinlets" : 1,
									"id" : "obj-41",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 150.0, 67.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Timings",
									"numinlets" : 1,
									"id" : "obj-33",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 96.0, 62.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Reset",
									"numinlets" : 1,
									"id" : "obj-32",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 27.0, 70.0, 48.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Stop the gesture part way through (note that the last output of each kernel is always retained).",
									"numinlets" : 1,
									"id" : "obj-30",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 366.0, 509.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Realise the gesture with the given timings in events mode.",
									"numinlets" : 1,
									"id" : "obj-29",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 339.0, 320.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Realise the gesture with the specified duration in drive mode.",
									"numinlets" : 1,
									"id" : "obj-28",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 312.0, 335.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the duration between outputs using drive mode.",
									"numinlets" : 1,
									"id" : "obj-27",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 123.0, 285.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the *next* inflection gesture kernel parameters. Each kernel is added to the internal inflection kernel list.",
									"numinlets" : 1,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 286.0, 581.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the main gesture kernel parameters.",
									"numinlets" : 1,
									"id" : "obj-25",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 258.0, 225.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the inital value (0-1) for the initiial layer (only needed when the first inflection starts with a value speciied by the \"last\" specifier.",
									"numinlets" : 1,
									"id" : "obj-24",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 231.0, 702.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the initial value (0-1) for the main layer (only needed when the main kernel starts with a value speciied by the \"last\" specifier.",
									"numinlets" : 1,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 204.0, 693.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the scaling for the inflection layer.",
									"numinlets" : 1,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 177.0, 209.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the scaling for the main layer.",
									"numinlets" : 1,
									"id" : "obj-21",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 150.0, 188.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "This is a list of all messages the object accepts, plus an outline of their functionality. The oredr is not crucial, but the reset message should always be sent first (if needed), and the transport command (\"drive\" or \"events\") last, after the gesture has been setup using the other messages. Further details can be found in the other subpatches.",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 23.0, 11.0, 995.0, 34.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Set the timings for inflection gestures. The values should be specified in normalised time (0-1).",
									"numinlets" : 1,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 96.0, 511.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Clear all stored gesture information",
									"numinlets" : 1,
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 363.0, 70.0, 197.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "graintime [time]",
									"numinlets" : 2,
									"id" : "obj-11",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 124.0, 93.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "events [list]",
									"numinlets" : 2,
									"id" : "obj-7",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 340.0, 71.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset",
									"numinlets" : 2,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 70.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "stop",
									"numinlets" : 2,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 367.0, 33.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "timings [list]",
									"numinlets" : 2,
									"id" : "obj-4",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 97.0, 73.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "drive [duration]",
									"numinlets" : 2,
									"id" : "obj-5",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 313.0, 90.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "initial_main [normalised value]",
									"numinlets" : 2,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 205.0, 171.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "initial_inflections [normalised value]",
									"numinlets" : 2,
									"id" : "obj-9",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 232.0, 199.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_main [list]",
									"numinlets" : 2,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 259.0, 108.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_inflections [duration]",
									"numinlets" : 2,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 286.0, 164.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main [params]",
									"numinlets" : 2,
									"id" : "obj-14",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 151.0, 131.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections [params]",
									"numinlets" : 2,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 149.0, 178.0, 158.0, 18.0 ]
								}

							}
 ],
						"lines" : [  ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Details - Follow the subpatches in order",
					"numinlets" : 1,
					"id" : "obj-34",
					"fontname" : "Arial Black",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 571.0, 346.0, 271.0, 23.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Inlets and Outlets",
					"numinlets" : 1,
					"id" : "obj-33",
					"fontname" : "Arial Black",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 69.0, 184.0, 128.0, 23.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "<- Click to realise gesture with given length in ms",
					"numinlets" : 1,
					"id" : "obj-32",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 231.0, 390.0, 271.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "<- see inside for how it's done",
					"numinlets" : 1,
					"id" : "obj-30",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 304.0, 451.0, 169.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "Example - Volume gesture imposed on pink noise",
					"numinlets" : 1,
					"id" : "obj-53",
					"fontname" : "Arial Black",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 69.0, 346.0, 334.0, 23.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~ 0.1",
					"numinlets" : 2,
					"id" : "obj-51",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 597.333374, 42.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "pink~",
					"numinlets" : 1,
					"id" : "obj-45",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 139.0, 535.333374, 40.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~",
					"numinlets" : 2,
					"id" : "obj-44",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 566.333374, 71.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_dac_text",
					"text" : "start audio",
					"linecount" : 2,
					"numinlets" : 1,
					"id" : "obj-64",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 11.595187,
					"patching_rect" : [ 46.0, 649.0, 38.0, 33.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "ezdac~",
					"varname" : "autohelp_dac",
					"numinlets" : 2,
					"id" : "obj-65",
					"numoutlets" : 0,
					"patching_rect" : [ 87.0, 643.0, 45.0, 45.0 ],
					"local" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "startwinwdow_panel",
					"numinlets" : 1,
					"id" : "obj-43",
					"border" : 2,
					"numoutlets" : 0,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"patching_rect" : [ 39.0, 638.0, 100.0, 55.0 ],
					"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "0 5.",
					"numinlets" : 2,
					"id" : "obj-42",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 249.0, 475.0, 32.5, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "1 5",
					"numinlets" : 2,
					"id" : "obj-40",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 288.0, 475.0, 32.5, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p Generate_Gesture",
					"numinlets" : 1,
					"id" : "obj-38",
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "", "" ],
					"fontsize" : 12.0,
					"color" : [ 0.976471, 0.129412, 0.956863, 1.0 ],
					"patching_rect" : [ 87.0, 450.0, 181.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 25.0, 69.0, 1412.0, 258.0 ],
						"bglocked" : 0,
						"defrect" : [ 25.0, 69.0, 1412.0, 258.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "outlet",
									"numinlets" : 1,
									"id" : "obj-5",
									"numoutlets" : 0,
									"patching_rect" : [ 113.0, 197.0, 25.0, 25.0 ],
									"comment" : ""
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "drive $1",
									"numinlets" : 2,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 39.0, 131.0, 53.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 0.02",
									"numinlets" : 1,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 589.0, 79.0, 89.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "prepend timings",
									"numinlets" : 1,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 524.200012, 131.0, 96.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "8",
									"numinlets" : 2,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 524.0, 80.0, 32.5, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "timemap 0.05 0.8 3.",
									"numinlets" : 5,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 524.0, 104.5, 117.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "initial_inflections 0.8",
									"numinlets" : 2,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 1216.199951, 133.0, 118.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main amp 0. 1. -45 0., scaling_inflections amp 0. 1. -5. 5.",
									"numinlets" : 2,
									"id" : "obj-4",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 854.200012, 133.0, 356.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset",
									"numinlets" : 2,
									"id" : "obj-7",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 1340.199951, 133.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_main line 1. 0.2 2 0. 2",
									"numinlets" : 2,
									"id" : "obj-10",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 676.200012, 133.0, 172.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_inflections triangle 4-6 last 7-9 2-3 0 2 3-6 0 2 3-6",
									"numinlets" : 2,
									"id" : "obj-11",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 199.0, 133.0, 319.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"numinlets" : 0,
									"id" : "obj-9",
									"numoutlets" : 1,
									"outlettype" : [ "float" ],
									"patching_rect" : [ 39.0, 24.0, 25.0, 25.0 ],
									"comment" : ""
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "outlet",
									"numinlets" : 1,
									"id" : "obj-14",
									"numoutlets" : 0,
									"patching_rect" : [ 75.0, 197.0, 25.0, 25.0 ],
									"comment" : ""
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "gesture_maker",
									"numinlets" : 1,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 39.0, 166.0, 91.0, 20.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-7", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.0, 1349.699951, 63.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-4", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.0, 863.700012, 63.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.5, 533.5, 63.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-12", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.0, 1225.699951, 63.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-11", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.5, 208.5, 63.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-10", 0 ],
									"hidden" : 0,
									"midpoints" : [ 48.5, 63.0, 685.700012, 63.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-7", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1349.699951, 160.5, 48.5, 160.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-4", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 863.700012, 160.5, 48.5, 160.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-22", 0 ],
									"destination" : [ "obj-16", 4 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 0 ],
									"destination" : [ "obj-5", 0 ],
									"hidden" : 0,
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"midpoints" : [ 48.5, 191.0, 122.5, 191.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 1 ],
									"destination" : [ "obj-14", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-19", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 533.700012, 160.5, 48.5, 160.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-16", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-16", 0 ],
									"destination" : [ "obj-19", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-12", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1225.699951, 160.5, 48.5, 160.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-11", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 208.5, 160.5, 48.5, 160.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-10", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 685.700012, 160.5, 48.5, 160.5 ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "line~",
					"numinlets" : 2,
					"id" : "obj-29",
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "signal", "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 288.0, 504.0, 36.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "3120",
					"numinlets" : 2,
					"id" : "obj-28",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 135.5, 390.0, 37.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "2300",
					"numinlets" : 2,
					"id" : "obj-27",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 390.0, 37.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "6000",
					"numinlets" : 2,
					"id" : "obj-24",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 183.5, 390.0, 37.0, 18.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~",
					"numinlets" : 2,
					"id" : "obj-22",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 535.333374, 32.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "line~",
					"numinlets" : 2,
					"id" : "obj-21",
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "signal", "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 504.0, 36.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "t f b",
					"numinlets" : 1,
					"id" : "obj-7",
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "float", "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 87.0, 418.0, 219.5, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p shapes",
					"numinlets" : 0,
					"id" : "obj-8",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.654902, 0.117647, 0.835294, 1.0 ],
					"patching_rect" : [ 630.0, 421.833344, 59.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 92.0, 57.0, 843.0, 821.0 ],
						"bglocked" : 0,
						"defrect" : [ 92.0, 57.0, 843.0, 821.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Note that all lines may be curved, but the descriptions imply straight lines for ease of identification.",
									"numinlets" : 1,
									"id" : "obj-48",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 23.0, 62.0, 533.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 1 3 5 7 8 6 4 2 3 4 5 6 7 8 9 10",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 469.0, 229.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-39",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 16,
									"patching_rect" : [ 25.0, 491.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 2 4 6 8 8 8 8 8 8 8 8 9 10 11 12",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 425.0, 231.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-41",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 15,
									"patching_rect" : [ 25.0, 447.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 1 3 5 7 8 8 8 8 8 8 8 7 5 3 1",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-9",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 337.0, 212.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-43",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 15,
									"patching_rect" : [ 25.0, 359.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 4 4 4 4 4 5 6 7 8 9 10 11",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-44",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 381.0, 194.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-45",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 12,
									"patching_rect" : [ 25.0, 403.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 11 9 7 5 3 1 2 3 4 5 6 7 8",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 293.0, 198.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-37",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 13,
									"patching_rect" : [ 25.0, 315.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 4 5 6 7 8 9 10 9 8 7 6 5 4",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 249.0, 199.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-35",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 13,
									"patching_rect" : [ 25.0, 271.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 11 11 11 11 11 10 9 8 7 6 5 4 3 2",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-24",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 161.0, 238.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-33",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 14,
									"patching_rect" : [ 25.0, 183.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 4 4 4 4 4 5 6 7 8 9 10 11",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-25",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 205.0, 194.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-27",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 12,
									"patching_rect" : [ 25.0, 227.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 1 2 3 4 5 6 7 8 9 10 11 12",
									"hidden" : 1,
									"numinlets" : 1,
									"id" : "obj-29",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 25.0, 117.0, 201.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-31",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"size" : 12,
									"patching_rect" : [ 25.0, 139.0, 66.0, 30.0 ],
									"setminmax" : [ 0.0, 12.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"numinlets" : 1,
									"id" : "obj-46",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"patching_rect" : [ 25.0, 95.0, 66.0, 30.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "All shapes are possible using the general shape, but it is usually simpler to use one of the more restricted forms.",
									"numinlets" : 1,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 23.0, 39.0, 607.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Each kernel can have one of several shape types that allow only the minimum number of values to be specified.",
									"numinlets" : 1,
									"id" : "obj-21",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 23.0, 16.0, 608.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- See here for details",
									"numinlets" : 1,
									"frgb" : [ 1.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 224.0, 623.0, 129.0, 20.0 ],
									"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Generate Gesture (main gesture - line + inflection gestures - triangle - different each time)",
									"numinlets" : 1,
									"id" : "obj-11",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 84.0, 582.0, 608.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "p Generate_And_Draw_Gesture",
									"numinlets" : 1,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 35.0, 622.0, 183.0, 20.0 ],
									"patcher" : 									{
										"fileversion" : 1,
										"rect" : [ 25.0, 69.0, 1507.0, 297.0 ],
										"bglocked" : 0,
										"defrect" : [ 25.0, 69.0, 1507.0, 297.0 ],
										"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"boxes" : [ 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Use uzi + chaining input to get desired values for multislider",
													"numinlets" : 1,
													"id" : "obj-26",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 117.0, 16.0, 327.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Clear all stored data",
													"numinlets" : 1,
													"id" : "obj-25",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 1372.0, 177.0, 118.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Set initial value for inflections",
													"numinlets" : 1,
													"id" : "obj-24",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 1180.0, 178.0, 178.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Set main and inflcction scalings",
													"numinlets" : 1,
													"id" : "obj-23",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 856.0, 178.0, 178.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Set inflection timings",
													"numinlets" : 1,
													"id" : "obj-21",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 525.0, 178.0, 121.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Set main gesture",
													"numinlets" : 1,
													"id" : "obj-13",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 695.0, 178.0, 101.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Set inflections gesture",
													"numinlets" : 1,
													"id" : "obj-3",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 285.0, 178.0, 129.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "loadmess 0.02",
													"numinlets" : 1,
													"id" : "obj-22",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 603.0, 86.0, 89.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "prepend timings",
													"numinlets" : 1,
													"id" : "obj-19",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 566.666687, 138.0, 96.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "8",
													"numinlets" : 2,
													"id" : "obj-18",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 567.0, 87.0, 32.5, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "timemap 0.05 0.8 3.",
													"numinlets" : 5,
													"id" : "obj-16",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 567.0, 111.5, 117.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "initial_inflections 0.8",
													"numinlets" : 2,
													"id" : "obj-12",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 1253.666626, 140.0, 118.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "scaling_main amp 0. 1. -45 0., scaling_inflections amp 0. 1. -5. 5.",
													"numinlets" : 2,
													"id" : "obj-4",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 876.666687, 140.0, 356.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "reset",
													"numinlets" : 2,
													"id" : "obj-7",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 1392.666626, 140.0, 37.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main line 1. 0.2 2 0. 2",
													"numinlets" : 2,
													"id" : "obj-10",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 683.666687, 140.0, 172.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_inflections triangle 4-6 last 7-9 2-3 0 2 3-6 0 2 3-6",
													"numinlets" : 2,
													"id" : "obj-11",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 226.666672, 140.0, 319.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "inlet",
													"numinlets" : 0,
													"id" : "obj-9",
													"numoutlets" : 1,
													"outlettype" : [ "bang" ],
													"patching_rect" : [ 78.0, 14.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "outlet",
													"numinlets" : 1,
													"id" : "obj-14",
													"numoutlets" : 0,
													"patching_rect" : [ 25.0, 260.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "pack 0 0.",
													"numinlets" : 2,
													"id" : "obj-2",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 25.0, 206.0, 81.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "prepend set",
													"numinlets" : 1,
													"id" : "obj-5",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 25.0, 232.0, 74.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "pack 0. 0.",
													"numinlets" : 2,
													"id" : "obj-6",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 142.0, 140.0, 63.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "- 1",
													"numinlets" : 2,
													"id" : "obj-8",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "int" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 142.0, 92.0, 32.5, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "/ 1023.",
													"numinlets" : 2,
													"id" : "obj-15",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 142.0, 116.0, 47.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "uzi 1024",
													"numinlets" : 2,
													"id" : "obj-17",
													"fontname" : "Arial",
													"numoutlets" : 3,
													"outlettype" : [ "bang", "bang", "int" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 78.0, 57.0, 56.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "gesture_maker",
													"numinlets" : 2,
													"id" : "obj-20",
													"fontname" : "Arial",
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 51.0, 173.0, 91.0, 20.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-7", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.0, 1402.166626, 48.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-4", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.0, 886.166687, 48.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-18", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.5, 576.5, 48.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-17", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-12", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.0, 1263.166626, 48.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-11", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.5, 236.166672, 48.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-10", 0 ],
													"hidden" : 0,
													"midpoints" : [ 87.5, 48.0, 693.166687, 48.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-8", 0 ],
													"destination" : [ "obj-15", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-7", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 1402.166626, 167.5, 60.5, 167.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-6", 0 ],
													"destination" : [ "obj-20", 1 ],
													"hidden" : 0,
													"midpoints" : [ 151.5, 163.0, 132.5, 163.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-5", 0 ],
													"destination" : [ "obj-14", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-4", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 886.166687, 167.5, 60.5, 167.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-22", 0 ],
													"destination" : [ "obj-16", 4 ],
													"hidden" : 0,
													"midpoints" : [ 612.5, 108.25, 674.5, 108.25 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-20", 1 ],
													"destination" : [ "obj-2", 1 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-2", 0 ],
													"destination" : [ "obj-5", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-19", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 576.166687, 167.5, 60.5, 167.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-18", 0 ],
													"destination" : [ "obj-16", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-17", 2 ],
													"destination" : [ "obj-8", 0 ],
													"hidden" : 0,
													"midpoints" : [ 124.5, 86.0, 151.5, 86.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-17", 2 ],
													"destination" : [ "obj-2", 0 ],
													"hidden" : 0,
													"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
													"midpoints" : [ 124.5, 127.0, 34.5, 127.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-16", 0 ],
													"destination" : [ "obj-19", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-15", 0 ],
													"destination" : [ "obj-6", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-12", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 1263.166626, 167.5, 60.5, 167.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-11", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 236.166672, 167.5, 60.5, 167.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-10", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 693.166687, 167.5, 60.5, 167.5 ]
												}

											}
 ]
									}
,
									"saved_object_attributes" : 									{
										"default_fontname" : "Arial",
										"fontname" : "Arial",
										"default_fontsize" : 12.0,
										"globalpatchername" : "",
										"fontface" : 0,
										"fontsize" : 12.0,
										"default_fontface" : 0
									}

								}

							}
, 							{
								"box" : 								{
									"maxclass" : "multislider",
									"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
									"numinlets" : 1,
									"id" : "obj-5",
									"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
									"numoutlets" : 2,
									"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
									"outlettype" : [ "", "" ],
									"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
									"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
									"size" : 1024,
									"patching_rect" : [ 35.0, 651.0, 727.0, 144.0 ],
									"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
									"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
									"ignoreclick" : 1,
									"setminmax" : [ 0.0, 2.0 ],
									"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "button",
									"numinlets" : 1,
									"id" : "obj-1",
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 35.0, 574.0, 40.0, 40.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "p specifiying_values_and_randomisation",
									"numinlets" : 0,
									"id" : "obj-7",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"color" : [ 0.894118, 0.0, 0.0, 1.0 ],
									"patching_rect" : [ 271.0, 549.200012, 227.0, 20.0 ],
									"patcher" : 									{
										"fileversion" : 1,
										"rect" : [ 235.0, 116.0, 918.0, 808.0 ],
										"bglocked" : 0,
										"defrect" : [ 235.0, 116.0, 918.0, 808.0 ],
										"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"boxes" : [ 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "A Few Examples:",
													"numinlets" : 1,
													"id" : "obj-40",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 36.0, 556.0, 123.0, 23.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main plateau 3-4 9 0-1 8 0.2 1 4 4 1 4 8",
													"numinlets" : 2,
													"id" : "obj-39",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 499.0, 589.0, 267.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main triangle 0.4 1 5-6 10 0 0 0 0 2 3",
													"numinlets" : 2,
													"id" : "obj-38",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 241.0, 589.0, 253.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main flat 4-5",
													"numinlets" : 2,
													"id" : "obj-37",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 589.0, 123.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "The timing of the inflections in relation to the main gesture is set using the \"timings\" message which is follwed by a list of points (0-1) at which new inflections start.",
													"linecount" : 2,
													"numinlets" : 1,
													"id" : "obj-35",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 118.0, 78.0, 788.0, 34.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "p Generate_And_Draw_Gesture",
													"numinlets" : 1,
													"id" : "obj-33",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 620.0, 183.0, 20.0 ],
													"patcher" : 													{
														"fileversion" : 1,
														"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
														"bglocked" : 0,
														"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
														"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 0,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 0,
														"toolbarvisible" : 1,
														"boxanimatetime" : 200,
														"imprint" : 0,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"boxes" : [ 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "zl reg",
																	"numinlets" : 2,
																	"id" : "obj-4",
																	"fontname" : "Arial",
																	"numoutlets" : 2,
																	"outlettype" : [ "", "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "t b l b",
																	"numinlets" : 1,
																	"id" : "obj-1",
																	"fontname" : "Arial",
																	"numoutlets" : 3,
																	"outlettype" : [ "bang", "", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																	"numinlets" : 1,
																	"id" : "obj-26",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Set main gesture",
																	"numinlets" : 1,
																	"id" : "obj-13",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "reset, and set \"no\" inflection gesture",
																	"numinlets" : 1,
																	"id" : "obj-3",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "message",
																	"text" : "reset, gesture_inflections flat 1.",
																	"numinlets" : 2,
																	"id" : "obj-11",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"id" : "obj-9",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																	"comment" : ""
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"id" : "obj-14",
																	"numoutlets" : 0,
																	"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																	"comment" : ""
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "pack 0 0.",
																	"numinlets" : 2,
																	"id" : "obj-2",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "prepend set",
																	"numinlets" : 1,
																	"id" : "obj-5",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "pack 0. 0.",
																	"numinlets" : 2,
																	"id" : "obj-6",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "- 1",
																	"numinlets" : 2,
																	"id" : "obj-8",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "int" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "/ 1023.",
																	"numinlets" : 2,
																	"id" : "obj-15",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "float" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "uzi 1024",
																	"numinlets" : 2,
																	"id" : "obj-17",
																	"fontname" : "Arial",
																	"numoutlets" : 3,
																	"outlettype" : [ "bang", "bang", "int" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "gesture_maker",
																	"numinlets" : 2,
																	"id" : "obj-20",
																	"fontname" : "Arial",
																	"numoutlets" : 2,
																	"outlettype" : [ "", "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"source" : [ "obj-9", 0 ],
																	"destination" : [ "obj-1", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-8", 0 ],
																	"destination" : [ "obj-15", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-6", 0 ],
																	"destination" : [ "obj-20", 1 ],
																	"hidden" : 0,
																	"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																	"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-5", 0 ],
																	"destination" : [ "obj-14", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-4", 0 ],
																	"destination" : [ "obj-20", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-20", 1 ],
																	"destination" : [ "obj-2", 1 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-2", 0 ],
																	"destination" : [ "obj-5", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-17", 2 ],
																	"destination" : [ "obj-8", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-17", 2 ],
																	"destination" : [ "obj-2", 0 ],
																	"hidden" : 0,
																	"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																	"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-15", 0 ],
																	"destination" : [ "obj-6", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-11", 0 ],
																	"destination" : [ "obj-20", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-1", 1 ],
																	"destination" : [ "obj-4", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-1", 0 ],
																	"destination" : [ "obj-17", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-1", 2 ],
																	"destination" : [ "obj-11", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
 ]
													}
,
													"saved_object_attributes" : 													{
														"default_fontname" : "Arial",
														"fontname" : "Arial",
														"default_fontsize" : 12.0,
														"globalpatchername" : "",
														"fontface" : 0,
														"fontsize" : 12.0,
														"default_fontface" : 0
													}

												}

											}
, 											{
												"box" : 												{
													"maxclass" : "multislider",
													"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
													"numinlets" : 1,
													"id" : "obj-34",
													"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
													"numoutlets" : 2,
													"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
													"outlettype" : [ "", "" ],
													"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
													"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
													"size" : 1024,
													"patching_rect" : [ 113.0, 649.0, 720.0, 136.0 ],
													"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
													"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
													"ignoreclick" : 1,
													"setminmax" : [ 0.0, 1.0 ],
													"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "p More_On_Understanding_Curves",
													"numinlets" : 0,
													"id" : "obj-32",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"color" : [ 1.0, 0.0, 0.0, 1.0 ],
													"patching_rect" : [ 324.0, 528.0, 200.0, 20.0 ],
													"patcher" : 													{
														"fileversion" : 1,
														"rect" : [ 141.0, 94.0, 1229.0, 544.0 ],
														"bglocked" : 0,
														"defrect" : [ 141.0, 94.0, 1229.0, 544.0 ],
														"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 0,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 0,
														"toolbarvisible" : 1,
														"boxanimatetime" : 200,
														"imprint" : 0,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"boxes" : [ 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "All Possible Curve Types",
																	"numinlets" : 1,
																	"id" : "obj-87",
																	"fontname" : "Arial Black",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 148.0, 126.0, 172.0, 23.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "The numbers in square brackets show the values relating to the 3 options:",
																	"numinlets" : 1,
																	"id" : "obj-86",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 16.0, 203.666672, 403.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "[s-curve / reverse vals / use reciprocal] ",
																	"numinlets" : 1,
																	"id" : "obj-83",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 94.0, 231.666672, 220.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Band type - denotes  type to use when specifiying  power curvature by band.",
																	"numinlets" : 1,
																	"id" : "obj-81",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 16.0, 179.666672, 417.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type - denotes  type to use when specifiying  power curvature as a float. ",
																	"numinlets" : 1,
																	"id" : "obj-80",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 16.0, 155.0, 425.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 7 / Band type 3 [1,0,1]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-78",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 1031.666626, 217.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 6 / Band type 3 [1,1,1]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-77",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 839.666626, 217.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 5 / Band type 2 [1,1,0]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-76",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 647.666626, 217.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 4 / Band type 2 [1,0,0]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-75",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 455.0, 217.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 3 / Band type 1 [0,0,1]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-74",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 1031.666626, 126.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 2 / Band type 1 [0,1,1]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-73",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 839.666626, 126.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 1 / Band type 0 [0,1,0]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-72",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 647.666626, 126.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Float type 0 / Band type 0 [0,0,0]",
																	"numinlets" : 1,
																	"frgb" : [ 0.0, 0.023529, 0.952941, 1.0 ],
																	"id" : "obj-63",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 455.0, 126.0, 184.0, 20.0 ],
																	"textcolor" : [ 0.0, 0.023529, 0.952941, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "p Draw_Four",
																	"hidden" : 1,
																	"numinlets" : 0,
																	"id" : "obj-61",
																	"fontname" : "Arial",
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 482.0, 126.0, 596.0, 20.0 ],
																	"patcher" : 																	{
																		"fileversion" : 1,
																		"rect" : [ 25.0, 69.0, 916.0, 336.0 ],
																		"bglocked" : 0,
																		"defrect" : [ 25.0, 69.0, 916.0, 336.0 ],
																		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																		"openinpresentation" : 0,
																		"default_fontsize" : 12.0,
																		"default_fontface" : 0,
																		"default_fontname" : "Arial",
																		"gridonopen" : 0,
																		"gridsize" : [ 15.0, 15.0 ],
																		"gridsnaponopen" : 0,
																		"toolbarvisible" : 1,
																		"boxanimatetime" : 200,
																		"imprint" : 0,
																		"enablehscroll" : 1,
																		"enablevscroll" : 1,
																		"devicewidth" : 0.0,
																		"boxes" : [ 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "loadbang",
																					"numinlets" : 1,
																					"id" : "obj-2",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "bang" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 50.0, 35.0, 60.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "deferlow",
																					"numinlets" : 1,
																					"id" : "obj-1",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 50.0, 61.0, 56.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 3 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-35",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 626.0, 100.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-36",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 626.0, 125.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 2 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-30",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 434.0, 100.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-31",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 434.0, 125.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 1 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-27",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 242.0, 100.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-28",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 242.0, 125.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 0 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-24",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 50.0, 100.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-25",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 50.0, 125.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-57",
																					"numoutlets" : 0,
																					"patching_rect" : [ 50.0, 158.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-58",
																					"numoutlets" : 0,
																					"patching_rect" : [ 242.0, 158.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-59",
																					"numoutlets" : 0,
																					"patching_rect" : [ 434.0, 158.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-60",
																					"numoutlets" : 0,
																					"patching_rect" : [ 626.0, 158.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
 ],
																		"lines" : [ 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-36", 0 ],
																					"destination" : [ "obj-60", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-35", 0 ],
																					"destination" : [ "obj-36", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-31", 0 ],
																					"destination" : [ "obj-59", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-30", 0 ],
																					"destination" : [ "obj-31", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-28", 0 ],
																					"destination" : [ "obj-58", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-27", 0 ],
																					"destination" : [ "obj-28", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-25", 0 ],
																					"destination" : [ "obj-57", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-24", 0 ],
																					"destination" : [ "obj-25", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-2", 0 ],
																					"destination" : [ "obj-1", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-35", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 59.5, 90.0, 635.5, 90.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-30", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 59.5, 90.0, 443.5, 90.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-27", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 59.5, 90.0, 251.5, 90.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-24", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
 ]
																	}
,
																	"saved_object_attributes" : 																	{
																		"default_fontname" : "Arial",
																		"fontname" : "Arial",
																		"default_fontsize" : 12.0,
																		"globalpatchername" : "",
																		"fontface" : 0,
																		"fontsize" : 12.0,
																		"default_fontface" : 0
																	}

																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "p Draw_Four",
																	"hidden" : 1,
																	"numinlets" : 0,
																	"id" : "obj-56",
																	"fontname" : "Arial",
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 482.0, 217.0, 596.0, 20.0 ],
																	"patcher" : 																	{
																		"fileversion" : 1,
																		"rect" : [ 25.0, 69.0, 807.0, 192.0 ],
																		"bglocked" : 0,
																		"defrect" : [ 25.0, 69.0, 807.0, 192.0 ],
																		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																		"openinpresentation" : 0,
																		"default_fontsize" : 12.0,
																		"default_fontface" : 0,
																		"default_fontname" : "Arial",
																		"gridonopen" : 0,
																		"gridsize" : [ 15.0, 15.0 ],
																		"gridsnaponopen" : 0,
																		"toolbarvisible" : 1,
																		"boxanimatetime" : 200,
																		"imprint" : 0,
																		"enablehscroll" : 1,
																		"enablevscroll" : 1,
																		"devicewidth" : 0.0,
																		"boxes" : [ 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "loadbang",
																					"numinlets" : 1,
																					"id" : "obj-1",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "bang" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 33.0, 15.0, 60.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "deferlow",
																					"numinlets" : 1,
																					"id" : "obj-51",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 33.0, 41.0, 56.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 7 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-39",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 600.666687, 69.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-40",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 600.666687, 94.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 6 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-42",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 411.666656, 69.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-43",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 411.666656, 94.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 5 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-45",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 222.666656, 69.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-46",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 222.666656, 94.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "gesture_main line 0.1 0.9 4 1. 1.5",
																					"numinlets" : 2,
																					"id" : "obj-48",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 33.0, 69.0, 189.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "p Generate_And_Draw_Gesture",
																					"numinlets" : 1,
																					"id" : "obj-49",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 33.0, 94.0, 183.0, 20.0 ],
																					"patcher" : 																					{
																						"fileversion" : 1,
																						"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"bglocked" : 0,
																						"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																						"openinpresentation" : 0,
																						"default_fontsize" : 12.0,
																						"default_fontface" : 0,
																						"default_fontname" : "Arial",
																						"gridonopen" : 0,
																						"gridsize" : [ 15.0, 15.0 ],
																						"gridsnaponopen" : 0,
																						"toolbarvisible" : 1,
																						"boxanimatetime" : 200,
																						"imprint" : 0,
																						"enablehscroll" : 1,
																						"enablevscroll" : 1,
																						"devicewidth" : 0.0,
																						"boxes" : [ 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "zl reg",
																									"numinlets" : 2,
																									"id" : "obj-4",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "t b l b",
																									"numinlets" : 1,
																									"id" : "obj-1",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "", "bang" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																									"numinlets" : 1,
																									"id" : "obj-26",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "Set main gesture",
																									"numinlets" : 1,
																									"id" : "obj-13",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "comment",
																									"text" : "reset, and set \"no\" inflection gesture",
																									"numinlets" : 1,
																									"id" : "obj-3",
																									"fontname" : "Arial",
																									"numoutlets" : 0,
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "message",
																									"text" : "reset, gesture_inflections flat 1.",
																									"numinlets" : 2,
																									"id" : "obj-11",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "inlet",
																									"numinlets" : 0,
																									"id" : "obj-9",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "outlet",
																									"numinlets" : 1,
																									"id" : "obj-14",
																									"numoutlets" : 0,
																									"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																									"comment" : ""
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0 0.",
																									"numinlets" : 2,
																									"id" : "obj-2",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "prepend set",
																									"numinlets" : 1,
																									"id" : "obj-5",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "pack 0. 0.",
																									"numinlets" : 2,
																									"id" : "obj-6",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "- 1",
																									"numinlets" : 2,
																									"id" : "obj-8",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "/ 1023.",
																									"numinlets" : 2,
																									"id" : "obj-15",
																									"fontname" : "Arial",
																									"numoutlets" : 1,
																									"outlettype" : [ "float" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "uzi 1024",
																									"numinlets" : 2,
																									"id" : "obj-17",
																									"fontname" : "Arial",
																									"numoutlets" : 3,
																									"outlettype" : [ "bang", "bang", "int" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																								}

																							}
, 																							{
																								"box" : 																								{
																									"maxclass" : "newobj",
																									"text" : "gesture_maker",
																									"numinlets" : 2,
																									"id" : "obj-20",
																									"fontname" : "Arial",
																									"numoutlets" : 2,
																									"outlettype" : [ "", "" ],
																									"fontsize" : 12.0,
																									"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																								}

																							}
 ],
																						"lines" : [ 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-9", 0 ],
																									"destination" : [ "obj-1", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-8", 0 ],
																									"destination" : [ "obj-15", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-6", 0 ],
																									"destination" : [ "obj-20", 1 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																									"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-5", 0 ],
																									"destination" : [ "obj-14", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-4", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-20", 1 ],
																									"destination" : [ "obj-2", 1 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-2", 0 ],
																									"destination" : [ "obj-5", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-8", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-17", 2 ],
																									"destination" : [ "obj-2", 0 ],
																									"hidden" : 0,
																									"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																									"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-15", 0 ],
																									"destination" : [ "obj-6", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-11", 0 ],
																									"destination" : [ "obj-20", 0 ],
																									"hidden" : 0,
																									"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 1 ],
																									"destination" : [ "obj-4", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 0 ],
																									"destination" : [ "obj-17", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
, 																							{
																								"patchline" : 																								{
																									"source" : [ "obj-1", 2 ],
																									"destination" : [ "obj-11", 0 ],
																									"hidden" : 0,
																									"midpoints" : [  ]
																								}

																							}
 ]
																					}
,
																					"saved_object_attributes" : 																					{
																						"default_fontname" : "Arial",
																						"fontname" : "Arial",
																						"default_fontsize" : 12.0,
																						"globalpatchername" : "",
																						"fontface" : 0,
																						"fontsize" : 12.0,
																						"default_fontface" : 0
																					}

																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-52",
																					"numoutlets" : 0,
																					"patching_rect" : [ 33.0, 126.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-53",
																					"numoutlets" : 0,
																					"patching_rect" : [ 222.666656, 126.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-54",
																					"numoutlets" : 0,
																					"patching_rect" : [ 411.666656, 126.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-55",
																					"numoutlets" : 0,
																					"patching_rect" : [ 600.666687, 126.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
 ],
																		"lines" : [ 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-51", 0 ],
																					"destination" : [ "obj-48", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-51", 0 ],
																					"destination" : [ "obj-45", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 42.5, 64.5, 232.166656, 64.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-51", 0 ],
																					"destination" : [ "obj-42", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 42.5, 64.5, 421.166656, 64.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-51", 0 ],
																					"destination" : [ "obj-39", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 42.5, 64.5, 610.166687, 64.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-49", 0 ],
																					"destination" : [ "obj-52", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-48", 0 ],
																					"destination" : [ "obj-49", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-46", 0 ],
																					"destination" : [ "obj-53", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-45", 0 ],
																					"destination" : [ "obj-46", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-43", 0 ],
																					"destination" : [ "obj-54", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-42", 0 ],
																					"destination" : [ "obj-43", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-40", 0 ],
																					"destination" : [ "obj-55", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-39", 0 ],
																					"destination" : [ "obj-40", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-51", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
 ]
																	}
,
																	"saved_object_attributes" : 																	{
																		"default_fontname" : "Arial",
																		"fontname" : "Arial",
																		"default_fontsize" : 12.0,
																		"globalpatchername" : "",
																		"fontface" : 0,
																		"fontsize" : 12.0,
																		"default_fontface" : 0
																	}

																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-41",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 1058.666626, 244.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-44",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 866.666626, 244.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-47",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 673.666626, 244.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-50",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 483.0, 244.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-38",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 1058.666626, 154.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-32",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 866.666626, 154.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-29",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 673.666626, 154.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-26",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 483.0, 154.0, 140.0, 54.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Explore the shape of different s-curves (using type 0 curve) specified either as an band or a float. On the left the s-curve is applied to a straight line. On the right the curve is applied to a power curve.",
																	"linecount" : 4,
																	"numinlets" : 1,
																	"id" : "obj-23",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 303.0, 433.0, 281.0, 62.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Explore the shape of different power curves using floats / ints - note that the float values equate to only half the range of the int values (you need to alter the type to use the reciprocal float value and access the other half of the range). Bands 0-4 equate to float values 1.-15 whilst bands 5-9 equate to the reciprocal values.",
																	"linecount" : 6,
																	"numinlets" : 1,
																	"id" : "obj-22",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 877.0, 433.0, 313.0, 89.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "The curve types are based on three factors: The direction of the s-curve, whether all values are reversed before and after calculation (which changes the shape) and whether or not the power value should be used as a reciprocal. For power curvature expressed as a float this gives 8 values (0-7) from 2 x 2 x 2 options. When specifiying the power curvature one of these options (reversal) is accounted for in the band range (0 is equivalent to the reversal of 9 for example - bands 4 and 5 are closest to straight) so only 4 types (0-3) are available.",
																	"linecount" : 3,
																	"numinlets" : 1,
																	"id" : "obj-20",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 131.0, 60.0, 1085.0, 48.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Curves are made by interpolationg between a power curve and a mapped s-curve version of that power curve (technically the power curve is the result of raising a straight line in the range 0-1 to the power ot a specified value and the s-curve maps the values 0-1 onto a quarter of a sine wave). This combination gives a large number of possible curve types, but they can be a little hard to understand.",
																	"linecount" : 2,
																	"numinlets" : 1,
																	"id" : "obj-18",
																	"fontname" : "Arial",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 23.0, 11.0, 1172.0, 34.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "S-Curve Interpolation",
																	"numinlets" : 1,
																	"id" : "obj-16",
																	"fontname" : "Arial Black",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 25.0, 317.0, 151.0, 23.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Power Curves",
																	"numinlets" : 1,
																	"id" : "obj-15",
																	"fontname" : "Arial Black",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 602.0, 317.0, 101.0, 23.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "number",
																	"numinlets" : 1,
																	"id" : "obj-11",
																	"fontname" : "Arial",
																	"minimum" : 0,
																	"numoutlets" : 2,
																	"maximum" : 4,
																	"outlettype" : [ "int", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 282.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "flonum",
																	"numinlets" : 1,
																	"id" : "obj-12",
																	"fontname" : "Arial",
																	"minimum" : 0.0,
																	"numoutlets" : 2,
																	"maximum" : 1.0,
																	"outlettype" : [ "float", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 221.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "message",
																	"text" : "gesture_main line 0.1 0.9 0 $1 1.",
																	"numinlets" : 2,
																	"id" : "obj-13",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 221.0, 383.0, 185.0, 18.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "number",
																	"numinlets" : 1,
																	"id" : "obj-5",
																	"fontname" : "Arial",
																	"minimum" : 0,
																	"numoutlets" : 2,
																	"maximum" : 4,
																	"outlettype" : [ "int", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 90.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "flonum",
																	"numinlets" : 1,
																	"id" : "obj-6",
																	"fontname" : "Arial",
																	"minimum" : 1.0,
																	"numoutlets" : 2,
																	"maximum" : 15.0,
																	"outlettype" : [ "float", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 31.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "message",
																	"text" : "gesture_main line 0.1 0.9 0 $1 2.",
																	"numinlets" : 2,
																	"id" : "obj-7",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 31.0, 383.0, 185.0, 18.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "p Generate_And_Draw_Gesture",
																	"numinlets" : 1,
																	"id" : "obj-8",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 31.0, 408.0, 183.0, 20.0 ],
																	"patcher" : 																	{
																		"fileversion" : 1,
																		"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																		"bglocked" : 0,
																		"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																		"openinpresentation" : 0,
																		"default_fontsize" : 12.0,
																		"default_fontface" : 0,
																		"default_fontname" : "Arial",
																		"gridonopen" : 0,
																		"gridsize" : [ 15.0, 15.0 ],
																		"gridsnaponopen" : 0,
																		"toolbarvisible" : 1,
																		"boxanimatetime" : 200,
																		"imprint" : 0,
																		"enablehscroll" : 1,
																		"enablevscroll" : 1,
																		"devicewidth" : 0.0,
																		"boxes" : [ 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "zl reg",
																					"numinlets" : 2,
																					"id" : "obj-4",
																					"fontname" : "Arial",
																					"numoutlets" : 2,
																					"outlettype" : [ "", "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "t b l b",
																					"numinlets" : 1,
																					"id" : "obj-1",
																					"fontname" : "Arial",
																					"numoutlets" : 3,
																					"outlettype" : [ "bang", "", "bang" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																					"numinlets" : 1,
																					"id" : "obj-26",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "Set main gesture",
																					"numinlets" : 1,
																					"id" : "obj-13",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "reset, and set \"no\" inflection gesture",
																					"numinlets" : 1,
																					"id" : "obj-3",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "reset, gesture_inflections flat 1.",
																					"numinlets" : 2,
																					"id" : "obj-11",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "inlet",
																					"numinlets" : 0,
																					"id" : "obj-9",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-14",
																					"numoutlets" : 0,
																					"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "pack 0 0.",
																					"numinlets" : 2,
																					"id" : "obj-2",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "prepend set",
																					"numinlets" : 1,
																					"id" : "obj-5",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "pack 0. 0.",
																					"numinlets" : 2,
																					"id" : "obj-6",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "- 1",
																					"numinlets" : 2,
																					"id" : "obj-8",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "int" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "/ 1023.",
																					"numinlets" : 2,
																					"id" : "obj-15",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "float" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "uzi 1024",
																					"numinlets" : 2,
																					"id" : "obj-17",
																					"fontname" : "Arial",
																					"numoutlets" : 3,
																					"outlettype" : [ "bang", "bang", "int" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "gesture_maker",
																					"numinlets" : 2,
																					"id" : "obj-20",
																					"fontname" : "Arial",
																					"numoutlets" : 2,
																					"outlettype" : [ "", "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																				}

																			}
 ],
																		"lines" : [ 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-9", 0 ],
																					"destination" : [ "obj-1", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-8", 0 ],
																					"destination" : [ "obj-15", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-6", 0 ],
																					"destination" : [ "obj-20", 1 ],
																					"hidden" : 0,
																					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																					"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-5", 0 ],
																					"destination" : [ "obj-14", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-4", 0 ],
																					"destination" : [ "obj-20", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-20", 1 ],
																					"destination" : [ "obj-2", 1 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-2", 0 ],
																					"destination" : [ "obj-5", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-17", 2 ],
																					"destination" : [ "obj-8", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-17", 2 ],
																					"destination" : [ "obj-2", 0 ],
																					"hidden" : 0,
																					"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																					"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-15", 0 ],
																					"destination" : [ "obj-6", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-11", 0 ],
																					"destination" : [ "obj-20", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 1 ],
																					"destination" : [ "obj-4", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-17", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 2 ],
																					"destination" : [ "obj-11", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
 ]
																	}
,
																	"saved_object_attributes" : 																	{
																		"default_fontname" : "Arial",
																		"fontname" : "Arial",
																		"default_fontsize" : 12.0,
																		"globalpatchername" : "",
																		"fontface" : 0,
																		"fontsize" : 12.0,
																		"default_fontface" : 0
																	}

																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-9",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 32.0, 433.0, 264.0, 79.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "number",
																	"numinlets" : 1,
																	"id" : "obj-4",
																	"fontname" : "Arial",
																	"minimum" : 0,
																	"numoutlets" : 2,
																	"maximum" : 9,
																	"outlettype" : [ "int", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 661.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "flonum",
																	"numinlets" : 1,
																	"id" : "obj-2",
																	"fontname" : "Arial",
																	"minimum" : 1.0,
																	"numoutlets" : 2,
																	"maximum" : 15.0,
																	"outlettype" : [ "float", "bang" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 602.0, 351.0, 50.0, 20.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "message",
																	"text" : "gesture_main line 0.1 0.9 0 0. $1",
																	"numinlets" : 2,
																	"id" : "obj-37",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 602.0, 383.0, 185.0, 18.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "newobj",
																	"text" : "p Generate_And_Draw_Gesture",
																	"numinlets" : 1,
																	"id" : "obj-33",
																	"fontname" : "Arial",
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"fontsize" : 12.0,
																	"patching_rect" : [ 602.0, 408.0, 183.0, 20.0 ],
																	"patcher" : 																	{
																		"fileversion" : 1,
																		"rect" : [ 25.0, 69.0, 641.0, 305.0 ],
																		"bglocked" : 0,
																		"defrect" : [ 25.0, 69.0, 641.0, 305.0 ],
																		"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
																		"openinpresentation" : 0,
																		"default_fontsize" : 12.0,
																		"default_fontface" : 0,
																		"default_fontname" : "Arial",
																		"gridonopen" : 0,
																		"gridsize" : [ 15.0, 15.0 ],
																		"gridsnaponopen" : 0,
																		"toolbarvisible" : 1,
																		"boxanimatetime" : 200,
																		"imprint" : 0,
																		"enablehscroll" : 1,
																		"enablevscroll" : 1,
																		"devicewidth" : 0.0,
																		"boxes" : [ 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "zl reg",
																					"numinlets" : 2,
																					"id" : "obj-4",
																					"fontname" : "Arial",
																					"numoutlets" : 2,
																					"outlettype" : [ "", "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 253.0, 116.0, 40.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "t b l b",
																					"numinlets" : 1,
																					"id" : "obj-1",
																					"fontname" : "Arial",
																					"numoutlets" : 3,
																					"outlettype" : [ "bang", "", "bang" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 80.0, 40.0, 364.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "N.B. Use uzi + chaining input to get desired values for multislider",
																					"numinlets" : 1,
																					"id" : "obj-26",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 126.0, 13.0, 354.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "Set main gesture",
																					"numinlets" : 1,
																					"id" : "obj-13",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 238.0, 185.0, 101.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "comment",
																					"text" : "reset, and set \"no\" inflection gesture",
																					"numinlets" : 1,
																					"id" : "obj-3",
																					"fontname" : "Arial",
																					"numoutlets" : 0,
																					"fontsize" : 12.0,
																					"patching_rect" : [ 424.0, 185.0, 207.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "message",
																					"text" : "reset, gesture_inflections flat 1.",
																					"numinlets" : 2,
																					"id" : "obj-11",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 424.666687, 147.0, 177.0, 18.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "inlet",
																					"numinlets" : 0,
																					"id" : "obj-9",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"patching_rect" : [ 80.0, 10.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "outlet",
																					"numinlets" : 1,
																					"id" : "obj-14",
																					"numoutlets" : 0,
																					"patching_rect" : [ 27.0, 267.0, 25.0, 25.0 ],
																					"comment" : ""
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "pack 0 0.",
																					"numinlets" : 2,
																					"id" : "obj-2",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 27.0, 213.0, 81.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "prepend set",
																					"numinlets" : 1,
																					"id" : "obj-5",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 27.0, 239.0, 74.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "pack 0. 0.",
																					"numinlets" : 2,
																					"id" : "obj-6",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 147.0, 63.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "- 1",
																					"numinlets" : 2,
																					"id" : "obj-8",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "int" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 99.0, 32.5, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "/ 1023.",
																					"numinlets" : 2,
																					"id" : "obj-15",
																					"fontname" : "Arial",
																					"numoutlets" : 1,
																					"outlettype" : [ "float" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 144.0, 123.0, 47.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "uzi 1024",
																					"numinlets" : 2,
																					"id" : "obj-17",
																					"fontname" : "Arial",
																					"numoutlets" : 3,
																					"outlettype" : [ "bang", "bang", "int" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 80.0, 64.0, 56.0, 20.0 ]
																				}

																			}
, 																			{
																				"box" : 																				{
																					"maxclass" : "newobj",
																					"text" : "gesture_maker",
																					"numinlets" : 2,
																					"id" : "obj-20",
																					"fontname" : "Arial",
																					"numoutlets" : 2,
																					"outlettype" : [ "", "" ],
																					"fontsize" : 12.0,
																					"patching_rect" : [ 53.0, 180.0, 91.0, 20.0 ]
																				}

																			}
 ],
																		"lines" : [ 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-9", 0 ],
																					"destination" : [ "obj-1", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-8", 0 ],
																					"destination" : [ "obj-15", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-6", 0 ],
																					"destination" : [ "obj-20", 1 ],
																					"hidden" : 0,
																					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
																					"midpoints" : [ 153.5, 170.0, 134.5, 170.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-5", 0 ],
																					"destination" : [ "obj-14", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-4", 0 ],
																					"destination" : [ "obj-20", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 262.5, 174.5, 62.5, 174.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-20", 1 ],
																					"destination" : [ "obj-2", 1 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-2", 0 ],
																					"destination" : [ "obj-5", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-17", 2 ],
																					"destination" : [ "obj-8", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 126.5, 93.0, 153.5, 93.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-17", 2 ],
																					"destination" : [ "obj-2", 0 ],
																					"hidden" : 0,
																					"color" : [ 1.0, 0.541176, 0.843137, 1.0 ],
																					"midpoints" : [ 126.5, 134.0, 36.5, 134.0 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-15", 0 ],
																					"destination" : [ "obj-6", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-11", 0 ],
																					"destination" : [ "obj-20", 0 ],
																					"hidden" : 0,
																					"midpoints" : [ 434.166687, 174.5, 62.5, 174.5 ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 1 ],
																					"destination" : [ "obj-4", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 0 ],
																					"destination" : [ "obj-17", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
, 																			{
																				"patchline" : 																				{
																					"source" : [ "obj-1", 2 ],
																					"destination" : [ "obj-11", 0 ],
																					"hidden" : 0,
																					"midpoints" : [  ]
																				}

																			}
 ]
																	}
,
																	"saved_object_attributes" : 																	{
																		"default_fontname" : "Arial",
																		"fontname" : "Arial",
																		"default_fontsize" : 12.0,
																		"globalpatchername" : "",
																		"fontface" : 0,
																		"fontsize" : 12.0,
																		"default_fontface" : 0
																	}

																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "multislider",
																	"candicane6" : [ 0.733333, 0.035294, 0.788235, 1.0 ],
																	"numinlets" : 1,
																	"id" : "obj-34",
																	"candicane5" : [ 0.584314, 0.827451, 0.431373, 1.0 ],
																	"numoutlets" : 2,
																	"candicane4" : [ 0.439216, 0.619608, 0.070588, 1.0 ],
																	"outlettype" : [ "", "" ],
																	"peakcolor" : [ 0.498039, 0.498039, 0.498039, 1.0 ],
																	"candicane3" : [ 0.290196, 0.411765, 0.713726, 1.0 ],
																	"size" : 1024,
																	"patching_rect" : [ 603.0, 433.0, 264.0, 79.0 ],
																	"candicane8" : [ 0.027451, 0.447059, 0.501961, 1.0 ],
																	"candicane2" : [ 0.145098, 0.203922, 0.356863, 1.0 ],
																	"ignoreclick" : 1,
																	"setminmax" : [ 0.0, 1.0 ],
																	"candicane7" : [ 0.878431, 0.243137, 0.145098, 1.0 ]
																}

															}
, 															{
																"box" : 																{
																	"maxclass" : "comment",
																	"text" : "Curve Types",
																	"numinlets" : 1,
																	"id" : "obj-10",
																	"fontname" : "Arial Black",
																	"numoutlets" : 0,
																	"fontsize" : 12.0,
																	"patching_rect" : [ 26.0, 59.0, 92.0, 23.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"source" : [ "obj-8", 0 ],
																	"destination" : [ "obj-9", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-7", 0 ],
																	"destination" : [ "obj-8", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-61", 3 ],
																	"destination" : [ "obj-38", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-61", 2 ],
																	"destination" : [ "obj-32", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-61", 1 ],
																	"destination" : [ "obj-29", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-61", 0 ],
																	"destination" : [ "obj-26", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-6", 0 ],
																	"destination" : [ "obj-7", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-56", 0 ],
																	"destination" : [ "obj-50", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-56", 1 ],
																	"destination" : [ "obj-47", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-56", 2 ],
																	"destination" : [ "obj-44", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-56", 3 ],
																	"destination" : [ "obj-41", 0 ],
																	"hidden" : 1,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-5", 0 ],
																	"destination" : [ "obj-7", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 99.5, 376.5, 40.5, 376.5 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-4", 0 ],
																	"destination" : [ "obj-37", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 670.5, 376.5, 611.5, 376.5 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-37", 0 ],
																	"destination" : [ "obj-33", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-33", 0 ],
																	"destination" : [ "obj-34", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-2", 0 ],
																	"destination" : [ "obj-37", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-13", 0 ],
																	"destination" : [ "obj-8", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 230.5, 404.0, 40.5, 404.0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-12", 0 ],
																	"destination" : [ "obj-13", 0 ],
																	"hidden" : 0,
																	"midpoints" : [  ]
																}

															}
, 															{
																"patchline" : 																{
																	"source" : [ "obj-11", 0 ],
																	"destination" : [ "obj-13", 0 ],
																	"hidden" : 0,
																	"midpoints" : [ 291.5, 376.5, 230.5, 376.5 ]
																}

															}
 ]
													}
,
													"saved_object_attributes" : 													{
														"default_fontname" : "Arial",
														"fontname" : "Arial",
														"default_fontsize" : 12.0,
														"globalpatchername" : "",
														"fontface" : 0,
														"fontsize" : 12.0,
														"default_fontface" : 0
													}

												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Specifiers:",
													"numinlets" : 1,
													"id" : "obj-26",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 18.0, 126.0, 81.0, 23.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Messages:",
													"numinlets" : 1,
													"id" : "obj-21",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 18.0, 17.0, 80.0, 23.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "To set the shape of the main kernel usage the 'gesture_main\" message followed by the name of the shape and the associated kernel parameters.",
													"numinlets" : 1,
													"id" : "obj-17",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 114.0, 17.0, 785.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "To set the shape of the inflection kernels use the \"gesture_inflections\" message in a similar manner. Each time you call this method the kernel is added to the end of the internally stored list until you call \"reset\" to remove all stored inflection kernel parameters.",
													"linecount" : 2,
													"numinlets" : 1,
													"id" : "obj-19",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 114.0, 41.0, 788.0, 34.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- The output values determine the start / end values that curves take (the y values)",
													"numinlets" : 1,
													"id" : "obj-15",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 371.0, 448.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- The time point determines when one line (or curve) ends, and another starts).",
													"numinlets" : 1,
													"id" : "obj-14",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 287.0, 428.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Random values will be chosen each time a kernel is executed, so the gesture will be different each time.",
													"numinlets" : 1,
													"id" : "obj-51",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 223.0, 563.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "When specifiying a band range a badn in the specified range will be randomly chosen before randomly choosing the final value.",
													"linecount" : 2,
													"numinlets" : 1,
													"id" : "obj-50",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 186.0, 557.0, 34.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Bands divide the available space for a parameter into a set of areas within which a value is randomly chosen.",
													"numinlets" : 1,
													"id" : "obj-49",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 163.0, 589.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Most values for kernels can either be specified exactly (using float values), OR by specifiying a band (using a single int) or band range (by specifying two ints with a dash (without spaces - e.g. 2-4) for randomisation.",
													"linecount" : 2,
													"numinlets" : 1,
													"id" : "obj-47",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 113.0, 126.0, 591.0, 34.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- Either an absolute value between 1-15 OR a band (or band range) between 0 and 9",
													"numinlets" : 1,
													"id" : "obj-31",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 394.0, 487.0, 462.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- Either an absolute value between 0-1 OR a band (or band range) between 0 and 4",
													"numinlets" : 1,
													"id" : "obj-29",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 394.0, 463.0, 455.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "\t- Power Curvature:",
													"numinlets" : 1,
													"id" : "obj-27",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 247.0, 487.0, 115.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- S Curve Interpolation:",
													"numinlets" : 1,
													"id" : "obj-25",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 247.0, 463.0, 134.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "\t- Type: ",
													"numinlets" : 1,
													"id" : "obj-22",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 247.0, 439.0, 55.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- The \"last\" specifier can also be used to specifiy the last output value of a kernel (useful for successive inflection kernels).",
													"numinlets" : 1,
													"id" : "obj-13",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 347.0, 660.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- an int between 0 and 7 if a float is given for power curvature or 0 to 3 if not\t",
													"numinlets" : 1,
													"id" : "obj-11",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 394.0, 439.0, 415.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- In Three parts:",
													"numinlets" : 1,
													"id" : "obj-9",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 411.0, 96.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "(Start / Target / End Vals)",
													"numinlets" : 1,
													"id" : "obj-7",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 65.0, 347.0, 144.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "- Either an absolute value between 0 and 1 OR a band (or band range) between 0 and 14",
													"numinlets" : 1,
													"id" : "obj-5",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 266.0, 485.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "-- Either an absolute value between 0-1 OR a band (or band range) between 0 and 10",
													"numinlets" : 1,
													"id" : "obj-24",
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 228.0, 324.0, 466.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Curvature Values",
													"numinlets" : 1,
													"id" : "obj-4",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 79.0, 411.0, 125.0, 23.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Output Values",
													"numinlets" : 1,
													"id" : "obj-3",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 79.0, 324.0, 104.0, 23.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "Timing Values",
													"numinlets" : 1,
													"id" : "obj-2",
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0,
													"patching_rect" : [ 79.0, 266.0, 104.0, 23.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"source" : [ "obj-39", 0 ],
													"destination" : [ "obj-33", 0 ],
													"hidden" : 0,
													"midpoints" : [ 508.5, 613.5, 122.5, 613.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-38", 0 ],
													"destination" : [ "obj-33", 0 ],
													"hidden" : 0,
													"midpoints" : [ 250.5, 613.0, 122.5, 613.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-37", 0 ],
													"destination" : [ "obj-33", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-33", 0 ],
													"destination" : [ "obj-34", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
 ]
									}
,
									"saved_object_attributes" : 									{
										"default_fontname" : "Arial",
										"fontname" : "Arial",
										"default_fontsize" : 12.0,
										"globalpatchername" : "",
										"fontface" : 0,
										"fontsize" : 12.0,
										"default_fontface" : 0
									}

								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "general",
									"numinlets" : 1,
									"id" : "obj-65",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 495.0, 61.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "return",
									"numinlets" : 1,
									"id" : "obj-63",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 451.0, 52.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "plateau",
									"numinlets" : 1,
									"id" : "obj-61",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 407.0, 61.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "plateau_return",
									"numinlets" : 1,
									"id" : "obj-59",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 363.0, 107.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "triangle",
									"numinlets" : 1,
									"id" : "obj-57",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 319.0, 62.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "triangle_return",
									"numinlets" : 1,
									"id" : "obj-56",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 275.0, 109.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "flat_line",
									"numinlets" : 1,
									"id" : "obj-55",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 231.0, 63.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "line_flat",
									"numinlets" : 1,
									"id" : "obj-53",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 187.0, 63.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "line",
									"numinlets" : 1,
									"id" : "obj-51",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 143.0, 35.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "flat ",
									"numinlets" : 1,
									"id" : "obj-49",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 105.0, 99.0, 37.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time , Start Val, Target Val, End Val,  Curve Vals 1, Curve Vals 2)",
									"numinlets" : 1,
									"id" : "obj-47",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 340.0, 360.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time , Start Val, Target Val, Curve Vals 1, Curve Vals 2)",
									"numinlets" : 1,
									"id" : "obj-42",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 296.0, 308.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time , Start Val, End Val, Curve Vals)",
									"numinlets" : 1,
									"id" : "obj-40",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 251.0, 212.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time 1, Time 2, Start Val, Target Val, Curve Vals 1, Curve Vals 2)",
									"numinlets" : 1,
									"id" : "obj-38",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 384.0, 358.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time 1, Time 2, Start Val, Target Val, End Val, Curve Vals 1, Curve Vals 2)",
									"numinlets" : 1,
									"id" : "obj-36",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 428.0, 406.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "\t(Time 1, Time 2, Start Val, First Target Val, Second Target Vals , Curve Vals 1, Curve Vals 2, Curve Vals 3)",
									"numinlets" : 1,
									"id" : "obj-34",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 472.0, 575.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Time 1, Time 2, Start Val, First Target Val, Second Target Val, End Val, , Curve Vals 1, Curve Vals 2, Curve Vals 3)",
									"numinlets" : 1,
									"id" : "obj-32",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 516.0, 625.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "\t(Time, Start Val, End Val, Curve Vals)",
									"numinlets" : 1,
									"id" : "obj-30",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 207.0, 212.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Start Val, End Val, Curve Vals)",
									"numinlets" : 1,
									"id" : "obj-28",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 164.0, 176.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "(Flat Val)",
									"numinlets" : 1,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 121.0, 58.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- kernel takes a single value for its duration.",
									"numinlets" : 1,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 100.0, 242.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- kernel is a single line or curved from one value to another.\t",
									"numinlets" : 1,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 143.0, 329.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- a single line or curve until the given time value followed by a single value for the rest of the kernel .\t\t",
									"numinlets" : 1,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 186.0, 547.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- a single value up until the specified time and then a single line or curve.",
									"numinlets" : 1,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 230.0, 397.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- two lines (or curves) - the second returns to the same value with which the kernel starts.",
									"numinlets" : 1,
									"id" : "obj-14",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 275.0, 484.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- two lines (or curves).\t\t",
									"numinlets" : 1,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 319.0, 135.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- three lines (or curves) - the central one is flat (single value) and the kernel ends on the same value it begins.",
									"numinlets" : 1,
									"id" : "obj-10",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 363.0, 591.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- three lines (or curves) - the central one is flat (single value).",
									"numinlets" : 1,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 407.0, 333.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "- three lines (or curves) - the kernel begins and ends on the same value.",
									"numinlets" : 1,
									"id" : "obj-6",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 451.0, 393.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "a kernel with three unconstrained lines (or curves). \t\t",
									"numinlets" : 1,
									"id" : "obj-4",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 215.0, 495.0, 340.0, 20.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-43", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-44", 0 ],
									"destination" : [ "obj-45", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-41", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-29", 0 ],
									"destination" : [ "obj-31", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-25", 0 ],
									"destination" : [ "obj-27", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-24", 0 ],
									"destination" : [ "obj-33", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-2", 0 ],
									"destination" : [ "obj-39", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-19", 0 ],
									"destination" : [ "obj-35", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-15", 0 ],
									"destination" : [ "obj-5", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-13", 0 ],
									"destination" : [ "obj-37", 0 ],
									"hidden" : 1,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-15", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "autohelp",
					"hidden" : 1,
					"numinlets" : 1,
					"id" : "obj-14",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 11.595187,
					"patching_rect" : [ 787.0, 646.0, 55.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_title",
					"text" : "gesture_maker",
					"numinlets" : 1,
					"frgb" : [ 0.93, 0.93, 0.97, 1.0 ],
					"id" : "obj-15",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontface" : 3,
					"fontsize" : 20.871338,
					"patching_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_digest",
					"text" : "Complex gestural control over parameters",
					"numinlets" : 1,
					"frgb" : [ 0.93, 0.93, 0.97, 1.0 ],
					"id" : "obj-16",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.754705,
					"patching_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_description",
					"text" : "gesture_maker is an object for creating multipart gestural control in MaxMSP. Typically, gesture_maker is used to drive a line~ object for audio rate control, and the output is internally scaled to give useful output for musical control. Each gesture has two layers - the main layer, and the inflection layer. Each layer is scaled according to a certain range and scaling type, and the two layers are then combined through multiplication (which is appropriate for most musical cases). The basis of the gesture is the kernel which is made up of up to three curves, which may take a variety of shapes, using power and s-shaped curves. The main layer has only one kernel shape per gesture - the inflection layer has one or more kernel shapes (as specified), which occur one after the other. The shape of each gestural kernel over time can be given with varying degrees of specificity to allow either micro variation of a gesture, or radically different shapes from a single set of values. The timings of the inflection gestures can also be specified as desired. The object can either be triggered to output values at regular intervals (drive mode) over a specified time period, or to produce output at given points in time (events mode). Additionally, gesture_maker objects may be chained together to control multiple parameters in parallel.",
					"linecount" : 8,
					"numinlets" : 1,
					"id" : "obj-17",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 11.595187,
					"patching_rect" : [ 10.0, 57.0, 908.0, 113.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "textbutton",
					"varname" : "autohelp_top_ref",
					"hidden" : 1,
					"numinlets" : 1,
					"spacing_y" : 0.0,
					"id" : "obj-26",
					"textoncolor" : [ 0.27, 0.35, 0.47, 1.0 ],
					"fontname" : "Arial",
					"numoutlets" : 3,
					"bgcolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"presentation_rect" : [ 90.0, 90.0, 193.740402, 14.666666 ],
					"fontlink" : 1,
					"textovercolor" : [ 0.4, 0.5, 0.65, 1.0 ],
					"text" : "open gesture_maker reference",
					"outlettype" : [ "", "", "int" ],
					"bgovercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"fontface" : 3,
					"fontsize" : 12.754705,
					"patching_rect" : [ 306.259583, 22.0, 193.740402, 14.666666 ],
					"bgoveroncolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"underline" : 1,
					"textcolor" : [ 0.348, 0.44, 0.578, 1.0 ],
					"bordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"spacing_x" : 0.0,
					"bgoncolor" : [ 0.0, 0.0, 0.0, 0.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_see_title",
					"text" : "See Also:",
					"numinlets" : 1,
					"id" : "obj-79",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontface" : 1,
					"fontsize" : 11.595187,
					"patching_rect" : [ 792.0, 650.0, 100.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "umenu",
					"varname" : "autohelp_see_menu",
					"numinlets" : 1,
					"id" : "obj-80",
					"fontname" : "Arial",
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"fontsize" : 11.595187,
					"types" : [  ],
					"patching_rect" : [ 792.0, 670.0, 130.0, 20.0 ],
					"items" : [ "(Objects:)", ",", "line", ",", "line~", ",", "timefilter", ",", "timemap" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "bangs at end of gesture",
					"numinlets" : 1,
					"id" : "obj-13",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 291.0, 137.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "parameters / messages in",
					"numinlets" : 1,
					"id" : "obj-12",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 73.0, 228.0, 149.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "external drive in (see chaining subpatch)",
					"numinlets" : 1,
					"id" : "obj-11",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 361.0, 228.0, 226.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "drive out (for chaining)",
					"numinlets" : 1,
					"id" : "obj-10",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 404.0, 291.0, 137.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "values out (val / time)",
					"numinlets" : 1,
					"id" : "obj-9",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"patching_rect" : [ 254.0, 291.0, 125.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p layers_and_kernels",
					"numinlets" : 0,
					"id" : "obj-3",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.0, 0.094118, 1.0, 1.0 ],
					"patching_rect" : [ 630.0, 389.0, 125.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 251.0, 87.0, 659.0, 666.0 ],
						"bglocked" : 0,
						"defrect" : [ 251.0, 87.0, 659.0, 666.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "With inflection layer on",
									"numinlets" : 1,
									"id" : "obj-34",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 402.0, 303.0, 163.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "With inflection layer off (flat)",
									"numinlets" : 1,
									"id" : "obj-33",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 58.0, 303.0, 200.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pink~",
									"numinlets" : 1,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 466.0, 481.333374, 40.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~",
									"numinlets" : 2,
									"id" : "obj-10",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 414.0, 512.333374, 71.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0 5.",
									"numinlets" : 2,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 515.0, 421.0, 32.5, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "1 5",
									"numinlets" : 2,
									"id" : "obj-14",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 554.0, 421.0, 32.5, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "p Generate_Gesture",
									"numinlets" : 1,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"color" : [ 0.976471, 0.129412, 0.956863, 1.0 ],
									"patching_rect" : [ 414.0, 396.0, 120.0, 20.0 ],
									"patcher" : 									{
										"fileversion" : 1,
										"rect" : [ 25.0, 69.0, 1412.0, 258.0 ],
										"bglocked" : 0,
										"defrect" : [ 25.0, 69.0, 1412.0, 258.0 ],
										"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"boxes" : [ 											{
												"box" : 												{
													"maxclass" : "outlet",
													"numinlets" : 1,
													"id" : "obj-5",
													"numoutlets" : 0,
													"patching_rect" : [ 113.0, 197.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "drive $1",
													"numinlets" : 2,
													"id" : "obj-3",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 39.0, 131.0, 53.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "loadmess 0.02",
													"numinlets" : 1,
													"id" : "obj-22",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 589.0, 79.0, 89.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "prepend timings",
													"numinlets" : 1,
													"id" : "obj-19",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 524.200012, 131.0, 96.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "8",
													"numinlets" : 2,
													"id" : "obj-18",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 524.0, 80.0, 32.5, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "timemap 0.05 0.8 3.",
													"numinlets" : 5,
													"id" : "obj-16",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 524.0, 104.5, 117.0, 20.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "initial_inflections 0.8",
													"numinlets" : 2,
													"id" : "obj-12",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 1216.199951, 133.0, 118.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "scaling_main amp 0. 1. -45 0., scaling_inflections amp 0. 1. -5. 5.",
													"numinlets" : 2,
													"id" : "obj-4",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 854.200012, 133.0, 356.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "reset",
													"numinlets" : 2,
													"id" : "obj-7",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 1340.199951, 133.0, 37.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main line 1. 0.2 2 0. 2",
													"numinlets" : 2,
													"id" : "obj-10",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 676.200012, 133.0, 172.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_inflections triangle 4-6 last 7-9 2-3 0 2 3-6 0 2 3-6",
													"numinlets" : 2,
													"id" : "obj-11",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 199.0, 133.0, 319.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "inlet",
													"numinlets" : 0,
													"id" : "obj-9",
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 39.0, 24.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "outlet",
													"numinlets" : 1,
													"id" : "obj-14",
													"numoutlets" : 0,
													"patching_rect" : [ 75.0, 197.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "gesture_maker",
													"numinlets" : 1,
													"id" : "obj-20",
													"fontname" : "Arial",
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 39.0, 166.0, 91.0, 20.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-7", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.0, 1349.699951, 63.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-4", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.0, 863.700012, 63.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-3", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-18", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.5, 533.5, 63.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-12", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.0, 1225.699951, 63.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-11", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.5, 208.5, 63.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-10", 0 ],
													"hidden" : 0,
													"midpoints" : [ 48.5, 63.0, 685.700012, 63.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-7", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 1349.699951, 160.5, 48.5, 160.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-4", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 863.700012, 160.5, 48.5, 160.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-3", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-22", 0 ],
													"destination" : [ "obj-16", 4 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-20", 0 ],
													"destination" : [ "obj-5", 0 ],
													"hidden" : 0,
													"color" : [ 1.0, 0.0, 0.0, 1.0 ],
													"midpoints" : [ 48.5, 191.0, 122.5, 191.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-20", 1 ],
													"destination" : [ "obj-14", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-19", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 533.700012, 160.5, 48.5, 160.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-18", 0 ],
													"destination" : [ "obj-16", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-16", 0 ],
													"destination" : [ "obj-19", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-12", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 1225.699951, 160.5, 48.5, 160.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-11", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 208.5, 160.5, 48.5, 160.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-10", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 685.700012, 160.5, 48.5, 160.5 ]
												}

											}
 ]
									}
,
									"saved_object_attributes" : 									{
										"default_fontname" : "Arial",
										"fontname" : "Arial",
										"default_fontsize" : 12.0,
										"globalpatchername" : "",
										"fontface" : 0,
										"fontsize" : 12.0,
										"default_fontface" : 0
									}

								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 554.0, 450.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "3120",
									"numinlets" : 2,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 462.5, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "2300",
									"numinlets" : 2,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 414.0, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "6000",
									"numinlets" : 2,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 510.5, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~",
									"numinlets" : 2,
									"id" : "obj-25",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 414.0, 481.333374, 32.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 414.0, 450.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "t f b",
									"numinlets" : 1,
									"id" : "obj-31",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 414.0, 364.0, 158.5, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- Click to realise gesture ->",
									"numinlets" : 1,
									"id" : "obj-32",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 228.0, 336.0, 161.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- see inside for how it's done ->",
									"numinlets" : 1,
									"id" : "obj-30",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 224.0, 397.0, 184.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Example - Volume gesture imposed on pink noise (as main help patch)",
									"numinlets" : 1,
									"id" : "obj-53",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 89.0, 270.0, 470.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~ 0.1",
									"numinlets" : 2,
									"id" : "obj-51",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 548.333374, 42.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pink~",
									"numinlets" : 1,
									"id" : "obj-45",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 120.0, 481.333374, 40.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~",
									"numinlets" : 2,
									"id" : "obj-44",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 512.333374, 71.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"varname" : "autohelp_dac_text",
									"text" : "start audio",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-64",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 11.595187,
									"patching_rect" : [ 27.0, 600.0, 38.0, 33.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "ezdac~",
									"varname" : "autohelp_dac",
									"numinlets" : 2,
									"id" : "obj-65",
									"numoutlets" : 0,
									"patching_rect" : [ 68.0, 594.0, 45.0, 45.0 ],
									"local" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "panel",
									"varname" : "startwinwdow_panel",
									"numinlets" : 1,
									"id" : "obj-43",
									"border" : 2,
									"numoutlets" : 0,
									"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
									"patching_rect" : [ 20.0, 589.0, 100.0, 55.0 ],
									"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0 5.",
									"numinlets" : 2,
									"id" : "obj-42",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 169.0, 421.0, 32.5, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "1 5",
									"numinlets" : 2,
									"id" : "obj-40",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 208.0, 421.0, 32.5, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "p Generate_Gesture",
									"numinlets" : 1,
									"id" : "obj-38",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"color" : [ 0.976471, 0.129412, 0.956863, 1.0 ],
									"patching_rect" : [ 68.0, 396.0, 120.0, 20.0 ],
									"patcher" : 									{
										"fileversion" : 1,
										"rect" : [ 25.0, 69.0, 797.0, 192.0 ],
										"bglocked" : 0,
										"defrect" : [ 25.0, 69.0, 797.0, 192.0 ],
										"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 0,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 0,
										"toolbarvisible" : 1,
										"boxanimatetime" : 200,
										"imprint" : 0,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"boxes" : [ 											{
												"box" : 												{
													"maxclass" : "outlet",
													"numinlets" : 1,
													"id" : "obj-5",
													"numoutlets" : 0,
													"patching_rect" : [ 104.0, 147.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "drive $1",
													"numinlets" : 2,
													"id" : "obj-3",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 30.0, 81.0, 53.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "scaling_main amp 0. 1. -45 0., scaling_inflections none",
													"numinlets" : 2,
													"id" : "obj-4",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 429.200012, 81.0, 302.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "reset",
													"numinlets" : 2,
													"id" : "obj-7",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 739.199951, 81.0, 37.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_main line 1. 0.2 2 0. 2",
													"numinlets" : 2,
													"id" : "obj-10",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 251.200012, 81.0, 172.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "message",
													"text" : "gesture_inflections flat 1.",
													"numinlets" : 2,
													"id" : "obj-11",
													"fontname" : "Arial",
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 97.0, 81.0, 143.0, 18.0 ]
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "inlet",
													"numinlets" : 0,
													"id" : "obj-9",
													"numoutlets" : 1,
													"outlettype" : [ "float" ],
													"patching_rect" : [ 30.0, 26.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "outlet",
													"numinlets" : 1,
													"id" : "obj-14",
													"numoutlets" : 0,
													"patching_rect" : [ 66.0, 147.0, 25.0, 25.0 ],
													"comment" : ""
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "newobj",
													"text" : "gesture_maker",
													"numinlets" : 1,
													"id" : "obj-20",
													"fontname" : "Arial",
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"fontsize" : 12.0,
													"patching_rect" : [ 30.0, 116.0, 91.0, 20.0 ]
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-7", 0 ],
													"hidden" : 0,
													"midpoints" : [ 39.5, 65.5, 748.699951, 65.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-4", 0 ],
													"hidden" : 0,
													"midpoints" : [ 39.5, 65.5, 438.700012, 65.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-3", 0 ],
													"hidden" : 0,
													"midpoints" : [ 39.5, 65.5, 39.5, 65.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-11", 0 ],
													"hidden" : 0,
													"midpoints" : [ 39.5, 65.5, 106.5, 65.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-9", 0 ],
													"destination" : [ "obj-10", 0 ],
													"hidden" : 0,
													"midpoints" : [ 39.5, 65.5, 260.700012, 65.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-7", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 748.699951, 110.5, 39.5, 110.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-4", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 438.700012, 110.5, 39.5, 110.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-3", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-20", 0 ],
													"destination" : [ "obj-5", 0 ],
													"hidden" : 0,
													"color" : [ 1.0, 0.0, 0.0, 1.0 ],
													"midpoints" : [ 39.5, 141.0, 113.5, 141.0 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-20", 1 ],
													"destination" : [ "obj-14", 0 ],
													"hidden" : 0,
													"midpoints" : [  ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-11", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 106.5, 110.5, 39.5, 110.5 ]
												}

											}
, 											{
												"patchline" : 												{
													"source" : [ "obj-10", 0 ],
													"destination" : [ "obj-20", 0 ],
													"hidden" : 0,
													"midpoints" : [ 260.700012, 110.5, 39.5, 110.5 ]
												}

											}
 ]
									}
,
									"saved_object_attributes" : 									{
										"default_fontname" : "Arial",
										"fontname" : "Arial",
										"default_fontsize" : 12.0,
										"globalpatchername" : "",
										"fontface" : 0,
										"fontsize" : 12.0,
										"default_fontface" : 0
									}

								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-29",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 208.0, 450.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "3120",
									"numinlets" : 2,
									"id" : "obj-28",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 116.5, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "2300",
									"numinlets" : 2,
									"id" : "obj-27",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "6000",
									"numinlets" : 2,
									"id" : "obj-24",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 164.5, 336.0, 37.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~",
									"numinlets" : 2,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 481.333374, 32.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-21",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 450.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "t f b",
									"numinlets" : 1,
									"id" : "obj-1",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 68.0, 364.0, 158.5, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Gestural shapes are made up of kernels. Each kernel is a shape consisting of between 1 and 3 lines or curves - allowing various basic shapes to be created. See the \"shape\" subpatch to see how to specify kernel shapes.",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 164.0, 628.0, 34.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The two levels are combined through multiplication of the final scaled values.",
									"numinlets" : 1,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 104.333328, 418.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Each gesture only has one main kernel, but can have multiple inflection kernel shapes. The gesture is divided into several sub-gestures. Each one will take the next inflection shape from a stored list. If there are less inflection kernel shapes than divisions then the stored list is traversed as a loop, with the kernel shapes being repeated as necessary.",
									"linecount" : 3,
									"numinlets" : 1,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 200.0, 628.0, 48.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Kernels",
									"numinlets" : 1,
									"id" : "obj-11",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 139.0, 61.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "2. - The inflections layer (the shape(s) of inflections within the main shape).",
									"numinlets" : 1,
									"id" : "obj-9",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 47.0, 82.333328, 408.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "1. - The main layer (the main shape of the gesture)",
									"numinlets" : 1,
									"id" : "obj-7",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 47.0, 60.333336, 280.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Each gesture has two layers:",
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 38.0, 165.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Layers",
									"numinlets" : 1,
									"id" : "obj-2",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 16.0, 13.0, 56.0, 23.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-8", 0 ],
									"destination" : [ "obj-10", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-51", 0 ],
									"destination" : [ "obj-65", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-51", 0 ],
									"destination" : [ "obj-65", 1 ],
									"hidden" : 0,
									"midpoints" : [ 77.5, 580.666687, 103.5, 580.666687 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-45", 0 ],
									"destination" : [ "obj-44", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-44", 0 ],
									"destination" : [ "obj-51", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 0 ],
									"destination" : [ "obj-29", 0 ],
									"hidden" : 0,
									"midpoints" : [ 178.5, 444.0, 217.5, 444.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-40", 0 ],
									"destination" : [ "obj-29", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-38", 1 ],
									"destination" : [ "obj-42", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-38", 0 ],
									"destination" : [ "obj-21", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-31", 0 ],
									"destination" : [ "obj-16", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-31", 1 ],
									"destination" : [ "obj-14", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-29", 0 ],
									"destination" : [ "obj-22", 1 ],
									"hidden" : 0,
									"midpoints" : [ 217.5, 476.5, 90.5, 476.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-28", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 126.0, 358.5, 77.5, 358.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-27", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 0 ],
									"destination" : [ "obj-25", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-25", 0 ],
									"destination" : [ "obj-10", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-24", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 174.0, 358.5, 77.5, 358.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-23", 0 ],
									"destination" : [ "obj-31", 0 ],
									"hidden" : 0,
									"midpoints" : [ 520.0, 358.5, 423.5, 358.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-22", 0 ],
									"destination" : [ "obj-44", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-21", 0 ],
									"destination" : [ "obj-22", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 0 ],
									"destination" : [ "obj-31", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-19", 0 ],
									"destination" : [ "obj-31", 0 ],
									"hidden" : 0,
									"midpoints" : [ 472.0, 358.5, 423.5, 358.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-25", 1 ],
									"hidden" : 0,
									"midpoints" : [ 563.5, 476.5, 436.5, 476.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-16", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-16", 1 ],
									"destination" : [ "obj-12", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-14", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-12", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [ 524.5, 444.0, 563.5, 444.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-10", 0 ],
									"destination" : [ "obj-51", 0 ],
									"hidden" : 0,
									"midpoints" : [ 423.5, 539.0, 77.5, 539.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 1 ],
									"destination" : [ "obj-40", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-38", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p events_mode",
					"numinlets" : 0,
					"id" : "obj-6",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.113725, 0.756863, 0.027451, 1.0 ],
					"patching_rect" : [ 630.0, 517.833313, 93.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 50.0, 94.0, 1063.0, 470.0 ],
						"bglocked" : 0,
						"defrect" : [ 50.0, 94.0, 1063.0, 470.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "This line object just fades in / out",
									"numinlets" : 1,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 437.0, 336.399994, 185.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- Trigger Events",
									"numinlets" : 1,
									"frgb" : [ 1.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 378.0, 47.0, 102.0, 20.0 ],
									"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "^ Gesture Setup ^",
									"numinlets" : 1,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 449.0, 201.0, 106.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "flonum",
									"numinlets" : 1,
									"id" : "obj-54",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "float", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 131.5, 249.399994, 58.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "-  Filter noise in a rhythmic fashion",
									"numinlets" : 1,
									"id" : "obj-52",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 231.0, 18.0, 193.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0. 300.",
									"numinlets" : 2,
									"id" : "obj-51",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 393.0, 305.399994, 47.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0.1 1.",
									"numinlets" : 2,
									"id" : "obj-48",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 913.5, 305.399994, 41.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-46",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 392.0, 336.399994, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 8.",
									"numinlets" : 1,
									"id" : "obj-45",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 303.0, 277.399994, 75.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "biquad~",
									"numinlets" : 6,
									"id" : "obj-44",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 336.399994, 263.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "filtercoeff~ resonant",
									"numinlets" : 3,
									"id" : "obj-42",
									"fontname" : "Arial",
									"numoutlets" : 5,
									"outlettype" : [ "signal", "signal", "signal", "signal", "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 109.0, 305.399994, 213.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "1 2 1 2 1 2 3 2 3 1 2 1",
									"numinlets" : 2,
									"id" : "obj-41",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 24.0, 49.0, 127.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "noise~",
									"numinlets" : 1,
									"id" : "obj-40",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 305.399994, 46.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadmess 55",
									"numinlets" : 1,
									"id" : "obj-37",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 180.0, 109.399994, 79.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "prepend events",
									"numinlets" : 1,
									"id" : "obj-32",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 24.0, 165.399994, 93.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "vexpr $f1 * $f2 @scalarmode 1",
									"numinlets" : 2,
									"id" : "obj-30",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 24.0, 137.399994, 175.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "1 1 1 1 2 3 4 1 1 3 2 1 6 1 1 4 1 1 1",
									"numinlets" : 2,
									"id" : "obj-22",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 167.0, 49.0, 197.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The gesture length is equal to the sum of all the timing values.",
									"numinlets" : 1,
									"id" : "obj-31",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 493.0, 40.5, 340.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The gesture is evaluated at time zero and then after each of the specified delays.",
									"numinlets" : 1,
									"id" : "obj-29",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 493.0, 106.5, 440.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "You should supply a list of times between in events in milliseconds.",
									"numinlets" : 1,
									"id" : "obj-27",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 493.0, 84.5, 366.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Frequency in Hz",
									"numinlets" : 1,
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 197.5, 249.399994, 98.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "initial_inflections 0.5",
									"numinlets" : 2,
									"id" : "obj-11",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 606.5, 165.399994, 118.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "unpack 0.",
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "float" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 221.399994, 63.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pack 0. 1.",
									"numinlets" : 2,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 249.399994, 63.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Use the events message to execute the gesture as a set of events. ",
									"numinlets" : 1,
									"id" : "obj-4",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 493.0, 62.5, 369.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "In events mode the gesture is evaluted at specific points in time as specified by a list of timing values. ",
									"numinlets" : 1,
									"id" : "obj-49",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 493.0, 18.0, 551.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Example - Frequency Gesture",
									"numinlets" : 1,
									"id" : "obj-35",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 28.0, 16.0, 204.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "t l b",
									"numinlets" : 1,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 24.0, 81.0, 259.5, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"varname" : "autohelp_dac_text",
									"text" : "start audio",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-64",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 11.595187,
									"patching_rect" : [ 19.0, 404.0, 38.0, 33.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "ezdac~",
									"varname" : "autohelp_dac",
									"numinlets" : 2,
									"id" : "obj-65",
									"numoutlets" : 0,
									"patching_rect" : [ 60.0, 398.0, 45.0, 45.0 ],
									"local" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "panel",
									"varname" : "startwinwdow_panel",
									"numinlets" : 1,
									"id" : "obj-21",
									"border" : 2,
									"numoutlets" : 0,
									"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
									"patching_rect" : [ 12.0, 393.0, 100.0, 55.0 ],
									"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~ 0.1",
									"numinlets" : 2,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 366.399994, 42.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 277.399994, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "gesture_maker",
									"numinlets" : 1,
									"id" : "obj-1",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 24.0, 193.399994, 91.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_main triangle 4 0 8 4",
									"numinlets" : 2,
									"id" : "obj-5",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 135.0, 165.399994, 166.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "gesture_inflections flat 1.",
									"numinlets" : 2,
									"id" : "obj-6",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 460.5, 165.399994, 143.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main log 200 6900",
									"numinlets" : 2,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 304.5, 165.399994, 153.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections none",
									"numinlets" : 2,
									"id" : "obj-9",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 727.5, 165.399994, 138.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset",
									"numinlets" : 2,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 868.5, 165.399994, 37.0, 18.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-9", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 737.0, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-8", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 314.0, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-6", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 470.0, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-51", 0 ],
									"destination" : [ "obj-46", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-5", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 144.5, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-48", 0 ],
									"destination" : [ "obj-46", 0 ],
									"hidden" : 0,
									"midpoints" : [ 923.0, 330.0, 401.5, 330.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-46", 0 ],
									"destination" : [ "obj-18", 1 ],
									"hidden" : 0,
									"midpoints" : [ 401.5, 360.899994, 92.5, 360.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-45", 0 ],
									"destination" : [ "obj-42", 2 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-44", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 0 ],
									"destination" : [ "obj-44", 1 ],
									"hidden" : 0,
									"midpoints" : [ 118.5, 326.600006, 118.300003, 326.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 1 ],
									"destination" : [ "obj-44", 2 ],
									"hidden" : 0,
									"midpoints" : [ 167.0, 326.600006, 167.100006, 326.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 2 ],
									"destination" : [ "obj-44", 3 ],
									"hidden" : 0,
									"midpoints" : [ 215.5, 326.600006, 215.899994, 326.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 3 ],
									"destination" : [ "obj-44", 4 ],
									"hidden" : 0,
									"midpoints" : [ 264.0, 326.600006, 264.700012, 326.600006 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-42", 4 ],
									"destination" : [ "obj-44", 5 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-41", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-40", 0 ],
									"destination" : [ "obj-44", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-37", 0 ],
									"destination" : [ "obj-30", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-32", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-30", 0 ],
									"destination" : [ "obj-32", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-54", 0 ],
									"hidden" : 0,
									"midpoints" : [ 69.5, 244.899994, 141.0, 244.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-2", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-9", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 737.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-8", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 314.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-6", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 470.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-5", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 144.5, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-48", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 923.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 0 ],
									"destination" : [ "obj-30", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 878.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-11", 0 ],
									"hidden" : 0,
									"midpoints" : [ 274.0, 159.199997, 616.0, 159.199997 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-22", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [ 176.5, 74.0, 33.5, 74.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 878.0, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-2", 0 ],
									"destination" : [ "obj-15", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-65", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-15", 0 ],
									"destination" : [ "obj-42", 0 ],
									"hidden" : 0,
									"midpoints" : [ 69.5, 300.899994, 118.5, 300.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-11", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 616.0, 187.899994, 33.5, 187.899994 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-51", 0 ],
									"hidden" : 0,
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"midpoints" : [ 33.5, 216.0, 402.5, 216.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 1 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p scaling",
					"numinlets" : 0,
					"id" : "obj-5",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 0.709804, 1.0, 0.047059, 1.0 ],
					"patching_rect" : [ 630.0, 453.833344, 58.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 60.0, 137.0, 863.0, 365.0 ],
						"bglocked" : 0,
						"defrect" : [ 60.0, 137.0, 863.0, 365.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "This allows direct control over musical parameters without the need for external scaling, and also allows appropriate combinations of the two layers.",
									"numinlets" : 1,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 42.0, 794.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "See the other subpatche to see examples of different scalings in practice.",
									"numinlets" : 1,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 326.0, 400.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Scaling Modes",
									"numinlets" : 1,
									"id" : "obj-14",
									"fontname" : "Arial Black",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 33.0, 139.0, 107.0, 23.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The arguments to the message are mode / min output value / max output value",
									"numinlets" : 1,
									"id" : "obj-12",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 111.0, 430.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Use the \"scaling_inflections\" message to set the scaling of the inflection kernel.",
									"numinlets" : 1,
									"id" : "obj-11",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 88.0, 430.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Use the \"scaling_main\" message to set the scaling of the main kernel.",
									"numinlets" : 1,
									"id" : "obj-10",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 65.0, 381.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The output of each layer can be scaled to any range using one of several useful musical scalings (similar to those available for the valconvert object).",
									"numinlets" : 1,
									"id" : "obj-8",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 18.0, 19.0, 807.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "scale from linear  (0-1) input to specified range in decibels (linear scaling) then output as linear amplitude",
									"numinlets" : 1,
									"id" : "obj-50",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 291.0, 567.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "scale from linear  (0-1)  input to range specified in semitones (linear scaling) then convert to  a speed / freq multiplier",
									"numinlets" : 1,
									"id" : "obj-48",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 267.0, 626.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "scale from exponetial (0-1)  input to linear output (logarithmic scaling)",
									"numinlets" : 1,
									"id" : "obj-47",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 243.0, 378.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "scale from logarithmic (0-1) input to linear output (exponential scaling)",
									"numinlets" : 1,
									"id" : "obj-46",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 219.0, 381.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "linear scaling",
									"numinlets" : 1,
									"id" : "obj-45",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 195.0, 81.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "no scaling (kernel is left in range 0-1)",
									"numinlets" : 1,
									"id" : "obj-44",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 218.0, 171.0, 207.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections amp -50. 10.",
									"numinlets" : 2,
									"id" : "obj-41",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 291.0, 179.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main pitch -12. 12.",
									"numinlets" : 2,
									"id" : "obj-39",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 267.0, 153.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections scale 1 100.",
									"numinlets" : 2,
									"id" : "obj-37",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 195.0, 176.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_inflections exp 1. 100.",
									"numinlets" : 2,
									"id" : "obj-25",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 243.0, 171.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main log 1 100.",
									"numinlets" : 2,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 219.0, 137.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "scaling_main none",
									"numinlets" : 2,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 30.0, 171.0, 111.0, 18.0 ]
								}

							}
 ],
						"lines" : [  ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"numinlets" : 1,
					"id" : "obj-4",
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 178.0, 291.0, 20.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p chaining",
					"numinlets" : 0,
					"id" : "obj-2",
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0,
					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
					"patching_rect" : [ 630.0, 549.833313, 65.0, 20.0 ],
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 256.0, 166.0, 1061.0, 588.0 ],
						"bglocked" : 0,
						"defrect" : [ 256.0, 166.0, 1061.0, 588.0 ],
						"openrect" : [ 0.0, 0.0, 0.0, 0.0 ],
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 0,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 0,
						"toolbarvisible" : 1,
						"boxanimatetime" : 200,
						"imprint" : 0,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"boxes" : [ 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0. 5.",
									"numinlets" : 2,
									"id" : "obj-34",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 347.0, 383.0, 34.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset, gesture_main plateau 2 9 0 8 3 0 3-5 2 0 2 3, scaling_main log 200 2000, gesture_inflections triangle_return 3-8 last 4-9, scaling_inflections pitch -6 6, initial_inflections 0.5, timings 0.25 0.6 0.87",
									"linecount" : 2,
									"numinlets" : 2,
									"id" : "obj-13",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 51.0, 188.0, 540.0, 32.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The format for the external drive in/out is list of two values: normalised time (0-1) / grain duration.",
									"numinlets" : 1,
									"id" : "obj-3",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 19.0, 53.0, 528.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "External drive messages are passed through to the drive output, so you can continue to chain objects one after another.",
									"numinlets" : 1,
									"id" : "obj-4",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 19.0, 32.0, 646.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Objects can be chained together to control multiple parameters in a synchronous manner, with one object acting as a master. ",
									"numinlets" : 1,
									"id" : "obj-49",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 19.0, 11.0, 676.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- Setup amplitude gesture",
									"numinlets" : 1,
									"id" : "obj-46",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 863.0, 258.0, 155.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadbang",
									"numinlets" : 1,
									"id" : "obj-47",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 261.0, 232.0, 60.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The chaining output is connected to the external drive input to synchronise the two gestures.",
									"numinlets" : 1,
									"frgb" : [ 1.0, 0.0, 0.0, 1.0 ],
									"id" : "obj-45",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 282.0, 314.0, 500.0, 20.0 ],
									"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Execute gesture",
									"numinlets" : 1,
									"id" : "obj-43",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 60.0, 84.0, 101.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<- Fade in and out to zero at either end",
									"numinlets" : 1,
									"id" : "obj-41",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 469.0, 381.0, 218.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "<-Setup frequency gesture",
									"numinlets" : 1,
									"id" : "obj-39",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 603.0, 189.0, 152.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "loadbang",
									"numinlets" : 1,
									"id" : "obj-37",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 51.0, 162.0, 60.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Amplitude Gesture",
									"numinlets" : 1,
									"id" : "obj-36",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 362.0, 353.0, 113.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Frequency Gesture",
									"numinlets" : 1,
									"id" : "obj-35",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0,
									"patching_rect" : [ 133.0, 231.0, 113.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "reset, gesture_main triangle_return 4-5 0 10 0 2 7, scaling_main amp -50 0., gesture_inflections triangle 3-8 last 8-9 0, initial_inflections 0.01, scaling_inflections amp -20 0., timings 0.1 0.3 0.4 0.45 0.49 0.51 0.76 0.8 0.82 0.85 0.9, graintime 10.",
									"linecount" : 3,
									"numinlets" : 2,
									"id" : "obj-27",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 261.0, 258.0, 585.0, 46.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "t b b",
									"numinlets" : 1,
									"id" : "obj-26",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "bang", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 34.0, 110.0, 1008.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "button",
									"numinlets" : 1,
									"id" : "obj-25",
									"numoutlets" : 1,
									"outlettype" : [ "bang" ],
									"patching_rect" : [ 34.0, 84.0, 20.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "0. 0. 1. 6.",
									"numinlets" : 2,
									"id" : "obj-23",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 395.0, 383.0, 61.0, 18.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"varname" : "autohelp_dac_text",
									"text" : "start audio",
									"linecount" : 2,
									"numinlets" : 1,
									"id" : "obj-64",
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 11.595187,
									"patching_rect" : [ 29.0, 527.0, 38.0, 33.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "ezdac~",
									"varname" : "autohelp_dac",
									"numinlets" : 2,
									"id" : "obj-65",
									"numoutlets" : 0,
									"patching_rect" : [ 70.0, 521.0, 45.0, 45.0 ],
									"local" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "panel",
									"varname" : "startwinwdow_panel",
									"numinlets" : 1,
									"id" : "obj-21",
									"border" : 2,
									"numoutlets" : 0,
									"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
									"patching_rect" : [ 22.0, 516.0, 100.0, 55.0 ],
									"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-20",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 347.0, 411.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "saw~",
									"numinlets" : 2,
									"id" : "obj-19",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 70.0, 423.0, 39.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~ 0.1",
									"numinlets" : 2,
									"id" : "obj-18",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 70.0, 471.0, 42.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "*~",
									"numinlets" : 2,
									"id" : "obj-17",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 297.0, 441.0, 68.5, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-16",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 297.0, 411.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "line~",
									"numinlets" : 2,
									"id" : "obj-15",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "signal", "bang" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 70.0, 393.0, 36.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "gesture_maker",
									"numinlets" : 2,
									"id" : "obj-2",
									"fontname" : "Arial",
									"numoutlets" : 2,
									"outlettype" : [ "", "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 261.0, 353.0, 91.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "gesture_maker",
									"numinlets" : 1,
									"id" : "obj-1",
									"fontname" : "Arial",
									"numoutlets" : 3,
									"outlettype" : [ "", "", "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 34.0, 232.0, 91.0, 20.0 ]
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "message",
									"text" : "drive 9000",
									"numinlets" : 2,
									"id" : "obj-14",
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 34.0, 136.0, 67.0, 18.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-47", 0 ],
									"destination" : [ "obj-27", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-37", 0 ],
									"destination" : [ "obj-13", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-34", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-27", 0 ],
									"destination" : [ "obj-2", 0 ],
									"hidden" : 0,
									"midpoints" : [ 270.5, 321.0, 270.5, 321.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 1 ],
									"destination" : [ "obj-23", 0 ],
									"hidden" : 0,
									"midpoints" : [ 1032.5, 377.5, 404.5, 377.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-26", 0 ],
									"destination" : [ "obj-14", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-25", 0 ],
									"destination" : [ "obj-26", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-23", 0 ],
									"destination" : [ "obj-20", 0 ],
									"hidden" : 0,
									"midpoints" : [ 404.5, 405.5, 356.5, 405.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-20", 0 ],
									"destination" : [ "obj-17", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-2", 1 ],
									"destination" : [ "obj-16", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-19", 0 ],
									"destination" : [ "obj-18", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-65", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-18", 0 ],
									"destination" : [ "obj-65", 1 ],
									"hidden" : 0,
									"midpoints" : [ 79.5, 502.0, 105.5, 502.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-17", 0 ],
									"destination" : [ "obj-18", 1 ],
									"hidden" : 0,
									"midpoints" : [ 306.5, 465.5, 102.5, 465.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-16", 0 ],
									"destination" : [ "obj-17", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-15", 0 ],
									"destination" : [ "obj-19", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-14", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-13", 0 ],
									"destination" : [ "obj-1", 0 ],
									"hidden" : 0,
									"midpoints" : [ 60.5, 225.5, 43.5, 225.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-34", 0 ],
									"hidden" : 0,
									"color" : [ 0.0, 1.0, 0.094118, 1.0 ],
									"midpoints" : [ 43.5, 378.0, 356.5, 378.0 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 2 ],
									"destination" : [ "obj-2", 1 ],
									"hidden" : 0,
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"midpoints" : [ 115.5, 342.5, 342.5, 342.5 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 1 ],
									"destination" : [ "obj-15", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "gesture_maker",
					"numinlets" : 0,
					"id" : "obj-1",
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 178.0, 254.0, 275.0, 20.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "autohelp_top_panel",
					"numinlets" : 1,
					"id" : "obj-36",
					"mode" : 1,
					"numoutlets" : 0,
					"grad1" : [ 0.27, 0.35, 0.47, 1.0 ],
					"background" : 1,
					"patching_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"grad2" : [ 0.85, 0.85, 0.85, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "autohelp_see_panel",
					"numinlets" : 1,
					"id" : "obj-81",
					"border" : 2,
					"numoutlets" : 0,
					"bgcolor" : [ 0.85, 0.85, 0.85, 0.75 ],
					"background" : 1,
					"patching_rect" : [ 787.0, 646.0, 140.0, 50.0 ],
					"bordercolor" : [ 0.5, 0.5, 0.5, 0.75 ]
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-80", 1 ],
					"destination" : [ "obj-18", 0 ],
					"hidden" : 1,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 1 ],
					"destination" : [ "obj-40", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-38", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-51", 0 ],
					"destination" : [ "obj-65", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-51", 0 ],
					"destination" : [ "obj-65", 1 ],
					"hidden" : 0,
					"midpoints" : [ 96.5, 629.666687, 122.5, 629.666687 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-45", 0 ],
					"destination" : [ "obj-44", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-44", 0 ],
					"destination" : [ "obj-51", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-42", 0 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"midpoints" : [ 258.5, 498.0, 297.5, 498.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-40", 0 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-38", 1 ],
					"destination" : [ "obj-42", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-38", 0 ],
					"destination" : [ "obj-21", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-29", 0 ],
					"destination" : [ "obj-22", 1 ],
					"hidden" : 0,
					"midpoints" : [ 297.5, 530.5, 109.5, 530.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-28", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"midpoints" : [ 145.0, 412.5, 96.5, 412.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-27", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-24", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"midpoints" : [ 193.0, 412.5, 96.5, 412.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-22", 0 ],
					"destination" : [ "obj-44", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-21", 0 ],
					"destination" : [ "obj-22", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
