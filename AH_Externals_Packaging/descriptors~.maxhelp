{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 3,
			"revision" : 1,
			"architecture" : "x64",
			"modernui" : 1
		}
,
		"classnamespace" : "box",
		"rect" : [ 34.0, 87.0, 869.0, 662.0 ],
		"bglocked" : 0,
		"openinpresentation" : 0,
		"default_fontsize" : 12.0,
		"default_fontface" : 0,
		"default_fontname" : "Arial",
		"gridonopen" : 1,
		"gridsize" : [ 15.0, 15.0 ],
		"gridsnaponopen" : 1,
		"objectsnaponopen" : 1,
		"statusbarvisible" : 2,
		"toolbarvisible" : 1,
		"lefttoolbarpinned" : 0,
		"toptoolbarpinned" : 0,
		"righttoolbarpinned" : 0,
		"bottomtoolbarpinned" : 0,
		"toolbars_unpinned_last_save" : 0,
		"tallnewobj" : 0,
		"boxanimatetime" : 200,
		"enablehscroll" : 1,
		"enablevscroll" : 1,
		"devicewidth" : 0.0,
		"description" : "",
		"digest" : "",
		"tags" : "",
		"style" : "",
		"subpatcher_template" : "",
		"assistshowspatchername" : 0,
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-19",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 3,
							"revision" : 1,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 50.0, 94.0, 264.0, 148.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 63.5, 83.0, 20.0 ],
									"text" : "prepend help",
									"varname" : "phelp"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-3",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 90.5, 56.0, 20.0 ],
									"text" : "pcontrol"
								}

							}
, 							{
								"box" : 								{
									"comment" : "From umenu label out",
									"id" : "obj-4",
									"index" : 1,
									"maxclass" : "inlet",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 99.0, 36.0, 25.0, 25.0 ]
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-2", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 768.0, 560.0, 69.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p see_also"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-10",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 489.0, 594.0, 135.0, 20.0 ],
					"text" : "<---- Load a buffer here",
					"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-12",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 430.0, 594.0, 50.0, 22.0 ],
					"text" : "replace"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-18",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 133.0, 150.0, 203.0, 20.0 ],
					"text" : "set the energy threshold off (default)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-11",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 152.0, 81.0, 22.0 ],
					"text" : "energythresh"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 3,
							"revision" : 1,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 62.0, 199.0, 713.0, 180.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-6",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 14.0, 133.0, 531.0, 20.0 ],
									"text" : "Note that the calculated values are stored as doubles (hence the 8 bytes in the calculation above)."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-4",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 120.0, 107.0, 407.0, 20.0 ],
									"text" : "(max buffer length in samples / hop size) * number of descriptors * 8 bytes "
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-2",
									"linecount" : 6,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 14.0, 12.0, 683.0, 89.0 ],
									"text" : "The default of 4MB should be fine for most purposes. If you are processing very large buffers you may wish to increase this value. Larger sizes will sometimes allow more efficient processing by reducing recalculation, but you are only likely to run into this scenario when you are both analysing long buffers and requesting a lot of descriptors. More importantly, if the buffer is sufficiently long there may not me enough space to store the necessary one value per frame in order to calculate even a single descriptor. If this happens you will receive an error message and should increase the memory allocation accordingly. For most efficient calculation (without memory re-usage) you can use the following calculation:"
								}

							}
 ],
						"lines" : [  ]
					}
,
					"patching_rect" : [ 103.0, 524.0, 155.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p calculation_memory_size"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4",
					"linecount" : 2,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 103.0, 482.0, 663.0, 33.0 ],
					"text" : "<- optional arguments are maximum fft size (default 65536) / calculation memory size in bytes (default 4194304 or 4 MB) / descriptor feedback (default off - when on the object will post in the max window when the descriptors are correctly set)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-6",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 56.0, 435.0, 106.0, 22.0 ],
					"text" : "r descriptors_help"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-5",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 287.0, 450.0, 248.0, 20.0 ],
					"text" : "descriptors~ currently only functions in mono"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-2",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 3,
							"revision" : 1,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 34.0, 117.0, 981.0, 601.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-4",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 567.0, 730.0, 20.0 ],
									"text" : "Thanks to Richard Parncutt for the original code to calculate roughness: http://www.uni-graz.at/richard.parncutt/computerprograms.html"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-20",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 17.0, 953.0, 33.0 ],
									"text" : "The descriptors object is very flexible and is capable of performing a large number of complex analysis tasks. This flexibility makes the descriptors message quite involved, but it worth spending time getting to grips with the various options. The examples subpatch may be helpful in understanding the message structure in practice."
								}

							}
, 							{
								"box" : 								{
									"color" : [ 1.0, 0.0, 0.0, 1.0 ],
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-18",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 0,
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 3,
											"revision" : 1,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 176.0, 151.0, 1147.0, 552.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"assistshowspatchername" : 0,
										"boxes" : [ 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-37",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 601.0, 509.0, 91.0, 20.0 ],
													"text" : "Loudness stats"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-36",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 599.0, 230.0, 83.0, 20.0 ],
													"text" : "Shape values"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-35",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 19.0, 509.0, 121.0, 20.0 ],
													"text" : "LFE MFE HFE ratios"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-30",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 599.0, 481.0, 517.0, 18.0 ],
													"text" : "-79.351318 -82.5 23.584726 84.033836 578.188416 -38.203716 -40.919136 -46.182068"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-31",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 662.0, 351.0, 276.0, 23.0 ],
													"text" : "Example 4: Various Statistics (loudness)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-32",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 613.0, 381.0, 466.0, 20.0 ],
													"text" : "Mean, median, standard deviation, range, time centroid and 3 max values of loudness"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-33",
													"linecount" : 2,
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 599.0, 406.0, 469.0, 32.0 ],
													"text" : "descriptors loudness mean median standard_dev range time_centroid max 3, analyse descriptors_help_buf2"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-34",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "list" ],
													"patching_rect" : [ 599.0, 449.0, 76.0, 20.0 ],
													"text" : "descriptors~"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-25",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 19.0, 481.0, 221.0, 18.0 ],
													"text" : "0.424482 0.150136 0.17555"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-26",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 99.0, 351.0, 287.0, 23.0 ],
													"text" : "Example 2: Energy Ratios (median values)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-27",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 381.0, 524.0, 20.0 ],
													"text" : "Median roportion of energy in 3 bands (LFE 0Hz-400Hz MFE 350Hz-5kHz HFE 4500Hz-20kHz)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-28",
													"linecount" : 2,
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 19.0, 406.0, 472.0, 32.0 ],
													"text" : "descriptors energy_ratio 0 400 median energy_ratio 350 5000 median energy_ratio 4500 20000 median, analyse descriptors_help_buf2"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-29",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "list" ],
													"patching_rect" : [ 19.0, 449.0, 76.0, 20.0 ],
													"text" : "descriptors~"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-24",
													"linecount" : 2,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 135.0, 292.0, 373.0, 34.0 ],
													"text" : "Note these time values (in ms) are ordered by the size of the energy value they relate to (not time order) - use zl sort to get time order "
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-22",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 599.0, 202.0, 517.0, 18.0 ],
													"text" : "2751.901611 535.305969 3688.909668 8.582689 0.025919 -0.188079 0.109154 2.283351"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-13",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 664.0, 72.0, 303.0, 23.0 ],
													"text" : "Example 2: Shape descriptors (mean values)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-16",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 615.0, 102.0, 239.0, 20.0 ],
													"text" : "Calculate all the spectral shape descriptors"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-17",
													"linecount" : 2,
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 599.0, 127.0, 422.0, 32.0 ],
													"text" : "descriptors lin_centroid log_centroid lin_spread log_spread lin_skewness log_skewness lin_kurtosis log_kurtosis, analyse descriptors_help_buf2"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-18",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "list" ],
													"patching_rect" : [ 599.0, 170.0, 76.0, 20.0 ],
													"text" : "descriptors~"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-11",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 84.0, 72.0, 324.0, 23.0 ],
													"text" : "Example 1: (could be used for sample splitting)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-8",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 19.0, 292.0, 113.0, 20.0 ],
													"text" : "print ernergy_cross"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-7",
													"linecount" : 2,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 180.0, 355.0, 34.0 ],
													"text" : "Now get up to 100 points where energy crosses the threshold of: mean energy - energy standard deviation (from above)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-5",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 102.0, 393.0, 20.0 ],
													"text" : "First get the standardard deviation of the energy across the whole buffer"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-3",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 382.0, 127.0, 138.0, 20.0 ],
													"text" : "loadmess fftparams 512"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-2",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 81.0, 13.0, 135.0, 20.0 ],
													"text" : "<---- Load a buffer here",
													"textcolor" : [ 1.0, 0.0, 0.0, 1.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-12",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 19.0, 13.0, 50.0, 18.0 ],
													"text" : "replace"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-52",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "float", "bang" ],
													"patching_rect" : [ 19.0, 39.5, 219.0, 20.0 ],
													"text" : "buffer~ descriptors_help_buf2 jongly.aif"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-54",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 19.0, 127.0, 354.0, 18.0 ],
													"text" : "descriptors energy standard_dev, analyse descriptors_help_buf2"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-53",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "list" ],
													"patching_rect" : [ 19.0, 152.0, 76.0, 20.0 ],
													"text" : "descriptors~"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-6",
													"linecount" : 2,
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 19.0, 227.0, 351.0, 32.0 ],
													"text" : "descriptors energy cross_above 100 threshold $1 mean_add duration, analyse descriptors_help_buf2"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-9",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "list" ],
													"patching_rect" : [ 19.0, 266.0, 76.0, 20.0 ],
													"text" : "descriptors~"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-52", 0 ],
													"source" : [ "obj-12", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-18", 0 ],
													"source" : [ "obj-17", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-22", 1 ],
													"midpoints" : [ 608.5, 195.5, 1106.5, 195.5 ],
													"source" : [ "obj-18", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-29", 0 ],
													"source" : [ "obj-28", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-25", 1 ],
													"midpoints" : [ 28.5, 474.5, 230.5, 474.5 ],
													"source" : [ "obj-29", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-53", 0 ],
													"midpoints" : [ 391.5, 149.0, 28.5, 149.0 ],
													"order" : 1,
													"source" : [ "obj-3", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"midpoints" : [ 391.5, 265.0, 28.5, 265.0 ],
													"order" : 0,
													"source" : [ "obj-3", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-34", 0 ],
													"source" : [ "obj-33", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-30", 1 ],
													"midpoints" : [ 608.5, 476.5, 1106.5, 476.5 ],
													"source" : [ "obj-34", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"source" : [ "obj-53", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-53", 0 ],
													"source" : [ "obj-54", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"source" : [ "obj-6", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 0 ],
													"source" : [ "obj-9", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 635.5, 127.0, 72.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p examples"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-17",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 0,
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 3,
											"revision" : 1,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 150.0, 123.0, 969.0, 259.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"assistshowspatchername" : 0,
										"boxes" : [ 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-5",
													"linecount" : 3,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 9.0, 11.0, 949.0, 48.0 ],
													"text" : "There are some desciptors that only return values for the entire chunk (rather than once per frame), and these do not take any statistical calculation arguments. Note that the spectral_peaks descriptor is one of these, and uses a cumulated spectral frame accross the entire buffer (unlike the descriptorsrt` object - which calculates the spectral peaks on a per frame basis). This works well for steady-state sounds, but will not return useful values in other cases."
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 205.0, 52.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-34",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 2,
													"outlettype" : [ "", "bang" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 637.0, 236.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"comment" : "To refer loadmess",
																	"id" : "obj-2",
																	"index" : 2,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 111.0, 91.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-1",
																	"maxclass" : "newobj",
																	"numinlets" : 2,
																	"numoutlets" : 3,
																	"outlettype" : [ "bang", "bang", "int" ],
																	"patching_rect" : [ 214.0, 120.0, 46.0, 20.0 ],
																	"text" : "uzi 7"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 29,
																		"data" : [ 																			{
																				"key" : "energy",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "energy_ratio",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "rolloff",
																				"value" : [ 1, "threshold", "0 - 1", 0.95, "the ratio of the total energy to use as the rolloff point" ]
																			}
, 																			{
																				"key" : "spectral_crest",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "flux",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 4, "normalise spectrum", "0 / 1", 0, "normalise the energy level in each frame", 5, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
																			}
, 																			{
																				"key" : "mkl",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "amplitude threshold", "dB", "-300 dB", "ignore amplitudes below this level", 4, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 5, "weight with second frame amplitudes", "0 / 1", 0, "weights the output using the amplitudes of the later frame", 6, "normalise spectrum", "0 / 1", 1, "normalise the energy level in each frame", 7, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
																			}
, 																			{
																				"key" : "foote",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 4, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
																			}
, 																			{
																				"key" : "abs",
																				"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "rms",
																				"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "peakamp",
																				"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "loudness",
																				"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
																			}
, 																			{
																				"key" : "lin_centroid",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "log_centroid",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "lin_spread",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "log_spread",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "lin_skewness",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "log_skewness",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "lin_kurtosis",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "log_kurtosis",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "lin_brightness",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "threshold", "0 - 1", 0.68, "the confidence threshold used for pitch detection" ]
																			}
, 																			{
																				"key" : "log_brightness",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "threshold", "0 - 1", 0.68, "the confidence threshold used for pitch detection" ]
																			}
, 																			{
																				"key" : "sfm",
																				"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
																			}
, 																			{
																				"key" : "noise_ratio",
																				"value" : [ 1, "median span", "bins", 15, "the width of the median filter is twice this value plus one" ]
																			}
, 																			{
																				"key" : "harmonic_ratio",
																				"value" : [ 1, "median span", "bins", 15, "the width of the median filter is twice this value plus one", "the width of the median filter is twice this value plus one" ]
																			}
, 																			{
																				"key" : "pitch",
																				"value" : [ 1, "threshold", "0 - 1", 0.68, "confidence level required to report a pitch (higher values indicate more confidence)" ]
																			}
, 																			{
																				"key" : "confidence",
																				"value" : [ 1, "threshold", "0 - 1", 0.68, "for use alongside the pitch descriptor you should provide the same value here" ]
																			}
, 																			{
																				"key" : "inharmonicity",
																				"value" : [ 1, "number of peaks", "n", 10, "the number of spectral peaks to use in the calculation (the n loudest peaks are used)", 2, "median span", "bins", 15, "the width of the median filter is twice this value plus one", 3, "threshold", "0 - 1", 0.68, "the confidence threshold used for pitch detection" ]
																			}
, 																			{
																				"key" : "roughness",
																				"value" : [ 1, "number of peaks", "n", 10, "the number of spectral peaks to use in the calculation (the n loudest peaks are used)", 2, "median span", "bins", 15, "the width of the median filter is twice this value plus one" ]
																			}
, 																			{
																				"key" : "spectral_peaks",
																				"value" : [ 1, "number of peaks", "n", 10, "the number of peaks to return", 2, "median span", "bins", 15, "the width of the median filter is twice this value plus one (determines valid peaks)" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-23",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 144.0, 90.0, 59.5, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 30.0, 87.0, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 2,
																		"data" : [ 																			{
																				"key" : "duration",
																				"value" : [ "the", "duration", "of", "the", "analysed", "buffer", "or", "chunk", "of", "the", "buffer", "(takes", "no", "parameters)" ]
																			}
, 																			{
																				"key" : "spectral_peaks",
																				"value" : [ "output", "the", "N", "largest", "sinusoidal", "peaks", "(in", "pairs", "of", "frequency", "(Hz)", "and", "linear", "amplitudes)", "-", "only", "works", "well", "for", "steady", "state", "chunks", "/", "buffers" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 30.0, 62.0, 59.5, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 111.0, 20.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 30.0, 120.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-16",
																	"maxclass" : "newobj",
																	"numinlets" : 2,
																	"numoutlets" : 2,
																	"outlettype" : [ "", "" ],
																	"patching_rect" : [ 144.0, 149.0, 49.0, 20.0 ],
																	"text" : "zl iter 5"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-14",
																	"maxclass" : "message",
																	"numinlets" : 2,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 241.0, 149.0, 67.0, 18.0 ],
																	"text" : "remove $1"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-12",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 3,
																	"outlettype" : [ "bang", "", "bang" ],
																	"patching_rect" : [ 111.0, 61.0, 122.0, 20.0 ],
																	"text" : "t b s b"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-7",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 316.0, 149.0, 308.0, 20.0 ],
																	"text" : "loadmess store Parameter Name Unit Default Comment"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-6",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 144.0, 187.0, 210.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 0,
																		"precision" : 6
																	}
,
																	"text" : "coll __descriptors_once_help_display"
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-14", 0 ],
																	"source" : [ "obj-1", 2 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-1", 0 ],
																	"source" : [ "obj-12", 2 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-2", 0 ],
																	"source" : [ "obj-12", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-23", 0 ],
																	"midpoints" : [ 172.0, 85.0, 153.5, 85.0 ],
																	"source" : [ "obj-12", 1 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-6", 0 ],
																	"midpoints" : [ 250.5, 176.5, 153.5, 176.5 ],
																	"source" : [ "obj-14", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-6", 0 ],
																	"source" : [ "obj-16", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-16", 0 ],
																	"source" : [ "obj-23", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-12", 0 ],
																	"order" : 0,
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"midpoints" : [ 120.5, 52.5, 39.5, 52.5 ],
																	"order" : 1,
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-6", 0 ],
																	"midpoints" : [ 325.5, 176.5, 153.5, 176.5 ],
																	"source" : [ "obj-7", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 342.0, 77.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-29",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 18.0, 147.0, 91.0, 23.0 ],
													"text" : "Parameters:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-27",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 19.0, 111.0, 90.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-25",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 19.0, 75.0, 181.0, 23.0 ],
													"text" : "Audio Desciptor / Feature:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-9",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 143.0, 144.0, 271.0, 20.0 ],
													"text" : "loadmess refer __descriptors_once_help_display"
												}

											}
, 											{
												"box" : 												{
													"border" : 0,
													"coldef" : [ [ 0, 70, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 1, 220, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 2, 85, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 3, 85, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 4, 470, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ] ],
													"colhead" : 1,
													"cols" : 5,
													"fontface" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"gridlinecolor" : [ 0.501961, 0.501961, 0.501961, 1.0 ],
													"headercolor" : [ 0.792157, 0.815686, 1.0, 1.0 ],
													"hscroll" : 0,
													"id" : "obj-8",
													"just" : 1,
													"maxclass" : "jit.cellblock",
													"numinlets" : 2,
													"numoutlets" : 4,
													"outlettype" : [ "list", "", "", "" ],
													"patching_rect" : [ 23.0, 184.0, 932.0, 55.0 ],
													"rowhead" : 1,
													"rows" : 1,
													"selmode" : 0,
													"vscroll" : 0
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-3",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 111.0, 113.0, 737.0, 20.0 ],
													"text" : "the duration of the analysed buffer or chunk of the buffer (takes no parameters)"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-1",
													"items" : [ "duration", ",", "spectral_peaks" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 205.0, 77.0, 131.0, 20.0 ],
													"textjustification" : 1
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-34", 0 ],
													"hidden" : 1,
													"midpoints" : [ 270.5, 99.0, 339.0, 99.0, 339.0, 72.0, 351.5, 72.0 ],
													"source" : [ "obj-1", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"hidden" : 1,
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-3", 0 ],
													"hidden" : 1,
													"midpoints" : [ 351.5, 102.5, 120.5, 102.5 ],
													"source" : [ "obj-34", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"hidden" : 1,
													"midpoints" : [ 390.5, 108.5, 152.5, 108.5 ],
													"source" : [ "obj-34", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 0 ],
													"hidden" : 1,
													"midpoints" : [ 152.5, 172.5, 32.5, 172.5 ],
													"source" : [ "obj-9", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 391.5, 127.0, 173.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p once_per_buffer_descriptors"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-15",
									"maxclass" : "newobj",
									"numinlets" : 0,
									"numoutlets" : 0,
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 3,
											"revision" : 1,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 106.0, 75.0, 835.0, 586.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"assistshowspatchername" : 0,
										"boxes" : [ 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-37",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 277.0, 792.0, 20.0 ],
													"text" : "These calculations take an argument for number of values to return (default 1 if none given) and are dependant on the threshold and the mask time:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-38",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 56.0, 307.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-39",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 374.0, 207.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-3",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 59.75, 96.0, 20.0 ],
																	"text" : "prepend symbol"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 115.75, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 20,
																		"data" : [ 																			{
																				"key" : "max",
																				"value" : [ "N", "maximum", "values" ]
																			}
, 																			{
																				"key" : "max_pos",
																				"value" : [ "positions", "of", "the", "N", "maximum", "values" ]
																			}
, 																			{
																				"key" : "min",
																				"value" : [ "N", "minimum", "values" ]
																			}
, 																			{
																				"key" : "min_pos",
																				"value" : [ "positions", "of", "the", "N", "minimum", "values" ]
																			}
, 																			{
																				"key" : "peak",
																				"value" : [ "N", "peak", "values", "(larger", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "peak_pos",
																				"value" : [ "positions", "of", "the", "N", "peak", "values", "(larger", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "trough",
																				"value" : [ "N", "trough", "values", "(smaller", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "trough_pos",
																				"value" : [ "positions", "of", "the", "N", "trough", "values", "(smaller", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "crossing_peak",
																				"value" : [ "N", "largest", "peaks", "that", "cross", "above", "a", "given", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_peak_pos",
																				"value" : [ "positions", "of", "the", "N", "largest", "peaks", "that", "cross", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "cross_above",
																				"value" : [ "positions", "of", "the", "first", "crossing", "point", "above", "the", "threshold", "for", "the", "N", "largest", "peaks", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossings_above",
																				"value" : [ "positions", "of", "both", "crossing", "points", "across", "the", "threshold", "for", "the", "N", "largest", "peaks", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_trough",
																				"value" : [ "N", "smallest", "troughs", "that", "cross", "below", "a", "given", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_trough_pos",
																				"value" : [ "positions", "of", "the", "N", "smallest", "troughs", "that", "cross", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "cross_below",
																				"value" : [ "positions", "of", "the", "first", "crossing", "point", "below", "the", "threshold", "for", "the", "N", "smallest", "troughs", "below", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossings_below",
																				"value" : [ "positions", "of", "both", "crossing", "points", "across", "the", "threshold", "for", "the", "N", "smallest", "troughs", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "longest_cross_above",
																				"value" : [ "N", "longest", "crossing", "points", "above", "the", "threshold", "(give", "only", "the", "first", "crossing", "point)" ]
																			}
, 																			{
																				"key" : "longest_crossings_above",
																				"value" : [ "N", "longest", "crossing", "points", "above", "the", "threshold", "(give", "both", "crossing", "points)" ]
																			}
, 																			{
																				"key" : "longest_cross_below",
																				"value" : [ "N", "longest", "crossing", "points", "below", "the", "threshold", "(give", "only", "the", "first", "crossing", "point)" ]
																			}
, 																			{
																				"key" : "longest_crossings_below",
																				"value" : [ "N", "longest", "crossing", "points", "below", "the", "threshold", "(give", "both", "crossing", "points)" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 117.0, 87.75, 59.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 31.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 117.0, 143.75, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-3", 0 ],
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 331.0, 307.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-40",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 333.0, 90.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-41",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 305.0, 89.0, 23.0 ],
													"text" : "Calculation:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-42",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 335.0, 575.0, 20.0 ],
													"text" : "N largest peaks that cross above a given threshold"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-43",
													"items" : [ "crossing_peak", ",", "crossing_peak_pos", ",", "cross_above", ",", "crossings_above", ",", "crossing_trough", ",", "crossing_trough_pos", ",", "cross_below", ",", "crossings_below", ",", "longest_cross_above", ",", "longest_crossings_above", ",", "longest_cross_below", ",", "longest_crossings_below" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 131.0, 307.0, 172.0, 20.0 ],
													"textjustification" : 1
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-36",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 189.0, 810.0, 20.0 ],
													"text" : "These calculations take an argument for number of values to return (default 1 if none given) and are dependant on the threshold but not the mask time:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-33",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 99.0, 752.0, 20.0 ],
													"text" : "The ratio calculations take no individual parameters but are dependant on the threshold value (they are not dependant on the mask time):"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-31",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 9.0, 564.0, 20.0 ],
													"text" : "These statistical calculations take no parameters and are not dependant on the threshold or mask time:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-21",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 489.0, 243.0, 20.0 ],
													"text" : "The threshold type can be any of the below:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-18",
													"linecount" : 4,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 12.0, 366.0, 811.0, 62.0 ],
													"text" : "The threshold and mask time can only be set once per descriptor (setting them twice will result in only the later value being used) and will apply to any relevant searches. If you wish to use the same descriptor with different thresholds or mask times you need to declare the descriptor again with different values for these parameters. The mask time sets the minimum distance between found values in the search-based calculations. The threshold and mask time may be specified in either order as follows:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-22",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 91.0, 520.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-23",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 374.0, 207.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-3",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 59.75, 96.0, 20.0 ],
																	"text" : "prepend symbol"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 115.75, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 7,
																		"data" : [ 																			{
																				"key" : "abs",
																				"value" : [ "absolute", "(use", "the", "given", "value", "as", "a", "threshold)", "-", "this", "is", "the", "default", "and", "need", "not", "be", "explicitly", "specified" ]
																			}
, 																			{
																				"key" : "peak_mul",
																				"value" : [ "the", "peak", "value", "multiplied", "by", "the", "given", "threshold", "value" ]
																			}
, 																			{
																				"key" : "peak_add",
																				"value" : [ "the", "peak", "value", "added", "to", "the", "given", "threshold", "value" ]
																			}
, 																			{
																				"key" : "peak_db",
																				"value" : [ "the", "line", "peak", "value", "adjusted", "by", "the", "specified", "value", "given", "in", "decibels", "(use", "for", "linear", "amplitude", "values)" ]
																			}
, 																			{
																				"key" : "mean_mul",
																				"value" : [ "the", "mean", "value", "multiplied", "by", "the", "given", "threshold", "value" ]
																			}
, 																			{
																				"key" : "mean_add",
																				"value" : [ "the", "mean", "value", "added", "to", "the", "given", "threshold", "value" ]
																			}
, 																			{
																				"key" : "mean_db",
																				"value" : [ "the", "linear", "mean", "value", "adjusted", "by", "the", "specified", "value", "given", "in", "decibels", "(use", "for", "linear", "amplitude", "values)" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 117.0, 87.75, 59.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 31.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 117.0, 143.75, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-3", 0 ],
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 378.0, 519.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-24",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 545.0, 90.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-26",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 517.0, 123.0, 23.0 ],
													"text" : "Threshold Types:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-28",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 547.0, 566.0, 20.0 ],
													"text" : "absolute (use the given value as a threshold) - this is the default and need not be explicitly specified"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-29",
													"items" : [ "abs", ",", "peak_mul", ",", "peak_add", ",", "peak_db", ",", "mean_mul", ",", "mean_add", ",", "mean_db" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 167.0, 519.0, 172.0, 20.0 ],
													"textjustification" : 1
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-19",
													"linecount" : 3,
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 434.0, 630.0, 48.0 ],
													"text" : "masktime / value\t\t\t\t\t\t\t\t                                                        - set the mask time for relevant statistics in ms\n\nthreshold / threshold value / (optional threshold type)\t\t     - set the threshold for relevant statistics (default type is abs)"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-10",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 56.0, 219.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-11",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 374.0, 207.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-3",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 59.75, 96.0, 20.0 ],
																	"text" : "prepend symbol"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 115.75, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 20,
																		"data" : [ 																			{
																				"key" : "max",
																				"value" : [ "N", "maximum", "values" ]
																			}
, 																			{
																				"key" : "max_pos",
																				"value" : [ "positions", "of", "the", "N", "maximum", "values" ]
																			}
, 																			{
																				"key" : "min",
																				"value" : [ "N", "minimum", "values" ]
																			}
, 																			{
																				"key" : "min_pos",
																				"value" : [ "positions", "of", "the", "N", "minimum", "values" ]
																			}
, 																			{
																				"key" : "peak",
																				"value" : [ "N", "peak", "values", "(larger", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "peak_pos",
																				"value" : [ "positions", "of", "the", "N", "peak", "values", "(larger", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "trough",
																				"value" : [ "N", "trough", "values", "(smaller", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "trough_pos",
																				"value" : [ "positions", "of", "the", "N", "trough", "values", "(smaller", "than", "the", "value", "either", "side)" ]
																			}
, 																			{
																				"key" : "crossing_peak",
																				"value" : [ "N", "largest", "peaks", "that", "cross", "above", "a", "given", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_peak_pos",
																				"value" : [ "positions", "of", "the", "N", "largest", "peaks", "that", "cross", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "cross_above",
																				"value" : [ "positions", "of", "the", "first", "crossing", "point", "above", "the", "threshold", "for", "the", "N", "largest", "peaks", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossings_above",
																				"value" : [ "positions", "of", "both", "crossing", "points", "across", "the", "threshold", "for", "the", "N", "largest", "peaks", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_trough",
																				"value" : [ "N", "smallest", "troughs", "that", "cross", "below", "a", "given", "threshold" ]
																			}
, 																			{
																				"key" : "crossing_trough_pos",
																				"value" : [ "positions", "of", "the", "N", "smallest", "troughs", "that", "cross", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "cross_below",
																				"value" : [ "positions", "of", "the", "first", "crossing", "point", "below", "the", "threshold", "for", "the", "N", "smallest", "troughs", "below", "the", "threshold" ]
																			}
, 																			{
																				"key" : "crossings_below",
																				"value" : [ "positions", "of", "both", "crossing", "points", "across", "the", "threshold", "for", "the", "N", "smallest", "troughs", "above", "the", "threshold" ]
																			}
, 																			{
																				"key" : "longest_cross_above",
																				"value" : [ "N", "longest", "crossing", "points", "above", "the", "threshold", "(give", "only", "the", "first", "crossing", "point)" ]
																			}
, 																			{
																				"key" : "longest_crossings_above",
																				"value" : [ "N", "longest", "crossing", "points", "above", "the", "threshold", "(give", "both", "crossing", "points)" ]
																			}
, 																			{
																				"key" : "longest_cross_below",
																				"value" : [ "N", "longest", "crossing", "points", "below", "the", "threshold", "(give", "only", "the", "first", "crossing", "point)" ]
																			}
, 																			{
																				"key" : "longest_crossings_below",
																				"value" : [ "N", "longest", "crossing", "points", "below", "the", "threshold", "(give", "both", "crossing", "points)" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 117.0, 87.75, 59.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 31.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 117.0, 143.75, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-3", 0 ],
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 331.0, 219.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-12",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 245.0, 90.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-13",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 217.0, 89.0, 23.0 ],
													"text" : "Calculation:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-14",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 247.0, 575.0, 20.0 ],
													"text" : "N maximum values"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-15",
													"items" : [ "max", ",", "max_pos", ",", "min", ",", "min_pos", ",", "peak", ",", "peak_pos", ",", "trough", ",", "trough_pos" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 131.0, 219.0, 172.0, 20.0 ],
													"textjustification" : 1
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-4",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 54.0, 129.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-5",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 374.0, 207.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-3",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 59.75, 96.0, 20.0 ],
																	"text" : "prepend symbol"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 115.75, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 2,
																		"data" : [ 																			{
																				"key" : "ratio_above",
																				"value" : [ "the", "ratio", "(0-1)", "of", "values", "above", "the", "given", "threshold" ]
																			}
, 																			{
																				"key" : "ratio_below",
																				"value" : [ "the", "ratio", "(0-1)", "of", "values", "below", "the", "given", "threshold" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 117.0, 87.75, 59.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 31.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 117.0, 143.75, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-3", 0 ],
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 329.0, 129.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-6",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 155.0, 90.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-7",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 127.0, 89.0, 23.0 ],
													"text" : "Calculation:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-8",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 157.0, 273.0, 20.0 ],
													"text" : "the ratio (0-1) of values above the given threshold"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-9",
													"items" : [ "ratio_above", ",", "ratio_below" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 131.0, 129.0, 172.0, 20.0 ],
													"textjustification" : 1
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-2",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 52.0, 39.0, 72.0, 20.0 ],
													"text" : "loadmess 0"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"hidden" : 1,
													"id" : "obj-34",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patcher" : 													{
														"fileversion" : 1,
														"appversion" : 														{
															"major" : 8,
															"minor" : 3,
															"revision" : 1,
															"architecture" : "x64",
															"modernui" : 1
														}
,
														"classnamespace" : "box",
														"rect" : [ 37.0, 412.0, 374.0, 207.0 ],
														"bglocked" : 0,
														"openinpresentation" : 0,
														"default_fontsize" : 12.0,
														"default_fontface" : 0,
														"default_fontname" : "Arial",
														"gridonopen" : 1,
														"gridsize" : [ 15.0, 15.0 ],
														"gridsnaponopen" : 1,
														"objectsnaponopen" : 1,
														"statusbarvisible" : 2,
														"toolbarvisible" : 1,
														"lefttoolbarpinned" : 0,
														"toptoolbarpinned" : 0,
														"righttoolbarpinned" : 0,
														"bottomtoolbarpinned" : 0,
														"toolbars_unpinned_last_save" : 0,
														"tallnewobj" : 0,
														"boxanimatetime" : 200,
														"enablehscroll" : 1,
														"enablevscroll" : 1,
														"devicewidth" : 0.0,
														"description" : "",
														"digest" : "",
														"tags" : "",
														"style" : "",
														"subpatcher_template" : "",
														"assistshowspatchername" : 0,
														"boxes" : [ 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-3",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 59.75, 96.0, 20.0 ],
																	"text" : "prepend symbol"
																}

															}
, 															{
																"box" : 																{
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-5",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 115.75, 74.0, 20.0 ],
																	"text" : "prepend set"
																}

															}
, 															{
																"box" : 																{
																	"coll_data" : 																	{
																		"count" : 5,
																		"data" : [ 																			{
																				"key" : "mean",
																				"value" : [ "mean", "value" ]
																			}
, 																			{
																				"key" : "median",
																				"value" : [ "median", "value" ]
																			}
, 																			{
																				"key" : "time_centroid",
																				"value" : [ "the", "time", "centroid", "is", "the", "mean", "of", "the", "frame", "times", "weighted", "by", "value", "-", "it", "is", "the", "tendency", "of", "the", "weight", "of", "the", "values", "over", "time" ]
																			}
, 																			{
																				"key" : "standard_dev",
																				"value" : [ "standard", "deviation" ]
																			}
, 																			{
																				"key" : "range",
																				"value" : [ "range", "(difference", "between", "maximum", "and", "minimum", "values)" ]
																			}
 ]
																	}
,
																	"fontname" : "Arial",
																	"fontsize" : 12.0,
																	"id" : "obj-4",
																	"maxclass" : "newobj",
																	"numinlets" : 1,
																	"numoutlets" : 4,
																	"outlettype" : [ "", "", "", "" ],
																	"patching_rect" : [ 117.0, 87.75, 59.0, 20.0 ],
																	"saved_object_attributes" : 																	{
																		"embed" : 1,
																		"precision" : 6
																	}
,
																	"text" : "coll"
																}

															}
, 															{
																"box" : 																{
																	"comment" : "From umenu",
																	"id" : "obj-32",
																	"index" : 1,
																	"maxclass" : "inlet",
																	"numinlets" : 0,
																	"numoutlets" : 1,
																	"outlettype" : [ "" ],
																	"patching_rect" : [ 117.0, 31.0, 25.0, 25.0 ]
																}

															}
, 															{
																"box" : 																{
																	"comment" : "To Description Comment",
																	"id" : "obj-33",
																	"index" : 1,
																	"maxclass" : "outlet",
																	"numinlets" : 1,
																	"numoutlets" : 0,
																	"patching_rect" : [ 117.0, 143.75, 25.0, 25.0 ]
																}

															}
 ],
														"lines" : [ 															{
																"patchline" : 																{
																	"destination" : [ "obj-4", 0 ],
																	"source" : [ "obj-3", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-3", 0 ],
																	"source" : [ "obj-32", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-5", 0 ],
																	"source" : [ "obj-4", 0 ]
																}

															}
, 															{
																"patchline" : 																{
																	"destination" : [ "obj-33", 0 ],
																	"source" : [ "obj-5", 0 ]
																}

															}
 ]
													}
,
													"patching_rect" : [ 327.0, 39.0, 58.0, 20.0 ],
													"saved_object_attributes" : 													{
														"description" : "",
														"digest" : "",
														"globalpatchername" : "",
														"tags" : ""
													}
,
													"text" : "p display"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-27",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 65.0, 91.0, 23.0 ],
													"text" : "Description:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial Black",
													"fontsize" : 12.0,
													"id" : "obj-25",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 35.0, 37.0, 89.0, 23.0 ],
													"text" : "Calculation:"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-3",
													"maxclass" : "comment",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 129.0, 67.0, 660.0, 20.0 ],
													"text" : "mean value"
												}

											}
, 											{
												"box" : 												{
													"align" : 1,
													"allowdrag" : 0,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-1",
													"items" : [ "mean", ",", "median", ",", "time_centroid", ",", "standard_dev", ",", "range" ],
													"maxclass" : "umenu",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "int", "", "" ],
													"parameter_enable" : 0,
													"patching_rect" : [ 131.0, 39.0, 172.0, 20.0 ],
													"textjustification" : 1
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-34", 0 ],
													"hidden" : 1,
													"midpoints" : [ 217.0, 61.0, 315.0, 61.0, 315.0, 34.0, 336.5, 34.0 ],
													"source" : [ "obj-1", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-15", 0 ],
													"hidden" : 1,
													"source" : [ "obj-10", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-14", 0 ],
													"hidden" : 1,
													"midpoints" : [ 340.5, 244.5, 138.5, 244.5 ],
													"source" : [ "obj-11", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-11", 0 ],
													"hidden" : 1,
													"midpoints" : [ 217.0, 241.0, 316.0, 241.0, 316.0, 214.0, 340.5, 214.0 ],
													"source" : [ "obj-15", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"hidden" : 1,
													"source" : [ "obj-2", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-29", 0 ],
													"hidden" : 1,
													"source" : [ "obj-22", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-28", 0 ],
													"hidden" : 1,
													"midpoints" : [ 387.5, 544.5, 138.5, 544.5 ],
													"source" : [ "obj-23", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-23", 0 ],
													"hidden" : 1,
													"midpoints" : [ 253.0, 541.0, 363.0, 541.0, 363.0, 514.0, 387.5, 514.0 ],
													"source" : [ "obj-29", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-3", 0 ],
													"hidden" : 1,
													"midpoints" : [ 336.5, 64.5, 138.5, 64.5 ],
													"source" : [ "obj-34", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-43", 0 ],
													"hidden" : 1,
													"source" : [ "obj-38", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-42", 0 ],
													"hidden" : 1,
													"midpoints" : [ 340.5, 332.5, 138.5, 332.5 ],
													"source" : [ "obj-39", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-9", 0 ],
													"hidden" : 1,
													"source" : [ "obj-4", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-39", 0 ],
													"hidden" : 1,
													"midpoints" : [ 217.0, 329.0, 316.0, 329.0, 316.0, 302.0, 340.5, 302.0 ],
													"source" : [ "obj-43", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-8", 0 ],
													"hidden" : 1,
													"midpoints" : [ 338.5, 154.5, 138.5, 154.5 ],
													"source" : [ "obj-5", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-5", 0 ],
													"hidden" : 1,
													"midpoints" : [ 217.0, 151.0, 317.0, 151.0, 317.0, 124.0, 338.5, 124.0 ],
													"source" : [ "obj-9", 1 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 182.0, 127.0, 139.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p statistical_calculations"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-10",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 158.0, 847.0, 20.0 ],
									"text" : "The message structure is therefore: descriptors [descriptor name / params... / [statistical calculation / param] (repeated) / [threshold…. masktime...]] (repeat)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-5",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 181.0, 957.0, 47.0 ],
									"text" : "Each descriptor may have a different number of parameters which are set in a pre-defined order, and each parameter has a default value which will be used in the absence of an argument to set the parameter explicitly. You may set as many, or as few of the parameters for each descriptor as you wish. Therefore, a list of descriptors to calculate is sufficient for default operation."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-16",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 255.5, 957.0, 20.0 ],
									"text" : "For more technical detail on audio descriptors / features see: http://recherche.ircam.fr/equipes/analyse-synthese/peeters/ARTICLES/Peeters_2003_cuidadoaudiofeatures.pdf"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-14",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 232.5, 525.0, 20.0 ],
									"text" : "Use the umenu below to view all the avaiable desciptors along with a list of available parameters."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-7",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 55.0, 954.0, 60.0 ],
									"text" : "The descriptors message should be followed by a list of descriptors, each followed by any parameters you wish to set. Each descriptor and parameter set may optionally be followed by a set of statistical calculations to perform on the data. If no calculations are present the mean of the values for all frames will be returned. You may specify as many statistics to calculate as you like after each descriptor and parameter set. Each statistic may be followed by a parameter argument if relevant. Finally you can specify a mask time and threshold for those statistical calculations which require them. (see the subpatch for more details on stastistical calulations, thresholds and mask times)."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 206.0, 273.0, 72.0, 22.0 ],
									"text" : "loadmess 0"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-34",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 2,
									"outlettype" : [ "", "bang" ],
									"patcher" : 									{
										"fileversion" : 1,
										"appversion" : 										{
											"major" : 8,
											"minor" : 3,
											"revision" : 1,
											"architecture" : "x64",
											"modernui" : 1
										}
,
										"classnamespace" : "box",
										"rect" : [ 37.0, 412.0, 598.0, 232.0 ],
										"bglocked" : 0,
										"openinpresentation" : 0,
										"default_fontsize" : 12.0,
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"gridonopen" : 1,
										"gridsize" : [ 15.0, 15.0 ],
										"gridsnaponopen" : 1,
										"objectsnaponopen" : 1,
										"statusbarvisible" : 2,
										"toolbarvisible" : 1,
										"lefttoolbarpinned" : 0,
										"toptoolbarpinned" : 0,
										"righttoolbarpinned" : 0,
										"bottomtoolbarpinned" : 0,
										"toolbars_unpinned_last_save" : 0,
										"tallnewobj" : 0,
										"boxanimatetime" : 200,
										"enablehscroll" : 1,
										"enablevscroll" : 1,
										"devicewidth" : 0.0,
										"description" : "",
										"digest" : "",
										"tags" : "",
										"style" : "",
										"subpatcher_template" : "",
										"assistshowspatchername" : 0,
										"boxes" : [ 											{
												"box" : 												{
													"comment" : "To refer loadmess",
													"id" : "obj-2",
													"index" : 2,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 111.0, 120.0, 25.0, 25.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-1",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 3,
													"outlettype" : [ "bang", "bang", "int" ],
													"patching_rect" : [ 176.0, 120.0, 46.0, 22.0 ],
													"text" : "uzi 7"
												}

											}
, 											{
												"box" : 												{
													"coll_data" : 													{
														"count" : 26,
														"data" : [ 															{
																"key" : "energy",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "energy_ratio",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "rolloff",
																"value" : [ 1, "threshold", "0 - 1", 0.95, "the ratio of the total energy to use as the rolloff point" ]
															}
, 															{
																"key" : "spectral_crest",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "flux",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 4, "square amplitudes", "0 / 1", 1, "square the amplitudes for calculation", 5, "normalise spectrum", "0 / 1", 1, "normalise the energy level in each frame", 6, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
															}
, 															{
																"key" : "mkl",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "amplitude threshold", "dB", "-300 dB", "ignore amplitudes below this level", 4, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 5, "weight with second frame amplitudes", "0 / 1", 0, "weights the output using the amplitudes of the later frame", 6, "normalise spectrum", "0 / 1", 1, "normalise the energy level in each frame", 7, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
															}
, 															{
																"key" : "foote",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "forward amplitudes only", "0 / 1", 1, "use only amplitudes from bins that have increased in level", 4, "frames to look back", "n", 1, "gap between frames for comparison (note that consecutive frames can overlap)" ]
															}
, 															{
																"key" : "abs",
																"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "rms",
																"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "peakamp",
																"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "loudness",
																"value" : [ 1, "decibel conversion", "0 / 1", 1, "when 0 the linear value is returned / when 1 the value is returned in dB" ]
															}
, 															{
																"key" : "lin_centroid",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "log_centroid",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "lin_spread",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "log_spread",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "lin_skewness",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "log_skewness",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "lin_kurtosis",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "log_kurtosis",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "lin_brightness",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "threshold", "0 - 1", 0.68, "the confidence threshold used for pitch detection" ]
															}
, 															{
																"key" : "log_brightness",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation", 3, "threshold", "0 - 1", 0.68, "the confidence threshold used for pitch detection" ]
															}
, 															{
																"key" : "sfm",
																"value" : [ 1, "min freq", "Hz", "0 Hz", "frequencies below this value will not be used in the calculation", 2, "max freq", "Hz", "192000 Hz", "frequencies above this value will not be used in the calculation" ]
															}
, 															{
																"key" : "noise_ratio",
																"value" : [ 1, "median span", "bins", 15, "the actual width of the median filter is twice this value plus one" ]
															}
, 															{
																"key" : "harmonic_ratio",
																"value" : [ 1, "median span", "bins", 15, "the actual width of the median filter is twice this value plus one", "the actual width of the median filter is twice this value plus one" ]
															}
, 															{
																"key" : "pitch",
																"value" : [ 1, "threshold", "0 - 1", 0.68, "confidence level required to report a pitch (higher values indicate more confidence)" ]
															}
, 															{
																"key" : "confidence",
																"value" : [ 1, "threshold", "0 - 1", 0.68, "for use alongside the pitch descriptor you should provide the same value here" ]
															}
 ]
													}
,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-23",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 4,
													"outlettype" : [ "", "", "", "" ],
													"patching_rect" : [ 111.0, 61.0, 59.5, 22.0 ],
													"saved_object_attributes" : 													{
														"embed" : 1,
														"precision" : 6
													}
,
													"text" : "coll"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-5",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 30.0, 87.0, 74.0, 22.0 ],
													"text" : "prepend set"
												}

											}
, 											{
												"box" : 												{
													"coll_data" : 													{
														"count" : 28,
														"data" : [ 															{
																"key" : "energy",
																"value" : [ "the", "energy", "contained", "within", "the", "given", "frequency", "range", "(either", "in", "linear", "value", "or", "in", "decibels)" ]
															}
, 															{
																"key" : "energy_ratio",
																"value" : [ "the", "ratio", "between", "the", "energy", "contained", "in", "the", "entire", "frequency", "range", "and", "the", "energy", "in", "the", "specified", "band" ]
															}
, 															{
																"key" : "rolloff",
																"value" : [ "the", "spectral", "rolloff", "is", "the", "point", "in", "Hz", "below", "which", "a", "specified", "amount", "of", "the", "entire", "signal", "energy", "is", "contained", "(usually", "95%)" ]
															}
, 															{
																"key" : "spectral_crest",
																"value" : [ "the", "spectral", "crest", "gives", "the", "ratio", "between", "the", "maximum", "peak", "bin", "value", "and", "the", "average", "bin", "value" ]
															}
, 															{
																"key" : "flux",
																"value" : [ "perhaps", "the", "simplest", "spectral", "difference", "measures", "-", "useful", "for", "onset", "detection", "applications", "etc." ]
															}
, 															{
																"key" : "mkl",
																"value" : [ "the", "Modified", "Kullback", "Leibler", "difference", "measre-", "this", "is", "more", "advanced", "than", "the", "flux", "measure" ]
															}
, 															{
																"key" : "foote",
																"value" : [ "a", "spectral", "difference", "measure", "that", "is", "always", "normalised", "(0-1)" ]
															}
, 															{
																"key" : "abs",
																"value" : [ "the", "average", "of", "the", "absolute", "amplitudes", "in", "each", "frame", "either", "in", "linear", "amplitude", "or", "decibels" ]
															}
, 															{
																"key" : "rms",
																"value" : [ "the", "rms", "amplitude", "of", "each", "frame", "either", "in", "linear", "amplitude", "or", "decibels" ]
															}
, 															{
																"key" : "peakamp",
																"value" : [ "the", "peak", "amplitude", "of", "each", "frame", "either", "in", "linear", "amplitude", "or", "decibels" ]
															}
, 															{
																"key" : "loudness",
																"value" : [ "the", "loudness", "is", "a", "similar", "to", "the", "energy", "measure", "but", "with", "the", "spectrum", "weighted", "according", "to", "human", "perception" ]
															}
, 															{
																"key" : "lin_centroid",
																"value" : [ "central", "spectral", "tendency", "in", "Hz", "calculated", "using", "a", "linear", "weighting" ]
															}
, 															{
																"key" : "log_centroid",
																"value" : [ "central", "spectral", "tendency", "in", "Hz", "calculated", "using", "a", "log", "weighting" ]
															}
, 															{
																"key" : "lin_spread",
																"value" : [ "spectral", "spread", "in", "Hz", "calculated", "using", "a", "linear", "weighting" ]
															}
, 															{
																"key" : "log_spread",
																"value" : [ "spectral", "spread", "calculated", "using", "a", "log", "weighting" ]
															}
, 															{
																"key" : "lin_skewness",
																"value" : [ "spectral", "skewness", "calculated", "using", "a", "linear", "weighting" ]
															}
, 															{
																"key" : "log_skewness",
																"value" : [ "spectral", "kurtosis", "calculated", "using", "a", "log", "weighting" ]
															}
, 															{
																"key" : "lin_kurtosis",
																"value" : [ "spectral", "kurtosis", "calculated", "using", "a", "linear", "weighting" ]
															}
, 															{
																"key" : "log_kurtosis",
																"value" : [ "spectral", "kurtosis", "calculated", "using", "a", "log", "weighting" ]
															}
, 															{
																"key" : "lin_brightness",
																"value" : [ "the", "linear", "brightness", "is", "found", "by", "dividing", "the", "lin", "centroid", "by", "the", "fundamental", "pitch", "estimate" ]
															}
, 															{
																"key" : "log_brightness",
																"value" : [ "the", "log", "brightness", "is", "found", "by", "dividing", "the", "log", "centroid", "by", "the", "fundamental", "pitch", "estimate" ]
															}
, 															{
																"key" : "sfm",
																"value" : [ "a", "value", "between", 0, 1, "-", 1, "indicates", "a", "very", "flat", "spectrum", "and", "hence", "more", "noisy", "/", "transient", "-", "more", "tonal", "sounds", "produce", "lower", "values" ]
															}
, 															{
																"key" : "noise_ratio",
																"value" : [ "a", 0, "to", 1, "estimate", "of", "the", "spectral", "noise", "content", "-", "values", "below", "the", "median", "spectrum", "are", "considered", "noise" ]
															}
, 															{
																"key" : "harmonic_ratio",
																"value" : [ "a", 0, "to", 1, "estimate", "of", "how", "much", "of", "the", "spectral", "content", "is", "*not*", "noise", "-", "the", "complement", "of", "the", "noise", "ratio" ]
															}
, 															{
																"key" : "pitch",
																"value" : [ "an", "autocorrelation", "fundamental", "pitch", "estimator", "with", "output", "in", "Hz" ]
															}
, 															{
																"key" : "confidence",
																"value" : [ "a", "value", "between", 0, "and1", "related", "to", "pitch", "estimation", "-", "higher", "values", "reflect", "more", "confidence", "in", "the", "pitch", "estimate" ]
															}
, 															{
																"key" : "inharmonicity",
																"value" : [ "The", "inharmonicity", "is", "a", "perceptual", "measure", "based", "on", "the", "sinusoidal", "components", "and", "estimated", "pitch." ]
															}
, 															{
																"key" : "roughness",
																"value" : [ "the", "roughness", "is", "a", "perceptual", "measure", "based", "on", "the", "sinusoidal", "components" ]
															}
 ]
													}
,
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-4",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 4,
													"outlettype" : [ "", "", "", "" ],
													"patching_rect" : [ 30.0, 62.0, 59.5, 22.0 ],
													"saved_object_attributes" : 													{
														"embed" : 1,
														"precision" : 6
													}
,
													"text" : "coll"
												}

											}
, 											{
												"box" : 												{
													"comment" : "From umenu",
													"id" : "obj-32",
													"index" : 1,
													"maxclass" : "inlet",
													"numinlets" : 0,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 111.0, 20.0, 25.0, 25.0 ]
												}

											}
, 											{
												"box" : 												{
													"comment" : "To Description Comment",
													"id" : "obj-33",
													"index" : 1,
													"maxclass" : "outlet",
													"numinlets" : 1,
													"numoutlets" : 0,
													"patching_rect" : [ 30.0, 120.0, 25.0, 25.0 ]
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-16",
													"maxclass" : "newobj",
													"numinlets" : 2,
													"numoutlets" : 2,
													"outlettype" : [ "", "" ],
													"patching_rect" : [ 144.0, 149.0, 49.0, 22.0 ],
													"text" : "zl iter 5"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-14",
													"maxclass" : "message",
													"numinlets" : 2,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 203.0, 149.0, 67.0, 22.0 ],
													"text" : "remove $1"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-12",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 3,
													"outlettype" : [ "bang", "", "bang" ],
													"patching_rect" : [ 111.0, 93.0, 84.0, 22.0 ],
													"text" : "t b l b"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-7",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 1,
													"outlettype" : [ "" ],
													"patching_rect" : [ 278.0, 149.0, 308.0, 22.0 ],
													"text" : "loadmess store Parameter Name Unit Default Comment"
												}

											}
, 											{
												"box" : 												{
													"fontname" : "Arial",
													"fontsize" : 12.0,
													"id" : "obj-6",
													"maxclass" : "newobj",
													"numinlets" : 1,
													"numoutlets" : 4,
													"outlettype" : [ "", "", "", "" ],
													"patching_rect" : [ 144.0, 187.0, 177.0, 22.0 ],
													"saved_object_attributes" : 													{
														"embed" : 0,
														"precision" : 6
													}
,
													"text" : "coll __descriptors_help_display"
												}

											}
 ],
										"lines" : [ 											{
												"patchline" : 												{
													"destination" : [ "obj-14", 0 ],
													"source" : [ "obj-1", 2 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-1", 0 ],
													"source" : [ "obj-12", 2 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-16", 0 ],
													"source" : [ "obj-12", 1 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-2", 0 ],
													"source" : [ "obj-12", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"midpoints" : [ 212.5, 176.5, 153.5, 176.5 ],
													"source" : [ "obj-14", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"source" : [ "obj-16", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-12", 0 ],
													"source" : [ "obj-23", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-23", 0 ],
													"order" : 0,
													"source" : [ "obj-32", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-4", 0 ],
													"midpoints" : [ 120.5, 52.5, 39.5, 52.5 ],
													"order" : 1,
													"source" : [ "obj-32", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-5", 0 ],
													"source" : [ "obj-4", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-33", 0 ],
													"source" : [ "obj-5", 0 ]
												}

											}
, 											{
												"patchline" : 												{
													"destination" : [ "obj-6", 0 ],
													"midpoints" : [ 287.5, 176.5, 153.5, 176.5 ],
													"source" : [ "obj-7", 0 ]
												}

											}
 ]
									}
,
									"patching_rect" : [ 343.0, 298.0, 58.0, 22.0 ],
									"saved_object_attributes" : 									{
										"description" : "",
										"digest" : "",
										"globalpatchername" : "",
										"tags" : ""
									}
,
									"text" : "p display"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-29",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 19.0, 368.0, 91.0, 23.0 ],
									"text" : "Parameters:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-27",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 332.0, 90.0, 23.0 ],
									"text" : "Description:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-25",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 296.0, 181.0, 23.0 ],
									"text" : "Audio Desciptor / Feature:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-9",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 144.0, 365.0, 239.0, 22.0 ],
									"text" : "loadmess refer __descriptors_help_display"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"coldef" : [ [ 0, 70, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 1, 220, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 2, 85, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 3, 85, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 4, 470, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ] ],
									"colhead" : 1,
									"cols" : 5,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"gridlinecolor" : [ 0.501961, 0.501961, 0.501961, 1.0 ],
									"headercolor" : [ 0.792157, 0.815686, 1.0, 1.0 ],
									"hscroll" : 0,
									"id" : "obj-8",
									"just" : 1,
									"maxclass" : "jit.cellblock",
									"numinlets" : 2,
									"numoutlets" : 4,
									"outlettype" : [ "list", "", "", "" ],
									"patching_rect" : [ 24.0, 405.0, 931.0, 147.0 ],
									"rowhead" : 1,
									"rows" : 7,
									"selmode" : 0,
									"vscroll" : 0
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-3",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 112.0, 334.0, 713.0, 20.0 ],
									"text" : "perhaps the simplest spectral difference measures - useful for onset detection applications etc."
								}

							}
, 							{
								"box" : 								{
									"align" : 1,
									"allowdrag" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-1",
									"items" : [ "energy", ",", "energy_ratio", ",", "rolloff", ",", "spectral_crest", ",", "flux", ",", "mkl", ",", "foote", ",", "abs", ",", "rms", ",", "peakamp", ",", "loudness", ",", "lin_centroid", ",", "log_centroid", ",", "lin_spread", ",", "log_spread", ",", "lin_skewness", ",", "log_skewness", ",", "lin_kurtosis", ",", "log_kurtosis", ",", "lin_brightness", ",", "log_brightness", ",", "sfm", ",", "noise_ratio", ",", "harmonic_ratio", ",", "pitch", ",", "confidence", ",", "inharmonicity", ",", "roughness" ],
									"maxclass" : "umenu",
									"numinlets" : 1,
									"numoutlets" : 3,
									"outlettype" : [ "int", "", "" ],
									"parameter_enable" : 0,
									"patching_rect" : [ 206.0, 298.0, 131.0, 22.0 ],
									"textjustification" : 1
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-34", 0 ],
									"hidden" : 1,
									"midpoints" : [ 271.5, 320.0, 340.0, 320.0, 340.0, 293.0, 352.5, 293.0 ],
									"source" : [ "obj-1", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"hidden" : 1,
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-3", 0 ],
									"hidden" : 1,
									"midpoints" : [ 352.5, 323.5, 121.5, 323.5 ],
									"source" : [ "obj-34", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-9", 0 ],
									"hidden" : 1,
									"midpoints" : [ 391.5, 329.5, 153.5, 329.5 ],
									"source" : [ "obj-34", 1 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"hidden" : 1,
									"midpoints" : [ 153.5, 393.5, 33.5, 393.5 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 531.0, 259.0, 273.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p setting_descriptors_and_statistical_calculations"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 11.595186999999999,
					"hidden" : 1,
					"id" : "obj-53",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 707.0, 589.0, 55.0, 21.0 ],
					"text" : "autohelp"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 3,
					"fontname" : "Arial",
					"fontsize" : 20.871338000000002,
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"text" : "descriptors~",
					"textcolor" : [ 0.2, 0.2, 0.2, 1.0 ],
					"varname" : "autohelp_top_title"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.754705,
					"id" : "obj-54",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"text" : "Calculate and analyse audio descriptors (features) from a buffer",
					"textcolor" : [ 0.2, 0.2, 0.2, 1.0 ],
					"varname" : "autohelp_top_digest"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 11.595187187194824,
					"id" : "obj-17",
					"linecount" : 6,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 57.0, 842.0, 84.0 ],
					"text" : "The descriptors~ object can be used to calculate a large number of audio descriptors (or features) from a sample stored in an MSP buffer~ (or an ibuffer~ object). Various statistical calculations may be performed on the resultant descriptors (mean / median / standard deviation / max n values etc.) to produce useful information about the sample. descriptors~ is intended as a comprehensive analysis solution, and is the non real-time counterpart to the descriptorsrt~ object - the two objects are very similar in terms of features and usage. descriptors~ only calculates and outputs the descriptors that the user requests, and can also operate on subsections of a buffer~ / ibuffer~. It is designed to be as efficient as possible, avoiding unnecessary calculations and re-calculations wherever possible and making extensive use of SIMD operations.",
					"varname" : "autohelp_top_description"
				}

			}
, 			{
				"box" : 				{
					"bgcolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"bgoncolor" : [ 0.0, 0.0, 0.0, 0.0 ],
					"fontface" : 3,
					"fontlink" : 1,
					"fontname" : "Arial",
					"fontsize" : 12.754705,
					"hidden" : 1,
					"id" : "obj-26",
					"legacytextcolor" : 1,
					"maxclass" : "textbutton",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "", "", "int" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 320.09796142578125, 22.0, 179.90203857421875, 14.249397277832031 ],
					"spacing_x" : 0.0,
					"spacing_y" : 0.0,
					"text" : "open descriptors~ reference",
					"textcolor" : [ 0.27, 0.35, 0.47, 1.0 ],
					"textoncolor" : [ 0.27, 0.35, 0.47, 1.0 ],
					"textovercolor" : [ 0.4, 0.5, 0.65, 1.0 ],
					"underline" : 1,
					"usebgoncolor" : 1,
					"usetextovercolor" : 1,
					"varname" : "autohelp_top_ref"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 1,
					"fontname" : "Arial",
					"fontsize" : 11.595186999999999,
					"id" : "obj-79",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 712.0, 593.0, 100.0, 19.0 ],
					"text" : "See Also:",
					"textcolor" : [ 0.0, 0.0, 0.0, 1.0 ],
					"varname" : "autohelp_see_title"
				}

			}
, 			{
				"box" : 				{
					"allowdrag" : 0,
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 11.595186999999999,
					"id" : "obj-80",
					"items" : [ "(Objects:)", ",", "descriptorsrt~", ",", "entrymatcher", ",", "entrymatcher~" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 712.0, 613.0, 130.0, 21.0 ],
					"varname" : "autohelp_see_menu"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-52",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 2,
					"outlettype" : [ "float", "bang" ],
					"patching_rect" : [ 430.0, 619.5, 213.0, 22.0 ],
					"text" : "buffer~ descriptors_help_buf jongly.aif"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-51",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 287.0, 422.0, 283.0, 20.0 ],
					"text" : "thus you can analyse specific chunks of a (i)buffer~"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-48",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 287.0, 403.0, 369.0, 20.0 ],
					"text" : "you can optionally specify channel / start point (ms) / end point (ms)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-49",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 404.0, 230.0, 22.0 ],
					"text" : "analyse descriptors_help_buf 1 500 2000"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-47",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 3,
							"revision" : 1,
							"architecture" : "x64",
							"modernui" : 1
						}
,
						"classnamespace" : "box",
						"rect" : [ 44.0, 112.0, 1089.0, 469.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"gridonopen" : 1,
						"gridsize" : [ 15.0, 15.0 ],
						"gridsnaponopen" : 1,
						"objectsnaponopen" : 1,
						"statusbarvisible" : 2,
						"toolbarvisible" : 1,
						"lefttoolbarpinned" : 0,
						"toptoolbarpinned" : 0,
						"righttoolbarpinned" : 0,
						"bottomtoolbarpinned" : 0,
						"toolbars_unpinned_last_save" : 0,
						"tallnewobj" : 0,
						"boxanimatetime" : 200,
						"enablehscroll" : 1,
						"enablevscroll" : 1,
						"devicewidth" : 0.0,
						"description" : "",
						"digest" : "",
						"tags" : "",
						"style" : "",
						"subpatcher_template" : "",
						"assistshowspatchername" : 0,
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-47",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 849.0, 25.0, 94.0, 20.0 ],
									"text" : "a few examples"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-43",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 375.0, 263.0, 20.0 ],
									"text" : "for most purposes the default (hann) will be fine"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-41",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 421.0, 719.0, 20.0 ],
									"text" : "rectangle / hann / hamming / kaiser / triangle / blackman / blackman62 / blackman70 / blackman74 / blackman92 / blackman / flattop"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-37",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 398.0, 359.0, 20.0 ],
									"text" : "available windows (the kaiser window provided is for alpha = 6.8):"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-32",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 70.0, 110.0, 376.0, 20.0 ],
									"text" : "fft size = 4096, hop size = 2048, window size = 4096, window = hann"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-31",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 24.0, 86.0, 197.0, 20.0 ],
									"text" : "The object default parameters are: "
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-29",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 352.0, 617.0, 20.0 ],
									"text" : "different windows have different characteristics - there are many good references (paper and internet) on this topic"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-27",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 351.0, 99.0, 23.0 ],
									"text" : "Window Type"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-25",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 298.0, 458.0, 20.0 ],
									"text" : "in this circumstance the pitch descriptor may not function correctly (use with caution)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-23",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 275.0, 493.0, 20.0 ],
									"text" : "if you wish to perform zero-padding you should set the window size smaller than the fft size"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-22",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 321.0, 270.0, 20.0 ],
									"text" : "the window size cannot be larger than the fft size"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-20",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 245.0, 661.0, 20.0 ],
									"text" : "if the hop size is greater than the fft size then parts of the data in the buffer~ will be missed (this is not advised but allowed)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-18",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 275.0, 95.0, 23.0 ],
									"text" : "WIndow Size"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-16",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 169.0, 524.0, 20.0 ],
									"text" : "higher FFt sizes give better frequency resolution at the expense of time resolution and vice versa"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-14",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 146.0, 387.0, 20.0 ],
									"text" : "there is (as always) a trade-off between time and frequency resolutions"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-12",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 145.0, 64.0, 23.0 ],
									"text" : "FFT Size"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-11",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 199.0, 307.0, 20.0 ],
									"text" : "is the distance in samples between consecutive frames"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-9",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 200.0, 66.0, 23.0 ],
									"text" : "Hop Size"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-7",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 71.0, 62.0, 327.0, 20.0 ],
									"text" : "hop size = fft size / 2, window size = fft size, window = hann"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-4",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 121.0, 222.0, 537.0, 20.0 ],
									"text" : "smaller hop sizes will require more time to calculate but can improve time resolution to some extent"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-34",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 24.0, 38.0, 713.0, 20.0 ],
									"text" : "Everything is optional except fft size - if not supplied (or zero) the other arguments will default to: "
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-35",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 95.0, 114.0, 18.0 ],
									"text" : "fftparams 4096 512"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-36",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 55.0, 114.0, 18.0 ],
									"text" : "fftparams 1024 256"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-38",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 75.0, 120.0, 18.0 ],
									"text" : "fftparams 8192 1024"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-39",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 115.0, 107.0, 18.0 ],
									"text" : "fftparams 128 128"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-45",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 235.0, 160.0, 18.0 ],
									"text" : "fftparams 4096 2048 0 hann"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-46",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 195.0, 160.0, 18.0 ],
									"text" : "fftparams 8192 2048 0 hann"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-60",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 215.0, 173.0, 18.0 ],
									"text" : "fftparams 65536 1024 0 flattop"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-61",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 135.0, 166.0, 18.0 ],
									"text" : "fftparams 8192 1024 0 flattop"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-62",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 175.0, 219.0, 18.0 ],
									"text" : "fftparams 8192 1024 0 blackman-harris"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-63",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 848.0, 155.0, 183.0, 18.0 ],
									"text" : "fftparams 8192 1024 0 rectangle"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-118",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 24.0, 14.0, 654.0, 20.0 ],
									"text" : "Arguments to fftparams are fft size (should be a power of 2) / hopsize / window size (0 is same as fftsize) / window type."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 832.0, 268.0, 108.0, 20.0 ],
									"text" : "s descriptors_help"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 114.0, 841.5, 114.0 ],
									"source" : [ "obj-35", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 75.0, 841.5, 75.0 ],
									"source" : [ "obj-36", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 93.0, 841.5, 93.0 ],
									"source" : [ "obj-38", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 135.0, 841.5, 135.0 ],
									"source" : [ "obj-39", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 255.0, 841.5, 255.0 ],
									"source" : [ "obj-45", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 213.0, 841.5, 213.0 ],
									"source" : [ "obj-46", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 234.0, 841.5, 234.0 ],
									"source" : [ "obj-60", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 153.0, 841.5, 153.0 ],
									"source" : [ "obj-61", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 195.0, 841.5, 195.0 ],
									"source" : [ "obj-62", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-6", 0 ],
									"midpoints" : [ 857.5, 174.0, 841.5, 174.0 ],
									"source" : [ "obj-63", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 531.0, 200.0, 98.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"tags" : ""
					}
,
					"text" : "p fft_parameters"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-46",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 583.5, 74.0, 22.0 ],
					"text" : "prepend set"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-45",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 19.0, 619.5, 393.0, 22.0 ]
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-39",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 230.0, 201.0, 262.0, 20.0 ],
					"text" : "set the fft parameters (see subpatch for details)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-43",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 155.0, 174.0, 370.0, 20.0 ],
					"text" : "set the energy threshold in dB - frames below this level are ignored"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-42",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 35.0, 554.0, 190.0, 20.0 ],
					"text" : "list of calculated descriptor values"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-40",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 69.0, 235.0, 447.0, 20.0 ],
					"text" : "set the descriptors and statistical calculations to perform (see subpatch for details)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-8",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 221.0, 350.0, 260.0, 20.0 ],
					"text" : "analyse the entire named (i)buffer~ (channel 1)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-7",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 226.0, 375.0, 203.0, 20.0 ],
					"text" : "you can spell it this way if you prefer"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-3",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 352.0, 167.0, 22.0 ],
					"text" : "analyse descriptors_help_buf"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 261.0, 465.0, 22.0 ],
					"text" : "descriptors mkl 200 1000 1 0 8 crossings_above 200 threshold 0.25 abs masktime 80"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-13",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 176.0, 101.0, 22.0 ],
					"text" : "energythresh -60"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-14",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 309.0, 309.0, 22.0 ],
					"text" : "descriptors rms crossings_above 200 threshold -10. abs"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-16",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 285.0, 398.0, 22.0 ],
					"text" : "descriptors log_brightness crossings_below 200 threshold 0.7 mean_mul"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-100",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 203.0, 174.0, 22.0 ],
					"text" : "fftparams 4096 256 4096 hann"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-101",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 43.0, 378.0, 167.0, 22.0 ],
					"text" : "analyze descriptors_help_buf"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-102",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "list" ],
					"patching_rect" : [ 19.0, 486.0, 76.0, 22.0 ],
					"text" : "descriptors~"
				}

			}
, 			{
				"box" : 				{
					"angle" : 0.0,
					"background" : 1,
					"grad1" : [ 0.88, 0.98, 0.78, 1.0 ],
					"grad2" : [ 0.9, 0.9, 0.9, 1.0 ],
					"id" : "obj-36",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"varname" : "autohelp_top_panel"
				}

			}
, 			{
				"box" : 				{
					"angle" : 0.0,
					"background" : 1,
					"bgcolor" : [ 0.85, 0.85, 0.85, 0.75 ],
					"border" : 2,
					"bordercolor" : [ 0.5, 0.5, 0.5, 0.75 ],
					"id" : "obj-81",
					"maxclass" : "panel",
					"mode" : 0,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 707.0, 589.0, 140.0, 50.0 ],
					"varname" : "autohelp_see_panel"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 281.5, 28.5, 281.5 ],
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 228.5, 28.5, 228.5 ],
					"source" : [ "obj-100", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 401.0, 28.5, 401.0 ],
					"source" : [ "obj-101", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-46", 0 ],
					"source" : [ "obj-102", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 173.0, 28.5, 173.0 ],
					"source" : [ "obj-11", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-52", 0 ],
					"source" : [ "obj-12", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 199.0, 28.5, 199.0 ],
					"source" : [ "obj-13", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 330.5, 28.5, 330.5 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 305.5, 28.5, 305.5 ],
					"source" : [ "obj-16", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 372.0, 28.5, 372.0 ],
					"source" : [ "obj-3", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-45", 0 ],
					"source" : [ "obj-46", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 52.5, 431.0, 28.5, 431.0 ],
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-102", 0 ],
					"midpoints" : [ 65.5, 458.5, 28.5, 458.5 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-19", 0 ],
					"hidden" : 1,
					"source" : [ "obj-80", 1 ]
				}

			}
 ],
		"autosave" : 0
	}

}
