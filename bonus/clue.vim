" vim syntax file for clue
"
"
if exists("b:current_syntax")
    finish
endif

syn keyword keywords return if else while do for then
syn match comment "`.*$"
syn keyword preprocessors #import


let b:current_syntax = "clue"

hi def link clueComment Comment
hi def link keywords Statement
hi def link preprocessors PreProc

