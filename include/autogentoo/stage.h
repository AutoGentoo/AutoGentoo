#ifndef __AUTOGENTOO_STAGE_H__
#define __AUTOGENTOO_STAGE_H__

#include <autogentoo/response.h>

typedef struct __HostTemplate HostTemplate;

typedef enum {
	OTHER = 0x00, //!< Anything else
	CXXFLAGS = 0x01, //!< The gcc flags to pass to CXX
	TMPDIR = 0x02, //!< The direcory to build ebuilds in (/autogentoo/tmp)
	PORTDIR = 0x04, //!< The path to the portage repo (/usr/portage)
	DISTDIR = 0x08, //!< The path to the distfiles (/usr/portage/distfiles)
	PKGDIR = 0x10, //!< The path to the built binaries (/autogentoo/pkg) dont change!
	LOGDIR = 0x20 //!< The path to the logs for ebuild builds (/autogentoo/log)
} template_selects;

/**
 * @brief The template to creating a new Host
 * 
 * There is a global list in stage.c that will have all 
 * of the availiable/supported templates. For now I commented
 * out all but the amd64-systemd one just for testing but I will
 * I have more avaiable after AutoGentoo is released
 * Feel free to uncomment other ones (or add other ones)
 */
struct __HostTemplate {
	
	/**
	 * @brief The HostTemplate ID
	 *
	 * this will decide which stage3 to download
	 * - eg. amd64-systemd
	 */
	char* id;
	char* arch; //!< The portage arch of the system (eg. amd64)
	char* cflags; //!< CFLAGS put in make.conf
	char* chost; //!< CHOST of the system
	
	int extra_c; //!< The number of fields filled in extras (must be set to use extras)
	struct {
		char* make_extra; //!< Content to be put in make.conf
		
		/**
		 * @brief Select what type of data this is
		 *
		 * If this is used (set to anything other than 0),
		 * It will copy the contents to the correct location
		 * when it is handed over to a Host*
		 *
		 * Usage:
		 *
		 * (if OTHER):
		 *  - VAR_NAME=\"my value\"
		 * (if !OTHER):
		 *  - my value
		 */
		template_selects select;
	} extras[32]; //!< The extra settings to put in make.conf
	
	char* dest_dir; //!< The temporary destination before handoff (don't init)
	Server* parent; //!< The parent server (don't init)
	host_id new_id; //!< The new id that will be passed in the handoff (don't init)
};

extern Vector* host_templates;

/**
 * Fill the Tempate vector with the predefined templates
 * @param srv parent server
 */
void host_template_list_init (Server* srv);

/**
 * Duplicate the HostTemplate and add it to the parent
 * @param srv parent server
 * @param ht host template to dup
 */
void host_template_add (Server* srv, HostTemplate* ht);

/**
 * Returns a list of all the avaiable templates
 * @return a StringVector* of all the avaiable templates
 */
//StringVector* host_template_get_all();

/**
 * Get a new Stage from an Template ID
 * @param parent the parent server
 * @param index the id of the target host template
 * @return the newly created template
 */
HostTemplate* stage_new (Server* parent, int index);

/**
 * Download the stage3 of the HostTemplate
 * @param t the target HostTemplate
 * @return the path to the downloaded file
 */
char* host_template_download(HostTemplate* t);

/**
 * Extract the downloaded stage3 file to the dest_dir of the HostTemplate
 * @param t the target HostTemplate to extract
 * @param fname the filename (path) of the stage3
 */
response_t host_template_extract(HostTemplate* t, char* fname);

/**
 * Initialize a dynamic HostTemplate given a static one. \
 * Initilizes unknown fields such as dest_dir and parent
 * @param parent the parent server to bind to
 * @param t the static template to copy
 * @return a pointer the dynamically allocated template
 */
HostTemplate* host_template_init(Server* parent, HostTemplate* t);

/**
 * Initialize stage3 given a HostTemplate
 * This downloads and extracts the a stage3 \
 * downloads are always from http://distfiles.gentoo.org/
 * @param t the HostTemplate to initilize
 */
response_t host_template_stage(HostTemplate* t);

/**
 * Mark The given HostTemplate as ready
 * 
 * This allows the new Host to mount chroot and emerge new packages
 * @param src The old parent (a HostTemplate) of the directory
 * @return a pointer to the newly inited Host, NULL if it fails
 */
Host* host_template_handoff(HostTemplate* src);

/**
 * Free a HostTemplate
 * @param temp the HostTemplate to free
 */
void host_template_free(HostTemplate* temp);

#endif