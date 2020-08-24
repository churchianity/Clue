" vim syntax file for clue
"
"
if exists("b:current_syntax")
    finish
endif

syn keyword keywords import return if else while do for
syn match comment "`.*$"


let b:current_syntax = "clue"

hi def link clueComment Comment
hi def link keywords Statement
