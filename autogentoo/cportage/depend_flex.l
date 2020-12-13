%option prefix="depend"

%{
#include <stdio.h>
#include "language.h"
#include <string.h>
#include "depend.tab.h"
#include <assert.h>

#include <signal.h>

int asprintf(char** strp, const char* fmt, ...);

#define YY_INPUT(buf,result,max_size)  {\
    result = language_get_next(buf, max_size); \
    if (  result <= 0  ) \
      result = YY_NULL; \
}

%}

letter          [a-zA-Z]
digit           [0-9]
version_suf     ("_alpha"|"_beta"|"_pre"|"_rc"|""|"_p")
version         {digit}+("."{digit}+)*{letter}?({version_suf}{digit}*)*("-r"{digit}+)?[\*]?
identifier      ({letter}|{digit}|_|\+|\-)+
atom            {identifier}\/{identifier}(\-{version})?
slotname        ({letter}|{digit}|"_")({letter}|{digit}|"_"|"-"|"."|"+")*
slot            [\:]({slotname}(\/{slotname})?)?[\*|\=]?
repo            [\:][\:]{identifier}
atomflag        [!]?{identifier}(\([\+|\-]\))?[\?|=]?
atomflags       [\[][ ]*{atomflag}([ ]*,[ ]*{atomflag})*[ ]*[\]]
use_select      [!]?{identifier}\?

%%
[\x01]                  {
                            language_begin_token(dependtext, "delim");
                            switch(language_get_type_delim())
                            {
                                case LANGUAGE_DEPEND: return DEPEND;
                                case LANGUAGE_IGNORE:
                                default:
                                    break;
                            }
                        }
[\n]                    {
                            language_begin_token(dependtext, "newline");
                            language_increment_line();
                        }
[ \t\r\\]+              {language_begin_token(dependtext, "tab");};
{repo}                  {language_begin_token(dependtext, "repo"); dependlval.identifier = strdup(dependtext + 2); return REPOSITORY;}
{slot}                  {
                            char* ref = dependtext;
                            int len = strlen(ref);
                            language_begin_token(ref, "slot");
                            dependlval.slot.slot_opts = ATOM_SLOT_IGNORE;
                            if (ref[len - 1] == '*') {
                                dependlval.slot.slot_opts = ATOM_SLOT_IGNORE;
                                ref[--len] = 0;
                            }
                            else if (ref[len - 1] == '=') {
                                dependlval.slot.slot_opts = ATOM_SLOT_REBUILD;
                                ref[--len] = 0;
                            }

                            char* name_splt = strchr(ref + 1, '/');
                            if (name_splt)
                                *name_splt = 0;
                            if (ref[1] == 0) {
                                dependlval.slot.name = NULL;
                                dependlval.slot.sub_name = NULL;
                                return SLOT;
                            }
                            dependlval.slot.name = strdup(ref + 1);
                            if (name_splt)
                                dependlval.slot.sub_name = strdup(name_splt + 1);
                            else
                                dependlval.slot.sub_name = NULL;

                            return SLOT;
                        }
{atomflags}             {
                            char* textref = dependtext;
                            language_begin_token(dependtext, "atomflags");
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

                            dependlval.atom_flags = start;
                            return ATOM_FLAGS;
                        }
{use_select}            {
                            char* ref = dependtext;
                            language_begin_token(ref, "use_select");

                            if (*ref == '!')
                            {
                                dependlval.depend_expr_select.operator = USE_OP_DISABLE;
                                ref++;
                            }
                            else
                            {
                                dependlval.depend_expr_select.operator = USE_OP_ENABLE;
                            }

                            size_t len = strlen(ref);
                            ref[len - 1] = 0;
                            dependlval.depend_expr_select.target = strndup(ref, len - 1);
                            return USESELECT;
                        }
"??"                    {language_begin_token(dependtext, NULL); dependlval.depend_expr_select.target = NULL; dependlval.depend_expr_select.operator = USE_OP_MOST_ONE; return USESELECT;}
"||"                    {language_begin_token(dependtext, NULL); dependlval.depend_expr_select.target = NULL; dependlval.depend_expr_select.operator = USE_OP_LEAST_ONE; return USESELECT;}
"^^"                    {language_begin_token(dependtext, NULL); dependlval.depend_expr_select.target = NULL; dependlval.depend_expr_select.operator = USE_OP_EXACT_ONE; return USESELECT;}
"-"                     {language_begin_token(dependtext, NULL); return '-';}
"<"                     {language_begin_token(dependtext, NULL); return '<';}
">"                     {language_begin_token(dependtext, NULL); return '>';}
"="                     {language_begin_token(dependtext, NULL); return '=';}
"!"                     {language_begin_token(dependtext, NULL); return '!';}
[\[]                    {language_begin_token(dependtext, NULL); return '[';}
[\]]                    {language_begin_token(dependtext, NULL); return ']';}
[\?]                    {language_begin_token(dependtext, NULL); return '?';}
[\(]                    {language_begin_token(dependtext, NULL); return '(';}
[\)]                    {language_begin_token(dependtext, NULL); return ')';}
[\^]                    {language_begin_token(dependtext, NULL); return '^';}
[\|]                    {language_begin_token(dependtext, NULL); return '|';}
[\,]                    {language_begin_token(dependtext, NULL); return ',';}
[\~]                    {language_begin_token(dependtext, NULL); return '~';}
{atom}                  {
                            char* reference = dependtext;
                            language_begin_token(reference, "atom");
                            dependlval.atom_type = (Atom*)PyAtom_new(&PyAtomType, NULL, NULL);
                            atom_init(dependlval.atom_type, reference);
                            return ATOM;
                        }

[\*]                    {language_begin_token(dependtext, "star"); return '*';}

{identifier}            {
                            char* ref = dependtext;
                            language_print_error("Invalid identifier '%s'", ref);
                            language_begin_token(ref, "identifier");
                        }


[<<EOF>>|'']            {language_begin_token(dependtext, "eof"); return END_OF_FILE;}
.                       {language_begin_token(dependtext, "invalid"); printf("Unknown character: %s\n", dependtext);}

%%