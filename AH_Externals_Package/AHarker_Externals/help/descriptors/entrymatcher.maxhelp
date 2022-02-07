{
	"patcher" : 	{
		"fileversion" : 1,
		"appversion" : 		{
			"major" : 8,
			"minor" : 0,
			"revision" : 0,
			"architecture" : "x86",
			"modernui" : 1
		}
,
		"rect" : [ 35.0, 79.0, 928.0, 857.0 ],
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
		"boxes" : [ 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-62",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 535.0, 770.0, 161.0, 22.0 ],
					"presentation_rect" : [ 535.0, 770.0, 161.0, 22.0 ],
					"style" : "",
					"text" : "print LOOKUP_AND_DUMP"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-64",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 266.0, 681.0, 185.0, 20.0 ],
					"presentation_rect" : [ 266.0, 681.0, 185.0, 20.0 ],
					"style" : "",
					"text" : "output all data from llokup output"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-63",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 212.0, 681.0, 41.0, 22.0 ],
					"presentation_rect" : [ 212.0, 681.0, 41.0, 22.0 ],
					"style" : "",
					"text" : "dump"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-3",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 0,
							"architecture" : "x86",
							"modernui" : 1
						}
,
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
									"presentation_rect" : [ 99.0, 63.5, 83.0, 20.0 ],
									"style" : "",
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
									"presentation_rect" : [ 99.0, 90.5, 56.0, 20.0 ],
									"style" : "",
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
									"patching_rect" : [ 99.0, 36.0, 25.0, 25.0 ],
									"presentation_rect" : [ 99.0, 36.0, 25.0, 25.0 ],
									"style" : ""
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
					"patching_rect" : [ 812.0, 754.0, 69.0, 22.0 ],
					"presentation_rect" : [ 812.0, 754.0, 69.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p see_also"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-60",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 375.0, 651.0, 447.0, 20.0 ],
					"presentation_rect" : [ 375.0, 651.0, 447.0, 20.0 ],
					"style" : "",
					"text" : "return up to 1024 matches keeping all matches with a distance value of 0.3 or less"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-18",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 651.0, 107.0, 22.0 ],
					"presentation_rect" : [ 188.0, 651.0, 107.0, 22.0 ],
					"style" : "",
					"text" : "match 1024 1. 0.3"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-67",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 375.0, 628.0, 409.0, 20.0 ],
					"presentation_rect" : [ 375.0, 628.0, 409.0, 20.0 ],
					"style" : "",
					"text" : "only return the top 50% of the found matches and no more than 2 matches"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-66",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 375.0, 605.0, 253.0, 20.0 ],
					"presentation_rect" : [ 375.0, 605.0, 253.0, 20.0 ],
					"style" : "",
					"text" : "only return the top 50% of the found matches"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-65",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 375.0, 582.0, 161.0, 20.0 ],
					"presentation_rect" : [ 375.0, 582.0, 161.0, 20.0 ],
					"style" : "",
					"text" : "only return the closet match"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-61",
					"linecount" : 3,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 185.0, 530.0, 710.0, 47.0 ],
					"presentation_rect" : [ 185.0, 530.0, 710.0, 47.0 ],
					"style" : "",
					"text" : "You can also optionally filter the results when matching. An int specifies the maximum number of matches to return and a float (0-1)  the ratio of found matches to return. These may be specified in either order. The entries returned will be the closest (best) matches. A a third value specificies the largest allowable distance value to match"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-59",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 376.0, 475.0, 499.0, 20.0 ],
					"presentation_rect" : [ 376.0, 475.0, 499.0, 20.0 ],
					"style" : "",
					"text" : "set to match entries where the bar column is within 1000 in either direction of the value 1100"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-58",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 376.0, 452.0, 393.0, 20.0 ],
					"presentation_rect" : [ 376.0, 452.0, 393.0, 20.0 ],
					"style" : "",
					"text" : "set to match entries where the labelcol column is equal to the symbol no"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-57",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 289.0, 628.0, 73.0, 22.0 ],
					"presentation_rect" : [ 289.0, 628.0, 73.0, 22.0 ],
					"style" : "",
					"text" : "match 2 0.5"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-56",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 628.0, 73.0, 22.0 ],
					"presentation_rect" : [ 188.0, 628.0, 73.0, 22.0 ],
					"style" : "",
					"text" : "match 0.5 2"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-55",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 605.0, 63.0, 22.0 ],
					"presentation_rect" : [ 188.0, 605.0, 63.0, 22.0 ],
					"style" : "",
					"text" : "match 0.5"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-54",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 582.0, 53.0, 22.0 ],
					"presentation_rect" : [ 188.0, 582.0, 53.0, 22.0 ],
					"style" : "",
					"text" : "match 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-53",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 475.0, 161.0, 22.0 ],
					"presentation_rect" : [ 188.0, 475.0, 161.0, 22.0 ],
					"style" : "",
					"text" : "matchers bar <-> 1000 1100"
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
					"patching_rect" : [ 376.0, 429.0, 178.0, 20.0 ],
					"presentation_rect" : [ 376.0, 429.0, 178.0, 20.0 ],
					"style" : "",
					"text" : "set to match all entries (default)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-52",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 429.0, 60.0, 22.0 ],
					"presentation_rect" : [ 188.0, 429.0, 60.0, 22.0 ],
					"style" : "",
					"text" : "matchers"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-50",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 301.0, 796.0, 113.0, 20.0 ],
					"presentation_rect" : [ 301.0, 796.0, 113.0, 20.0 ],
					"style" : "",
					"text" : "Matched Distances"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-47",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 126.0, 823.0, 238.0, 20.0 ],
					"presentation_rect" : [ 126.0, 823.0, 238.0, 20.0 ],
					"style" : "",
					"text" : "^-------------- Matching Outputs -------------- ^"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-35",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 183.0, 796.0, 112.0, 20.0 ],
					"presentation_rect" : [ 183.0, 796.0, 112.0, 20.0 ],
					"style" : "",
					"text" : "Matched Identifiers"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-32",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 84.0, 796.0, 98.0, 20.0 ],
					"presentation_rect" : [ 84.0, 796.0, 98.0, 20.0 ],
					"style" : "",
					"text" : "Matched Indices"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-19",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 417.0, 796.0, 141.0, 20.0 ],
					"presentation_rect" : [ 417.0, 796.0, 141.0, 20.0 ],
					"style" : "",
					"text" : "Data from lookup / index"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"hidden" : 1,
					"id" : "obj-49",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 564.0, 737.0, 81.0, 22.0 ],
					"presentation_rect" : [ 564.0, 737.0, 81.0, 22.0 ],
					"style" : "",
					"text" : "loadmess set"
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
					"patching_rect" : [ 340.0, 186.0, 257.0, 20.0 ],
					"presentation_rect" : [ 340.0, 186.0, 257.0, 20.0 ],
					"style" : "",
					"text" : "(try clicking through in turn to demo the object)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-46",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 342.0, 401.0, 225.0, 20.0 ],
					"presentation_rect" : [ 342.0, 401.0, 225.0, 20.0 ],
					"style" : "",
					"text" : "return the data for the specified identifier"
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
					"patching_rect" : [ 188.0, 379.0, 49.0, 22.0 ],
					"presentation_rect" : [ 188.0, 379.0, 49.0, 22.0 ],
					"style" : "",
					"text" : "index 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-44",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 342.0, 379.0, 209.0, 20.0 ],
					"presentation_rect" : [ 342.0, 379.0, 209.0, 20.0 ],
					"style" : "",
					"text" : "return the data for the specified index"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-43",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 281.0, 401.0, 56.0, 22.0 ],
					"presentation_rect" : [ 281.0, 401.0, 56.0, 22.0 ],
					"style" : "",
					"text" : "lookup 3"
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
					"patching_rect" : [ 268.0, 324.142853, 354.0, 20.0 ],
					"presentation_rect" : [ 268.0, 324.142853, 354.0, 20.0 ],
					"style" : "",
					"text" : "if an entry with the given ideniier already exists it will be replaced"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-41",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 268.0, 302.142853, 299.0, 20.0 ],
					"presentation_rect" : [ 268.0, 302.142853, 299.0, 20.0 ],
					"style" : "",
					"text" : "identifiers can be of any data type (here we use an int)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-40",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 307.0, 770.0, 104.0, 22.0 ],
					"presentation_rect" : [ 307.0, 770.0, 104.0, 22.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-39",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 194.0, 770.0, 104.0, 22.0 ],
					"presentation_rect" : [ 194.0, 770.0, 104.0, 22.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-38",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 81.0, 770.0, 104.0, 22.0 ],
					"presentation_rect" : [ 81.0, 770.0, 104.0, 22.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-37",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 420.0, 770.0, 104.0, 22.0 ],
					"presentation_rect" : [ 420.0, 770.0, 104.0, 22.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-34",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 268.0, 258.142853, 599.0, 20.0 ],
					"presentation_rect" : [ 268.0, 258.142853, 599.0, 20.0 ],
					"style" : "",
					"text" : "add an entry - entries are specified by an identifier followed by data for each column in turn (of the correct type)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-33",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 188.0, 711.0, 502.0, 20.0 ],
					"presentation_rect" : [ 188.0, 711.0, 502.0, 20.0 ],
					"style" : "",
					"text" : "optional arguments are max number of entries (default 1) and number of columns (default 1)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-30",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 104.0, 324.142853, 144.0, 22.0 ],
					"presentation_rect" : [ 104.0, 324.142853, 144.0, 22.0 ],
					"style" : "",
					"text" : "entry 3 -20.3 0.45 maybe"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-28",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 88.0, 302.142853, 98.0, 22.0 ],
					"presentation_rect" : [ 88.0, 302.142853, 98.0, 22.0 ],
					"style" : "",
					"text" : "entry 3 0 -0.3 no"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-24",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 72.0, 280.142853, 151.0, 22.0 ],
					"presentation_rect" : [ 72.0, 280.142853, 151.0, 22.0 ],
					"style" : "",
					"text" : "entry entry2 19.3 1900. no"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-31",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 286.0, 503.0, 199.0, 20.0 ],
					"presentation_rect" : [ 286.0, 503.0, 199.0, 20.0 ],
					"style" : "",
					"text" : "return all matches (maximum 1024)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-29",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 268.0, 214.0, 579.0, 20.0 ],
					"presentation_rect" : [ 268.0, 214.0, 579.0, 20.0 ],
					"style" : "",
					"text" : "set which columns take symbols (here third column is symbolic - others are numeric - default is all numeric)"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-27",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 268.0, 236.142853, 603.0, 20.0 ],
					"presentation_rect" : [ 268.0, 236.142853, 603.0, 20.0 ],
					"style" : "",
					"text" : "set the symbolic names of the columns in order (you should not use repeated names, or the symbol \"identifier\") "
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-25",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 266.0, 346.142853, 258.0, 20.0 ],
					"presentation_rect" : [ 266.0, 346.142853, 258.0, 20.0 ],
					"style" : "",
					"text" : "synonyms for clearing all entries and matchers"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"hidden" : 1,
					"id" : "obj-20",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 751.0, 782.0, 55.0, 21.0 ],
					"presentation_rect" : [ 751.0, 782.0, 55.0, 21.0 ],
					"style" : "",
					"text" : "autohelp"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 3,
					"fontname" : "Arial",
					"fontsize" : 20.871338,
					"id" : "obj-21",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"presentation_rect" : [ 10.0, 8.0, 485.0, 30.0 ],
					"style" : "",
					"text" : "entrymatcher",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"varname" : "autohelp_top_title"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 12.754705,
					"id" : "obj-22",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"presentation_rect" : [ 10.0, 36.0, 485.0, 21.0 ],
					"style" : "",
					"text" : "Flexible matching / storage / querying of multidimensional data",
					"textcolor" : [ 0.93, 0.93, 0.97, 1.0 ],
					"varname" : "autohelp_top_digest"
				}

			}
, 			{
				"box" : 				{
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"id" : "obj-23",
					"linecount" : 9,
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 10.0, 57.0, 867.0, 123.0 ],
					"presentation_rect" : [ 10.0, 57.0, 867.0, 123.0 ],
					"style" : "",
					"text" : "entrymatcher is an object for finding the most closely matching items in a scalable N-dimensional space, using variable matching criteria. The original reason for creating the entrymatcher object was to provide a way of matching audio samples using audio features (or descriptors) that had been calculated using the descriptors~ object. Data is added to the object in \"entries\", which are rows of values (ints, floats or symbols) that fill a predetermined number of columns. Columns are named to allow them to be specified by descriptive name, rather than only by index number, making it possible to alter the order and size of data sets easily. Each entry is also specified with an identifier (of any basic max data type), so that it can also be referred to regardless of position in the data set. Various tests may be performed on each column of data, either individually or in combination, as appropriate to the data (exact match, within given distance, greater than, less than etc.) to determine matching entries. Values from specified columns may be queried at will (in the case where each entry represents a sample this is useful in retrieving data once a particular sample has been matched). Matched entries are returned as a set of lists, with the best (closest) matches first. In practice a large number of matching scenarios and data lookup requirements can be satisfied.",
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
					"patching_rect" : [ 316.19931, 22.0, 183.80069, 14.249397 ],
					"presentation_rect" : [ 316.19931, 22.0, 183.80069, 14.249397 ],
					"spacing_x" : 0.0,
					"spacing_y" : 0.0,
					"style" : "",
					"text" : "open entrymatcher reference",
					"textcolor" : [ 0.309, 0.395, 0.524, 1.0 ],
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
					"fontsize" : 11.595187,
					"id" : "obj-79",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 756.0, 786.0, 100.0, 19.0 ],
					"presentation_rect" : [ 756.0, 786.0, 100.0, 19.0 ],
					"style" : "",
					"text" : "See Also:",
					"varname" : "autohelp_see_title"
				}

			}
, 			{
				"box" : 				{
					"allowdrag" : 0,
					"fontface" : 0,
					"fontname" : "Arial",
					"fontsize" : 11.595187,
					"id" : "obj-80",
					"items" : [ "(Objects:)", ",", "descriptors~", ",", "descriptorsrt~", ",", "entrymatcher~" ],
					"maxclass" : "umenu",
					"numinlets" : 1,
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"parameter_enable" : 0,
					"patching_rect" : [ 756.0, 806.0, 130.0, 21.0 ],
					"presentation_rect" : [ 756.0, 806.0, 130.0, 21.0 ],
					"style" : "",
					"varname" : "autohelp_see_menu"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"id" : "obj-17",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 16.0, 430.0, 129.0, 23.0 ],
					"presentation_rect" : [ 16.0, 430.0, 129.0, 23.0 ],
					"style" : "",
					"text" : "Perform matching"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"id" : "obj-16",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 16.0, 379.0, 109.0, 23.0 ],
					"presentation_rect" : [ 16.0, 379.0, 109.0, 23.0 ],
					"style" : "",
					"text" : "Query the data"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial Black",
					"fontsize" : 12.0,
					"id" : "obj-15",
					"maxclass" : "comment",
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 16.0, 184.0, 316.0, 23.0 ],
					"presentation_rect" : [ 16.0, 184.0, 316.0, 23.0 ],
					"style" : "",
					"text" : "Set / change the contents of the entrymatcher"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-13",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 0,
							"architecture" : "x86",
							"modernui" : 1
						}
,
						"rect" : [ 305.0, 138.0, 561.0, 437.0 ],
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
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-15",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 202.0, 189.199997, 298.0, 20.0 ],
									"presentation_rect" : [ 202.0, 189.199997, 298.0, 20.0 ],
									"style" : "",
									"text" : "the symbol \"identifier\" returns the identifier of the entry"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-14",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 38.0, 189.199997, 123.0, 18.0 ],
									"presentation_rect" : [ 38.0, 189.199997, 123.0, 18.0 ],
									"style" : "",
									"text" : "index 2 identifier col1"
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
									"patching_rect" : [ 15.0, 403.0, 135.0, 20.0 ],
									"presentation_rect" : [ 15.0, 403.0, 135.0, 20.0 ],
									"style" : "",
									"text" : "Data from looup / index"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-13",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 202.0, 121.199997, 330.0, 20.0 ],
									"presentation_rect" : [ 202.0, 121.199997, 330.0, 20.0 ],
									"style" : "",
									"text" : "thus you can loopup specific data that you are interested in"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-11",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 39.0, 155.199997, 143.0, 18.0 ],
									"presentation_rect" : [ 39.0, 155.199997, 143.0, 18.0 ],
									"style" : "",
									"text" : "lookup entry2 col1 3 col5"
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
									"patching_rect" : [ 277.0, 239.0, 255.0, 20.0 ],
									"presentation_rect" : [ 277.0, 239.0, 255.0, 20.0 ],
									"style" : "",
									"text" : "(populates the object with some labelled data)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-6",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 202.0, 19.0, 337.0, 20.0 ],
									"presentation_rect" : [ 202.0, 19.0, 337.0, 20.0 ],
									"style" : "",
									"text" : "simply specifying an index or identifier returns the entire entry"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-5",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 42.0, 241.0, 228.0, 20.0 ],
									"presentation_rect" : [ 42.0, 241.0, 228.0, 20.0 ],
									"style" : "",
									"text" : "loadmess names col1 col2 col3 col4 col5"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-4",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 78.0, 313.0, 237.0, 20.0 ],
									"presentation_rect" : [ 78.0, 313.0, 237.0, 20.0 ],
									"style" : "",
									"text" : "loadmess entry entry3 3 -12 23.4 5.6 -9.34"
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
									"patching_rect" : [ 66.0, 289.0, 239.0, 20.0 ],
									"presentation_rect" : [ 66.0, 289.0, 239.0, 20.0 ],
									"style" : "",
									"text" : "loadmess entry entry2 2 3000 45 67.8 91.2"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-2",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 54.0, 265.0, 195.0, 20.0 ],
									"presentation_rect" : [ 54.0, 265.0, 195.0, 20.0 ],
									"style" : "",
									"text" : "loadmess entry entry1 1 12 19 300"
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
									"patching_rect" : [ 17.0, 377.0, 117.0, 18.0 ],
									"presentation_rect" : [ 17.0, 377.0, 117.0, 18.0 ],
									"style" : "",
									"text" : "1 12 19 300 0"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-46",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 202.0, 155.199997, 225.0, 20.0 ],
									"presentation_rect" : [ 202.0, 155.199997, 225.0, 20.0 ],
									"style" : "",
									"text" : "you may use column names, or indices"
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
									"patching_rect" : [ 39.0, 19.0, 49.0, 18.0 ],
									"presentation_rect" : [ 39.0, 19.0, 49.0, 18.0 ],
									"style" : "",
									"text" : "index 1"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-44",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 202.0, 87.199997, 350.0, 20.0 ],
									"presentation_rect" : [ 202.0, 87.199997, 350.0, 20.0 ],
									"style" : "",
									"text" : "you can also specify which columns to return and in which order"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-43",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 39.0, 121.199997, 159.0, 18.0 ],
									"presentation_rect" : [ 39.0, 121.199997, 159.0, 18.0 ],
									"style" : "",
									"text" : "lookup entry3 col1 col5 col2"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-1",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "list", "", "list", "list" ],
									"patching_rect" : [ 20.0, 345.0, 114.0, 20.0 ],
									"presentation_rect" : [ 20.0, 345.0, 114.0, 20.0 ],
									"saved_object_attributes" : 									{
										"embed" : 0,
										"parameter_enable" : 0
									}
,
									"style" : "",
									"text" : "entrymatcher 5 5"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-8",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 39.0, 53.200001, 83.0, 18.0 ],
									"presentation_rect" : [ 39.0, 53.200001, 83.0, 18.0 ],
									"style" : "",
									"text" : "lookup entry2"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-9",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 39.0, 87.199997, 100.0, 18.0 ],
									"presentation_rect" : [ 39.0, 87.199997, 100.0, 18.0 ],
									"style" : "",
									"text" : "index 2 col2 col4"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-39", 1 ],
									"source" : [ "obj-1", 3 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 48.5, 175.0, 29.5, 175.0 ],
									"source" : [ "obj-11", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 47.5, 213.0, 29.5, 213.0 ],
									"source" : [ "obj-14", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-2", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-3", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-4", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 48.5, 138.0, 29.5, 138.0 ],
									"source" : [ "obj-43", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 48.5, 42.0, 29.5, 42.0 ],
									"source" : [ "obj-45", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"source" : [ "obj-5", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 48.5, 69.0, 29.5, 69.0 ],
									"source" : [ "obj-8", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-1", 0 ],
									"midpoints" : [ 48.5, 105.0, 29.5, 105.0 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 24.0, 406.0, 118.0, 22.0 ],
					"presentation_rect" : [ 24.0, 406.0, 118.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p index_and_lookup"
				}

			}
, 			{
				"box" : 				{
					"color" : [ 1.0, 0.0, 0.0, 1.0 ],
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-12",
					"maxclass" : "newobj",
					"numinlets" : 0,
					"numoutlets" : 0,
					"patcher" : 					{
						"fileversion" : 1,
						"appversion" : 						{
							"major" : 8,
							"minor" : 0,
							"revision" : 0,
							"architecture" : "x86",
							"modernui" : 1
						}
,
						"rect" : [ 95.0, 87.0, 1002.0, 653.0 ],
						"bglocked" : 0,
						"openinpresentation" : 0,
						"default_fontsize" : 12.0,
						"default_fontface" : 0,
						"default_fontname" : "Arial Bold",
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
						"boxes" : [ 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-33",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 535.0, 275.0, 455.0, 47.0 ],
									"presentation_rect" : [ 535.0, 275.0, 455.0, 47.0 ],
									"style" : "",
									"text" : "will match any entry where the \"height\" column is less than 300 with the results ranked according to absolute distance between the width column and the value 100 (values closer to 100 match better - ie. have smaller distance values)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-34",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 535.0, 248.0, 270.0, 22.0 ],
									"presentation_rect" : [ 535.0, 248.0, 270.0, 22.0 ],
									"style" : "",
									"text" : "matchers height lessthan 300 width distance 100"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-26",
									"linecount" : 3,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 535.0, 196.0, 454.0, 47.0 ],
									"presentation_rect" : [ 535.0, 196.0, 454.0, 47.0 ],
									"style" : "",
									"text" : "will match any entry where the \"amp\" column is between -20 and -10 (within 5 of -15) and the \"freq\" column is between 50 and 200 or between 450 and 1800 (within a ratio of 2 in either direction of either 100 or 900)"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-32",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 535.0, 174.0, 250.0, 22.0 ],
									"presentation_rect" : [ 535.0, 174.0, 250.0, 22.0 ],
									"style" : "",
									"text" : "matchers amp <-> 5 -15 freq </> 2. 100. 900."
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
									"patching_rect" : [ 535.0, 147.0, 454.0, 20.0 ],
									"presentation_rect" : [ 535.0, 147.0, 454.0, 20.0 ],
									"style" : "",
									"text" : "will match any entry where the column named \"col1\" is either \"yes\", \"no\" or \"maybe\""
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-18",
									"maxclass" : "message",
									"numinlets" : 2,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 535.0, 125.0, 181.0, 22.0 ],
									"presentation_rect" : [ 535.0, 125.0, 181.0, 22.0 ],
									"style" : "",
									"text" : "matchers col1 == yes no maybe"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-38",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 364.0, 743.0, 20.0 ],
									"presentation_rect" : [ 20.0, 364.0, 743.0, 20.0 ],
									"style" : "",
									"text" : "S is the given scaling value, which is expected to be positive, and greater than 1. in the case of the ratio matches (otherwise 1 / S is used)."
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
									"patching_rect" : [ 75.0, 255.5, 301.0, 20.0 ],
									"presentation_rect" : [ 75.0, 255.5, 301.0, 20.0 ],
									"style" : "",
									"text" : "V is the value of an entry. T is the closest target value. "
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-28",
									"linecount" : 2,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 608.5, 928.0, 33.0 ],
									"presentation_rect" : [ 20.0, 608.5, 928.0, 33.0 ],
									"style" : "",
									"text" : "Tests are performed in the given order. If the test does not match an entry no further tests are performed on that entry. Thus, if possible it is most efficicient to perform  tests so as to reject the greatest number of entries first, the second greatest number second and so on."
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-30",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 586.0, 133.0, 23.0 ],
									"presentation_rect" : [ 20.0, 586.0, 133.0, 23.0 ],
									"style" : "",
									"text" : "Order of Matching:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-27",
									"linecount" : 4,
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 21.0, 513.5, 923.0, 60.0 ],
									"presentation_rect" : [ 21.0, 513.5, 923.0, 60.0 ],
									"style" : "",
									"text" : "Matches are ranked by their distance (D) from the target values. Each entry starts with a nominal distance of zero from the targets, which means it matches the target exactly (larger values mean the entry is further away from the targets).For each test with a distance calculation the value is added to the total distance for that entry. If there are multiple target values only the lowest rated match is used. The final distance is then calculated as sqrt(D). Thus, the distance, scale and within tests produce scaled euclidean distances (note the squaring of the distance calculations). The ratio tests are more suitable for parameters that are perceived logarithmically (such as frequency),"
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
									"patching_rect" : [ 20.0, 492.0, 151.0, 23.0 ],
									"presentation_rect" : [ 20.0, 492.0, 151.0, 23.0 ],
									"style" : "",
									"text" : "Distance Calculation:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-19",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 532.0, 91.0, 81.0, 23.0 ],
									"presentation_rect" : [ 532.0, 91.0, 81.0, 23.0 ],
									"style" : "",
									"text" : "Examples:"
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
									"patching_rect" : [ 16.0, 35.0, 554.0, 20.0 ],
									"presentation_rect" : [ 16.0, 35.0, 554.0, 20.0 ],
									"style" : "",
									"text" : "For each test you may specify multiple target values. If one of these matches then the entry will match."
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
									"patching_rect" : [ 16.0, 14.0, 257.0, 20.0 ],
									"presentation_rect" : [ 16.0, 14.0, 257.0, 20.0 ],
									"style" : "",
									"text" : "Matchers are set using the matchers message"
								}

							}
, 							{
								"box" : 								{
									"coll_data" : 									{
										"count" : 5,
										"data" : [ 											{
												"key" : "Symbol",
												"value" : [ "Name", "Matches", "Distance Calculation" ]
											}
, 											{
												"key" : "%",
												"value" : [ "scale", "any value", "((V - T) / S) ^ 2" ]
											}
, 											{
												"key" : "%%",
												"value" : [ "scale_ratio", "any value", "((MAX(T / V, V / T) - 1.) / (S - 1.)) ^ 2" ]
											}
, 											{
												"key" : "<->",
												"value" : [ "within", "values within the scaling value in either direction of the closest target", "((V - T) / S) ^ 2" ]
											}
, 											{
												"key" : "</>",
												"value" : [ "within_ratio", "values within the specified ratio in either direction of the closest target", "((MAX(T / V, V / T) - 1.) / (S - 1.)) ^ 2" ]
											}
 ]
									}
,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-22",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "", "", "", "" ],
									"patching_rect" : [ 616.0, 60.0, 205.0, 22.0 ],
									"presentation_rect" : [ 616.0, 60.0, 205.0, 22.0 ],
									"saved_object_attributes" : 									{
										"embed" : 1
									}
,
									"style" : "",
									"text" : "coll __energymatcher_help_display3"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-20",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 119.0, 342.0, 267.0, 22.0 ],
									"presentation_rect" : [ 119.0, 342.0, 267.0, 22.0 ],
									"style" : "",
									"text" : "loadmess refer __energymatcher_help_display3"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"coldef" : [ [ 0, 84, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 1, 84, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 2, 388, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 3, 209, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ] ],
									"colhead" : 1,
									"cols" : 4,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"gridlinecolor" : [ 0.501961, 0.501961, 0.501961, 1.0 ],
									"headercolor" : [ 0.792157, 0.815686, 1.0, 1.0 ],
									"hscroll" : 0,
									"id" : "obj-21",
									"just" : 1,
									"maxclass" : "jit.cellblock",
									"numinlets" : 2,
									"numoutlets" : 4,
									"outlettype" : [ "list", "", "", "" ],
									"patching_rect" : [ 20.0, 388.0, 770.0, 94.0 ],
									"presentation_rect" : [ 20.0, 388.0, 770.0, 94.0 ],
									"rowhead" : 1,
									"rows" : 5,
									"selmode" : 0,
									"vscroll" : 0
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-13",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 1,
									"outlettype" : [ "" ],
									"patching_rect" : [ 118.0, 234.0, 267.0, 22.0 ],
									"presentation_rect" : [ 118.0, 234.0, 267.0, 22.0 ],
									"style" : "",
									"text" : "loadmess refer __energymatcher_help_display2"
								}

							}
, 							{
								"box" : 								{
									"coll_data" : 									{
										"count" : 3,
										"data" : [ 											{
												"key" : "Symbol",
												"value" : [ "Name", "Matches", "Distance Calculation" ]
											}
, 											{
												"key" : "-",
												"value" : [ "distance", "any value", "(V - T) ^ 2" ]
											}
, 											{
												"key" : "/",
												"value" : [ "distance_ratio", "any value", "(MAX(T / V, V / T) - 1.) ^ 2" ]
											}
 ]
									}
,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-12",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "", "", "", "" ],
									"patching_rect" : [ 616.0, 36.0, 205.0, 22.0 ],
									"presentation_rect" : [ 616.0, 36.0, 205.0, 22.0 ],
									"saved_object_attributes" : 									{
										"embed" : 1
									}
,
									"style" : "",
									"text" : "coll __energymatcher_help_display2"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"coldef" : [ [ 0, 84, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 2, 144, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 3, 176, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 1, 101, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ] ],
									"colhead" : 1,
									"cols" : 4,
									"fontface" : 0,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"gridlinecolor" : [ 0.501961, 0.501961, 0.501961, 1.0 ],
									"headercolor" : [ 0.792157, 0.815686, 1.0, 1.0 ],
									"hscroll" : 0,
									"id" : "obj-11",
									"just" : 1,
									"maxclass" : "jit.cellblock",
									"numinlets" : 2,
									"numoutlets" : 4,
									"outlettype" : [ "list", "", "", "" ],
									"patching_rect" : [ 20.0, 280.0, 511.0, 55.0 ],
									"presentation_rect" : [ 20.0, 280.0, 511.0, 55.0 ],
									"rowhead" : 1,
									"rows" : 3,
									"selmode" : 0,
									"vscroll" : 0
								}

							}
, 							{
								"box" : 								{
									"coll_data" : 									{
										"count" : 6,
										"data" : [ 											{
												"key" : "Symbol",
												"value" : [ "Name", "Matches" ]
											}
, 											{
												"key" : "==",
												"value" : [ "match", "values exactly equal to one of the target values" ]
											}
, 											{
												"key" : "<",
												"value" : [ "less", "values less than one of the target values" ]
											}
, 											{
												"key" : ">",
												"value" : [ "greater", "values greater than one of the target values" ]
											}
, 											{
												"key" : "<=",
												"value" : [ "lesseq", "values less than or equal to one of the target values" ]
											}
, 											{
												"key" : ">=",
												"value" : [ "greatereq", "values greater than or equal to one of the target values" ]
											}
 ]
									}
,
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"hidden" : 1,
									"id" : "obj-6",
									"maxclass" : "newobj",
									"numinlets" : 1,
									"numoutlets" : 4,
									"outlettype" : [ "", "", "", "" ],
									"patching_rect" : [ 616.0, 10.0, 199.0, 22.0 ],
									"presentation_rect" : [ 616.0, 10.0, 199.0, 22.0 ],
									"saved_object_attributes" : 									{
										"embed" : 1
									}
,
									"style" : "",
									"text" : "coll __energymatcher_help_display"
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
									"patching_rect" : [ 111.0, 91.0, 260.0, 22.0 ],
									"presentation_rect" : [ 111.0, 91.0, 260.0, 22.0 ],
									"style" : "",
									"text" : "loadmess refer __energymatcher_help_display"
								}

							}
, 							{
								"box" : 								{
									"border" : 0,
									"coldef" : [ [ 0, 86, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 1, 85, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ], [ 2, 312, 1, 0.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0.0, 1.0, -1, -1, -1 ] ],
									"colhead" : 1,
									"cols" : 3,
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
									"patching_rect" : [ 20.0, 119.0, 489.0, 112.0 ],
									"presentation_rect" : [ 20.0, 119.0, 489.0, 112.0 ],
									"rowhead" : 1,
									"rows" : 6,
									"selmode" : 0,
									"vscroll" : 0
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-24",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 342.0, 402.0, 23.0 ],
									"presentation_rect" : [ 20.0, 342.0, 402.0, 23.0 ],
									"style" : "",
									"text" : "Tests with a scaling value and a distance calculation:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial Black",
									"fontsize" : 12.0,
									"id" : "obj-23",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 20.0, 234.0, 402.0, 23.0 ],
									"presentation_rect" : [ 20.0, 234.0, 402.0, 23.0 ],
									"style" : "",
									"text" : "Tests with no scaling value but with a distance calculation:"
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
									"patching_rect" : [ 20.0, 91.0, 349.0, 23.0 ],
									"presentation_rect" : [ 20.0, 91.0, 349.0, 23.0 ],
									"style" : "",
									"text" : "Tests with no scaling value or distance calculation:"
								}

							}
, 							{
								"box" : 								{
									"fontname" : "Arial",
									"fontsize" : 12.0,
									"id" : "obj-17",
									"maxclass" : "comment",
									"numinlets" : 1,
									"numoutlets" : 0,
									"patching_rect" : [ 16.0, 56.0, 935.0, 20.0 ],
									"presentation_rect" : [ 16.0, 56.0, 935.0, 20.0 ],
									"style" : "",
									"text" : "The structure of the matchers message is: matchers [column symbol or number] [test symbol or name] [scaling value if required] [target values .......] (repeat for multiple tests)"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"destination" : [ "obj-11", 0 ],
									"hidden" : 1,
									"midpoints" : [ 127.5, 260.0, 29.5, 260.0 ],
									"source" : [ "obj-13", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-21", 0 ],
									"hidden" : 1,
									"midpoints" : [ 128.5, 368.0, 29.5, 368.0 ],
									"source" : [ "obj-20", 0 ]
								}

							}
, 							{
								"patchline" : 								{
									"destination" : [ "obj-8", 0 ],
									"hidden" : 1,
									"midpoints" : [ 120.5, 112.5, 29.5, 112.5 ],
									"source" : [ "obj-9", 0 ]
								}

							}
 ]
					}
,
					"patching_rect" : [ 24.0, 459.0, 112.0, 22.0 ],
					"presentation_rect" : [ 24.0, 459.0, 112.0, 22.0 ],
					"saved_object_attributes" : 					{
						"description" : "",
						"digest" : "",
						"fontname" : "Arial Bold",
						"globalpatchername" : "",
						"style" : "",
						"tags" : ""
					}
,
					"style" : "",
					"text" : "p setting_matchers"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-1",
					"maxclass" : "newobj",
					"numinlets" : 1,
					"numoutlets" : 4,
					"outlettype" : [ "list", "", "list", "list" ],
					"patching_rect" : [ 166.0, 737.0, 358.0, 22.0 ],
					"presentation_rect" : [ 166.0, 737.0, 358.0, 22.0 ],
					"saved_object_attributes" : 					{
						"embed" : 0,
						"parameter_enable" : 0
					}
,
					"style" : "",
					"text" : "entrymatcher 5 3"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-2",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 226.0, 503.0, 43.0, 22.0 ],
					"presentation_rect" : [ 226.0, 503.0, 43.0, 22.0 ],
					"style" : "",
					"text" : "match"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-4",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 166.0, 346.142853, 37.0, 22.0 ],
					"presentation_rect" : [ 166.0, 346.142853, 37.0, 22.0 ],
					"style" : "",
					"text" : "reset"
				}

			}
, 			{
				"box" : 				{
					"id" : "obj-5",
					"maxclass" : "button",
					"numinlets" : 1,
					"numoutlets" : 1,
					"outlettype" : [ "bang" ],
					"patching_rect" : [ 188.0, 503.0, 20.0, 20.0 ],
					"presentation_rect" : [ 188.0, 503.0, 20.0, 20.0 ],
					"style" : ""
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-6",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 24.0, 214.0, 102.0, 22.0 ],
					"presentation_rect" : [ 24.0, 214.0, 102.0, 22.0 ],
					"style" : "",
					"text" : "labelmodes 0 0 1"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-7",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 56.0, 258.142853, 133.0, 22.0 ],
					"presentation_rect" : [ 56.0, 258.142853, 133.0, 22.0 ],
					"style" : "",
					"text" : "entry entry1 4 900. yes"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-8",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 188.0, 401.0, 83.0, 22.0 ],
					"presentation_rect" : [ 188.0, 401.0, 83.0, 22.0 ],
					"style" : "",
					"text" : "lookup entry2"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-9",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 281.0, 379.0, 49.0, 22.0 ],
					"presentation_rect" : [ 281.0, 379.0, 49.0, 22.0 ],
					"style" : "",
					"text" : "index 2"
				}

			}
, 			{
				"box" : 				{
					"fontname" : "Arial",
					"fontsize" : 12.0,
					"id" : "obj-10",
					"maxclass" : "message",
					"numinlets" : 2,
					"numoutlets" : 1,
					"outlettype" : [ "" ],
					"patching_rect" : [ 40.0, 236.142853, 131.0, 22.0 ],
					"presentation_rect" : [ 40.0, 236.142853, 131.0, 22.0 ],
					"style" : "",
					"text" : "names foo bar labelcol"
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
					"patching_rect" : [ 188.0, 452.0, 138.0, 22.0 ],
					"presentation_rect" : [ 188.0, 452.0, 138.0, 22.0 ],
					"style" : "",
					"text" : "matchers labelcol == no"
				}

			}
, 			{
				"box" : 				{
					"angle" : 0.0,
					"background" : 1,
					"grad1" : [ 0.27, 0.35, 0.47, 1.0 ],
					"grad2" : [ 0.85, 0.85, 0.85, 1.0 ],
					"id" : "obj-36",
					"maxclass" : "panel",
					"mode" : 1,
					"numinlets" : 1,
					"numoutlets" : 0,
					"patching_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"presentation_rect" : [ 5.0, 5.0, 495.0, 52.0 ],
					"proportion" : 0.39,
					"style" : "",
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
					"patching_rect" : [ 751.0, 782.0, 140.0, 50.0 ],
					"presentation_rect" : [ 751.0, 782.0, 140.0, 50.0 ],
					"proportion" : 0.39,
					"style" : "",
					"varname" : "autohelp_see_panel"
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
					"patching_rect" : [ 212.0, 346.142853, 37.0, 22.0 ],
					"presentation_rect" : [ 212.0, 346.142853, 37.0, 22.0 ],
					"style" : "",
					"text" : "clear"
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"destination" : [ "obj-37", 1 ],
					"order" : 1,
					"source" : [ "obj-1", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-38", 1 ],
					"source" : [ "obj-1", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 1 ],
					"source" : [ "obj-1", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 1 ],
					"source" : [ "obj-1", 2 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-62", 0 ],
					"midpoints" : [ 514.5, 762.0, 544.5, 762.0 ],
					"order" : 0,
					"source" : [ "obj-1", 3 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 49.5, 368.5, 175.5, 368.5 ],
					"source" : [ "obj-10", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 474.700012, 175.5, 474.700012 ],
					"source" : [ "obj-11", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 221.5, 368.0, 175.5, 368.0 ],
					"source" : [ "obj-14", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 674.0, 175.5, 674.0 ],
					"source" : [ "obj-18", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 235.5, 525.5, 175.5, 525.5 ],
					"source" : [ "obj-2", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 81.5, 368.75, 175.5, 368.75 ],
					"source" : [ "obj-24", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 97.5, 368.25, 175.5, 368.25 ],
					"source" : [ "obj-28", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 113.5, 368.75, 175.5, 368.75 ],
					"source" : [ "obj-30", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 175.5, 368.5, 175.5, 368.5 ],
					"source" : [ "obj-4", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 290.5, 422.0, 175.5, 422.0 ],
					"source" : [ "obj-43", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 399.0, 175.5, 399.0 ],
					"source" : [ "obj-45", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-37", 0 ],
					"hidden" : 1,
					"midpoints" : [ 573.5, 763.0, 429.5, 763.0 ],
					"order" : 0,
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-38", 0 ],
					"hidden" : 1,
					"midpoints" : [ 573.5, 763.0, 90.5, 763.0 ],
					"order" : 3,
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-39", 0 ],
					"hidden" : 1,
					"midpoints" : [ 573.5, 763.0, 203.5, 763.0 ],
					"order" : 2,
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-40", 0 ],
					"hidden" : 1,
					"midpoints" : [ 573.5, 763.0, 316.5, 763.0 ],
					"order" : 1,
					"source" : [ "obj-49", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 525.700012, 175.5, 525.700012 ],
					"source" : [ "obj-5", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 451.5, 175.5, 451.5 ],
					"source" : [ "obj-52", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 498.5, 175.5, 498.5 ],
					"source" : [ "obj-53", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 600.5, 175.5, 600.5 ],
					"source" : [ "obj-54", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 625.5, 175.5, 625.5 ],
					"source" : [ "obj-55", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 648.5, 175.5, 648.5 ],
					"source" : [ "obj-56", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 298.5, 648.5, 175.5, 648.5 ],
					"source" : [ "obj-57", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 33.5, 368.0, 175.5, 368.0 ],
					"source" : [ "obj-6", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 221.5, 705.0, 175.5, 705.0 ],
					"source" : [ "obj-63", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"midpoints" : [ 65.5, 368.0, 175.5, 368.0 ],
					"source" : [ "obj-7", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 197.5, 422.200012, 175.5, 422.200012 ],
					"source" : [ "obj-8", 0 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-3", 0 ],
					"hidden" : 1,
					"source" : [ "obj-80", 1 ]
				}

			}
, 			{
				"patchline" : 				{
					"destination" : [ "obj-1", 0 ],
					"hidden" : 1,
					"midpoints" : [ 290.5, 399.0, 175.5, 399.0 ],
					"source" : [ "obj-9", 0 ]
				}

			}
 ],
		"dependency_cache" : [ 			{
				"name" : "entrymatcher.mxo",
				"type" : "iLaX"
			}
 ],
		"autosave" : 0
	}

}
