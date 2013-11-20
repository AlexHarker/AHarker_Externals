{
	"patcher" : 	{
		"fileversion" : 1,
		"rect" : [ 74.0, 75.0, 818.0, 513.0 ],
		"bglocked" : 0,
		"defrect" : [ 74.0, 75.0, 818.0, 513.0 ],
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
					"text" : "ensure that we catch every trigger immediately (set phasor~  output to 0 when it is 1).",
					"linecount" : 2,
					"patching_rect" : [ 31.0, 175.0, 264.0, 34.0 ],
					"id" : "obj-16",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~",
					"patching_rect" : [ 304.0, 200.0, 32.5, 20.0 ],
					"id" : "obj-10",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "!=~ 1",
					"patching_rect" : [ 318.0, 174.0, 38.0, 20.0 ],
					"id" : "obj-8",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "see here for subsample accuracy",
					"frgb" : [ 0.0, 0.372549, 1.0, 1.0 ],
					"patching_rect" : [ 451.0, 200.0, 187.0, 20.0 ],
					"id" : "obj-9",
					"numinlets" : 1,
					"fontname" : "Arial",
					"textcolor" : [ 0.0, 0.372549, 1.0, 1.0 ],
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "p subsample_offset",
					"patching_rect" : [ 472.0, 237.333328, 114.0, 20.0 ],
					"id" : "obj-7",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"color" : [ 0.0, 0.396078, 1.0, 1.0 ],
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0,
					"patcher" : 					{
						"fileversion" : 1,
						"rect" : [ 80.0, 110.0, 700.0, 520.0 ],
						"bglocked" : 0,
						"defrect" : [ 80.0, 110.0, 700.0, 520.0 ],
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
									"text" : "look in here to see how this calculation works in terms of algebra",
									"patching_rect" : [ 244.0, 223.0, 353.0, 20.0 ],
									"id" : "obj-11",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "p maths",
									"patching_rect" : [ 183.0, 223.0, 53.0, 20.0 ],
									"id" : "obj-7",
									"numinlets" : 0,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"color" : [ 0.0, 0.396078, 1.0, 1.0 ],
									"fontsize" : 12.0,
									"patcher" : 									{
										"fileversion" : 1,
										"rect" : [ 50.0, 94.0, 807.0, 365.0 ],
										"bglocked" : 0,
										"defrect" : [ 50.0, 94.0, 807.0, 365.0 ],
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
													"text" : "= 1 - ((d - v) / d)",
													"patching_rect" : [ 145.0, 327.846161, 94.0, 20.0 ],
													"id" : "obj-49",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "subsample offset = 1 - x0",
													"patching_rect" : [ 51.0, 305.846161, 150.0, 20.0 ],
													"id" : "obj-47",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "and d is the increment or wrapped difference between the last two samples",
													"patching_rect" : [ 51.0, 83.846161, 409.0, 20.0 ],
													"id" : "obj-45",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "To found out how late the trigger is we subtract that value from one : ",
													"patching_rect" : [ 13.0, 279.846161, 470.0, 23.0 ],
													"id" : "obj-43",
													"numinlets" : 1,
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "x0 = (d - v) / d",
													"patching_rect" : [ 51.0, 253.846161, 85.0, 20.0 ],
													"id" : "obj-41",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "dx0 = d - v",
													"patching_rect" : [ 51.0, 231.846161, 67.0, 20.0 ],
													"id" : "obj-38",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "0 = dx0 + v - d",
													"patching_rect" : [ 51.0, 209.846161, 87.0, 20.0 ],
													"id" : "obj-36",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : " So when y = 0 :",
													"patching_rect" : [ 13.0, 183.846161, 114.0, 23.0 ],
													"id" : "obj-34",
													"numinlets" : 1,
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "y = dx + v - d",
													"patching_rect" : [ 51.0, 157.846161, 80.0, 20.0 ],
													"id" : "obj-32",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "y = ax + c (a = d, c = v - d)",
													"patching_rect" : [ 51.0, 135.846161, 151.0, 20.0 ],
													"id" : "obj-30",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "So, the line has a gradient of d and an offset of v - d :",
													"patching_rect" : [ 13.0, 109.846161, 377.0, 23.0 ],
													"id" : "obj-28",
													"numinlets" : 1,
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "where v is the current value of the phasor~",
													"patching_rect" : [ 51.0, 61.846161, 238.0, 20.0 ],
													"id" : "obj-26",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "The line goes through the points (0, v - d) and (1, v)",
													"patching_rect" : [ 51.0, 39.846161, 284.0, 20.0 ],
													"id" : "obj-24",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "First we model the phasor~ signal as a straight line between two samples on the x axis (0 and 1) :",
													"patching_rect" : [ 13.0, 13.0, 654.0, 23.0 ],
													"id" : "obj-22",
													"numinlets" : 1,
													"fontname" : "Arial Black",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "1 - x0",
													"patching_rect" : [ 670.0, 247.0, 41.0, 20.0 ],
													"id" : "obj-20",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "1",
													"patching_rect" : [ 722.0, 267.0, 19.0, 20.0 ],
													"id" : "obj-18",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "0",
													"patching_rect" : [ 521.0, 264.0, 19.0, 20.0 ],
													"id" : "obj-16",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "x0 = (d - v) / d",
													"patching_rect" : [ 554.0, 247.0, 85.0, 20.0 ],
													"id" : "obj-14",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "(v)",
													"patching_rect" : [ 486.0, 143.0, 24.0, 20.0 ],
													"id" : "obj-12",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "1",
													"patching_rect" : [ 489.0, 89.0, 19.0, 20.0 ],
													"id" : "obj-10",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "0",
													"patching_rect" : [ 489.0, 168.0, 19.0, 20.0 ],
													"id" : "obj-8",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "v - d",
													"patching_rect" : [ 479.0, 224.0, 34.0, 20.0 ],
													"id" : "obj-6",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "d",
													"patching_rect" : [ 782.0, 180.0, 19.0, 20.0 ],
													"id" : "obj-5",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "comment",
													"text" : "v",
													"patching_rect" : [ 737.0, 154.0, 19.0, 20.0 ],
													"id" : "obj-4",
													"numinlets" : 1,
													"fontname" : "Arial",
													"numoutlets" : 0,
													"fontsize" : 12.0
												}

											}
, 											{
												"box" : 												{
													"maxclass" : "fpic",
													"embed" : 1,
													"patching_rect" : [ 524.0, 96.0, 252.0, 148.0 ],
													"id" : "obj-2",
													"numinlets" : 1,
													"numoutlets" : 0,
													"data" : [ 4488, "", "IBkSG0fBZn....PCIgDQRA...vO....kHX....vsLPDT....DLmPIQEBHf.B7g.YHB..Q7SRDEDU3wY6c2GbTUueGG+8l.DgDBjHf.BVhAaCkJvDQkLEw8RHkVPdxzFI.lBA1rqV6nLs5zpVb45boJsido1oM6tgXBMBkmDQdLhODBZUq7fQnbo7vkTBBHACPBARBvd5ebH4lk8jPBY28r64780L6H696.620jO4W9dd32ADBgPHDBgPHDFDV.FOPQ.t02RQHDAKCB3kANNfRqdzW8rnDBQfSz.SCXi.2.eC5M+Hacq5DBQ.Qx.+Rfyf1g7V+PHDQftGfr.9D.u3ev1KP4.+kHAdgHh0HAdWfZP6YvOCvxQcV+lIAdgHBR7.KD36P6P9M.1NvSCzMM96KAdgHBv3.J.ndzNnebf+AT2i7sGIvKDgo5OvKA7aP6P90.VKPFndL16Hj.uPDFIJTCvqEnIzNneHT+AAIbW7uuD3Ehv.+dn9qjeJzNjeYT+U5GaW78QB7BgNoGnty01NvMQ6f9Wi5NoqWAn2SIvKDgX+gnd3xpFsC4mG0C2VJAg2aIvKDg.wh5I9R4ncH+lndhyjEP2Ch0gD3EhfnGA3eGnVzNnWIpmJrCMDUORfWHBvRD3E.9AzNj2HpWTK+YntW4Ckj.uPD.XAvJPI.Mf1A8+GTuLU6m9Th.RfWH5Rtef+dfeKZGxuBpK3DiWmpuamD3EhNotg50Z9losuVy2KfCfdqS0XaQB7BQGzCA7q.NKZGx+Yf2C3g0qBrCPB7BQ6nm.yA3yQ6PtWfu.Xt.wnS0XmgD3EBMLZf+EfKh1A8SCrLfjzqB7tjD3EhaoO.1A1GZGxuNvGCLcTWy3hDIAdgo23Adefqh1A8ih5dhef5UAF.IAdgoz8A72B7+h1g7qB7A.+B8p.CRj.uvzHZf+TfMf5udtVA8uG3uFi6Z1tD3EFdIArTT2QaZExuDP9.opWEXHjD3EFRw.7W.TJZuzMq.rGf4i5gdyrPB7BCk+Hf2A3BncH+b.+S.+95UApyj.uHhWb.4B7eg1g7lW5lyDsW5lMSj.uHh0iC3AnNzNn+aAVBpWfKBURfWDQoe.uHpqbqZExa.Xc.+IzwW5lMSj.uHrmEfIA7eh5hGgVA8CBrXf6UmpwHERfWD1Zn.uNpKCTZExqCXk.ooS0WjHIvKBqzcfYArMZ6kt4uAXQnty5DcNRfWDV3O.3sA9IzNjWMvuF06Xph6dRfWna5EPNz1KcydA1Evyf5MsAQWmD3EgboB7ug5sOIsB5mB3MAFlNUeFYRfWDRzWf+JTu3TzJj2DvGBLEB8KcylIRfWDzXA3IA9OP8VarVA8i.7J.CPmpQyFIvKB3FDveGvwP6Pd8.EC7D5UAZhIAdQ.Qz.OEvGQaesluOfmCHdcpFERfWzEkLpKcy+HZGxuHv+JvXzqBT3CIvK5ztGfrA9Lz9ZM2KPY.y6VaqH7gD3EcXiBXE.0f1yleFf+QfgqWEn3NRB7h1U7.4A7eSaesluEfYfbslGIPB7BM8GCTHp6QcsB5GG3UAFrdUfh6JRfWzh9C72.7aP6P90.VMvDQtVyiTIAdStnPcoad8nd1toUPuBTW5lSPmpQQfiD3Mod..m.+encH+x.t.dTcp9DAGRf2DoG.+4.6j19ZM+KAV.pWIaBiGIvahjCZGxOOv+LPJ5WoIBQj.uIRr.0h5WruIvNPcF+tqmEkHjxrE3GJpst9K6.aqyfZknCZq6.KxC4gQ+gWZ+kDMq2Z6r1NaS3Hms9I29gNSQQQIzUJhvNVr769VBi12Kb1ydVJrvBwsa2bpScJs1jEg5hdpVJC0Kk6cSjSn2JvWf5c33xzZCTDlazpY8LB750qxt10tTxLyLU5V25leyrawhEkIMoI07yaqyDTq21eOqAn.YvVYnVuk07KHyvK7gQYF9pqtZJpnhvkKWbhSbB+Fu+8u+L+4Oera2NImbxM+4tsNYwJC0qly3Qceb88D9G5sh5r6MSyY404edrPuQD9L7kUVYJYmc1J8nG8Pyd0mvDlfxpW8pUZrwF84u2sFWKiA09f66s1l9dqmGte4bWFp2ly8dq+aYfLCu31DINCeM0TCqZUqBWtbwQNxQ7a7DRHAxImbvgCGjRJZejkuCyv2LkNv1DNXL.yD0ks8Kh5Y.6KA7QRfW3iHo.+W+0eM4me9rt0sNZngF7a7zRKMra2NOyy7LbO2S6urKXvB7slO0rb4pJhnTas0RIkTB4me9bvCdP+FO93im4Mu4gCGN3ge3GVGpvvaRfWDQXu6cu3xkKVyZVC0We89M9i7HOBNb3fryNahM1X0gJLxfD3EgstxUtBqYMqg7yOe1+92ueiGarwR1YmMO2y8bjZpopCUXjGIvKB6TQEUfKWtnjRJg5pqN+FezidzjWd4wy9rOK8t28VGpvHWRfWDV3ZW6Zrt0sNxO+74a9luwuw6YO6IYkUVX2tcRKM4t+8cKIvKzUG9vGF2tcSwEWLW5RWxuwGwHFA1sambxIGRHAY8UoqRB7hPtFarQ13F2Htb4hxKub+FOlXhgm9oeZb3vASXBSPGpPiKIvKBYN1wNFtc6lhJpHtvEtfeiO7gObxKu7XAKXAzu90OcnBM9j.uHn55W+5r4MuYxO+74y+7O2uSlmt28tyLlwLvtc6jd5o6yI9iHvSB7hfhJqrR73wCqbkqje5m9I+FeXCaXXylMxM2bYfCbf5PEZNIAdQ.yMu4MYqacq3xkKJszRwqWu9LdzQGMScpSEGNbvjm7jIpnhRmpTyKIvK5xN8oOMETPATPAEvO9i+neiOjgLDV3BWHKZQKhgLjgnCUnnYRfWbWwqWuTZokR94mOaaaaiadya5y3QEUTL4IOYra2NO0S8TDczQqSUpn0j.unS4bm6bTXgEhGOdnxJqzuwG3.GH4latXylMF1vFVHu9DsOIvKtiTTT3y9rOCWtbwl27l45W+59LtEKVXhSbh3vgClwLlAcu6xhbb3JIvKZSW3BWf2+8eeb61MG+3G2uw6W+5WKKSTCe3F16.3sdwj.TWwadIfOB0k5pHJxBfgvGs93fGSLwPiM1neayDlvDvtc6jYlYRLwDSnr7BZLfqocMymE.C+NtHVrXgktzkB.KcoKUdtI64sVqC6IjPB73O9iC.SbhSj4Lm4va8VuktWuAxm2NbBzGTCN8gH3aHExL7BNwINAETPA7Ue0Wwd1yd7YrwMtwgCGNHqrxhd1ydpSUXvWGXItpLh7VW5gaaFdIvaR0TSMwl1zlviGOZdJuBpWW5iZTiRGptPuNPf2J2gapCgoj.u.xHiL3S+zOsc2Fyz2KzAWDKcRj2uNe62CuvbHyLyrk+bzQGMSaZSiO9i+XtwMtAJJJ3zoS8q3Be4TuKftJYFdCrCcnCwIO4IYZSaZ9MVs0VKokVZL6YOaxM2b49u+6WGpvvGcvY3iD0tKs1A+aMHhfp5qudkBKrPkwMtwo.nbe228ozTSM0o+2woSmAgpK7Es8cdlHc974Rlg2fPQQgW3EdAJojRn1Zq0mw1vF1fO+J7cDVrXQ5g2XPtQTXDYwhEN5QOpOg8dzidvrl0rHojRpS+umzCuwjLCeDnFZnAMu0Is90udxJqrHkTRAa1rQN4jirTQ0AYVlg2uAEgmt3Eunx68dumxnF0nTVzhVjlaSiM1nR4kWd.48S5g2vP5gORxW9keId73g0u90y0t10.f3hKNNyYNSP8lvfzCuggzCejhe9m+YRO8zoolZxmWWQQg8u+8yS9jOYP68V5g2XRlgOL2rm8rYsqcs.PpolJ1rYi4Lm4P7wGuNWYFKlkY3kyzNc14O+4Y4Ke49cQqzrm+4edra2N6cu6k8su8gCGNBIg8NvUPlHBjLCuNPQQgcsqcgGOdZYEjYVyZV7ge3Gp2kVKjd3MLjd30SG7fGjoO8o625A2V1xV3bm6bgMqQ6RO7FSxL7gXW8pWkAO3Ayku7ka40F+3GO1rYirxJKMO95hfOyxL7RO7AIm5TmRykGpd0qdwbm6b4du26kEu3EygO7gYO6YOjSN4DVE1kd3MljY3CftwMtAacqaE2tcSokVJqZUqh4N24521USM0PrwFaX85AmzCuggLCefVUUUEu5q9pLzgNTl0rlE6XG6.ud8ha2t0b6SLwDCqC6fzCuQkLCe.vV1xVX5Se597ZQEUTjQFYvl27lC6C2BYFdQmvTlxTZYAjXvCdv75u9qyINwIXm6bmQrgcoGdiIYF9NfFZnA13F2HEWbwr10tVRHgD7aaV8pWMwEWbL0oNUCw8QMoGdCC4pkqi5PG5PJu3K9hJIlXhMe0TorhUrB8trBIjqVNCC4pkqi3Mey2jkrjk32qOlwLFNvANfNTQhfIyxL7RO7sgzSOced9C9fOHKaYKicricnSUTnkzCuwjodF9qbkqP4kWNSYJSQywSM0T4gdnGBa1rQ5omtO220L5jd3MLjyk9u669N73wCqYMqgqd0qxIO4I4AdfGPysyHrC3taHGGdiIS0L7qZUqh24cdGpnhJ740WxRVh7qvZxYVlg2T0CeEUTgeg8QNxQRxImrNUQguje.nwjgbFdEEEM629HG4HLhQLB5Uu5EYkUVjWd4QZoklNTgg+jd3MLLt8vu6cuab61Mm9zmlcu6c623ojRJrgMrAxHiLjkHp6.oGdioH9Y3qt5ponhJhBJn.N5QOZKutY5VcrnqyrLCeDeO7OwS7D7Juxq3SXGvzb7xCVjd3Mlh3mgeYKaY7Zu1qA.8oO8g4Mu4gMa1Xzidz5bkEYS5g2vHxpGdud8xN24NoolZhYNyY523KXAKfRKsTxM2bIqrxhd1ydpCUowizCuwTX6L7UUUUTXgExJW4JoppphgO7gyQO5QMUmsahPGyxL7gc8vWe80yTm5TYXCaX3zoSpppp.fie7iSYkUl9VblHRO7FSgcA9XiMVN24NGd85skWa.CX.7xu7KKmfLgPxuRuwjtE3u90utO2Kyas7xKOrXwBYjQFrt0sNN8oOMKe4KWyy2cQvgD3MlB48vericL73wCEWbwL+4Oed629s8aatxUtBUWc0jTRIETqEgnYlkd38avfgFZnAkO3C9.EqVs1xJGCfx.Fv.TZpolBJumhtFYEuwvHzuh2bxSdRRN4j8635NzgNT9jO4SHkTRIf+dJ5ZjiCugQneuzmTRIwjlzj.ft0stwLlwLXaaaaTYkUJg8vTRO7FSArY3qnhJviGOL+4OeF6XGqeiWZokx9129XAKXALnAMn6p2CgHXwrLCueC1YTWc0o3wiGkG6wdrV5KegKbgA79qDgdRO7FFAld320t1EYlYlTWc04yqGarwxYO6Yo28t2copTnujd3MLBL8vmZpoRSM0TKOOlXhg4Lm4vV25VkvtAfzCuwzcbF9u8a+VdzG8QIpn7+mMjc1YyO7C+.1rYibxIGRLwDChkpPD7XVlg2uAUTTTpolZTVwJVgxHG4HU.T1912tl88TWc0Eh5vRDpI8vaXz98vO24NW13F2HMzPCs7hyblyjMsoMEJJNQXBoGdCi1uG9CbfC3SXOt3hSNLZlPRO7FS9MC+69tuKKdwKlwN1wRd4kGYmc1DWbwoKEmPDpXvlgeL.yD3WCbQfD.dIfOxu.+Eu3EoxJqjwLlwDhqQQ3jktzkxa7FugdWFgLFr.O.kgZvOdfZA9d.q29GPuXr9PKDcTJDFt9PzEXE3KZ0y+En9CADBMYdld23pLT+AYkoukgPHBErhZf2p9VFBgHTwodW.BgPHDhvExwnyXqy70Wix2KXT9bzobmNLDVQcO74+s7EgQxLQ8qyVamswJFquWni7Y1zvJ+tco+k.5qdVLhft9h5Wma9P3XsUiYEi42KzdelMUbRqVYYQ8jwQQdX3eb6ec1IF+uWPqOylRVwX9S0EZSlg2DOCeqYE0+mgScsJDAaNoi2CuyfbsDp3DIn2lLk6QSSDYuzaR7+Gmgg9NsvZqB.....jTQNQjqBAlf" ]
												}

											}
 ],
										"lines" : [  ]
									}
,
									"saved_object_attributes" : 									{
										"default_fontface" : 0,
										"default_fontname" : "Arial",
										"fontname" : "Arial",
										"default_fontsize" : 12.0,
										"globalpatchername" : "",
										"fontface" : 0,
										"fontsize" : 12.0
									}

								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "divide the result by the increment (gives subsample crossing point in relation to the previous sample)",
									"patching_rect" : [ 130.0, 355.833344, 544.0, 20.0 ],
									"id" : "obj-25",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Here, we used use some simple algebra to find the point at which the zero-crossing took place (assuming a straight line between the last two samples).",
									"linecount" : 2,
									"patching_rect" : [ 28.0, 150.0, 634.0, 34.0 ],
									"id" : "obj-23",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "subtract the current phasor~ value",
									"patching_rect" : [ 130.0, 324.833313, 193.0, 20.0 ],
									"id" : "obj-21",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "take away from one to calculate how long ago zero-crossing took place (in samples)",
									"patching_rect" : [ 130.0, 384.833344, 456.0, 20.0 ],
									"id" : "obj-19",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "calculate wrapped increment between last 2 samples",
									"patching_rect" : [ 130.0, 294.0, 292.0, 20.0 ],
									"id" : "obj-17",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "Subsample accuracy out (0-1)",
									"patching_rect" : [ 81.0, 448.0, 171.0, 20.0 ],
									"id" : "obj-12",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "It works by pre-advancing the ramps by the given subsample offset (between 0 and 1). The logic behind this is that for detection process (such as the one used here with phasor~), detection can only happen after the event (in this case crossing the 0 or 1 point) has taken place. Thus, the trigger occurs late by a fraction of a sample. To compenstae for this, we advance the ramps by that amount to acheive subsample accuracy. Offset is applied to both the normalised and position ramps of voicedrive~ and voicemanager~ also takes account of the subsample offset.",
									"linecount" : 5,
									"patching_rect" : [ 28.0, 58.0, 636.0, 75.0 ],
									"id" : "obj-10",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "comment",
									"text" : "The subsample offset is for many applications not necessary. However, if for some reason you need better than sample-accurate timing (for pitched prcoesses et.) you can utilise this functionality.",
									"linecount" : 2,
									"patching_rect" : [ 28.0, 17.0, 633.0, 34.0 ],
									"id" : "obj-8",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 0,
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "clip~ 0 1",
									"patching_rect" : [ 41.0, 416.0, 56.0, 20.0 ],
									"id" : "obj-5",
									"numinlets" : 3,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "!-~ 1",
									"patching_rect" : [ 41.0, 384.833344, 35.0, 20.0 ],
									"id" : "obj-15",
									"numinlets" : 2,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "!/~",
									"patching_rect" : [ 41.0, 355.833344, 32.5, 20.0 ],
									"id" : "obj-14",
									"numinlets" : 2,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "-~",
									"patching_rect" : [ 55.0, 324.833313, 64.0, 20.0 ],
									"id" : "obj-13",
									"numinlets" : 2,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "clip~ -1 0",
									"patching_rect" : [ 41.0, 264.833313, 60.0, 20.0 ],
									"id" : "obj-6",
									"numinlets" : 3,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "+~ 1",
									"patching_rect" : [ 41.0, 294.833313, 35.0, 20.0 ],
									"id" : "obj-4",
									"numinlets" : 2,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "newobj",
									"text" : "delta~",
									"patching_rect" : [ 41.0, 234.833328, 44.0, 20.0 ],
									"id" : "obj-3",
									"numinlets" : 1,
									"fontname" : "Arial",
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"fontsize" : 12.0
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "outlet",
									"patching_rect" : [ 41.0, 446.0, 25.0, 25.0 ],
									"id" : "obj-2",
									"numinlets" : 1,
									"numoutlets" : 0,
									"comment" : "Subsample Offset"
								}

							}
, 							{
								"box" : 								{
									"maxclass" : "inlet",
									"patching_rect" : [ 41.0, 198.0, 25.0, 25.0 ],
									"id" : "obj-1",
									"numinlets" : 0,
									"numoutlets" : 1,
									"outlettype" : [ "signal" ],
									"comment" : "phasor~ In"
								}

							}
 ],
						"lines" : [ 							{
								"patchline" : 								{
									"source" : [ "obj-4", 0 ],
									"destination" : [ "obj-14", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-13", 0 ],
									"destination" : [ "obj-14", 1 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-14", 0 ],
									"destination" : [ "obj-15", 0 ],
									"hidden" : 0,
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
									"source" : [ "obj-5", 0 ],
									"destination" : [ "obj-2", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-3", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-3", 0 ],
									"destination" : [ "obj-6", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-6", 0 ],
									"destination" : [ "obj-4", 0 ],
									"hidden" : 0,
									"midpoints" : [  ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-4", 0 ],
									"destination" : [ "obj-13", 0 ],
									"hidden" : 0,
									"midpoints" : [ 50.5, 319.333313, 64.5, 319.333313 ]
								}

							}
, 							{
								"patchline" : 								{
									"source" : [ "obj-1", 0 ],
									"destination" : [ "obj-13", 1 ],
									"hidden" : 0,
									"midpoints" : [ 50.5, 229.0, 109.5, 229.0 ]
								}

							}
 ]
					}
,
					"saved_object_attributes" : 					{
						"default_fontface" : 0,
						"default_fontname" : "Arial",
						"fontname" : "Arial",
						"default_fontsize" : 12.0,
						"globalpatchername" : "",
						"fontface" : 0,
						"fontsize" : 12.0
					}

				}

			}
, 			{
				"box" : 				{
					"maxclass" : "umenu",
					"types" : [  ],
					"patching_rect" : [ 241.0, 316.0, 57.0, 20.0 ],
					"id" : "obj-59",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 3,
					"outlettype" : [ "int", "", "" ],
					"fontsize" : 12.0,
					"items" : [ "Off", ",", "On" ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "This demo illustrates how you might use voicemanger~ and voicedrive~ in combination to create a subsample-accurate granular synth patch with the voices hosted in a poly~ object. Unfortunately, you can't use the muting facilities of poly~, so all voices must be left on.",
					"linecount" : 2,
					"patching_rect" : [ 21.0, 17.0, 729.0, 34.0 ],
					"id" : "obj-32",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "grain rate",
					"patching_rect" : [ 299.0, 64.0, 61.0, 20.0 ],
					"id" : "obj-30",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "set length to get overlap of 10 voices",
					"patching_rect" : [ 408.0, 164.0, 206.0, 20.0 ],
					"id" : "obj-28",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "gives number of active voices",
					"patching_rect" : [ 621.0, 403.0, 168.0, 20.0 ],
					"id" : "obj-25",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "sum busy flags ",
					"patching_rect" : [ 665.0, 376.0, 94.0, 20.0 ],
					"id" : "obj-19",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"text" : "offset into buffer (ms)",
					"patching_rect" : [ 666.0, 200.0, 123.0, 20.0 ],
					"id" : "obj-17",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "comment",
					"varname" : "autohelp_dac_text",
					"text" : "start audio",
					"linecount" : 2,
					"patching_rect" : [ 200.0, 435.0, 38.0, 33.0 ],
					"id" : "obj-38",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 0,
					"fontsize" : 11.595187
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "ezdac~",
					"varname" : "autohelp_dac",
					"patching_rect" : [ 241.0, 429.0, 45.0, 45.0 ],
					"id" : "obj-39",
					"local" : 1,
					"numinlets" : 2,
					"numoutlets" : 0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "panel",
					"varname" : "startwinwdow_panel",
					"patching_rect" : [ 193.0, 424.0, 100.0, 55.0 ],
					"id" : "obj-11",
					"border" : 2,
					"numinlets" : 1,
					"bgcolor" : [ 1.0, 1.0, 1.0, 1.0 ],
					"numoutlets" : 0,
					"bordercolor" : [ 0.392157, 0.792157, 0.117647, 1.0 ]
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "selector~",
					"patching_rect" : [ 241.0, 349.0, 82.0, 20.0 ],
					"id" : "obj-13",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~ 0.05",
					"patching_rect" : [ 241.0, 385.0, 49.0, 20.0 ],
					"id" : "obj-15",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "flonum",
					"patching_rect" : [ 681.0, 237.333328, 50.0, 20.0 ],
					"id" : "obj-26",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "float", "bang" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "sig~",
					"patching_rect" : [ 681.0, 289.0, 33.0, 20.0 ],
					"id" : "obj-24",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "*~ 10",
					"patching_rect" : [ 388.0, 263.333313, 39.0, 20.0 ],
					"id" : "obj-23",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "sig~ 5",
					"patching_rect" : [ 304.0, 113.0, 43.0, 20.0 ],
					"id" : "obj-21",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "!/~ 1000.",
					"patching_rect" : [ 388.0, 237.333328, 58.0, 20.0 ],
					"id" : "obj-20",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "flonum",
					"patching_rect" : [ 304.0, 90.0, 50.0, 20.0 ],
					"id" : "obj-14",
					"minimum" : 5.0,
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "float", "bang" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "<~",
					"patching_rect" : [ 304.0, 263.333313, 32.0, 20.0 ],
					"id" : "obj-6",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "delta~",
					"patching_rect" : [ 304.0, 237.333328, 44.0, 20.0 ],
					"id" : "obj-5",
					"numinlets" : 1,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "phasor~ 20",
					"patching_rect" : [ 304.0, 148.333328, 71.0, 20.0 ],
					"id" : "obj-4",
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 1,
					"outlettype" : [ "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "voicemanager~ 20",
					"patching_rect" : [ 304.0, 286.5, 270.0, 20.0 ],
					"id" : "obj-3",
					"numinlets" : 4,
					"fontname" : "Arial",
					"numoutlets" : 3,
					"outlettype" : [ "signal", "signal", "signal" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "number~",
					"sig" : 0.0,
					"patching_rect" : [ 681.0, 348.0, 56.0, 20.0 ],
					"id" : "obj-2",
					"mode" : 2,
					"numinlets" : 2,
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "signal", "float" ],
					"fontsize" : 12.0
				}

			}
, 			{
				"box" : 				{
					"maxclass" : "newobj",
					"text" : "poly~ voice_demo_poly 20 args jongly",
					"patching_rect" : [ 304.0, 316.0, 396.0, 20.0 ],
					"id" : "obj-1",
					"numinlets" : 4,
					"fontname" : "Arial",
					"numoutlets" : 2,
					"outlettype" : [ "signal", "signal" ],
					"fontsize" : 12.0
				}

			}
 ],
		"lines" : [ 			{
				"patchline" : 				{
					"source" : [ "obj-1", 1 ],
					"destination" : [ "obj-2", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-24", 0 ],
					"destination" : [ "obj-1", 3 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-1", 0 ],
					"destination" : [ "obj-13", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 1 ],
					"destination" : [ "obj-1", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 0 ],
					"destination" : [ "obj-1", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-3", 2 ],
					"destination" : [ "obj-1", 2 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-23", 0 ],
					"destination" : [ "obj-3", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-6", 0 ],
					"destination" : [ "obj-3", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-7", 0 ],
					"destination" : [ "obj-3", 2 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-13", 0 ],
					"destination" : [ "obj-15", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-15", 0 ],
					"destination" : [ "obj-39", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-15", 0 ],
					"destination" : [ "obj-39", 1 ],
					"hidden" : 0,
					"midpoints" : [ 250.5, 412.0, 276.5, 412.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-7", 0 ],
					"hidden" : 0,
					"color" : [ 0.078431, 0.639216, 0.941176, 1.0 ],
					"midpoints" : [ 313.5, 229.0, 481.5, 229.0 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-10", 0 ],
					"destination" : [ "obj-5", 0 ],
					"hidden" : 0,
					"color" : [ 0.078431, 0.639216, 0.941176, 1.0 ],
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-8", 0 ],
					"destination" : [ "obj-10", 1 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-10", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-4", 0 ],
					"destination" : [ "obj-8", 0 ],
					"hidden" : 0,
					"midpoints" : [ 313.5, 170.666656, 327.5, 170.666656 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-59", 0 ],
					"destination" : [ "obj-13", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-14", 0 ],
					"destination" : [ "obj-21", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-26", 0 ],
					"destination" : [ "obj-24", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-20", 0 ],
					"destination" : [ "obj-23", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-5", 0 ],
					"destination" : [ "obj-6", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-21", 0 ],
					"destination" : [ "obj-20", 0 ],
					"hidden" : 0,
					"midpoints" : [ 313.5, 142.166656, 397.5, 142.166656 ]
				}

			}
, 			{
				"patchline" : 				{
					"source" : [ "obj-21", 0 ],
					"destination" : [ "obj-4", 0 ],
					"hidden" : 0,
					"midpoints" : [  ]
				}

			}
 ]
	}

}
