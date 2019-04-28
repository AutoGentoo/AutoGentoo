//
// Created by atuser on 4/23/19.
//


#include <openssl/evp.h>
#include "portage.h"
#include "portage_log.h"
#include "manifest.h"

int main (int argc, char** argv) {
	OpenSSL_add_all_digests();
	SHA_HASH test_hash;
	int md_len;
	PLOG_BENCHMARK(md_len = portage_get_hash(test_hash, "/etc/portage/make.conf", EVP_sha256());, "PORTAGE_GET_HASH")
	
	Manifest* category_manifest;
	PLOG_BENCHMARK({
		category_manifest = manifest_metadata_parse("test");
		for (Manifest* current = category_manifest; current; current = current->next) {
			;
		}
	}, "CATEGORY_MANIFEST")
	
	
	return 0;
}