//
// Created by atuser on 4/23/19.
//


#include <openssl/evp.h>
#include <share.h>
#include <autogentoo/hacksaw/hacksaw.h>
#include "portage.h"
#include "portage_log.h"
#include "manifest.h"

int main (int argc, char** argv) {
	OpenSSL_add_all_digests();
	sha_hash test_hash;
	int md_len;
	
	
	PLOG_BENCHMARK(md_len = portage_get_hash(&test_hash, "/etc/portage/make.conf", EVP_sha256());, "PORTAGE_GET_HASH")
	
	Manifest* category_manifest;
	PLOG_BENCHMARK({
		category_manifest = manifest_metadata_parse("test");
	}, "CATEGORY_MANIFEST")
	
	PLOG_BENCHMARK({
		manifest_metadata_deep(category_manifest);
	}, "ALL_MANIFEST")
	
	
	Repository* test_repo;
	PLOG_BENCHMARK({test_repo = repository_new("gentoo", "/", "etc/portage");}, "REPO")
	
	Manifest* current_cat;
	Manifest* current_pkg;
	
	PLOG_BENCHMARK({
		for (current_cat = category_manifest; current_cat; current_cat = current_cat->next) {
			for(current_pkg = current_cat->parsed; current_pkg; current_pkg = current_pkg->next) {
				package_init(test_repo, current_cat, current_pkg);
			}
		}
	}, "PKG_INIT")
	
	Package* gcc = (Package*)map_get(test_repo->packages, "sys-devel/gcc");
	
	return 0;
}