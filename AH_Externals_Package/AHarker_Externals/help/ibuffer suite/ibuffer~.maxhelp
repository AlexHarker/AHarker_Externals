{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 61.0, 85.0, 867.0, 532.0 ],
		"bglocked" : 0,
		"defrect" : [ 61.0, 85.0, 867.0, 532.0 ],
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
					"maxclass" : "newobj",
					"text" : "p see_also",
					"hidden" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 771.0, 431.0, 69.0, 20.0 ],
					"id" : "obj-33",
					"numinlets" : 1,
					"numoutlets" : 0,
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
									"fontname" : "Arial",
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 99.0, 63.5, 83.0, 20.0 ],
									"id" : "obj-2",
									"numinlets" : 1,
									"numoutlets" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "pcontrol",
									"fontname" : "Arial",
									"outlettype" : [ "" ],
									"fontsize" : 12.0,
									"patching_rect" : [ 99.0, 90.5, 56.0, 20.0 ],
									"id" : "obj-3",
									"numinlets" : 1,
									"numoutlets" : 1
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 36.0, 25.0, 25.0 ],
									"id" : "obj-4",
									"numinlets" : 0,
									"numoutlets" : 1,
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
						"fontname" : "Arial",
						"default_fontname" : "Arial",
						"default_fontsize" : 12.0,
						"fontface" : 0,
						"fontsize" : 12.0,
						"default_fontface" : 0,
						"globalpatchername" : ""
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "play the ibuffer~s (after you have loaded them)",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 589.0, 238.0, 259.0, 20.0 ],
					"id" : "obj-24",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "play ibuf4buf",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 668.0, 347.0, 79.0, 18.0 ],
					"id" : "obj-22",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "play ibuf3buf",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 654.0, 321.0, 79.0, 18.0 ],
					"id" : "obj-23",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "play ibuf2buf",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 620.0, 295.0, 79.0, 18.0 ],
					"id" : "obj-25",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "play ibuf1buf",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 590.0, 269.0, 79.0, 18.0 ],
					"id" : "obj-27",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibufplayer~",
					"fontname" : "Arial",
					"outlettype" : [ "signal", "signal", "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 590.0, 388.0, 70.0, 20.0 ],
					"id" : "obj-28",
					"numinlets" : 1,
					"numoutlets" : 3
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "ezdac~",
					"varname" : "autohelp_dac",
					"patching_rect" : [ 590.0, 464.0, 45.0, 45.0 ],
					"id" : "obj-29",
					"numinlets" : 2,
					"local" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_dac_text",
					"text" : "start audio",
					"linecount" : 2,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"patching_rect" : [ 549.0, 470.0, 38.0, 33.0 ],
					"id" : "obj-30",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "startwinwdow_panel",
					"border" : 2,
					"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ],
					"patching_rect" : [ 542.0, 459.0, 100.0, 55.0 ],
					"id" : "obj-31",
					"numinlets" : 1,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~ 0.2",
					"fontname" : "Arial",
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 590.0, 415.666656, 42.0, 20.0 ],
					"id" : "obj-32",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibuffer~ ibuf2buf cello-f2.aif",
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 225.454544, 155.0, 20.0 ],
					"id" : "obj-7",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "autohelp",
					"hidden" : 1,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"patching_rect" : [ 710.0, 464.0, 55.0, 20.0 ],
					"id" : "obj-17",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_title",
					"text" : "ibuffer~",
					"fontname" : "Arial",
					"frgb" : [ 0.2, 0.2, 0.2, 1.0 ],
					"fontface" : 3,
					"fontsize" : 20.871338,
					"patching_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"id" : "obj-18",
					"numinlets" : 1,
					"textcolor" : [ 0.2, 0.2, 0.2, 1.0 ],
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_digest",
					"text" : "Load an audio file into memory in the same sample format as on disk",
					"fontname" : "Arial",
					"frgb" : [ 0.2, 0.2, 0.2, 1.0 ],
					"fontsize" : 12.754705,
					"patching_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"id" : "obj-20",
					"numinlets" : 1,
					"textcolor" : [ 0.2, 0.2, 0.2, 1.0 ],
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_top_description",
					"text" : "ibuffer~ is an audio file buffer that loads audio files in their stored integer format (or float format if relevant). This object is designed primarily for applications with large memory requirements where loading in 16 bit or 24 bit formats saves vital space in memory. It supports 16, 24 and 32 integer formats, as well as 32 bit float and can load either all channels or specified channels (up to 4 in either case). There are a corresponding set of playback and other objects that will also function with standard MSP buffers. Sound file loading is done using libsndfile by Erik de Castro Lopo (http://www.mega-nerd.com/libsndfile/).",
					"linecount" : 4,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"patching_rect" : [ 10.0, 57.0, 830.0, 60.0 ],
					"id" : "obj-21",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "textbutton",
					"varname" : "autohelp_top_ref",
					"hidden" : 1,
					"fontname" : "Arial",
					"outlettype" : [ "", "", "int" ],
					"bgoveroncolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"presentation_rect" : [ 90.0, 90.0, 149.883652, 14.666666 ],
					"text" : "open ibuffer~ reference",
					"spacing_x" : 0.0,
					"bgoncolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"fontface" : 3,
					"fontsize" : 12.754705,
					"bordercolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"spacing_y" : 0.0,
					"textoncolor" : [ 0.27, 0.35, 0.47, 1.0 ],
					"patching_rect" : [ 350.116333, 22.0, 149.883652, 14.666666 ],
					"id" : "obj-26",
					"numinlets" : 1,
					"fontlink" : 1,
					"underline" : 1,
					"bgcolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"textovercolor" : [ 0.4, 0.5, 0.65, 1.0 ],
					"textcolor" : [ 0.361, 0.455, 0.596, 1.0 ],
					"numoutlets" : 3,
					"bgovercolor" : [ 0.0, 0.0, 0.0, 0.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_see_title",
					"text" : "See Also:",
					"fontname" : "Arial",
					"fontface" : 1,
					"fontsize" : 11.595187,
					"patching_rect" : [ 715.0, 468.0, 100.0, 20.0 ],
					"id" : "obj-79",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "umenu",
					"varname" : "autohelp_see_menu",
					"fontname" : "Arial",
					"outlettype" : [ "int", "", "" ],
					"items" : [ "(Objects:)", ",", "ibufconcatedrive~", ",", "ibufconcatenate~", ",", "ibuffermulti~", ",", "ibufinfo~", ",", "ibufplayer~", ",", "ibufmultitable~", ",", "ibuftable~" ],
					"fontsize" : 11.595187,
					"types" : [  ],
					"patching_rect" : [ 715.0, 488.0, 130.0, 20.0 ],
					"id" : "obj-80",
					"numinlets" : 1,
					"numoutlets" : 3
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "load only the left channel of a stereo file.",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 159.0, 462.454559, 225.0, 20.0 ],
					"id" : "obj-19",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "if you follow a file name or path with a list of channels, only the specified channels will be loaded into the ibuffer~ in the specified order. You may load a channel more than once.",
					"linecount" : 2,
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 417.454559, 484.0, 34.0 ],
					"id" : "obj-1",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibuffer~ ibuf4buf",
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 494.454559, 97.0, 20.0 ],
					"id" : "obj-2",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "replace cello-f2.aif 1",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 462.454559, 118.0, 18.0 ],
					"id" : "obj-5",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "replace and open are synonms that replace the contents of the ibuffer~ with that of the named file.",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 289.454559, 530.0, 20.0 ],
					"id" : "obj-13",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibuffer~ ibuf3buf",
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 353.454559, 97.0, 20.0 ],
					"id" : "obj-14",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "replace sho0630.aif",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 321.454559, 115.0, 18.0 ],
					"id" : "obj-15",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "open vibes-a1.aif",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 158.0, 319.0, 102.0, 18.0 ],
					"id" : "obj-16",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "unlike buffer~ only one ibuffer~ object can use a specific name",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 257.454529, 342.0, 20.0 ],
					"id" : "obj-12",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "name and set are synonms that change the name used to refer to the ibuffer~.",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 161.454544, 426.0, 20.0 ],
					"id" : "obj-11",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "bang on load",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 62.0, 379.0, 80.0, 20.0 ],
					"id" : "obj-8",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "optional arguments set name and intial file to load",
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"patching_rect" : [ 201.0, 129.0, 275.0, 20.0 ],
					"id" : "obj-6",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "ibuffer~ ibuf1buf jongly.aif",
					"fontname" : "Arial",
					"outlettype" : [ "bang" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 129.0, 146.0, 20.0 ],
					"id" : "obj-4",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "button",
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 29.0, 385.454559, 20.0, 20.0 ],
					"id" : "obj-3",
					"numinlets" : 1,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "name newname",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 29.0, 193.454544, 96.0, 18.0 ],
					"id" : "obj-9",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "message",
					"text" : "set samename",
					"fontname" : "Arial",
					"outlettype" : [ "" ],
					"fontsize" : 12.0,
					"patching_rect" : [ 138.0, 195.0, 89.0, 18.0 ],
					"id" : "obj-10",
					"numinlets" : 2,
					"numoutlets" : 1
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "autohelp_top_panel",
					"grad1" : [ 0.88, 0.98, 0.78, 1.0 ],
					"mode" : 1,
					"grad2" : [ 0.9, 0.9, 0.9, 1.0 ],
					"background" : 1,
					"patching_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"id" : "obj-36",
					"numinlets" : 1,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "autohelp_see_panel",
					"border" : 2,
					"bordercolor" : [ 0.5, 0.5, 0.5, 0.75 ],
					"background" : 1,
					"patching_rect" : [ 710.0, 464.0, 140.0, 50.0 ],
					"id" : "obj-81",
					"numinlets" : 1,
					"bgcolor" : [ 0.85, 0.85, 0.85, 0.75 ],
					"numoutlets" : 0
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-9", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-80", 1 ],
					"destination" : [ "obj-33", 0 ],
					"hidden" : 1,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-2", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-32", 0 ],
					"destination" : [ "obj-29", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-32", 0 ],
					"destination" : [ "obj-29", 1 ],
					"hidden" : 0,
					"midpoints" : [ 599.5, 447.5, 625.5, 447.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-28", 0 ],
					"destination" : [ "obj-32", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-27", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-25", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-23", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-22", 0 ],
					"destination" : [ "obj-28", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-16", 0 ],
					"destination" : [ "obj-14", 0 ],
					"hidden" : 0,
					"midpoints" : [ 167.5, 342.5, 38.5, 342.5 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-15", 0 ],
					"destination" : [ "obj-14", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-14", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"midpoints" : [ 147.5, 217.0, 38.5, 217.0 ]
				}

			}
 ]
	}

}
