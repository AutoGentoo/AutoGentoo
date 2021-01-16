%option prefix="depend"

%{
#include <stdio.h>
#include "language.h"
#include <string.h>
#include "depend.tab.h"

#undef YY_INPUT
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

%%