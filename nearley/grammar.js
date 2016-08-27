// Generated automatically by nearley
// http://github.com/Hardmath123/nearley
(function () {
function id(x) {return x[0]; }
var grammar = {
    ParserRules: [
    {"name": "_$ebnf$1", "symbols": []},
    {"name": "_$ebnf$1", "symbols": ["wschar", "_$ebnf$1"], "postprocess": function arrconcat(d) {return [d[0]].concat(d[1]);}},
    {"name": "_", "symbols": ["_$ebnf$1"], "postprocess": function(d) {return null;}},
    {"name": "__$ebnf$1", "symbols": ["wschar"]},
    {"name": "__$ebnf$1", "symbols": ["wschar", "__$ebnf$1"], "postprocess": function arrconcat(d) {return [d[0]].concat(d[1]);}},
    {"name": "__", "symbols": ["__$ebnf$1"], "postprocess": function(d) {return null;}},
    {"name": "wschar", "symbols": [/[ \t\n\v\f]/], "postprocess": id},
    {"name": "program", "symbols": ["block"]},
    {"name": "program", "symbols": ["expression"]},
    {"name": "block", "symbols": [{"literal":"("}, "expression", {"literal":")"}]},
    {"name": "expression", "symbols": ["operand"]},
    {"name": "expression$subexpression$1", "symbols": ["operand", "_", "expression", "_", "binop"]},
    {"name": "expression", "symbols": ["expression$subexpression$1"]},
    {"name": "binop", "symbols": [{"literal":"+"}]},
    {"name": "binop", "symbols": [{"literal":"-"}]},
    {"name": "binop", "symbols": [{"literal":"/"}]},
    {"name": "binop", "symbols": [{"literal":"*"}]},
    {"name": "operand", "symbols": ["number"]},
    {"name": "operand", "symbols": ["block"]},
    {"name": "number", "symbols": ["digit"]},
    {"name": "number$subexpression$1", "symbols": ["natural", "digits"]},
    {"name": "number", "symbols": ["number$subexpression$1"]},
    {"name": "natural", "symbols": [/[1-9]/]},
    {"name": "digits$ebnf$1", "symbols": []},
    {"name": "digits$ebnf$1", "symbols": ["digit", "digits$ebnf$1"], "postprocess": function arrconcat(d) {return [d[0]].concat(d[1]);}},
    {"name": "digits", "symbols": ["digits$ebnf$1"]},
    {"name": "digit", "symbols": ["natural"]},
    {"name": "digit", "symbols": [/[0]/]}
]
  , ParserStart: "program"
}
if (typeof module !== 'undefined'&& typeof module.exports !== 'undefined') {
   module.exports = grammar;
} else {
   window.grammar = grammar;
}
})();
