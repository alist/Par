/* Used by ParPar.cpp to Bootstrap the grammar parser - see ParBoot.h for details */
Par_(def     ,Mny(Any(cmt),One(par),One(list),Any(cmt)))
Par_(par     ,One(name))
Par_(list    ,And(Quo("("),Mny(Or(One(waves),One(ors),One(ands),One(item),One(list))),Quo(")"),Opt(repeat)))
Par_(waves   ,And(One(item),Mny(And(One(wave),One(item)))))
Par_(wave    ,Regx("^([\\~][^A-Za-z0-9_])"))
Par_(ors     ,And(One(andits),Mny(Quo("|"),One(andits))))
Par_(andits  ,Or(One(ands),One(item)))
Par_(ands    ,And(One(item),Mny(item)))
Par_(item    ,And(Or(One(regx),One(quote),One(named),One(ahead)),Opt(pred),Opt(cmt)))
Par_(named   ,And(One(name),Opt(repeat)))
Par_(quote   ,Regx("^\"([^\"]*)\""))        // return "anything in between"
Par_(regx    ,Regx("^([in]*\'[^\']*)\'"))   // return option, opening ', but not closing '
Par_(pred    ,Regx("^[ ]*[=][>][ ]*[']([^']*)[ ]*[']"))
Par_(ahead   ,Regx("^([\\~][a-z_][a-z0-9_]*)"))
Par_(repeat  ,Or(One(any),One(mny),One(opt),One(quant)))
Par_(any     ,Quo("*"))
Par_(mny     ,Quo("+"))
Par_(opt     ,Quo("?"))
Par_(quant   ,And(Quo("{"),Or(And(One(min),One(comma),Opt(max)),And(One(comma),One(max)),One(exact)),Quo("}")))
Par_(min     ,Regx("^[0-9]+"))
Par_(max     ,Regx("^[0]?[0-9]+")) // non-zero number
Par_(exact   ,Regx("^[0]?[0-9]+"))
Par_(comma   ,Quo(","))
Par_(name    ,Regx("^([A-Za-z_][A-Za-z0-9_]*)"))
Par_(cmt     ,Regx("^[/][/][ ]*(.*?)[\r\n]+|^[ \r\n\t]+"))