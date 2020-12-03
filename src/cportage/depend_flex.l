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
identifier      ({letter}|{digit}|_|\+|\*|\-)+
atom            {identifier}\/{identifier}(\-{version})?
slotname        ({letter}|{digit}|"_")({letter}|{digit}|"_"|"-"|"."|"+")*
slot            [\:]({slotname}(\/{slotname})?)?[\*|\=]?
repo            [\:][\:]{identifier}

%%
[\x01]                  {
                            language_begin_token(yytext, "delim");
                            switch(language_get_type_delim())
                            {
                                case LANGUAGE_DEPEND: return DEPEND;
                                case LANGUAGE_REQUIRED_USE: return REQUIRED_USE;
                                case LANGUAGE_CMDLINE: return COMMAND_LINE;
                                case LANGUAGE_IGNORE:
                                default:
                                    break;
                            }
                        }
[\n]                    {
                            language_begin_token(yytext, "\\n");
                            language_increment_line();
                        }
[ \t\r\\]+              {language_begin_token(yytext, "tab");};
{repo}                  {language_begin_token(yytext, "repo"); yylval.identifier = strdup(yytext + 2); return REPOSITORY;}
{slot}                  {
                            language_begin_token(yytext, "slot");
                            size_t len = strlen(yytext);
                            yylval.slot.slot_opts = ATOM_SLOT_IGNORE;
                            if (yytext[len - 1] == '*') {
                                yylval.slot.slot_opts = ATOM_SLOT_IGNORE;
                                yytext[--len] = 0;
                            }
                            else if (yytext[len - 1] == '=') {
                                yylval.slot.slot_opts = ATOM_SLOT_REBUILD;
                                yytext[--len] = 0;
                            }

                            char* name_splt = strchr(yytext + 1, '/');
                            if (name_splt)
                                *name_splt = 0;
                            if (yytext[1] == 0) {
                                yylval.slot.name = NULL;
                                yylval.slot.sub_name = NULL;
                                return SLOT;
                            }
                            yylval.slot.name = strdup(yytext + 1);
                            if (name_splt)
                                yylval.slot.sub_name = strdup(name_splt + 1);
                            else
                                yylval.slot.sub_name = NULL;

                            return SLOT;
                        }
"??"                    {language_begin_token(yytext, NULL); yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_MOST_ONE; return USESELECT;}
"||"                    {language_begin_token(yytext, NULL); yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_LEAST_ONE; return USESELECT;}
"^^"                    {language_begin_token(yytext, NULL); yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_EXACT_ONE; return USESELECT;}
"(+)"                   {language_begin_token(yytext, NULL); yylval.use_default = ATOM_DEFAULT_ON; return USE_DEFAULT;}
"(-)"                   {language_begin_token(yytext, NULL); yylval.use_default = ATOM_DEFAULT_OFF; return USE_DEFAULT;}
"-"                     {language_begin_token(yytext, NULL); return '-';}
"<"                     {language_begin_token(yytext, NULL); return '<';}
">"                     {language_begin_token(yytext, NULL); return '>';}
"="                     {language_begin_token(yytext, NULL); return '=';}
"!"                     {language_begin_token(yytext, NULL); return '!';}
[\[]                    {language_begin_token(yytext, NULL); return '[';}
[\]]                    {language_begin_token(yytext, NULL); return ']';}
[\?]                    {language_begin_token(yytext, NULL); return '?';}
[\(]                    {language_begin_token(yytext, NULL); return '(';}
[\)]                    {language_begin_token(yytext, NULL); return ')';}
[\^]                    {language_begin_token(yytext, NULL); return '^';}
[\|]                    {language_begin_token(yytext, NULL); return '|';}
[\,]                    {language_begin_token(yytext, NULL); return ',';}
[\~]                    {language_begin_token(yytext, NULL); return '~';}
{atom}                  {
                            yylval.atom_type = (Atom*)PyAtom_new(&PyAtomType, NULL, NULL);
                            atom_init(yylval.atom_type, yytext);
                            language_begin_token(yytext, "atom");
                            return ATOM;
                        }

[\*]                    {language_begin_token(yytext, "star"); return '*';}

{identifier}            {
                            yylval.identifier = strdup(yytext);
                            language_begin_token(yytext, "identifier");
                            return IDENTIFIER;
                        }


[<<EOF>>|'']            {language_begin_token(yytext, "eof"); return END_OF_FILE;}
.                       {language_begin_token(yytext, "invalid"); printf("Unknown character: %s\n", yytext);}

%%