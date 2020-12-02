%{
#include <stdio.h>
#include "language.h"
#include <string.h>
#include "depend.tab.h"
#include <assert.h>

int error = 0;
#include <signal.h>

int asprintf(char **strp, const char *fmt, ...);

int yyerror(const char *s) {
    fprintf(stderr, "DEPEND ERROR %s in line %d near '%s'\n", s, yylineno, yytext);
    error = 1;
    yyout = NULL;
    assert(0);
    return error;
}

Dependency* depend_parse (const char* buffer) {
    yylineno = 0;
    char* depend_expr_str = NULL;
    asprintf(&depend_expr_str, "DEPEND_EXPR_AUTOGENTOO %s", buffer);

    YY_BUFFER_STATE bs = yy_scan_string(depend_expr_str);
    yy_switch_to_buffer(bs);
    yyparse();

    free(depend_expr_str);
    yy_delete_buffer(bs);

    return (Dependency*)yyout;
}

Dependency* cmdline_parse (const char* buffer) {
    yylineno = 0;
    char* depend_expr_str = NULL;
    asprintf(&depend_expr_str, "COMMAND_LINE_AUTOGENTOO %s", buffer);

    YY_BUFFER_STATE bs = yy_scan_string(depend_expr_str);
    yy_switch_to_buffer(bs);
    yyparse();

    free(depend_expr_str);
    yy_delete_buffer(bs);

    return (Dependency*)yyout;
}

Atom* atom_parse (const char* buffer) {
    yylineno = 0;
    char* depend_expr_str = NULL;
    asprintf(&depend_expr_str, "DEPEND_EXPR_AUTOGENTOO %s", buffer);

    YY_BUFFER_STATE bs = yy_scan_string(depend_expr_str);
    yy_switch_to_buffer(bs);
    yyparse();

    free(depend_expr_str);
    yy_delete_buffer(bs);

    if (!yyout)
        return NULL;
    Atom* atom_out = ((Dependency*)yyout)->atom;
    free((void*)yyout);
    return atom_out;
}

RequiredUse* required_use_parse (const char* buffer) {
    yylineno = 0;
    char* depend_expr_str = NULL;
    asprintf(&depend_expr_str, "RUSE_EXPR_AUTOGENTOO %s", buffer);

    YY_BUFFER_STATE bs = yy_scan_string(depend_expr_str);
    yy_switch_to_buffer(bs);
    yyparse();

    free(depend_expr_str);
    yy_delete_buffer(bs);
    return (RequiredUse*)yyout;
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
DEPEND_EXPR_AUTOGENTOO  {return DEPEND;}
RUSE_EXPR_AUTOGENTOO    {return REQUIRED_USE;}
COMMAND_LINE_AUTOGENTOO {return COMMAND_LINE;}
[\n]                    {yylineno++;}
[ \t\r\\]+              ;
{repo}                  {yylval.identifier = strdup(yytext + 2); return REPOSITORY;}
{slot}                  {
                            size_t len = strlen(yytext);
                            yylval.slot.sub_opts = ATOM_SLOT_IGNORE;
                            if (yytext[len - 1] == '*') {
                                yylval.slot.sub_opts = ATOM_SLOT_IGNORE;
                                yytext[--len] = 0;
                            }
                            else if (yytext[len - 1] == '=') {
                                yylval.slot.sub_opts = ATOM_SLOT_REBUILD;
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
"??"                    {yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_MOST_ONE; return USESELECT;}
"||"                    {yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_LEAST_ONE; return USESELECT;}
"^^"                    {yylval.use_select.target = strdup(yytext); yylval.use_select.val = USE_OP_EXACT_ONE; return USESELECT;}
"(+)"                   {yylval.use_default = ATOM_DEFAULT_ON; return USE_DEFAULT;}
"(-)"                   {yylval.use_default = ATOM_DEFAULT_OFF; return USE_DEFAULT;}
"-"                     {return '-';}
"<"                     {return '<';}
">"                     {return '>';}
"="                     {return '=';}
"!"                     {return '!';}
[\[]                    {return '[';}
[\]]                    {return ']';}
[\?]                    {return '?';}
[\(]                    {return '(';}
[\)]                    {return ')';}
[\^]                    {return '^';}
[\|]                    {return '|';}
[\,]                    {return ',';}
[\~]                    {return '~';}
{atom}                  {
                            yylval.atom_type = atom_new(yytext);
                            return ATOM;
                        }

[\*]                    {return '*';}

{identifier}            {
                            yylval.identifier = strdup(yytext);
                            return IDENTIFIER;
                        }


[<<EOF>>|'']            {return END_OF_FILE;}
.                       {printf("Unknown character: %s\n", yytext);}

%%