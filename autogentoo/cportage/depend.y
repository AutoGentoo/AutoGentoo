%include {
    #include <autogentoo/cportage/dependency.h>
    #include <autogentoo/cportage/use.h>
}

%top {
#include <stdlib.h>
#include <string.h>
}

%option prefix="depend"
%option debug_table="TRUE"
%option annotate_line="FALSE"

%union {
    char* identifier;
    Atom* atom_type;
    Dependency* depend_type;
    RequiredUse* use_type;

    struct {
        char* target;
        use_operator_t operator;
    } depend_expr_select;

    struct {
        char* name;
        char* sub_name;
        atom_slot_t slot_opts;
    } slot;

    AtomFlag* atom_flags;
}

%token '-'
%token '<'
%token '>'
%token '='
%token '!'
%token '['
%token ']'
%token '?'
%token '('
%token ')'
%token '|'
%token '^'
%token ','
%token '~'
%token <identifier> REPOSITORY
%token <atom_type> ATOM
%token <atom_flags> ATOM_FLAGS
%token <slot> SLOT
%token <depend_expr_select> USESELECT

%type <atom_type> atom_version
%type <atom_type> atom_block
%type <atom_type> atom_slot
%type <atom_type> atom_repo
%type <atom_type> atom
%type <depend_type> depend_expr
%type <depend_type> depend_expr_single
%type <depend_type> program_depend
%start<depend_type> program_depend

%option debug_ids="$-<>=![]?()|^,~rafsuVBSRADSP"

%destructor <depend_expr_select> { if ($$.target) free($$.target); }
%destructor <atom_type>          { Py_DECREF($$); }
%destructor <atom_flags>         { Py_DECREF($$); }
%destructor <slot>               { free($$.name); if($$.sub_name) free($$.sub_name); }
%destructor <identifier>         { free($$); }

%bottom {
Atom* atom_parse(void* buffers, const char* input)
{
    Dependency* out = depend_parse(buffers, input);

    if (!out)
    {
        return NULL;
    }

    Atom* atom_out = out->atom;
    Py_INCREF(atom_out);
    Py_DECREF(out);

    return atom_out;
}
}

+letter          A-Za-z
+version_suf     (?:(?:_alpha|_beta|_pre|_rc|_p)[0-9]*)
+revision        (?:-r[0-9]+)
+version         [0-9]+(?:\.[0-9]+)*[{letter}]?{version_suf}*{revision}?
+ident_word      [{letter}_0-9][{letter}0-9\+_]*
+identifier      {ident_word}(?:-{ident_word})*
+atom            {identifier}\/{identifier}(?:[-\.]?(?:{version}))?[\*]?
+slotname        [A-Za-z0-9][A-Za-z0-9+_.-]*
+slot            [\:]({slotname}(?:\/{slotname})?)?[\*|\=]?
+repo            [\:][\:]{identifier}
+atomflag        [!-]?{identifier}(?:\([\+|\-]\))?[\?|=]?
+atomflags       [\[][ ]*{atomflag}(?:[ ]*,[ ]*{atomflag})*[ ]*[\]]
+use_select      [!]?[A-Za-z0-9_-]+\?

==

"[\n]"                  {}
"[ \t\r\\]+"            {/* skip */}
"{repo}"                {yyval->identifier = strdup(yytext + 2); return REPOSITORY;}
"{slot}"                {
                            yyval->slot.slot_opts = ATOM_SLOT_IGNORE;
                            if (yytext[len - 1] == '*') {
                                yyval->slot.slot_opts = ATOM_SLOT_IGNORE;
                                len--;
                            }
                            else if (yytext[len - 1] == '=') {
                                yyval->slot.slot_opts = ATOM_SLOT_REBUILD;
                                len--;
                            }

                            const char* name_splt = strchr(yytext + 1, '/');
                            if (!name_splt)
                                name_splt = yytext + len;
                            if (len <= 1 || yytext[1] == 0) {
                                yyval->slot.name = NULL;
                                yyval->slot.sub_name = NULL;
                                return SLOT;
                            }
                            yyval->slot.name = strndup(yytext + 1, name_splt - yytext - 1);
                            if (*name_splt)
                                yyval->slot.sub_name = strdup(name_splt + 1);
                            else
                                yyval->slot.sub_name = NULL;

                            return SLOT;
                        }
"{atomflags}"           {
                            char* textref = strdup(yytext);
                            AtomFlag* start = NULL;
                            AtomFlag* last = NULL;
                            AtomFlag* current;

                            for (char* tok = strtok(textref + 1, " ,]"); tok; tok = strtok(NULL, " ,]"))
                            {
                                size_t len = strlen(tok);
                                if (!len)
                                    continue;
                                atom_use_t option;

                                if (*tok == '!' && tok[len - 1] == '?')
                                {
                                    tok[len - 1] = 0;
                                    tok++;
                                    option = ATOM_USE_DISABLE_IF_OFF;
                                }
                                else if (*tok == '!' && tok[len - 1] == '=')
                                {
                                    tok[len - 1] = 0;
                                    tok++;
                                    option = ATOM_USE_OPPOSITE;
                                }
                                else if (tok[len - 1] == '?')
                                {
                                    tok[len - 1] = 0;
                                    option = ATOM_USE_ENABLE_IF_ON;
                                }
                                else if (tok[len - 1] == '=')
                                {
                                    tok[len - 1] = 0;
                                    option = ATOM_USE_EQUAL;
                                }
                                else if (*tok == '-')
                                {
                                    tok++;
                                    option = ATOM_USE_DISABLE;
                                }
                                else
                                    option = ATOM_USE_ENABLE;

                                current = atomflag_build(tok);
                                current->option = option;

                                if (!last)
                                    last = current;
                                else
                                {
                                    last->next = current;
                                    last = current;
                                }

                                if (!start)
                                    start = current;
                            }

                            yyval->atom_flags = start;
                            free(textref);
                            return ATOM_FLAGS;
                        }
"{use_select}"          {
                            if (*yytext == '!')
                            {
                                yyval->depend_expr_select.operator = USE_OP_DISABLE;
                                yytext++;
                            }
                            else
                            {
                                yyval->depend_expr_select.operator = USE_OP_ENABLE;
                            }

                            yyval->depend_expr_select.target = strndup(yytext, len - 1);
                            return USESELECT;
                        }
"\?\?"                  {yyval->depend_expr_select.target = NULL; yyval->depend_expr_select.operator = USE_OP_MOST_ONE; return USESELECT;}
"\|\|"                  {yyval->depend_expr_select.target = NULL; yyval->depend_expr_select.operator = USE_OP_LEAST_ONE; return USESELECT;}
"\^\^"                  {yyval->depend_expr_select.target = NULL; yyval->depend_expr_select.operator = USE_OP_EXACT_ONE; return USESELECT;}
"-"                     {return '-';}
"<"                     {return '<';}
">"                     {return '>';}
"="                     {return '=';}
"!"                     {return '!';}
"[\[]"                  {return '[';}
"[\]]"                  {return ']';}
"[\?]"                  {return '?';}
"[\(]"                  {return '(';}
"[\)]"                  {return ')';}
"[\^]"                  {return '^';}
"[\|]"                  {return '|';}
"[\,]"                  {return ',';}
"[\~]"                  {return '~';}
"{atom}"                {
                            yyval->atom_type = (Atom*)PyAtom_new(&PyAtomType, NULL, NULL);
                            atom_init(yyval->atom_type, yytext);
                            return ATOM;
                        }

==

%%

program_depend : depend_expr                {$$ = (void*)$1;}
               |                            {$$ = NULL;}
               ;

depend_expr  : depend_expr_single               {$$ = $1;}
             | depend_expr_single depend_expr   {$$ = $1; $$->next = $2;}
             ;

depend_expr_single  : atom                          {$$ = dependency_build_atom($1);}
                    | USESELECT '(' depend_expr ')' {$$ = dependency_build_use($1.target, $1.operator, $3); free($1.target);}
                    | '(' depend_expr ')'           {$$ = dependency_build_grouping($2);}
                    ;

atom        : atom_repo ATOM_FLAGS          {$$ = $1; $$->useflags = $2;}
            | atom_repo                     {$$ = $1; $$->useflags = NULL;}
            ;

atom_repo   : atom_slot REPOSITORY  {
                                        $$ = $1;
                                        if ($$->repository) free($$->repository);
                                        $$->repository = $2;
                                    }
            | atom_slot             {$$ = $1;
                                     if ($$->repository) free($$->repository);
                                     $$->repository = NULL;
                                    }
            ;

atom_slot   : atom_block SLOT             {
                                               $$ = $1;
                                               $$->slot = $2.name;
                                               $$->sub_slot = $2.sub_name;
                                               $$->slot_opts = $2.slot_opts;
                                          }
            | atom_block                {$$ = $1; $$->slot = NULL; $$->sub_slot = NULL; $$->slot_opts = ATOM_SLOT_IGNORE;}
            ;

atom_block  :  '!' '!' atom_version     {$$ = $3; $$->blocks = ATOM_BLOCK_HARD;}
            |      '!' atom_version     {$$ = $2; $$->blocks = ATOM_BLOCK_SOFT;}
            |          atom_version     {$$ = $1; $$->blocks = ATOM_BLOCK_NONE;}
            ;

atom_version   : '>' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_GE;}
               | '<' '=' ATOM           {$$ = $3; $$->range = ATOM_VERSION_LE;}
               |     '=' ATOM           {$$ = $2; $$->range = ATOM_VERSION_E;}
               |     '>' ATOM           {$$ = $2; $$->range = ATOM_VERSION_G;}
               |     '~' ATOM           {$$ = $2; $$->range = ATOM_VERSION_REV;}
               |     '<' ATOM           {$$ = $2; $$->range = ATOM_VERSION_L;}
               |         ATOM           {$$ = $1; $$->range = ATOM_VERSION_ALL;}
               ;

%%
