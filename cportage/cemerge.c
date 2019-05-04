//
// Created by atuser on 4/23/19.
//


#include <openssl/evp.h>
#include <share.h>
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
	}, "CATEGORY_MANIFEST")
	
	
	PLOG_BENCHMARK({depend_parse("baz? (!=sys-devel/gcc-4.800.3_pre_beta-r1:1.0=[bar?,foo(-)]) foo?(cat-name/f)");}, "DEPEND_PARSE")
	
	return 0;
}