Hello Par
0, 1: def :
...
290, 3:   lvalue :
291, 4:    name : dur
292, 3:   list :
293, 7:       ands :
294, 9:         item :
295,11:           named :
296,13:             name : for
297, 9:         item :
298,11:           named :
299,13:             name : num
300, 7:       list :
301,11:           ors :
302,13:             item :
303,15:               named :
304,17:                 name : days
305,15:               item :
306,17:                 named :
307,19:                   name : hours
308,15:               item :
309,17:                 named :
310,19:                   name : mins
311, 7:       list :
312,11:           item :
313,13:             named :
314,15:               name : and
315,11:           list :
316,15:               ors :
317,17:                 item :
318,19:                   named :
319,21:                     name : hours
320,19:                   item :
321,21:                     named :
322,23:                       name : mins
323, 9:         cardi :
324,11:           opt :

dur (for num (days|hours|mins) (and (hours|mins))?)

Par_(dur, And(One(for),One(num),Or(One(days),One(hours),One(mins)),Opt(And(One(and),Or(One(hours),One(mins)))))