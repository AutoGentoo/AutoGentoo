#include <portage/use_flags.h>

IUSE read_iuse (Ebuild* ebuild) {
    char* temp_use = conf_get(ebuild->metadata, "", "IUSE");
    IUSE out = iuse_new ();
    
}

IUSE iuse_new () {
    return vector_new (sizeof (Use), REMOVE | UNORDERED);
}

use_t iuse_get (IUSE to_check, char* flag_name);